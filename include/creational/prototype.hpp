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

				virtual std::unique_ptr<IPrototype> Clone() const noexcept = 0;
			};

			class PrototypeA : public IPrototype {
			public:
				PrototypeA() = default;
				explicit PrototypeA(int state_p) noexcept : observer_state_{ state_p } {
				};

				inline std::unique_ptr<IPrototype> Clone() const noexcept override {
					return std::make_unique<PrototypeA>(observer_state_);
				};

			private:
				int observer_state_{};
			};

			class Client {
			public:
				std::unique_ptr<IPrototype> creator_{};
			};

		} // !namespace prototype

	} // !namespace creational
} // !namespace pattern

#endif // !PROTOTYPE_HPP
