#ifndef MEMENTO_HPP
#define MEMENTO_HPP

#include <memory>
#include <utility>
#include <concepts>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {

		namespace memento {
			// https://en.wikipedia.org/wiki/Memento_pattern
			// Friend patterns: Command - put state information, needed for cancel execution in memento.
			//					Iterator - to execute iterations.


			template<typename ConcreteOriginatorT, typename StateType>
			class IOriginator;

			template<typename ConcreteOriginatorT, typename StateType>
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, MementoStateT>>
			class Memento;


			/**
			 * Can be used only by class derived from IOriginator and only by owner, created memento.
			 * Memento must use only Concrete Originator, not interface.
			 */
			template<typename ConcreteOriginatorT, typename MementoStateT = ConcreteOriginatorT>
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, MementoStateT>>
			class Memento final {
			public:
				//using IOriginatorType = IOriginator<ConcreteOriginatorT, MementoStateT>;
				//friend IOriginatorType;
				//friend class IOriginator<ConcreteOriginatorT, MementoStateT>;
				//friend IOriginator<ConcreteOriginatorT, MementoStateT>;
				friend ConcreteOriginatorT;

				Memento() = delete;	// Must be created only by Concrete Originator
				Memento(const Memento&) = default;
				Memento& operator=(const Memento&) = default;
				Memento(Memento&&) noexcept = default;
				Memento& operator=(Memento&&) noexcept = default;


				/**  */
				/*explicit Memento(const IOriginatorType& owner_p, MementoStateT& owners_state) noexcept
							: owner_{ owner_p }, originator_state_{ owner_p.GetStateForMemento() } {*/

							/**
							 * Two params let ConcreteOriginatorT to create memento
							 * with different state, different part of Originator.
							 */
				explicit Memento(const ConcreteOriginatorT& owner_p, const MementoStateT& owners_state_p) noexcept
					: owner_{ owner_p }, originator_state_{ owners_state_p } {
				};

			private:	// Interface is closed for all classes except owner ConcreteOriginatorT

				constexpr bool IsOwner(const ConcreteOriginatorT& caller) const noexcept {
					return &caller == &owner_;
				};

				/** Used Only by MementoStateT. */
				void set_state(const ConcreteOriginatorT& caller, const MementoStateT& state_p) noexcept {
					if (IsOwner(caller)) {
						originator_state_ = state_p;
					}
				};

				/**
				 * Used Only by Originator, that has created memento can restore it state_ptr.
				 * Can be used only by class derived from IOriginator and only by
				 * owner, created memento.
				 * May be nullptr, cos caller can be other then owner. And only owner can use memento.
				 */
				inline MementoStateT* state_ptr(const ConcreteOriginatorT& caller) noexcept {
					return (IsOwner(caller)) ? &originator_state_ : nullptr;
				};


				/** Only owner can use memento to restore state_ptr */
				const ConcreteOriginatorT& owner_;

				/**
				 * State of concrete Originator object.
				 * From this state originator object will be restored.
				 */
				MementoStateT originator_state_{};	// Is Non Const, cause move operation, when restore.

			}; // !class Memento


			/**
			 * Abstract. Stateless. AbstractOriginator class let to create Memento
			 * and to Restore State of Originator from Memento.
			 * Is used for public inheritance.
			 */
			template<typename MementoStateT>
			class AbstractOriginator {
				// TODO: Design. One originator can create multiple Memento. Whole or parts of object state. ???
			public:
				virtual ~AbstractOriginator() = default;
			protected:
				AbstractOriginator() = default;
				AbstractOriginator(const AbstractOriginator&) = default;
				AbstractOriginator& operator=(const AbstractOriginator&) = default;
				AbstractOriginator(AbstractOriginator&&) noexcept = default;
				AbstractOriginator& operator=(AbstractOriginator&&) noexcept = default;

			public:

				/**
				 * .
				 *
				 * @param MementoStateT must be state struct or concrete Originator class.
				 */
				template<typename StateType>
				std::unique_ptr<Memento<AbstractOriginator, StateType>> CreateMemento() const {
					return std::make_unique<Memento<AbstractOriginator, StateType>>();
				};

				/**
				 * Restore State of Originator from moved pointer to Memento.
				 * Previous unique pointer will be empty.
				 */
				template<typename StateType>
				std::unique_ptr<Memento<AbstractOriginator, StateType>>
				bool RestoreMove(Memento<AbstractOriginator, StateType>&& memento_p) {
					if (State * memento_state_ptr{ memento_p->state_ptr(*this) }) {
						SaveMementoToOriginator(memento_state_ptr);
						return true;
					}
					else {
						return false;
					}
				};

				/**
				 * Restore State of Originator from Memento using copy operation.
				 * Previous unique pointer will be empty.
				 */
				template<typename StateType>
				std::unique_ptr<Memento<AbstractOriginator, StateType>>
				bool RestoreCopy(Memento<AbstractOriginator, StateType>&& memento_p) {
					if (State * memento_state_ptr{ memento_p->state_ptr(*this) }) {
						SaveMementoToOriginator(memento_state_ptr);
						return true;
					}
					else {
						return false;
					}
				};

			protected:
				template<typename StateType>
				std::unique_ptr<Memento<AbstractOriginator, StateType>>
				virtual void CopyOriginatorToMemento(State* memento_state_p) = 0;
				// originator_state_ = std::move(*memento_state_ptr);

				template<typename StateType>
				std::unique_ptr<Memento<AbstractOriginator, StateType>>
				virtual void CopyMementoToOriginator(State* memento_state_p) {};
				// originator_state_ = std::move(*memento_state_ptr);


				template<typename StateType>
				std::unique_ptr<Memento<AbstractOriginator, StateType>>
				virtual void MoveMementoToOriginator(State* memento_state_p) {};
				// originator_state_ = std::move(*memento_state_ptr);
			};


			template<typename ConcreteOriginatorT, typename StateType = ConcreteOriginatorT>
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, MementoStateT>>
			class CareTaker final {
			public:
				using MementoType = Memento<ConcreteOriginatorT, StateType>;
				using MementoPtr = std::unique_ptr<MementoType>;

				inline void set_memento(MementoPtr&& memento_p) noexcept {
					memento_ = std::move(memento_p);
				};

				inline MementoPtr memento() noexcept {
					return memento_ ? std::move(memento_) : nullptr;
				}

			private:
				MementoPtr memento_;
			};


			/** Concrete State as interesting part of Originator. Part or whole object. */
			struct State {
				int a{};
				int b{};
			};


			/**
			 * Abstract. Stateless. Interface for class, whose state will be saved and restored.
			 * Memento must use only Concrete Originator, not interface.
			 *
			 * @param MementoStateT must be state struct or concrete Originator class derived from IOriginator.
			 */
			template<typename ConcreteOriginatorT, typename MementoStateT = ConcreteOriginatorT>
			class IOriginator {
			public:
				using MementoType = Memento<ConcreteOriginatorT, MementoStateT>;
				using MementoPtr = std::unique_ptr<MementoType>;

			protected:
				IOriginator() = default;
				IOriginator(const IOriginator&) = default; // C.67	C.21
				IOriginator& operator=(const IOriginator&) = default;
				IOriginator(IOriginator&&) noexcept = default;
				IOriginator& operator=(IOriginator&&) noexcept = default;
			public:
				virtual ~IOriginator() = default;

				virtual MementoPtr CreateMemento() const = 0;
				/** @return indicator of restoration success */
				virtual bool Restore(MementoPtr&& memento_p) = 0;

				//protected:
					/**
					 * For memento initialization needed to get originator's state.
					 * State can be the whole object or it's part. Depends on the interest.
					 */
					 //virtual const MementoStateT& GetStateForMemento() const noexcept = 0;
			};

		} // !namespace memento






//===============================================================================================================================================
		// This code is not for direct public inheritance. It is straight wiki example.
		namespace memento_old_code {
			// https://en.wikipedia.org/wiki/Memento_pattern
			// Friend patterns: Command - put state information, needed for cancel execution in memento.
			//					Iterator - to execute iterations.


			template<typename ConcreteOriginatorT, typename StateType>
			class IOriginator;

			template<typename ConcreteOriginatorT, typename StateType>
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, MementoStateT>>
			class Memento;


			/**
			 * Abstract. Stateless. Interface for class, whose state will be saved and restored.
			 * Memento must use only Concrete Originator, not interface.
			 *
			 * @param MementoStateT must be state struct or concrete Originator class derived from IOriginator.
			 */
			template<typename ConcreteOriginatorT, typename StateType = ConcreteOriginatorT>
			class IOriginator {
			public:
				using MementoType = Memento<ConcreteOriginatorT, StateType>;
				using MementoPtr = std::unique_ptr<MementoType>;
				//using IOriginatorType = IOriginator<MementoStateT>;

			protected:
				IOriginator() = default;
				IOriginator(const IOriginator&) = default; // C.67	C.21
				IOriginator& operator=(const IOriginator&) = default;
				IOriginator(IOriginator&&) noexcept = default;
				IOriginator& operator=(IOriginator&&) noexcept = default;
			public:
				virtual ~IOriginator() = default;

				virtual MementoPtr CreateMemento() const = 0;
				/** @return indicator of restoration success */
				virtual bool Restore(MementoPtr&& memento_p) = 0;

			//protected:
				/**
				 * For memento initialization needed to get originator's state.
				 * State can be the whole object or it's part. Depends on the interest.
				 */
				//virtual const StateType& GetStateForMemento() const noexcept = 0;
			};


			/**
			 * Can be used only by class derived from IOriginator and only by owner, created memento.
			 * Memento must use only Concrete Originator, not interface.
			 */
			template<typename ConcreteOriginatorT, typename StateType = ConcreteOriginatorT>
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, MementoStateT>>
			class Memento final {
			public:
				//using IOriginatorType = IOriginator<ConcreteOriginatorT, MementoStateT>;
				//friend IOriginatorType;
				//friend class IOriginator<ConcreteOriginatorT, MementoStateT>;
				//friend IOriginator<ConcreteOriginatorT, MementoStateT>;
				friend ConcreteOriginatorT;

				Memento() = delete;	// Must be created only by Concrete Originator
				Memento(const Memento&) = default;
				Memento& operator=(const Memento&) = default;
				Memento(Memento&&) noexcept = default;
				Memento& operator=(Memento&&) noexcept = default;


				/**
				 * Two params let ConcreteOriginatorT to create memento
				 * with different state, different part of Originator.
				 */
				explicit Memento(const ConcreteOriginatorT& owner_p, const StateType& owners_state_p) noexcept
							: owner_{ owner_p }, originator_state_{ owners_state_p } {
				};

			private:	// Interface is closed for all classes except owner IOriginator<ConcreteOriginatorT, MementoStateT>

				constexpr bool IsOwner(const ConcreteOriginatorT& caller) const noexcept {
					return &caller == &owner_;
				};

				/** Used Only by MementoStateT. */
				void set_state(const ConcreteOriginatorT& caller, const StateType& state_p) noexcept {
					if (IsOwner(caller)) {
						originator_state_ = state_p;
					}
				};

				/**
				 * Used Only by Originator, that has created memento can restore it state_ptr.
				 * Can be used only by class derived from IOriginator and only by
				 * owner, created memento.
				 * May be nullptr.
				 */
				inline StateType* state_ptr(const ConcreteOriginatorT& caller) noexcept {
					return (IsOwner(caller)) ? &originator_state_ : nullptr;
				};


				/** Only owner can use memento to restore state_ptr */
				const ConcreteOriginatorT& owner_;

				/**
				 * State of concrete Originator object.
				 * From this state originator object will be restored.
				 */
				StateType originator_state_{};	// Is Non Const, cause move operation, when restore.

			}; // !class Memento


			template<typename ConcreteOriginatorT, typename StateType = ConcreteOriginatorT>
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, MementoStateT>>
			class CareTaker final {
			public:
				using MementoType = Memento<ConcreteOriginatorT, StateType>;
				using MementoPtr = std::unique_ptr<MementoType>;

				inline void set_memento(MementoPtr&& memento_p) noexcept {
					memento_ = std::move(memento_p);
				};

				inline MementoPtr memento() noexcept {
					return memento_ ? std::move(memento_) : nullptr;
				}

			private:
				MementoPtr memento_;
			};


			/** Concrete State as interesting part of Originator. Part or whole object. */
			struct State {
				int a{};
				int b{};
			};

			/**
			 * Example of Originator custom class.
			 * Is used for public inheritance.
			 */

			//template<typename MementoStateT>
			//class Originator final : public IOriginator<Originator<MementoStateT>, MementoStateT> {
			class Originator final : public IOriginator<Originator, State> {
			public:
				//using MementoType = Memento<Originator<MementoStateT>, MementoStateT>;
				using MementoType = Memento<Originator, State>;
				using MementoPtr = std::unique_ptr<MementoType>;

				Originator() = default;
				Originator(const Originator&) = default;
				Originator& operator=(const Originator&) = default;
				Originator(Originator&&) noexcept = default;
				Originator& operator=(Originator&&) noexcept = default;


				MementoPtr CreateMemento() const override {
					//return std::make_unique<MementoType>(*this);
					return MementoPtr();
					//return std::make_unique<MementoType>(originator_state_);
				};

				/**
				 * Restore State of Originator from moved pointer to Memento.
				 * Previous unique pointer will be empty.
				 */
				bool Restore(MementoPtr&& memento_p) override {
					//if (Originator* memento_state_ptr{ memento_p->state_ptr(*this) }) {
					if (State* memento_state_ptr{ memento_p->state_ptr(*this) }) {
						//*this = std::move(*memento_state_ptr);
						originator_state_ = std::move(*memento_state_ptr);
						return true;
					} else {
						return false;
					}
				};

				// Accessors & Mutators
				inline const State& get_state() const noexcept { return originator_state_; };
				inline void set_state(const State& state_p) noexcept { originator_state_ = state_p; };

			protected:
				//inline const MementoStateT& GetStateForMemento() const noexcept override { return *this; };
				//inline const State& GetStateForMemento() const noexcept override { return originator_state_; };

			private:
				State originator_state_{};
			};

		} // !namespace memento_old_code

	} // !namespace behavioral
} // !namespace pattern

#endif // !MEMENTO_HPP
