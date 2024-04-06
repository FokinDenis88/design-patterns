#ifndef PROTOTYPE_HPP
#define PROTOTYPE_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace prototype {


			/** Interface of Prototype design pattern. With Clone function. */
			class IPrototype {
			protected:
				IPrototype() = default;
				IPrototype(const IPrototype&) = default; // C.67	C.21
				IPrototype& operator=(const IPrototype&) = default;
				IPrototype(IPrototype&&) noexcept = default;
				IPrototype& operator=(IPrototype&&) noexcept = default;
			public:
				virtual ~IPrototype() = default;

				virtual std::unique_ptr<IPrototype> Clone() const = 0;
			};

			class PrototypeA : public IPrototype {
			public:
				PrototypeA() = default;
				explicit PrototypeA(int state_p) noexcept : originator_state_{ state_p } {
				};

				inline std::unique_ptr<IPrototype> Clone() const override {
					return std::make_unique<PrototypeA>(originator_state_);
				};

			private:
				int originator_state_{};
			};

			class Client {
			public:
				std::unique_ptr<IPrototype> creator_{};
			};

		} // !namespace prototype

	} // !namespace creational
} // !namespace pattern

#endif // !PROTOTYPE_HPP
