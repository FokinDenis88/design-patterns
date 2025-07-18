#ifndef LAZY_INITIALIZATION_HPP
#define LAZY_INITIALIZATION_HPP

#include <memory>
#include <utility> // make_pair
#include <map>

/** Software Design Patterns */
namespace pattern {
	namespace creational {

		namespace lazy_initialization {
//==================Variant 1====================================
			class LazyObject {
			public:
				int a() {
					if (!a_) {
						a_ = std::make_unique<int>();
					}
					return *a_;
				};
				void set_a(int a_p) {
					if (!a_) {
						a_ = std::make_unique<int>();
					}
					*a_ = a_p;
				};

			private:
				std::unique_ptr<int> a_{};
			};

//==================Variant 2====================================

			class ExpensiveObject {
			public:
				void compute() const {
					std::cout << "Expensive computation done." << std::endl;
				}
			};

			inline std::shared_ptr<ExpensiveObject> createExpensiveObject() {
				return std::make_shared<ExpensiveObject>();
			}

			inline int MainRun_1() {
				std::shared_ptr<ExpensiveObject> obj;	// deferred initialization

				bool useObj = true; // Condition based on which we decide whether to initialize

				if (useObj) {
					obj = createExpensiveObject();
					obj->compute();
				}

				return 0;
			}

//=====================Variant 3============================================

			class LazyInt {
			private:
				mutable bool initialized = false;
				mutable int value;

			public:
				int getValue() const {
					if (!initialized) {
						value = 42; // Создается значение при первом вызове
						initialized = true;
					}
					return value;
				}
			};


//=====================Variant 4============================================

			class Logger {
			private:
				friend std::unique_ptr<Logger> std::make_unique<Logger>();

				static std::unique_ptr<Logger> instance;

			public:
				static Logger& getInstance() {
					if (!instance)
						instance = std::make_unique<Logger>();
					return *instance.get();
				};

			private:
				Logger() = default; // Конструктор приватный
			};

			// Объявление статического члена вне определения класса
			std::unique_ptr<Logger> Logger::instance;


//=====================Variant 5============================================

			class DatabaseConnection {
			public:
				static DatabaseConnection& getInstance() {
					static DatabaseConnection instance; // Локальная статическая переменная
					return instance;
				}

			private:
				DatabaseConnection() {} // Приватный конструктор
			};


//=====================Variant 6============================================

			struct BigObject {
				BigObject(int val) : value(val) {}
				int value;
			};

			class LazyHolder {
			private:
				std::unique_ptr<BigObject> ptr;

			public:
				LazyHolder(std::function<BigObject* (void)> factoryFunc)
					: ptr(factoryFunc()) {
				}

				int getValue() const {
					return ptr->value;
				}
			};

			inline int MainRun_6() {
				auto lazyObj = LazyHolder([]() { return new BigObject(42); });
				std::cout << lazyObj.getValue() << '\n';
				return 0;
			};


		} // !namespace lazy_initialization



		namespace lazy_evaluation {
//==================Variant 1 Pair====================================

			// Example of lazy evaluation with lambdas
			inline auto lazySum(int x, int y) {
				return std::make_pair([x, y]() { return x + y; }, false); // Pair contains a lambda and a boolean indicating if it's been evaluated
			}

			inline int MainRun_1() {
				auto sum = lazySum(5, 7);

				if (!sum.second) { // Check if not already evaluated
					std::cout << "Evaluating..." << std::endl;
					sum.first();   // Force evaluation
					sum.second = true;
				}

				std::cout << "Result: " << sum.first() << std::endl;

				return 0;
			}

//==================Variant 2 Lambda====================================

			void Lambda() {
				auto calculate_expensive_value = []() { /* тяжелое вычисление */ };
				bool condition = true;
				if (condition) {
					//result = calculate_expensive_value();
				}
			}

//==================Variant 3 Proxy Classes====================================

			template<typename T>
			class LazyProxy {
			public:
				explicit LazyProxy(const std::function<T()>& factory) : factory_(factory) {}

				operator T() const {
					if (!cached_) cached_.reset(new T(factory_()));
					return *cached_;
				}

			private:
				std::function<T()> factory_; // Фабрика значений
				mutable std::unique_ptr<T> cached_; // Кэшированное значение
			};

			int MainRun_3() {
				LazyProxy<int> proxy([]() { return 42; }); // Замещение объекта прокси
				std::cout << proxy << "\n";               // Вычисляется при выводе
				return 0;
			}

//==================Variant 4 Memoization====================================

			std::map<int, long> fib_cache;
			inline long fibonacci(int n) {
				if (fib_cache.count(n)) return fib_cache[n];
				if (n <= 1) return n;
				return fib_cache[n] = fibonacci(n - 1) + fibonacci(n - 2);
			}

		} // !namespace lazy_evaluation

	} // !namespace creational
} // !namespace pattern

#endif // !LAZY_INITIALIZATION_HPP
