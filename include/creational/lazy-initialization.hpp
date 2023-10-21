#ifndef LAZY_INITIALIZATION_HPP
#define LAZY_INITIALIZATION_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace lazy_initialization {
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

			inline void Run() {};

		} // !namespace lazy_initialization
	} // !namespace creational
} // !namespace pattern

#endif // !LAZY_INITIALIZATION_HPP
