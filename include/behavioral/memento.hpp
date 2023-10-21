#ifndef MEMENTO_HPP
#define MEMENTO_HPP

#include <memory>
#include <utility>
#include <concepts>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace memento_simple {
			// https://en.wikipedia.org/wiki/Memento_pattern


			struct State {
				int a{};
				int b{};
			};

			class Memento;

			/** Abstract. Forward declaring for Memento. Memento and Originator can be only concrete classes. */
			class IOriginator {
			public:
				virtual ~IOriginator() = default;

				virtual std::unique_ptr<Memento> CreateMemento() const = 0;
				virtual bool Restore(std::unique_ptr<Memento>&& memento_p) = 0;

				/** For memento initialization */
				virtual const State& state() const noexcept = 0;
			};

			class Originator;

			/** Can be used only by class derived from IOriginator and only by owner, created memento. */
			class Memento final {
			public:
				friend Originator;

				Memento() = delete;
				Memento(const Memento&) = delete;
				Memento& operator=(const Memento&) = delete;

				explicit Memento(const IOriginator& owner_p) noexcept
					: owner_{ owner_p }, observer_state_{ owner_p.state() } {
				};

			private:
				/** Only owner can use memento to restore state_ptr */
				const IOriginator& owner_;

				inline bool IsOwner(const IOriginator& caller) const noexcept {
					return &caller == &owner_;
				};

				void set_state(const IOriginator& caller, const State& state_p) noexcept {
					if (IsOwner(caller)) {
						observer_state_ = state_p;
					}
				};

				/**
				 * Only Originator, that has created memento can restore it state_ptr.
				 * Can be used only by class derived from IOriginator and only by
				 * owner, created memento.
				 */
				inline State* state_ptr(const IOriginator& caller) noexcept {
					return (IsOwner(caller)) ? &observer_state_ : nullptr;
				};

				State observer_state_{};
			};

			class Originator final : public IOriginator {
			public:
				std::unique_ptr<Memento> CreateMemento() const override {
					return std::make_unique<Memento>(*this);
				};

				/**
				 * Restore State of Originator from moved pointer to Memento.
				 * Previous unique pointer will be empty.
				 */
				bool Restore(std::unique_ptr<Memento>&& memento_p) override {
					if (State* memento_state_ptr{ memento_p->state_ptr(*this) }) {
						observer_state_ = std::move(*memento_state_ptr);
						return true;
					} else {
						return false;
					}
				};

				inline const State& state() const noexcept override { return observer_state_; };
				inline void set_state(const State& state_p) noexcept { observer_state_ = state_p; };

			private:
				State observer_state_{};
			};

			class CareTaker final {
			public:
				inline void set_memento(std::unique_ptr<Memento>&& memento_p) noexcept {
					memento_ = std::move(memento_p);
				};

				inline std::unique_ptr<Memento> memento() noexcept {
					return std::move(memento_);
				}

			private:
				std::unique_ptr<Memento> memento_;
			};

			inline void Run() {
				CareTaker care_take{};
				Originator originator{};
				care_take.set_memento(originator.CreateMemento());
				const State new_state{ 99, 99 };
				originator.set_state(new_state);
				originator.Restore(care_take.memento());
			};

		} // !namespace memento_simple


//#ifdef FULL_MEMENTO
		namespace memento_templated {
			// https://en.wikipedia.org/wiki/Memento_pattern


			struct State {
				int a{};
				int b{};
			};

			/** Can be used only by class derived from IOriginator and only by owner, created memento. */
			template<typename OriginatorType>
			class Memento final {
			public:
				using StateType = typename OriginatorType::StateType;

				friend OriginatorType;

				Memento() = delete;
				Memento(const Memento&) = delete;
				Memento& operator=(const Memento&) = delete;

				explicit Memento(const OriginatorType& owner_p) noexcept
					: owner_{ owner_p }, observer_state_{ owner_p.state() } {
				};

			private:
				/** Only owner can use memento to restore state_ptr */
				const OriginatorType& owner_;

				inline bool IsOwner(const OriginatorType& caller) const noexcept {
					return &caller == &owner_;
				};

				void set_state(const OriginatorType& caller, const StateType& state_p) noexcept {
					if (IsOwner(caller)) {
						observer_state_ = state_p;
					}
				};

				/**
				 * Only Originator, that has created memento can restore it state_ptr.
				 * Can be used only by class derived from IOriginator and only by
				 * owner, created memento.
				 */
				inline StateType* state_ptr(const OriginatorType& caller) noexcept {
					return (IsOwner(caller)) ? &observer_state_ : nullptr;
				};

				StateType observer_state_{};
			};

			template<typename TemplateStateType>
			class Originator final {
			public:
				using StateType = TemplateStateType;
				using MementoType = Memento<Originator>;

				std::unique_ptr<MementoType> CreateMemento() const {
					return std::make_unique<MementoType>(*this);
				};

				/**
				 * Restore State of Originator from moved pointer to Memento.
				 * Previous unique pointer will be empty.
				 */
				bool Restore(std::unique_ptr<MementoType>&& memento_p) {
					if (StateType * memento_state_ptr{ memento_p->state_ptr(*this) }) {
						observer_state_ = std::move(*memento_state_ptr);
						return true;
					}
					else {
						return false;
					}
				};

				inline const StateType& state() const noexcept { return observer_state_; };
				inline void set_state(const StateType& state_p) noexcept { observer_state_ = state_p; };

			private:
				StateType observer_state_{};
			};

			template<typename OriginatorType>
			class CareTaker final {
			public:
				using MementoType = Memento<OriginatorType>;

				inline void set_memento(std::unique_ptr<MementoType>&& memento_p) noexcept {
					memento_ = std::move(memento_p);
				};

				inline std::unique_ptr<MementoType> memento() noexcept {
					return std::move(memento_);
				}

			private:
				std::unique_ptr<MementoType> memento_;
			};

			inline void Run() {
				using MyOriginatorType = Originator<State>;

				CareTaker<MyOriginatorType> care_take{};
				Originator<State> originator{};
				care_take.set_memento(originator.CreateMemento());
				const State new_state{ 99, 99 };
				originator.set_state(new_state);
				originator.Restore(care_take.memento());

				int a = 66 - 33;
			};

		} // !namespace memento_templated
//#endif // FULL_MEMENTO


	} // !namespace behavioral
} // !namespace pattern

#endif // !MEMENTO_HPP
