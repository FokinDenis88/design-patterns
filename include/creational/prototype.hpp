#ifndef PROTOTYPE_HPP
#define PROTOTYPE_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace prototype {

			class IPrototype {
			public:
				virtual ~IPrototype() = default;

				virtual std::unique_ptr<IPrototype> Clone() = 0;
			};

			class PrototypeA : public IPrototype {
			public:
				PrototypeA() = default;
				explicit PrototypeA(int state_p) : state{ state_p } {
				};

				inline std::unique_ptr<IPrototype> Clone() override {
					return std::make_unique<PrototypeA>(PrototypeA(state));
				};
			private:
				int state{};
			};

			class Client {
			public:
				std::unique_ptr<IPrototype> creator{};
			};

			void Run();
		} // !namespace prototype
	} // !namespace creational
} // !namespace pattern

#endif // !PROTOTYPE_HPP
