#ifndef PROTOTYPE_HPP
#define PROTOTYPE_HPP

#include <memory>
#include <concepts>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace prototype {
            // https://
            // Friend patterns:
            //


			/** Abstract. Interface of Prototype design pattern. With Clone function. */
			class IPrototype {
			protected:
				IPrototype() = default;
				IPrototype(const IPrototype&) = delete; // C.67	C.21 Polymorphic class
				IPrototype& operator=(const IPrototype&) = delete;
				IPrototype(IPrototype&&) noexcept = delete;
				IPrototype& operator=(IPrototype&&) noexcept = delete;
			public:
				virtual ~IPrototype() = default;

				virtual std::unique_ptr<IPrototype> Clone() const = 0;
			};

			template<typename ConcretePrototypeT>
			concept Prototype = std::derived_from<ConcretePrototypeT, IPrototype>;

			/**
			 * Clone Sample template function for concrete prototype class.
			 *
			 * @param ConcretePrototypeT Concrete prototype class
			 * @param constructor_args arguments for constructor of concrete prototype
			 * @param Prototype_Construct_Arg arguments for constructor
			 */
			template<Prototype ConcretePrototypeT, typename... Constructor_ArgsT>
            inline std::unique_ptr<IPrototype> CloneHelper(Constructor_ArgsT... constructor_args) {
                return std::make_unique<ConcretePrototypeT>(constructor_args...);
            };


			class PrototypeA : public IPrototype {
			public:
				PrototypeA() = default;
				explicit PrototypeA(int state_p) noexcept : originator_state_{ state_p } {
				};

				inline std::unique_ptr<IPrototype> Clone() const override {
					//return std::make_unique<PrototypeA>(originator_state_);
					return CloneHelper<PrototypeA>(originator_state_);
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
