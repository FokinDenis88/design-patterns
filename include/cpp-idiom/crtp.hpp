#ifndef CRTP_HPP
#define CRTP_HPP

#include <memory>
#include <utility>

//#include <ostream>
#include <iostream>

/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace crtp {
			// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
			// https://en.cppreference.com/w/cpp/language/crtp

			/**
			 * Use cases: static polymorphism and other metaprogramming techniques.
			 * This technique achieves a similar effect to the use of virtual functions,
			 * without the costs (and some flexibility) of dynamic polymorphism.
			 * Dynamic polymorphism can be 10 times slowly than static polymorphism.
			 * This particular use of the CRTP has been called "simulated dynamic binding" by some.
			 *
			 * Whenever the base class calls another member function, it will always call its own base class functions.
			 * When we derive a class from this base class, we inherit all the member variables and member functions that were not overridden
			 * (no constructors or destructors). If the derived class calls an inherited function which then calls another member function, then
			 * that function will never call any derived or overridden member functions in the derived class.
			 *
			 * One issue with static polymorphism is that without using a com base class like AbstractShape from the above example,
			 * derived classes cannot be stored homogeneously – that is, putting different types derived from the same base class in the same container.
			 *
			 * Another issue is infinite loop, if in Derived class there is no Base class (overriden) function.
			 * So there must be different names for Base class interface function and Derived class implementation function.
			 */


			/**
			 * The Curiously Recurring Template Pattern (CRTP).
			 * Static Interface variant of crtp pattern.
			 */
			template <class T>
			class Base {
			public:
				inline T* DownCast() noexcept { // must not be const, for modification member functions
					return static_cast<T*>(this);
				}

				// methods within Base can use template to access members of Derived

				void Interface() {
					// ...
					// No run-time checks, when down cast
					static_cast<T*>(this)->Implementation();
					static_cast<T&>(*this).Implementation();
					DownCast()->Implementation();
					// ...
				}

				static void StaticFunc() {
					// ...
					T::StaticSubFunc();
					// ...
				}

				int Foo(int arg) {
					return DownCast()->FooImpl(arg);
				}

				// Hybrid class with virtual Destructor.
				//virtual ~Base() = default;

			private:
				friend T;	// To give possibility to create Base subobject in Derived class

				// To protect against error Derived_2 : Base<Derived_1>
				// It will be like static polymorphism abstract class. Can be instantiated only from correct Derived class
				Base() = default;


				/**
				 * Base class default implementation of virtual function.
				 * Must not be seen from inside.
				 */
				int FooImpl(int arg) {
					return arg;
				}

				T& down_cast{ *DownCast() };
			};


			class Derived : public Base<Derived> {
			public:
				void Implementation() {};
				static void StaticSubFunc() {};

			private:
				friend class Base<Derived>;	// To give access to FooImpl from Base class
				int FooImpl(int arg) { return arg; };
			};


			/** Now Function Template can be used with ANY class Base<D> */
			template<typename D>
			int ApplyFooFn(Base<D>* obj, int arg) {
				return obj->Foo(arg);
			}

			/**
			 * For break infinite loop. Alternative: Can be used virtual destructor, but
			 * this will increase storage of object.
			 */
			template<typename D>
			void DeleteBase(Base<D>* obj) {
				static_cast<D*>(obj)->~D();
			}


//============Interface Extension and Delegation Version of CRTP======================================
			/**
			 * Base class is not defining interface. Derived class is not only realization.
			 * Derived extend interface of Base. Base delegates behavior to Derived.
			 */


			/**
			 * Partial Interface and Partial Extension variant of crtp pattern.
			 * Will be used only Derived class.
			 */
			template<typename ExtendableT>
			struct NotEqual {
				friend bool operator!=(const ExtendableT& lhs, const ExtendableT& rhs) noexcept {
					return !(lhs == rhs);
				}
			};

			/** Interface and realization */
			class C : public NotEqual<C> {
			public:
				friend bool operator==(const C& lhs, const C& rhs) noexcept {
					return lhs.i == rhs.i;
				}

				int i{};
			};


			/**
			 * Registry with Object Counter.
			 * Double linked list.
			 */
			template<typename ObjectT>
			class Registry {
			public:
				friend ObjectT;

				static size_t count_;
				static ObjectT* head_;

				ObjectT* prev_{};
				ObjectT* next_{};

			private:
				Registry() {
					++count_;
					prev_ = nullptr;
					next_ = head_;
					head_ = static_cast<ObjectT*>(this);
					if (next_) { next_->prev_ = head_; }
				}
				Registry(const Registry&) {
					Registry();
				}

				~Registry() {
					--count_;
					if (prev_) { prev_->next_ = next_; }
					if (next_) { next_->prev_ = prev_; }
					if (head_ == this) { head_ = next_; }
				}
			};
			template<typename ObjectT> size_t Registry<ObjectT>::count_{ 0 };
			template<typename ObjectT> ObjectT* Registry<ObjectT>::head_{ nullptr };

			struct Animal {
				enum T { Cat, Dog, Rat };
				Animal(T t, const char* n) : type{t}, name{n} {}

				const T type;
				const char* const name;
			};

			template<typename D> class GenericVisitor {
			public:
				template<typename it> void visit(it from, it to) {
					for (it i = from; i != to; ++i) {
						this->visit(*i);
					}
				}

			private:
				friend D;
				GenericVisitor() = default;

				D& derived() { return *static_cast<D*>(this); }
				void visit(const Animal& animal) {
					switch (animal.type) {
					case Animal::Cat:
						derived().visit_cat(animal);
						break;
					case Animal::Dog:
						derived().visit_dog(animal);
						break;
					case Animal::Rat:
						derived().visit_rat(animal);
						break;
					}
				}

				// Default Actions
				void visit_cat(const Animal& animal) {
					std::cout << "Feed cat " << animal.name << '\n';
				}
				void visit_dog(const Animal& animal) {
					std::cout << "Feed dog " << animal.name << '\n';
				}
				void visit_rat(const Animal& animal) {
					std::cout << "Feed rat " << animal.name << '\n';
				}
			};

			/** Actions Extender */
			class TrainerVisitor : public GenericVisitor<TrainerVisitor> {
				friend class GenericVisitor<TrainerVisitor>;
				void visit_dog(const Animal& animal) {
					std::cout << "Train dog " << animal.name << '\n';
				}
			};


//============================Object Counter===========================

			template <typename T>
			struct counter {
				static inline int objects_created = 0;
				static inline int objects_alive = 0;

				counter() {
					++objects_created;
					++objects_alive;
				}

				counter(const counter&) {
					++objects_created;
					++objects_alive;
				}
			protected:
				~counter() { // objects should never be removed through pointers of this type
					--objects_alive;
				}
			};

			class X : counter<X> {
				// ...
			};

			class Y : counter<Y> {
				// ...
			};


//==========================Polymorphic chaining===========================
			// Base class
			template <typename ConcretePrinter>
			class Printer {
			public:
				Printer(std::ostream& pstream) : m_stream(pstream) {}

				template <typename T>
				ConcretePrinter& print(T&& t) {
					m_stream << t;
					return static_cast<ConcretePrinter&>(*this);
				}

				template <typename T>
				ConcretePrinter& println(T&& t) {
					m_stream << t << std::endl;
					return static_cast<ConcretePrinter&>(*this);
				}
			private:
				std::ostream& m_stream;
			};

			// Derived class
			class CoutPrinter : public Printer<CoutPrinter> {
			public:
				CoutPrinter() : Printer(std::cout) {}

				CoutPrinter& SetConsoleColor(size_t c) {
					// ...
					return *this;
				}
			};

			// usage
			//CoutPrinter().print("Hello ").SetConsoleColor(Color.red).println("Printer!");


//========================Polymorphic copy construction===========================
			// Base class has a pure virtual function for cloning
			/** This interface is needed for homogeneous container storage */
			class AbstractShape {
			public:
				virtual ~AbstractShape() = default;
				virtual std::unique_ptr<AbstractShape> clone() const = 0;
			};

			// This CRTP class implements clone() for Derived
			// Maybe, it can be abstract too
			template <typename Derived>
			class Shape : public AbstractShape {
			public:
				std::unique_ptr<AbstractShape> clone() const override {
					return std::make_unique<Derived>(static_cast<Derived const&>(*this));
				}

			protected:
				// We make clear Shape class needs to be inherited
				Shape() = default;
				Shape(const Shape&) = default;
				Shape(Shape&&) = default;
			};

			// Every derived class inherits from CRTP class instead of abstract class

			class Square : public Shape<Square> {};

			class Circle : public Shape<Circle> {};

		} // !namespace crtp

	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !CRTP_HPP
