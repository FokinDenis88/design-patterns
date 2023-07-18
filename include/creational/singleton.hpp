#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace singleton {

			/** Class, which can be instantiated only one time. Instance is stored in heap. */
			template<typename DataType>
			class Singleton {
			public:
				/** Get the only one instance of class */
				static Singleton& Get() {
					static Singleton instance{};
					if (instance.data) {
						instance.data = std::make_unique<DataType>();
					}
					return instance;
				}

				/** Data to be stored in singleton */
				std::unique_ptr<DataType> data{};

			private:
				/** There must be only one instance of class, so hidden constructor */
				Singleton() = default;
				Singleton(const Singleton&) = delete;
				Singleton& operator=(const Singleton&) = delete;
				~Singleton() = default;
			};



			template<typename SingletonType>
			SingletonType SingletonFunction() {
				static const SingletonType singleton{};
				return singleton;
			}




			/** Class, which can be instantiated only one time.  Instance is stored in global memory. */
			template<typename SingletonType>
			class SingletonStatic {
			public:
				static SingletonType instance_{};

			private:
				SingletonStatic() = default;
				SingletonStatic(const SingletonStatic<SingletonType>&) = default;
				SingletonStatic& operator=(const SingletonStatic<SingletonType>&) = default;
				~SingletonStatic() = default;
			};

			/** Class, which can be instantiated only one time. Instance is stored in heap. */
			template<typename DataType>
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
					return instance_->get();
				}

				/** Data to be stored in singleton */
				DataType data{};

			private:
				/** There must be only one instance of class, so hidden constructor */
				Singleton_2() = default;

				inline static std::unique_ptr<Singleton_2> instance_{};
			};



			void Run();

		} // !namespace singleton
	} // !namespace creational
} // !namespace pattern

#endif // !SINGLETON_HPP
