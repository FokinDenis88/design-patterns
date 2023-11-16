#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <forward_list>
#include <functional>
#include <memory>


/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace observer_ref {
			// https://en.wikipedia.org/wiki/Observer_pattern
			// Classes ObserverRef and SubjectRef depends on each other by state

			/**
			 * If too many subjects are observed by little amount of observers you can use hash table
			 * to economy space, making access time worser.
			 * If there are many subject, that observer must be subscribed. Needs to add Subject& param to Observer.Update()
			 */


//==================================Interfaces=====================================

			/** Abstract. */
			template<typename StateType>
			class IObserver {
			protected:
				IObserver() = default;
				IObserver(const IObserver&) = delete; // C.67	C.21
				IObserver& operator=(const IObserver&) = delete;
				IObserver(IObserver&&) noexcept = delete;
				IObserver& operator=(IObserver&&) noexcept = delete;
			public:
				virtual ~IObserver() = default;

				/**
				 * Update the information about observer_state_ of observable object.
				 * There is Push and Pull Variants of realization. SubjectRef push aspect information of what
				 * is changing. Push is less decoupled.
				 */
				virtual void Update(const StateType& observable_state) = 0;
			};

			template<typename StateType>
			class ISubjectExtended;

			/** Abstract. */
			template<typename StateType>
			class IObserverExtended : public IObserver<StateType> {
			public:
				~IObserverExtended() override = default;

				/** Change the subject_ref of which observer_state_ we will be notified */
				virtual void SetObservable(ISubjectExtended<StateType>& new_observable) = 0;
			};


			/** Abstract. */
			template<typename StateType>
			class ISubject {
			protected:
				ISubject() = default;
				ISubject(const ISubject&) = delete; // C.67	C.21
				ISubject& operator=(const ISubject&) = delete;
				ISubject(ISubject&&) noexcept = delete;
				ISubject& operator=(ISubject&&) noexcept = delete;
			public:
				virtual ~ISubject() = default;

				/** Add ObserverRef to list of notification */
				virtual void AttachObserver(IObserver<StateType>& observer) = 0;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserver(IObserver<StateType>& observer) = 0;

				/** Update all attached observers */
				virtual void Notify() const = 0;
			};

			/** Abstract. */
			template<typename StateType>
			class ISubjectExtended : public ISubject<StateType> {
			public:
				~ISubjectExtended() override = default;

				/** Detach all attached observers. */
				virtual void ClearAllObservers() = 0;
			};


//======================================Ref Version=====================================
// Ref Version can be used with stack objects


			template<typename StateType>
			class ObserverRef : public IObserver<StateType> {
			public:
				/** Update the information about observer_state_ of observable object */
				inline void Update(const StateType& observable_state) noexcept override {
					observer_state_ = observable_state;
				};

			private:
				StateType observer_state_{};
			};

			/**
			 * Observers will be notified on SubjectRef state changes.
			 * Ref Version can be used with stack objects
			 */
			template<typename StateType>
			class SubjectRef : public ISubject<StateType> {
			public:
				using IObserverRef = std::reference_wrapper<IObserver<StateType>>;

				/** Add ObserverRef to list of notification */
				inline void AttachObserver(IObserver<StateType>& observer) override {
					notifying_list_.emplace_front(std::ref(observer));
				};

				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserver(IObserver<StateType>& observer) override {
					notifying_list_.remove_if([&observer](const IObserverRef& current) {
														return &current.get() == &observer;
														});
				};

				/** Update all attached observers */
				inline void Notify() const override {
					for (auto& observer : notifying_list_) {
						observer.get().Update(observable_state_);
					}
				};

				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept {
					notifying_list_.clear();
				};

				inline void set_state(const StateType& new_state) noexcept {
					observable_state_ = new_state;
					//Notify();
				}

			private:
				/**
				 * List of observers, that will be attach to observable object.
				 * SubjectRef is not interested in owning of its Observers.
				 * So can be used weak_ptr, created from shared_ptr.
				 * Also can be used ref IObserver&.
				 * Containers can't hold IObserver&. Only wrapper.
				 */
				std::forward_list<IObserverRef> notifying_list_{};
				// TODO: Maybe unordered_set ???

				/** The observable_state_ of SubjectRef object */
				StateType observable_state_{};
			};


			struct State {
				int a_{ 0 };
				int b_{ 0 };
			};

		} // !namespace observer_ref_1



		namespace observer_smart_ptr {
//==================================Interfaces=====================================

			struct State {
				int a_{ 0 };
				int b_{ 0 };
			};


			/** Abstract. */
			class IObserver {
			public:
				virtual ~IObserver() = default;

				/**
				 * Update the information about observer_state_ of observable object.
				 * There is Push and Pull Variants of realization. SubjectRef push aspect information of what
				 * is changing. Push is less decoupled.
				 */
				virtual void Update(const State& observable_state) = 0;
			};

			class ISubject;

			/** Abstract. */
			class IObserverExtended : public IObserver {
			public:
				~IObserverExtended() override = default;

				/** Change the subject_ref of which observer_state_ we will be notified */
				virtual void SetObservable(std::shared_ptr<ISubject>& new_observable) = 0;
			};


			/** Abstract. */
			class ISubject {
			public:
				virtual ~ISubject() = default;

				/** Add ObserverRef to list of notification */
				virtual void AttachObserver(std::shared_ptr<IObserver>& observer) = 0;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserver(std::shared_ptr<IObserver>& observer) = 0;

				/** Update all attached observers */
				virtual void Notify() const = 0;
			};

			// Can be realized by templating ISubject Interface
			/** Abstract. */
			class ISubjectExtended : public ISubject {
			public:
				~ISubjectExtended() override = default;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserverByRef(IObserver& observer) = 0;

				/** Detach all attached observers. */
				virtual void ClearAllObservers() = 0;
			};

//================================Simple Version=====================================
// Pointers version can be used with only dynamically allocated objects

			class Subject;

			class Observer : public IObserver {
			public:
				/** Update the information about observer_state_ of observable object */
				inline void Update(const State& observable_state) noexcept override {
					observer_state_ = observable_state;
				};

			private:
				State observer_state_{};
			};

			/** Observers will be notified on SubjectRef state changes */
			class Subject : public ISubject {
			public:
				/** Add ObserverRef to list of notification */
				inline void AttachObserver(std::shared_ptr<IObserver>& observer) override {
					notifying_list_.emplace_front(std::weak_ptr<IObserver>(observer));
				};

				/** Detach observer from notifying list */
				inline void DetachObserver(std::shared_ptr<IObserver>& observer) override {
					notifying_list_.remove_if([&observer](const std::weak_ptr<IObserver>& current) {
													return current.lock().get() == observer.get();
													});
				};

				/** Update all attached observers */
				inline void Notify() const override {
					for (auto& observer : notifying_list_) {
						observer.lock()->Update(observable_state_);
					}
				};

				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept {
					notifying_list_.clear();
				};

				inline void set_state(const State& new_state) noexcept {
					observable_state_ = new_state;
					//Notify();
				}

			private:
				/**
				 * List of observers, that will be attach to observable object.
				 * SubjectRef is not interested in owning of its Observers.
				 * So can be used weak_ptr, created from shared_ptr.
				 * Also can be used ref IObserver&.
				 */
				std::forward_list<std::weak_ptr<IObserver>> notifying_list_{};

				/** The observable_state_ of SubjectRef object */
				State observable_state_{};
			};


			//===================================Extended Version=====================================
#ifdef DEBUG
			class SubjectExtended;

			/** Extended version */
			class ObserverExtended : public IObserverExtended {
			public:
				/*explicit Observer(const std::shared_ptr<ISubject>& new_observable)
					: observable_{ new_observable } {
				}
				Observer(const Observer&) = delete
				;
				Observer& operator=(const Observer&) = delete;*/

				/** Update the information about observer_state_ of observable object */
				inline void Update(const State& observable_state) noexcept override {
					observer_state_ = observable_state;
				};


				/** Change the subject_ref of which observer_state_ we will be notified */
				inline void SetObservable(std::shared_ptr<ISubject>& new_observable) override {
					if (auto ptr{ observable_.lock() }) {
						ptr->DetachObserverByRef(*this);
					}
					observable_ = std::weak_ptr<ISubject>(new_observable);
				};

			private:
				/** Delegator, whose observer_state_ we would be notified */
				std::weak_ptr<ISubject> observable_{};

				State observer_state_{};
			};

			/** Observers will be notified on Subject state changes */
			class SubjectExtended : public ISubjectExtended {
			public:
				/*explicit SubjectExtended(const std::shared_ptr<IObserver>& new_observer) :
				{
					notifying_list_.push_front(std::make_shared<IObserver>(new_observer));
				}
				SubjectExtended(const SubjectExtended&) = delete;
				SubjectExtended& operator=(const SubjectExtended&) = delete;*/


				/** Add ObserverRef to list of notification */
				inline void AttachObserver(std::shared_ptr<IObserver>& observer) override {
					notifying_list_.emplace_front(std::weak_ptr<IObserver>(observer));
				};

				/** Is used for Setting new observable in observer */
				inline void DetachObserverByRef(IObserver& observer) {
					notifying_list_.remove_if([&observer](const std::weak_ptr<IObserver>& current) {
															return current.lock().get() == &observer;
															});
				};

				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserver(std::shared_ptr<IObserver>& observer) override {
					DetachObserverByRef(*observer.get());
				};

				/** Update all attached observers */
				inline void Notify() const override {
					for (auto& observer : notifying_list_) {
						observer.lock()->Update(observable_state_);
					}
				};

				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept override {
					notifying_list_.clear();
				};

				inline void set_state(const State& new_state) noexcept {
					observable_state_ = new_state;
				}

			private:
				/**
				 * List of observers, that will be attach to observable object.
				 * SubjectRef is not interested in owning of its Observers. So can be used weak_ptr
				 */
				std::forward_list<std::weak_ptr<IObserver>> notifying_list_{};

				/** The observable_state_ of SubjectRef object */
				State observable_state_{};
			};
#endif // DEBUG

		} // !namespace observer_smart_ptr



		// Common to first Ref realization
		namespace observer_wiki_version { // Realization: https://en.wikipedia.org/wiki/Observer_pattern

			class Subject; //Forward declaration for usage in Observer

			class Observer {
			public:
				Observer(const Observer&) = delete;	// rule of three
				Observer& operator=(const Observer&) = delete;

				explicit Observer(Subject& subj);
				//virtual ~Observer() = default;
				virtual ~Observer();

				virtual void update(Subject& s) const = 0;

			private:
				// Rerence to a Subject object to detach in the destructor
				Subject& subject;
			};

			// Example of usage
			class ConcreteObserver : public Observer {
			public:
				explicit ConcreteObserver(Subject& subj) noexcept : Observer{ subj } {
				}

				// Get notification
				void update(Subject&) const override {
					//std::cout << "Got a notification" << std::endl;
				}
			};


			// Subject is the base class for event generation
			class Subject {
			public:
				using RefObserver = std::reference_wrapper<const Observer>;

				// Notify all the attached obsevers
				void notify() {
					for (const auto& x : observers)
					{
						x.get().update(*this);
					}
				}

				// Add an observer_ref_1
				void attach(const Observer& observer) {
					observers.push_front(observer);
				}

				// Remove an observer_ref_1
				void detach(Observer& observer) {
					observers.remove_if([&observer](const RefObserver& obj)
						{
							return &obj.get() == &observer;
						});
				}

			private:
				std::forward_list<RefObserver> observers;
			};

		} // namespace observer_wiki_version


	} // !namespace behavioral
} // !namespace pattern

#endif // !OBSERVER_HPP
