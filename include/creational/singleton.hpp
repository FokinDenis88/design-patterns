#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <memory>
#include <vector>

/** Software Design Patterns */
namespace pattern {
	namespace creational {

		namespace singleton {
			/**
			 * https://en.wikipedia.org/wiki/Singleton_pattern
			 * Friend patterns: abstract factory, factory method, builder, prototype.
			 *
			 * Implementations of the singleton pattern ensure that only one instance of the singleton class ever exists and typically
			 * provide global access to that instance.
			 *
             * Declaring all constructors of the class to be private, which prevents it from being instantiated by other objects
             * Providing a static method that returns a reference to the instance
             *
             * Realization: Singleton registry. Stores registry of unique objects of class.
             * Multi Singleton. Helps to make classes with custom number of instances.
             * In classic singleton there is only one instance. But in multi singleton number of instances is n.
			 */


			/**
			 * Class, which can be instantiated only one time. Instance is stored in dynamic memory - heap.
			 * Code and Data of real class must be inside of Singleton class.
			 */
			template<typename SingletonT>
			class SingletonDynamic {
			public:

				/** Defines an class operation that lets clients access its unique instance. */
				static SingletonT& GetSingleton() { // may be responsible for creating its own unique instance.
					if (!instance_ptr) { instance_ptr = std::make_unique<SingletonT>(); }
					return (*instance_ptr); // reference
				};

				/** Delete memory for the only one instance object of class */
				static void DestructSingleton() {
					instance_ptr.reset();
				};

			protected:
				SingletonDynamic() = default; // no public constructor
				SingletonDynamic(const SingletonDynamic&) = delete; // rule of five
				SingletonDynamic& operator=(const SingletonDynamic&) = delete;
				SingletonDynamic(SingletonDynamic&&) noexcept = delete;
				SingletonDynamic& operator=(SingletonDynamic&&) noexcept = delete;
				virtual ~SingletonDynamic() = default; // no public destructor

			private:
				/** Pointer to the only one object of class. Singleton instance. */
				inline static std::unique_ptr<SingletonT> instance_ptr{ nullptr };
			};


			/**
			 * Class, which can be instantiated only one time. Instance is stored in global memory, static storage duration.
			 * Code and Data of real class must be inside of Singleton class.
			 *
			 * @tparam SingletonT concrete class of singleton object
			 */
			template<typename SingletonT>
			class SingletonStatic {
			public:
				/** Get the only one instance of class */
				static SingletonT& GetSingleton() {
					static SingletonT instance{};
					return (instance);
				};

			protected:	// protected for inheritance; private for single class with data
				SingletonStatic() = default; // There must be only one instance of class, so hidden constructor
				SingletonStatic(const SingletonStatic&) = delete;
				SingletonStatic& operator=(const SingletonStatic&) = delete;
				SingletonStatic(SingletonStatic&&) noexcept = delete;
				SingletonStatic& operator=(SingletonStatic&&) noexcept = delete;
				virtual ~SingletonStatic() = default;
			};



			/**
			 * Function, that store Singleton object inside it self.
			 *
			 * @return Singleton object, stored statically inside function, in global memory..
			 */
            template<typename SingletonT>
            SingletonT& SingletonFn() {
                static SingletonT singleton{};
                return (singleton);
            }

            /** Class, which can be instantiated only one time.  Instance is stored in global memory. */
            template<typename SingletonT>
            class SingletonStaticField {
            public:
                static SingletonT instance_{};

            private:
                SingletonStaticField() = default;
                SingletonStaticField(const SingletonStaticField<SingletonT>&) = delete;
				SingletonStaticField& operator=(const SingletonStaticField<SingletonT>&) = delete;
				SingletonStaticField(SingletonStaticField<SingletonT>&&) noexcept = delete;
				SingletonStaticField& operator=(SingletonStaticField<SingletonT>&&) noexcept = delete;
                ~SingletonStaticField() = default;
            };


			/**
			 * Class, which can be instantiated custom number of time. Instance is stored in global memory, static storage duration.
			 * Code and Data of real class must be inside of Singleton class.
			 *
			 * @tparam SingletonT concrete class of singleton object
			 */
			template<typename SingletonT>
			class SingletonStaticMulti {
			public:
				/**
				 * Get the only one instance of class.
				 *
				 * @param index - index of singleton instance. [0, +eternity].
				 * @param max_size_new - number of singleton instances. [1, +eternity].
				 * @return
				 */
				static SingletonT& GetSingleton(const size_t index = 0,
												   const size_t max_size_new = 0) {
					static std::vector<SingletonT> instance(1);
					if (max_size_new >= 1) { instance.resize(max_size_new); }
					if (index <= max_size_new - 1) { return (instance[index]); } // TODO: review different variants of index and max_size
					return instance[0];
				};

			protected:	// protected for inheritance; private for single class with data
				SingletonStaticMulti() = default; // There must be only one instance of class, so hidden constructor
				SingletonStaticMulti(const SingletonStaticMulti&) = delete;
				SingletonStaticMulti& operator=(const SingletonStaticMulti&) = delete;
				SingletonStaticMulti(SingletonStaticMulti&&) noexcept = delete;
				SingletonStaticMulti& operator=(SingletonStaticMulti&&) noexcept = delete;
				virtual ~SingletonStaticMulti() = default;

			private:
				size_t max_size_{};
			};


			class MySingletonStatic : public SingletonStatic<MySingletonStatic> {
			public:
                MySingletonStatic() = default; // no public constructor
                MySingletonStatic(const MySingletonStatic&) = delete; // rule of five
                MySingletonStatic& operator=(const MySingletonStatic&) = delete;
                MySingletonStatic(MySingletonStatic&&) noexcept = delete;
                MySingletonStatic& operator=(MySingletonStatic&&) noexcept = delete;
                ~MySingletonStatic() override = default; // no public destructor

				int a = 1;
			};


            class MySingletonDynamic : public SingletonDynamic<MySingletonDynamic> {
            public:
                MySingletonDynamic() = default; // no public constructor
                MySingletonDynamic(const MySingletonDynamic&) = delete; // rule of five
                MySingletonDynamic& operator=(const MySingletonDynamic&) = delete;
                MySingletonDynamic(MySingletonDynamic&&) noexcept = delete;
                MySingletonDynamic& operator=(MySingletonDynamic&&) noexcept = delete;
                ~MySingletonDynamic() override = default; // no public destructor

				int b = 2;
            };


   //         template<typename SingletonT>
   //         class SingletonRegistry {
   //         public:
   //             static SingletonT& GetSingleton() {
   //                 static SingletonT instance{};
   //                 return (instance);
   //             };

			//	static void Register(const char* name, Singleton*);
			//	static Singleton* Instance();
			//protected:
			//	static Singleton* Lookup(const char* name);
			//private:
			//	static Singleton* _instance;
			//	static List<NameSingletonPair>* _registry;

			//	Singleton* Singleton::Instance() {
			//		if (_instance == 0) {
			//			const char* singletonName = getenv("SINLETON");
			//			_instance = Lookup(singletonName);
			//		}
			//	}

   //         protected:	// protected for inheritance; private for single class with data
   //             SingletonRegistry() = default; // There must be only one instance of class, so hidden constructor
   //             SingletonRegistry(const SingletonRegistry&) = delete;
   //             SingletonRegistry& operator=(const SingletonRegistry&) = delete;
   //             SingletonRegistry(SingletonRegistry&&) noexcept = delete;
   //             SingletonRegistry& operator=(SingletonRegistry&&) noexcept = delete;
   //             virtual ~SingletonRegistry() = default;
   //         };



			/* TODO: Make SingletonFabric
			GetSingleton may choose what class to instantiate
			template<typename SingletonT>
			class SingletonFabric {
            public:
                static SingletonT& GetSingleton(std::string class_type) {
                    static SingletonT instance{};
                    return (instance);
                };

            protected:	// protected for inheritance; private for single class with data
                SingletonFabric() = default; // There must be only one instance of class, so hidden constructor
                SingletonFabric(const SingletonFabric&) = delete;
                SingletonFabric& operator=(const SingletonFabric&) = delete;
                SingletonFabric(SingletonFabric&&) noexcept = delete;
                SingletonFabric& operator=(SingletonFabric&&) noexcept = delete;
                virtual ~SingletonFabric() = default;
			};*/


		} // !namespace singleton




		namespace singleton_classic {

            // https://en.wikipedia.org/wiki/Singleton_pattern  - for c++ 23

            class SingletonMayers {
            public:
                static SingletonMayers& get() {
                    static SingletonMayers instance;
                    return instance;
                }
                int getValue() {
                    return value;
                }
                void setValue(int value_) {
                    value = value_;
                }
            private:
                SingletonMayers() = default;
                ~SingletonMayers() = default;
                int value;
            };

            class SingletonNew {
            public:
                // defines an class operation that lets clients access its unique instance.
                static SingletonNew& get() {
                    // may be responsible for creating its own unique instance.
                    if (nullptr == instance) instance = new SingletonNew;
                    return *instance;
                }
                SingletonNew(const SingletonNew&) = delete; // rule of three
                SingletonNew& operator=(const SingletonNew&) = delete;
                static void destruct() {
                    delete instance;
                    instance = nullptr;
                }
                // existing interface goes here
                int getValue() {
                    return value;
                }
                void setValue(int value_) {
                    value = value_;
                }
            private:
                SingletonNew() = default; // no public constructor
                ~SingletonNew() = default; // no public destructor
                static SingletonNew* instance; // declaration class variable
                int value;
            };

            SingletonNew* SingletonNew::instance = nullptr; // definition class variable

		} // !namespace singleton_classic



		namespace singleton_example {

			/** Class, which can be instantiated only one time. Instance is stored in heap. */
			template<typename DataT>
			class Singleton {
			public:
				/** Get the only one instance of class */
				static Singleton& Get() {
					static Singleton instance{};
					if (!instance.data) {
						instance.data = std::make_unique<DataT>();
					}
					return (instance);
				}

				/** Data to be stored in singleton */
				std::unique_ptr<DataT> data{};

			private:
				/** There must be only one instance of class, so hidden constructor */
				Singleton() = default;
				Singleton(const Singleton&) = delete;
				Singleton& operator=(const Singleton&) = delete;
				~Singleton() = default;
			};



			template<typename SingletonT>
			SingletonT& SingletonFunction() {
				static SingletonT singleton{};
				return (singleton);
			}




			/** Class, which can be instantiated only one time.  Instance is stored in global memory. */
			template<typename SingletonT>
			class SingletonStatic {
			public:
				static SingletonT instance_{};

			private:
				SingletonStatic() = default;
				SingletonStatic(const SingletonStatic<SingletonT>&) = default;
				SingletonStatic& operator=(const SingletonStatic<SingletonT>&) = default;
				~SingletonStatic() = default;
			};

			/** Class, which can be instantiated only one time. Instance is stored in heap. */
			template<typename DataT>
			class Singleton_2 {
			public:
				Singleton_2(const Singleton_2&) = delete;
				Singleton_2& operator=(const Singleton_2&) = delete;

				~Singleton_2() {
					instance_.reset();
				};

				/** Get the only one instance of class */
				static Singleton_2& Get() {
					if (!instance_) {
						instance_ = std::make_unique<Singleton_2>();
					}
					return (instance_->get());
				}

				/** Data to be stored in singleton */
				DataT data{};

			private:
				/** There must be only one instance of class, so hidden constructor */
				Singleton_2() = default;

				inline static std::unique_ptr<Singleton_2> instance_{};
			};

		} // !namespace singleton_example

	} // !namespace creational
} // !namespace pattern

#endif // !SINGLETON_HPP
