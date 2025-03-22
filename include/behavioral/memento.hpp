#ifndef MEMENTO_HPP
#define MEMENTO_HPP


#include <concepts>
#include <memory>
#include <utility>


/** Software Design Patterns */
namespace pattern {
	namespace behavioral {

		namespace memento {
			// https://en.wikipedia.org/wiki/Memento_pattern
			// Friend patterns: Command - put state information, needed for cancel execution in memento.
			//					Iterator - to execute iterations.
			//
			// Purpose: to restore an object to its previous state (undo via rollback), another is versioning,
			// another is custom serialization.
			//
			// If Originator is large in size, holds non-GC resources, is a Singleton, or is otherwise
			// non-copyable, memento allows to save multiple different states of Originator without
			// affecting Originator or its clients.
			// When using this pattern, care should be taken if the originator may change other objects
			// or resources—the memento pattern operates on a single object.
			//
			// Use Memento Class and AbstractOriginator.


            template<typename ConcreteOriginatorT, typename OriginatorStateT>
			class AbstractOriginator;

			/**
			 * Can be used only by class derived from AbstractOriginator and only by owner object, created memento.
			 * Memento must use only Concrete Originator, not abstract interface.
			 *
			 * @param ConcreteOriginatorT Concrete Originator type
			 */
			template<typename ConcreteOriginatorT, typename OriginatorStateT>
			class Memento {
			public:	// Public Interface of Memento
				using MementoType = Memento<ConcreteOriginatorT, OriginatorStateT>;

				friend ConcreteOriginatorT;
				friend AbstractOriginator<ConcreteOriginatorT, OriginatorStateT>;

				Memento() = delete;	// Must be created only by Concrete Originator
				Memento(const Memento&) = delete;	// Polymorphic suppress Copy & Move
				Memento& operator=(const Memento&) = delete;
				Memento(Memento&&) noexcept = delete;
				Memento& operator=(Memento&&) noexcept = delete;
				virtual ~Memento() = default;

				// TODO: To minimize copy, move operations while creating memento from Originator in which
				// OriginatorStateT consists of part of data member maybe created struct of reference to interested data members.
				// Can be hold by std::list<std::ref>

				// There is two Variants of creation memento. One is to create memento from
				// whole OriginatorStateT object inside of Originator.
                // Another is to create memento from Separated data members in Originator.

				/**
				* Two params let ConcreteOriginatorT to create memento with different state, different part of Originator.
				* To use when there is whole OriginatorStateT object inside of Originator.
				*
				* Design: give oppurtunity to save different versions of Originator state by choosing param owners_state_p.
				*/
				explicit Memento(const ConcreteOriginatorT& owner_p, const OriginatorStateT& owners_state_p) noexcept
					: owner_{ owner_p }, memento_state_{ owners_state_p } {
				};

                /**
                * Two params let ConcreteOriginatorT to create memento with different state, different part of Originator.
                * To use when there is whole OriginatorStateT object inside of Originator.
                *
                * @param owner_p ConcreteOriginatorT object, that created memento.
                * @param owners_state_p temporary rvalue OriginatorStateT object.
                *
                * Design: give oppurtunity to save different versions of Originator state by choosing param owners_state_p.
                */
                explicit Memento(const ConcreteOriginatorT& owner_p, OriginatorStateT&& owners_state_p) noexcept
                    : owner_{ owner_p }, memento_state_{ std::move(owners_state_p) } {
                };

				/** Clone memento and return smart ptr */
				std::unique_ptr<MementoType> ClonePtr() const noexcept {
					return std::make_unique<MementoType>(owner_, memento_state_);
				};

				/** Clone memento and return rvalue MementoType */
                MementoType CloneValue() const noexcept {
					return MementoType{ owner_, memento_state_ };
                };

			private:	// Private Interface is closed for all classes except owner ConcreteOriginatorT

				/**
				 * Used Only by OriginatorStateT.
				 * You can disable owner check.
				 */
				inline bool set_state(const ConcreteOriginatorT& caller, const OriginatorStateT& state_p,
									  bool to_check_owner = true) noexcept {
					if (CanWorkWithState(caller, to_check_owner)) {
						memento_state_ = state_p;
						return true;
					}
					return false;
				};

				/**
				 * Used Only by Originator, that has created memento and can restore it get_state_cref.
				 * Can be used only by class derived from IOriginator and only by owner, created memento.
				 * You can disable owner check.
				 */
                inline std::pair<const OriginatorStateT&, bool> get_state_cref(const ConcreteOriginatorT& caller,
                														  bool to_check_owner = true) const noexcept { // Mustn't be const
					return std::pair<const OriginatorStateT&, bool>(memento_state_,
																	CanWorkWithState(caller, to_check_owner));
                };

				/**
				 * Used Only by Originator, that has created memento and can restore it get_state_cref.
				 * Can be used only by class derived from IOriginator and only by owner, created memento.
				 * You can disable owner check.
				 * May be changed after moving memento.
				 */
                inline std::pair<OriginatorStateT&&, bool> get_state_rvref(const ConcreteOriginatorT& caller,
																			bool to_check_owner = true) noexcept { // Mustn't be const
					return std::pair<OriginatorStateT&&, bool>(std::move(memento_state_),
																CanWorkWithState(caller, to_check_owner));
                };


				/** Checks if caller is owner */
				constexpr bool IsOwner(const ConcreteOriginatorT& caller) const noexcept {
					return &caller == &owner_;
				};

				/** Checks if caller can use memento. Return true if originator is the owner of memento. */
				constexpr bool CanWorkWithState(const ConcreteOriginatorT& caller, bool to_check_owner) const noexcept {
					return to_check_owner && IsOwner(caller) || !to_check_owner;
				}


// Data
				/** Only owner can use memento to restore get_state_cref */
				const ConcreteOriginatorT& owner_;

				/**
				 * The State of concrete Originator object in some period of time.
				 * From this state originator object will be restored.
				 * Can be different with current state of the owner.
				 * May be part of Originators whole state.
				 */
				OriginatorStateT memento_state_{};	// Is Non Const, cause move operation, when restore.

			}; // !class Memento


			/**
			 * Abstract. Stateless. AbstractOriginator class let to create Memento
			 * and to Restore State of Originator from Memento.
			 * Is used for public inheritance to ConcreteOriginator class.
			 *
			 * How to use: override in ConcreteOriginator functions: SetOriginatorState() and GetOriginatorStateValue().
			 *
			 * @param ConcreteOriginatorT Concrete Originator type
			 */
			template<typename ConcreteOriginatorT, typename OriginatorStateT>
			class AbstractOriginator {
			public:
				using MementoType = Memento<ConcreteOriginatorT, OriginatorStateT>;
				using MementoPtr = std::unique_ptr<MementoType>;

			protected:
				AbstractOriginator() = default;
                AbstractOriginator(const AbstractOriginator&) = delete; // Polymorphic suppress Copy, Move
				AbstractOriginator& operator=(const AbstractOriginator&) = delete;
				AbstractOriginator(AbstractOriginator&&) noexcept = delete;
				AbstractOriginator& operator=(AbstractOriginator&&) noexcept = delete;
			public:
				virtual ~AbstractOriginator() = default;

				/**
				 * Create Memento from State of Originator.
				 * Use, when there is no whole OriginatorStateT object inside of Originator.
				 * Creating memento by moving from temporary OriginatorStateT object.
				 * Default Creation is by CreateMementoByValue.
				 */
                MementoType CreateMemento() const { return CreateMementoByValue(); };

                /**
                 * Create Memento from State of Originator.
                 * Use, when there is no whole OriginatorStateT object inside of Originator.
                 * Creating memento by moving from temporary OriginatorStateT object.
                 * Additional temporary object OriginatorStateT construction.
                 */
				MementoType CreateMementoByValue() const {
                    return MementoType(GetConcreteOriginatorCRef(), std::move(GetOriginatorStateValue()));
                };

				/**
				 * Create unique_ptr Memento from State of Originator.
				 * Use, when there is no whole OriginatorStateT object inside of Originator.
				 * Creating memento by moving from temporary OriginatorStateT object.
				 * Additional temporary object OriginatorStateT construction.
				 */
				MementoPtr CreateMementoPtrByValue() const {
					return std::make_unique<MementoType>(GetConcreteOriginatorCRef(), std::move(GetOriginatorStateValue()));
				};


				/**
				 * Create Memento from State of Originator.
				 * Use, when there is whole OriginatorStateT object inside of Originator.
				 * Creating memento by coping from const reference to OriginatorStateT object inside Originator.
				 * No Additional temporary object OriginatorStateT construction.
				 */
				MementoType CreateMementoByCRef() const {
					return MementoType(GetConcreteOriginatorCRef(), GetOriginatorStateCRef());
				};

				/**
				 * Create unique_ptr Memento from State of Originator.
				 * Use, when there is whole OriginatorStateT object inside of Originator.
				 * Creating memento by moving from temporary OriginatorStateT object.
				 * No Additional temporary object OriginatorStateT construction.
				 */
				MementoPtr CreateMementoPtrByCRef() const {
					return std::make_unique<MementoType>(GetConcreteOriginatorCRef(), GetOriginatorStateCRef());
				};


				/**
				 * Restore State of Originator from Memento using copy operation.
				 * You can disable owner check.
				 *
				 * @param memento_p moved pointer to Memento
				 * @return true if originator is the owner of memento
				 */
				bool RestoreByCopy(const MementoType& memento_p, bool to_check_owner = true) {
					const std::pair<const OriginatorStateT&, bool>
						get_state_res{ memento_p.get_state_cref(GetConcreteOriginatorCRef(),
																to_check_owner) };

					if (get_state_res.second) {
						SetOriginatorState(get_state_res.first);
						return true;
					}
					return false;
				};

				/**
				 * Restore State of Originator from moved state of Memento.
				 * You can disable owner check.
				 *
				 * @param memento_p moved pointer to Memento
				 * @return true if originator is the owner of memento
				 */
				bool RestoreByMove(MementoType&& memento_p, bool to_check_owner = true) {
					std::pair<OriginatorStateT&&, bool>
						get_state_res{ memento_p.get_state_rvref(GetConcreteOriginatorRef(),																								to_check_owner) };

                    if (get_state_res.second) {
						SetOriginatorState(std::move(get_state_res.first));
                        return true;
                    }
                    return false;
				};

			protected:
				/**
				 * Used for restoring State of Concrete Originator from Memento by operation Copy.
				 * Will save the state of memento.
				 */
				virtual void SetOriginatorState(const OriginatorStateT& memento_state_p) = 0;

				/**
				 * Used for restoring State of Concrete Originator from Memento by operation Move.
				 * The state of memento be will changed.
				 * May be overriden in derived classes. Stub realization.
				 */
				virtual void SetOriginatorState(OriginatorStateT&& memento_state_p) {
					SetOriginatorState(static_cast<const OriginatorStateT&>(memento_state_p));	// Stub
				};

                /**
                 * Get the State of Originator.
                 * Use if there is no whole OriginatorStateT object in data member of originator.
                 * OriginatorStateT is created from different data member of originator. Part of originator.
                 *
                 * @return temporary object state of Originator. Ready to be moved.
                 */
                virtual OriginatorStateT GetOriginatorStateValue() const = 0;

                /**
                 * Get the State of Originator.
                 * Use if there is whole OriginatorStateT object in data member of originator.
                 * May be overriden in derived classes. Stub realization.
                 *
                 * @return state of part of Originator.
                 */
				virtual const OriginatorStateT& GetOriginatorStateCRef() const {
					return GetOriginatorStateValue();	// Stub
				};

			private:
				/** Donwcast. Support func. Get const reference to Concrete Originator from pointer this AbstractOriginator* */
				inline const ConcreteOriginatorT& GetConcreteOriginatorCRef() const noexcept {
					return *dynamic_cast<const ConcreteOriginatorT*>(this);
				};
				/** Donwcast. Support func. Get reference to Concrete Originator from pointer this AbstractOriginator* */
                inline ConcreteOriginatorT& GetConcreteOriginatorRef() noexcept {
                    return *dynamic_cast<ConcreteOriginatorT*>(this);
                };

			}; // !class AbstractOriginator



            /** Concrete State as interesting part of Originator. Part or whole object. */
            struct MyMementoState {
                MyMementoState() = default;
                MyMementoState(const MyMementoState&) = default;
                MyMementoState(MyMementoState&&) noexcept = default;
                MyMementoState& operator=(const MyMementoState&) = default;
				MyMementoState& operator=(MyMementoState&& rhs) noexcept {
					a = std::exchange(rhs.a, 0);
					b = std::exchange(rhs.b, 0);
					return *this;
				};
                ~MyMementoState() = default;

                int a{};
                int b{};
            };


			class MyOriginator : public AbstractOriginator<MyOriginator, MyMementoState> {
			public:
				MyOriginator() = default;
				MyOriginator(const MyOriginator&) = delete; // Polymorphic suppress Copy, Move
				MyOriginator& operator=(const MyOriginator&) = delete;
				MyOriginator(MyOriginator&&) noexcept = delete;
				MyOriginator& operator=(MyOriginator&&) noexcept = delete;
				~MyOriginator() override = default;

			protected:
				void SetOriginatorState(const MyMementoState& memento_state_p) override {
					state_ = memento_state_p;
                };

                void SetOriginatorState(MyMementoState&& memento_state_p) override {
					state_ = std::move(memento_state_p);
                };

				MyMementoState GetOriginatorStateValue() const override {
					return state_;
                };

                const MyMementoState& GetOriginatorStateCRef() const override {
                    return state_;
                };

			public:
				MyMementoState state_{};
				int c{};
			};


			template<typename ConcreteOriginatorT, typename OriginatorStateT>
			class CareTaker final {
			public:
				using MementoType = Memento<ConcreteOriginatorT, OriginatorStateT>;
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


			/**
			 * Abstract. Stateless. Interface for class, whose state will be saved and restored.
			 * Memento must use only Concrete Originator, not interface.
			 *
			 * @param OriginatorStateT must be state struct or concrete Originator class derived from IOriginator.
			 * Just for example.
			 */
			template<typename ConcreteOriginatorT, typename OriginatorStateT>
			class IOriginator {
			public:
				using MementoType = Memento<ConcreteOriginatorT, OriginatorStateT>;
				using MementoPtr = std::unique_ptr<MementoType>;

			protected:
				IOriginator() = default;
				IOriginator(const IOriginator&) = delete; // C++ Core Guidelines C.67	C.21
				IOriginator& operator=(const IOriginator&) = delete;
				IOriginator(IOriginator&&) noexcept = delete;
				IOriginator& operator=(IOriginator&&) noexcept = delete;
			public:
				virtual ~IOriginator() = default;

				/** Creates Memento, that will hold Originator's state */
				virtual MementoPtr CreateMementoPtr() const = 0;

				/**
				 * Restore State of Originator from moved pointer to Memento.
				 *
				 * @return indicator of restoration success.
				 */
				virtual bool Restore(MementoPtr&& memento_p) = 0;

				//protected:
					/**
					 * For memento initialization needed to get originator's state.
					 * State can be the whole object or it's part. Depends on the interest.
					 */
					 //virtual const OriginatorStateT& GetStateForMemento() const noexcept = 0;
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
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, OriginatorStateT>>
			class Memento;


			/**
			 * Abstract. Stateless. Interface for class, whose state will be saved and restored.
			 * Memento must use only Concrete Originator, not interface.
			 *
			 * @param OriginatorStateT must be state struct or concrete Originator class derived from IOriginator.
			 */
			template<typename ConcreteOriginatorT, typename StateType>
			class IOriginator {
			public:
				using MementoType = Memento<ConcreteOriginatorT, StateType>;
				using MementoPtr = std::unique_ptr<MementoType>;
				//using IOriginatorType = IOriginator<OriginatorStateT>;

			protected:
				IOriginator() = default;
				IOriginator(const IOriginator&) = delete; // C.67	C.21
				IOriginator& operator=(const IOriginator&) = delete;
				IOriginator(IOriginator&&) noexcept = delete;
				IOriginator& operator=(IOriginator&&) noexcept = delete;
			public:
				virtual ~IOriginator() = default;

				virtual MementoPtr CreateMementoPtr() const = 0;
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
			template<typename ConcreteOriginatorT, typename StateType>
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, OriginatorStateT>>
			class Memento final {
			public:
				//using IOriginatorType = IOriginator<ConcreteOriginatorT, OriginatorStateT>;
				//friend IOriginatorType;
				//friend class IOriginator<ConcreteOriginatorT, OriginatorStateT>;
				//friend IOriginator<ConcreteOriginatorT, OriginatorStateT>;
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
							: owner_{ owner_p }, memento_state_{ owners_state_p } {
				};

			private:	// Interface is closed for all classes except owner IOriginator<ConcreteOriginatorT, OriginatorStateT>

				constexpr bool IsOwner(const ConcreteOriginatorT& caller) const noexcept {
					return &caller == &owner_;
				};

				/** Used Only by OriginatorStateT. */
				void set_state(const ConcreteOriginatorT& caller, const StateType& state_p) noexcept {
					if (IsOwner(caller)) {
						memento_state_ = state_p;
					}
				};

				/**
				 * Used Only by Originator, that has created memento can restore it get_state_cref.
				 * Can be used only by class derived from IOriginator and only by
				 * owner, created memento.
				 * May be nullptr.
				 */
				inline StateType* get_state_cref(const ConcreteOriginatorT& caller) noexcept {
					return (IsOwner(caller)) ? &memento_state_ : nullptr;
				};


				/** Only owner can use memento to restore get_state_cref */
				const ConcreteOriginatorT& owner_;

				/**
				 * State of concrete Originator object.
				 * From this state originator object will be restored.
				 */
				StateType memento_state_{};	// Is Non Const, cause move operation, when restore.

			}; // !class Memento


			template<typename ConcreteOriginatorT, typename StateType>
			//requires std::derived_from<ConcreteOriginatorT, IOriginator<ConcreteOriginatorT, OriginatorStateT>>
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

			//template<typename OriginatorStateT>
			//class Originator final : public IOriginator<Originator<OriginatorStateT>, OriginatorStateT> {
			class Originator final : public IOriginator<Originator, State> {
			public:
				//using MementoType = Memento<Originator<OriginatorStateT>, OriginatorStateT>;
				using MementoType = Memento<Originator, State>;
				using MementoPtr = std::unique_ptr<MementoType>;

				Originator() = default;
				Originator(const Originator&) = delete;
				Originator& operator=(const Originator&) = delete;
				Originator(Originator&&) noexcept = delete;
				Originator& operator=(Originator&&) noexcept = delete;
				~Originator() override = default;

				MementoPtr CreateMementoPtr() const override {
					//return std::make_unique<MementoType>(*this);
					return MementoPtr();
					//return std::make_unique<MementoType>(memento_state_);
				};

				/**
				 * Restore State of Originator from moved pointer to Memento.
				 * Previous unique pointer will be empty.
				 */
				bool Restore(MementoPtr&& memento_p) override {
					//if (Originator* memento_state_ptr{ memento_p->get_state_cref(*this) }) {
					if (State* memento_state_ptr{ memento_p->get_state_cref(*this) }) {
						//*this = std::move(*memento_state_ptr);
						memento_state_ = std::move(*memento_state_ptr);
						return true;
					} else {
						return false;
					}
				};

				// Accessors & Mutators
				inline const State& get_state_cref() const noexcept { return memento_state_; };
				inline void set_state(const State& state_p) noexcept { memento_state_ = state_p; };

			protected:
				//inline const OriginatorStateT& GetStateForMemento() const noexcept override { return *this; };
				//inline const State& GetStateForMemento() const noexcept override { return memento_state_; };

			private:
				State memento_state_{};
			};

		} // !namespace memento_old_code

	} // !namespace behavioral
} // !namespace pattern

#endif // !MEMENTO_HPP
