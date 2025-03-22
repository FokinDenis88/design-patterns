#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <forward_list>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <unordered_set>

// TODO: maybe separate classes to different files to make less includes

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace observer_ref {
			// https://en.wikipedia.org/wiki/Observer_pattern
			// Friend patterns: Mediator. ChangeManager act like mediator between Observers and Subjects,
			// encapsulate complex mechanics of Update.
			// Singleton is used in ChangeManager.
			//
			// Classes Observer and Subject depends on each other by state

			/**
			 * If too many subjects are observed by little amount of observers you can use hash table
			 * to economy space, making access time worser.
			 * If there are many subject, that observer must be subscribed. Needs to add Subject& param to Observer.Update()
			 */

			/**
			 * Observer with "event propagation" or in realization of "Listener".
			 *
			 * Push, Pull protocols of update.
			 * Push is less decoupled. Subject knows what data is needed by observer.
			 * Pull is more decoupled. Subject knows nothing about observer. Observer decides what data is needed.
			 *
			 * Aspect defines what events are interested for observer.
			 * Realization by subscribing to concrete events.
			 * Second, by sending information about event when Updating
			 *
			 * One Observer may has many Subjects. One Subject can has many Observers.
			 */


//==================================Interfaces=====================================

			/**
			 * Abstract. Stateless. Interface of Observer.
			 * Alternative name - Subscriber.
			 */
			class IObserver {
			protected:
				IObserver() = default;	// Must be created with Subject reference inside
				IObserver(const IObserver&) = delete; // C.67	C.21
				IObserver& operator=(const IObserver&) = delete;
				IObserver(IObserver&&) noexcept = delete;
				IObserver& operator=(IObserver&&) noexcept = delete;
			public:
				virtual ~IObserver() = default;

                /** Update the information about Subject */
				virtual void Update() = 0;
			};

            template<typename ObserverT>
			class ISubject;

			/** Abstract. Stateless. Interface for Observer with multiple subjects */
			class IObserverMulti {
            public:
                using SubjectType = ISubject<IObserverMulti>;

			protected:
				IObserverMulti() = default;
				IObserverMulti(const IObserverMulti&) = delete; // C.67	C.21
				IObserverMulti& operator=(const IObserverMulti&) = delete;
				IObserverMulti(IObserverMulti&&) noexcept = delete;
				IObserverMulti& operator=(IObserverMulti&&) noexcept = delete;
			public:
				virtual ~IObserverMulti() = default;

                /** Update the information about Subject */
                virtual void Update() = 0;

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				virtual void Update(const SubjectType& subject) = 0;

				/**
				 * Detach Subject from Observer, when Subject is destructed..
				 *
				 * @param subject subject, that is destructing.
				 */
				virtual void DetachSubject(SubjectType& subject) = 0;

				/** Check if there is subject reference in Observer */
				virtual bool HasSubject(SubjectType& subject) const noexcept = 0;
			};

			/**
			 * Abstract. Stateless.
			 * Alternative name - Publisher.
			 * May be used for IObserver, IObserverMulti.
			 */
			template<typename ObserverT = IObserver>
			class ISubject {
			public:
				using ObserverType = IObserverMulti;

			protected:
				ISubject() = default;
				ISubject(const ISubject&) = delete; // C.67	C.21
				ISubject& operator=(const ISubject&) = delete;
				ISubject(ISubject&&) noexcept = delete;
				ISubject& operator=(ISubject&&) noexcept = delete;
			public:
				virtual ~ISubject() = default;

				/** Add Observer to list of notification */
				virtual void AttachObserver(ObserverT& observer) = 0;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserver(ObserverT& observer) = 0;

				/** Update all attached observers */
				virtual void NotifyObservers() const = 0;

                /** Check if there is observer reference in Subject */
                virtual bool HasObserver(ObserverType& observer) const noexcept = 0;
			};


//======================================Ref Version=====================================
// Ref Version can be used with stack objects


			// TODO: Realize pull version of Update

			/**
			 * Functor, calculating hash of referent for reference_wrapper.
			 *
			 * @tparam T Class type of referent (T&), that wrapped by reference_wrapper.
			 */
			template<typename T>
			struct HashRefWrapperReferent {
                constexpr size_t operator()(const std::reference_wrapper<T>& ref) const noexcept {
                    return std::hash<T*>()(&ref.get());
                }
			};
            /** Functor, calculating hash of reference address for reference_wrapper. */
            template<typename T>
            struct HashRefWrapperRefAddress {
                constexpr size_t operator()(const T& ref) const noexcept {
                    return std::hash<T*>()(&ref.get());
                }
            };
			/** Functor for EqualTo operation for unordered_set<reference_wrapper> */
            template<typename T>
            struct HashRefWrapperEqualTo {
                constexpr bool operator()(const T& lhs_ref, const T& rhs_ref) const noexcept {
                    return &lhs_ref.get() == &rhs_ref.get();
                }
            };


			class SubjectSetMulti;

			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 * One Observer may has many Subjects.
			 * Subjects list is unordered_set.
			 */
			class ObserverSetMulti : public IObserverMulti {
			public:
                using SubjectType = ISubject<IObserverMulti>;
				using SubjectRef = std::reference_wrapper<SubjectType>;
				using HashFunctorType = HashRefWrapperReferent<SubjectType>;
				using EqualToFunctorType = HashRefWrapperEqualTo<SubjectRef>;
				using ContainerType = std::unordered_set<SubjectRef, HashFunctorType, EqualToFunctorType>;

                explicit ObserverSetMulti(SubjectType& subject) noexcept
                    : subjects_refs_{ {std::ref(subject)} } {
                    subject.AttachObserver(*this);
                }

			protected:
				ObserverSetMulti(const ObserverSetMulti&) = delete; // C.67	C.21
				ObserverSetMulti& operator=(const ObserverSetMulti&) = delete;
				ObserverSetMulti(ObserverSetMulti&&) noexcept = delete;
				ObserverSetMulti& operator=(ObserverSetMulti&&) noexcept = delete;
			public:
				~ObserverSetMulti() override {
					ContainerType::iterator it = subjects_refs_.begin();
					while (it != subjects_refs_.end()) {
						it++->get().DetachObserver(*this);
						//.get().DetachObserver(*this);
					}
                        /*for (SubjectRef subject_ref : subjects_refs_) {
                            subject_ref.get().DetachObserver(*this);
                        }*/
				};

				/** Update the information about Subject */
				void Update() override {
				};

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				void Update(const SubjectType& subject) override {
				};

				/**
				 * Delete Subject from set of subjects in Observer, when Subject is destructed.
				 *
				 * @param subject subject, that is destructing.
				 */
				void DetachSubject(SubjectType& subject) override {
					subjects_refs_.erase(std::ref(subject));
					if (subject.HasObserver(*this)) { subject.DetachObserver(*this); }
				};

				/** Check if there is subject reference in Observer */
                bool HasSubject(SubjectType& subject) const noexcept override {
                    return subjects_refs_.find(subject) != subjects_refs_.end();
                };

			private:
				/** Subjects for which we will be notified. */
				ContainerType subjects_refs_{};
				// Is not const, cause Attach, Detach functions call.

			};	// !class ObserverSetMulti


			/**
			 * Concrete. Observers will be notified on Subject state changes.
			 * Ref Version can be used with stack objects.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 * One Subject can has many Observers.
			 * Observers list is unordered_set.
			 */
			//template<typename HashFunctorT>
			class SubjectSetMulti : public ISubject<IObserverMulti> {
			public:
                using ObserverType = IObserverMulti;
				using ObserverRef = std::reference_wrapper<ObserverType>;
				using HashFunctorType = HashRefWrapperReferent<ObserverType>;
				using EqualToFunctorType = HashRefWrapperEqualTo<ObserverRef>;
				using ContainerType = std::unordered_set<ObserverRef, HashFunctorType, EqualToFunctorType>;

				SubjectSetMulti() = default;
			protected:
				SubjectSetMulti(const SubjectSetMulti&) = delete; // C.67	C.21
				SubjectSetMulti& operator=(const SubjectSetMulti&) = delete;
				SubjectSetMulti(SubjectSetMulti&&) noexcept = delete;
				SubjectSetMulti& operator=(SubjectSetMulti&&) noexcept = delete;
			public:
				~SubjectSetMulti() override {
					ContainerType::iterator it = observers_refs.begin();
					while (it != observers_refs.end()) {
						it++->get().DetachSubject(*this);
					}
					/*for (ObserverRef observer_ref : observers_refs) {
						observer_ref.get().DetachSubject(*this);
					}*/
				};

				/** Add Observer to list of notification */
				inline void AttachObserver(ObserverType& observer) override {
					observers_refs.emplace(observer);
				};
				// TODO: Attach Observers() initializer_list, vector. Other operations with multiple observers.

				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserver(ObserverType& observer) override {
					observers_refs.erase(std::ref(observer));
					if (observer.HasSubject(*this)) observer.DetachSubject(*this);
				};

				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (ObserverRef observer_ref : observers_refs) {
						observer_ref.get().Update();
					}
				};

                /** Update all attached observers with multiple subjects sending Subject& */
                inline void NotifyObserversMulti() const {
                    for (ObserverRef observer : observers_refs) {
                        observer.get().Update(*this);
                    }
                };

				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept {
					observers_refs.clear();
					DetachAllObservers();
				};

				/** Check if there is observer reference in Subject */
				bool HasObserver(ObserverType& observer) const noexcept override {
					return observers_refs.find(observer) != observers_refs.end();
				};

			private:
				/** Delete this subject from sets in all attached observers  */
				inline void DetachAllObservers() {
                    for (ObserverRef observer_ref : observers_refs) {
                        observer_ref.get().DetachSubject(*this);
                    }
				}

				/**
				 * List of observers, that will be attach to observable object.
				 * Subject is not interested in owning of its Observers.
				 * So can be used weak_ptr, created from shared_ptr.
				 * Also can be used ref IObserver&.
				 * Containers can't hold IObserver&. Only wrapper.
				 *
				 * Design: If there is too many subjects with few observers you can use hash table.
				 */
				ContainerType observers_refs{};
				/*
				* observers_refs maybe unordered_set for quicker search and detach
				* List is used, when the order of observers is important.
				*/

			};	// !class SubjectSetMulti



			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 * One Observer may has many Subjects.
			 * Subjects list is unordered_set.
			 */
			class ObserverListMulti : public IObserverMulti {
			public:
				using SubjectType = ISubject<IObserverMulti>;
				using SubjectRef = std::reference_wrapper<SubjectType>;

                explicit ObserverListMulti(SubjectType& subject) noexcept
                    : subjects_refs_{ {std::ref(subject)} } {
                    subject.AttachObserver(*this);
                }
			protected:
				ObserverListMulti(const ObserverListMulti&) = delete; // C.67	C.21
				ObserverListMulti& operator=(const ObserverListMulti&) = delete;
				ObserverListMulti(ObserverListMulti&&) noexcept = delete;
				ObserverListMulti& operator=(ObserverListMulti&&) noexcept = delete;
			public:
				~ObserverListMulti() override {
					for (SubjectRef subject_ref : subjects_refs_) {
						subject_ref.get().DetachObserver(*this);
					}
				};

				/** Update the information about Subject */
				void Update() override {
				};

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				void Update(const SubjectType& subject) override {
				};

				/**
				 * Delete Subject from set of subjects in Observer, when Subject is destructed.
				 *
				 * @param subject subject, that is destructing.
				 */
				void DetachSubject(SubjectType& subject) override {
					subjects_refs_.remove_if([&subject](const SubjectRef& current) {
                        return &current.get() == &subject;
                        });
				};

			private:
				/** Subjects for which we will be notified. */
				std::forward_list<SubjectRef> subjects_refs_{};
				// Is not const, cause Attach, Detach functions call.

			};	// !class ObserverListMulti


			/**
			 * Concrete. Observers will be notified on Subject state changes.
			 * Ref Version can be used with stack objects.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 * One Subject can has many Observers.
			 * Observers list is unordered_set.
			 */
			 //template<typename HashFunctorT>
			class SubjectListMulti : public ISubject<IObserverMulti> {
			public:
				using ObserverType = IObserverMulti;
				using ObserverRef = std::reference_wrapper<ObserverType>;

				SubjectListMulti() = default;
			protected:
				SubjectListMulti(const SubjectListMulti&) = delete; // C.67	C.21
				SubjectListMulti& operator=(const SubjectListMulti&) = delete;
				SubjectListMulti(SubjectListMulti&&) noexcept = delete;
				SubjectListMulti& operator=(SubjectListMulti&&) noexcept = delete;
			public:
				~SubjectListMulti() override {
					for (ObserverRef observer_ref : observers_refs) {
						observer_ref.get().DetachSubject(*this);
					}
				};

				/** Add Observer to list of notification */
				inline void AttachObserver(ObserverType& observer) override {
					observers_refs.emplace_front(std::ref(observer));
				};


				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserver(ObserverType& observer) override {
                    observers_refs.remove_if([&observer](const ObserverRef& current) {
                        return &current.get() == &observer;
                        });
				};


				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (ObserverRef observer_ref : observers_refs) {
						observer_ref.get().Update();
					}
				};

				/** Update all attached observers with multiple subjects sending Subject& */
				inline void NotifyObserversMulti() const {
					for (ObserverRef observer : observers_refs) {
						observer.get().Update(*this);
					}
				};


				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept {
					DetachAllObservers();
					observers_refs.clear();
				};

			private:
				/** Delete this subject from sets in all attached observers  */
				inline void DetachAllObservers() {
					for (ObserverRef observer_ref : observers_refs) {
						observer_ref.get().DetachSubject(*this);
					}
				}

				/**
				 * List of observers, that will be attach to observable object.
				 * Subject is not interested in owning of its Observers.
				 * So can be used weak_ptr, created from shared_ptr.
				 * Also can be used ref IObserver&.
				 * Containers can't hold IObserver&. Only wrapper.
				 *
				 * Design: If there is too many subjects with few observers you can use hash table.
				 */
				std::forward_list<ObserverRef> observers_refs{};
				/*
				* observers_refs maybe unordered_set for quicker search and detach
				* List is used, when the order of observers is important.
				*/

			};	// !class SubjectListMulti



			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 * One Observer may has many Subjects.
			 * Subjects list is unordered_set.
			 */
			class ObserverMsgMulti : public ObserverListMulti {
			public:
                explicit ObserverMsgMulti(SubjectType& subject) noexcept
                    : ObserverListMulti{ subject } {
                }
			protected:
				ObserverMsgMulti(const ObserverMsgMulti&) = delete; // C.67	C.21
				ObserverMsgMulti& operator=(const ObserverMsgMulti&) = delete;
				ObserverMsgMulti(ObserverMsgMulti&&) noexcept = delete;
				ObserverMsgMulti& operator=(ObserverMsgMulti&&) noexcept = delete;
			public:
				~ObserverMsgMulti() override = default;

				/** Update the information about Subject */
				void Update() override {
				};

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				void Update(const SubjectType& subject) override {
				};

				/** Update Observer, using event message */
                virtual void Update(const SubjectType& subject, const std::string& msg) {
                };

                /** Update Observer, using event message */
                template<typename EnumT>
				requires std::is_enum_v<EnumT>
                void Update(const SubjectType& subject, const EnumT event) {
                };
				// TODO: maybe delete event param and make only template param

			};	// !class ObserverMsgMulti


			/**
			 * Observer recieve Aspect, when Update.
			 * Aspect - part of Subject, that is changing. Subject data members.
			 * One Observer can has many Subjects.
			 */
			class ObserverAspect : public IObserverMulti {
			public:
                using SubjectType = ISubject<IObserverMulti>;
                using SubjectRef = std::reference_wrapper<SubjectType>;

                explicit ObserverAspect(SubjectType& subject) noexcept
                    : subjects_refs_{ {std::ref(subject)} } {
                    subject.AttachObserver(*this);
                }
			protected:
                ObserverAspect(const ObserverAspect&) = delete; // C.67	C.21
                ObserverAspect& operator=(const ObserverAspect&) = delete;
                ObserverAspect(ObserverAspect&&) noexcept = delete;
				ObserverAspect& operator=(ObserverAspect&&) noexcept = delete;
            public:
                ~ObserverAspect() override {
                    for (SubjectRef subject_ref : subjects_refs_) {
                        subject_ref.get().DetachObserver(*this);
                    }
                };


				/** Update the information about Subject */
				void Update() override {
				};

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				void Update(const SubjectType& subject) override {
				};

				/**
				 * Update the information about observer_state_ of observable object.
				 * There is Push and Pull Variants of realization. AbstractSubjectRef push aspect information of what
				 * is changing. Push is less decoupled.
				 * Use, when subject_part is created from data memeber in Subject - one struct of SubjectPartT.
				 * Copy or move assignment. Always use std::move.
				 * There is two protocols of update. Push update protocol. Pull Update protocol.
				 *
				 * @tparam SubjectPartT type of subjects aspect, part of subject, that is changing and observable.
				 * @param subject concrete	subject from subjects_list_, that requested Update.
				 * @param subject_part		universal reference. Part of subject, that is changing. Aspect.
				 */
				template<typename SubjectPartT>
				void Update(const SubjectType& subject,
							SubjectPartT&& subject_part) { // Aspect& interest
					//originator_state_ = std::move(subject_part);
					// Always use std::move.
				};
				// TODO: Notify only those observers, that is attached to chosen event.


				/**
				 * Delete Subject from set of subjects in Observer, when Subject is destructed.
				 *
				 * @param subject subject, that is destructing.
				 */
				void DetachSubject(SubjectType& subject) override {
					subjects_refs_.remove_if([&subject](const SubjectRef& current) {
						return &current.get() == &subject;
						});
				};

			private:
				/** Subjects for which we will be notified. */
				std::forward_list<SubjectRef> subjects_refs_{};
				// Is not const, cause Attach, Detach functions call.

			};	// !class ObserverAspect


			/**
			 * Abstract.With State. Observers will be notified on AbstractSubjectRef state changes.
			 * Ref Version can be used with stack objects.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 * One Subject can has many Observers.
			 */
			template<typename SubjectPartT>//, typename NotifyingListT = std::unordered_set<IObserver>>
			class AbstractSubjectAspect : public ISubject<IObserverMulti> {
			public:
				using ObserverType = IObserverMulti;
				using ObserverRef = std::reference_wrapper<ObserverType>;

				AbstractSubjectAspect() = default;
			protected:
				AbstractSubjectAspect(const AbstractSubjectAspect&) = delete; // C.67	C.21
				AbstractSubjectAspect& operator=(const AbstractSubjectAspect&) = delete;
				AbstractSubjectAspect(AbstractSubjectAspect&&) noexcept = delete;
				AbstractSubjectAspect& operator=(AbstractSubjectAspect&&) noexcept = delete;
			public:
				~AbstractSubjectAspect() override {
					for (ObserverRef observer_ref : observers_refs) {
						observer_ref.get().DetachSubject(*this);
					}
				};

				/** Add Observer to list of notification */
				inline void AttachObserver(ObserverType& observer) override {
					observers_refs.emplace_front(std::ref(observer));
				};

				/**
				 * Add Observer to list of notification. To recieve concrete event.
				 *
				 * @tparam SubjectPartT type of subjects aspect, part of subject, that is changing and observable.
				 * @param observer observer, that will be notified.
				 * @param subject_part aspect, that is changing and will be sent to observer.
				 */
				template<typename SubjectPartT>
				void AttachObserver(ObserverType& observer, SubjectPartT&& subject_part) {
					//observers_refs.emplace_front(std::ref(observer));
				};
				// TODO: How to notify only those observers, that need changing aspect of subject?


				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserver(ObserverType& observer) override {
					observers_refs.remove_if([&observer](const ObserverRef& current) {
						return &current.get() == &observer;
						});
				};


				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (ObserverRef observer_ref : observers_refs) {
						observer_ref.get().Update();
					}
				};

				/** Update all attached observers with multiple subjects sending Subject& */
				inline void NotifyObserversMulti() const {
					for (ObserverRef observer : observers_refs) {
						observer.get().Update(*this);
					}
				};


				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept {
					DetachAllObservers();
					observers_refs.clear();
				};


				/**
				 * Use, when subject_part is created from separated data members in Subject.
				 * Used for move from temp object of SubjectPartT constructed from separated data members in Subject.
				 */
				virtual SubjectPartT GetSubjectStateValue() const = 0;

				/**
				 * Use, when subject_part is created from one data member type SubjectPartT in Subject.
				 * Used for Copy in Observer from const ref in Subject.
				 */
				virtual const SubjectPartT& GetSubjectStateCRef() const = 0;

				/*inline size_t GetObserversCount() const noexcept {
					return observers_refs.size();
				}*/

			private:
				/** Delete this subject from sets in all attached observers  */
				inline void DetachAllObservers() {
					for (ObserverRef observer_ref : observers_refs) {
						observer_ref.get().DetachSubject(*this);
					}
				}

				/**
				 * List of observers, that will be attach to observable object.
				 * Subject is not interested in owning of its Observers.
				 * So can be used weak_ptr, created from shared_ptr.
				 * Also can be used ref IObserver&.
				 * Containers can't hold IObserver&. Only wrapper.
				 *
				 * Design: If there is too many subjects with few observers you can use hash table.
				 */
				std::forward_list<ObserverRef> observers_refs{};
				/*
				* observers_refs maybe unordered_set for quicker search and detach
				* List is used, when the order of observers is important.
				*/

			};	// !class AbstractSubjectAspect




            struct State {
                int a_{ 0 };
                int b_{ 0 };
            };

            class MySubject : public SubjectSetMulti {
            public:
                MySubject() = default;
            protected:
                MySubject(const MySubject&) = delete; // C.67	C.21
                MySubject& operator=(const MySubject&) = delete;
                MySubject(MySubject&&) noexcept = delete;
                MySubject& operator=(MySubject&&) noexcept = delete;
            public:
                ~MySubject() override = default;

                State state_{};
            };

			class MyObserver : public ObserverSetMulti {
            public:
                explicit MyObserver(MySubject& observer) noexcept
                    : ObserverSetMulti{ dynamic_cast<ObserverSetMulti::SubjectType&>(observer) } {
                };
            protected:
                MyObserver(const MyObserver&) = delete; // C.67	C.21
                MyObserver& operator=(const MyObserver&) = delete;
                MyObserver(MyObserver&&) noexcept = delete;
                MyObserver& operator=(MyObserver&&) noexcept = delete;
            public:
                ~MyObserver() override = default;

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				void Update(const SubjectType& subject) override {
					//state_observer_ = subject
				};

				State state_{};
			};



		} // !namespace observer_ref_1



		namespace observer_ptr {
//==================================Interfaces=====================================

			// TODO: Make Review of ptr version of Observer Pattern

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
				 * There is Push and Pull Variants of realization. Subject push aspect information of what
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

				/** Add Observer to list of notification */
				virtual void AttachObserver(std::shared_ptr<IObserver>& observer) = 0;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserver(std::shared_ptr<IObserver>& observer) = 0;

				/** Update all attached observers */
				virtual void NotifyObservers() const = 0;
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
					originator_state_ = observable_state;
				};

			private:
				State originator_state_{};
			};

			/** Observers will be notified on Subject state changes */
			class Subject : public ISubject {
			public:
				/** Add Observer to list of notification */
				inline void AttachObserver(std::shared_ptr<IObserver>& observer) override {
					observers_refs.emplace_front(std::weak_ptr<IObserver>(observer));
				};

				/** Detach observer from notifying list */
				inline void DetachObserver(std::shared_ptr<IObserver>& observer) override {
					observers_refs.remove_if([&observer](const std::weak_ptr<IObserver>& current) {
													return current.lock().get() == observer.get();
													});
				};

				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (auto& observer : observers_refs) {
						observer.lock()->Update(observable_state_);
					}
				};

				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept {
					observers_refs.clear();
				};

				inline void set_state(const State& new_state) noexcept {
					observable_state_ = new_state;
					//NotifyObservers();
				}

			private:
				/**
				 * List of observers, that will be attach to observable object.
				 * Subject is not interested in owning of its Observers.
				 * So can be used weak_ptr, created from shared_ptr.
				 * Also can be used ref IObserver&.
				 */
				std::forward_list<std::weak_ptr<IObserver>> observers_refs{};

				/** The observable_state_ of Subject object */
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
				inline void Update(const State& subject_state) noexcept override {
					observer_state_ = subject_state;
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
					observers_refs.push_front(std::make_shared<IObserver>(new_observer));
				}
				SubjectExtended(const SubjectExtended&) = delete;
				SubjectExtended& operator=(const SubjectExtended&) = delete;*/


				/** Add Observer to list of notification */
				inline void AttachObserver(std::shared_ptr<IObserver>& observer) override {
					observers_refs.emplace_front(std::weak_ptr<IObserver>(observer));
				};

				/** Is used for Setting new observable in observer */
				inline void DetachObserverByRef(IObserver& observer) {
					observers_refs.remove_if([&observer](const std::weak_ptr<IObserver>& current) {
															return current.lock().get() == &observer;
															});
				};

				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserver(std::shared_ptr<IObserver>& observer) override {
					DetachObserverByRef(*observer.get());
				};

				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (auto& observer : observers_refs) {
						observer.lock()->Update(observable_state_);
					}
				};

				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept override {
					observers_refs.clear();
				};

				inline void set_state(const State& new_state) noexcept {
					observable_state_ = new_state;
				}

			private:
				/**
				 * List of observers, that will be attach to observable object.
				 * Subject is not interested in owning of its Observers. So can be used weak_ptr
				 */
				std::forward_list<std::weak_ptr<IObserver>> observers_refs{};

				/** The observable_state_ of Subject object */
				State observable_state_{};
			};
#endif // DEBUG

		} // !namespace observer_ptr



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

				// NotifyObservers all the attached obsevers
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
