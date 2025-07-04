#ifndef OBSERVER_OTHERS_HPP
#define OBSERVER_OTHERS_HPP

// TODO: compress include list
#include <atomic>
#include <algorithm> // remove_if
#include <execution> // execution policies
#include <forward_list>
#include <functional>
#include <future>	// for Async Update
#include <memory>
#include <mutex>
#include <iterator>
#include <initializer_list>	// for AttachObservers
#include <string>
#include <shared_mutex>
#include <system_error>	// thread execution exception
#include <set>
#include <thread>	// concurrency and thread safety SubjectWeakHub
#include <type_traits>
#include <list>
#include <utility> // pair
#include <unordered_set>
#include <vector>

#include "diagnostics-library/custom-exception.hpp"
//#include "error/error.hpp"


// TODO: maybe separate classes to different files to make less includes

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {

		/** Observer pattern namespace with realization by reference wrapper container in Subject */
		namespace observer_ref {
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


			/**
			 * Abstract. Stateless.
			 * Alternative name - Publisher.
			 * May be used for IObserver, IObserverMulti.
			 */
			class ISubject {
			public:
				using IObserverT = IObserver;

			protected:
				ISubject() = default;
				ISubject(const ISubject&) = delete; // C.67	C.21
				ISubject& operator=(const ISubject&) = delete;
				ISubject(ISubject&&) noexcept = delete;
				ISubject& operator=(ISubject&&) noexcept = delete;
			public:
				virtual ~ISubject() = default;

				/**
				 * Add Observer to list of notification in Subject.
				 * Add Subject to list of Observers in Observer..
				 */
				virtual void AttachObserver(IObserverT& observer) = 0;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserver(IObserverT& observer) = 0;

				/** Update all attached observers */
				virtual void NotifyObservers() const = 0;
			};

//==================================Multi Interfaces=====================================

			class ISubjectMulti;

			/** Abstract. Stateless. Interface for Observer with multiple subjects */
			class IObserverMulti : public IObserver {
            public:
                using ISubjectT = ISubjectMulti;

			protected:
				IObserverMulti() = default;
				IObserverMulti(const IObserverMulti&) = delete; // C.67	C.21
				IObserverMulti& operator=(const IObserverMulti&) = delete;
				IObserverMulti(IObserverMulti&&) noexcept = delete;
				IObserverMulti& operator=(IObserverMulti&&) noexcept = delete;
			public:
				~IObserverMulti() override = default;

                /** Update the information about Subject */
                void Update() override = 0;

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				virtual void Update(const ISubjectT& subject) = 0;

				/** Add Subject to list of observing. Add Observer to list of Observers in Subject */
				virtual void AttachSubjectNObserver(ISubjectT& subject) = 0;

				/**
				 * Detach Subject from Observer, when Subject is destructed..
				 *
				 * @param subject subject, that is destructing.
				 */
				virtual void DetachSubjectNObserver(ISubjectT& subject) = 0;

				/** Check if there is subject reference in Observer */
				virtual bool HasSubject(const ISubjectT& subject) const noexcept = 0;
			};


			/**
			 * Abstract. Stateless.
			 * Alternative name - Publisher.
			 * May be used for IObserver, IObserverMulti.
			 */
			class ISubjectMulti {
			public:
				using IObserverMultiT = IObserverMulti;

			protected:
				ISubjectMulti() = default;
				ISubjectMulti(const ISubjectMulti&) = delete; // C.67	C.21
				ISubjectMulti& operator=(const ISubjectMulti&) = delete;
				ISubjectMulti(ISubjectMulti&&) noexcept = delete;
				ISubjectMulti& operator=(ISubjectMulti&&) noexcept = delete;
			public:
				virtual ~ISubjectMulti() = default;

				/**
				 * Add Observer to list of notification in Subject.
				 * Add Subject to list of Observers in Observer..
				 */
				virtual void AttachObserverNSubject(IObserverMultiT& observer) = 0;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserverNSubject(IObserverMultiT& observer) = 0;

				/** Update all attached observers */
				virtual void NotifyObservers() const = 0;

                /** Check if there is observer reference in Subject */
                virtual bool HasObserver(const IObserverMultiT& observer) const noexcept = 0;
			};


//======================================Ref Version=====================================
// Ref Version can be used with stack objects


			// TODO: Realize pull version of Update

			/**
			 * Functor, calculating hash of referent for reference_wrapper.
			 * Hash calculated by hash of address of referent object.
			 *
			 * @tparam T Class type of referent (T&), that wrapped by reference_wrapper.
			 */
			template<typename T>
			struct HashRefWrapperReferent { // for hash table in unordered_set of reference_wrapper
                constexpr size_t operator()(const std::reference_wrapper<T>& ref) const noexcept {
                    return std::hash<T*>()(&ref.get());
                }
			};
            /** Functor, calculating hash of reference address for reference_wrapper. */
            template<typename T>
            struct HashRefWrapperRefAddress { // for hash table in unordered_set of reference_wrapper
                constexpr size_t operator()(const std::reference_wrapper<T>& ref) const noexcept {
                    return std::hash<const std::reference_wrapper<T>&*>()(&ref);
                }
            };
			/** Functor for EqualTo operation for unordered_set<reference_wrapper> */
            template<typename T>
            struct HashRefWrapperEqualTo { // for hash table in unordered_set of reference_wrapper
                constexpr bool operator()(const T& lhs_ref, const T& rhs_ref) const noexcept {
                    return &lhs_ref.get() == &rhs_ref.get();
                }
            };


			class SubjectRefSetMulti;

			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 * One Observer may has many Subjects.
			 * Don't use with dynamic objects, cause dangling references.
			 * Subjects list is unordered_set of reference wrapper.
			 * Use, when the order of Update is not important.
			 */
			class ObserverRefSetMulti : public IObserverMulti {
			public:
                using ISubjectT = ISubjectMulti;
				using ISubjectRefT = std::reference_wrapper<ISubjectT>;
				using HashFunctorT = HashRefWrapperReferent<ISubjectT>;
				using EqualToFunctorT = HashRefWrapperEqualTo<ISubjectRefT>;
				using ContainerT = std::unordered_set<ISubjectRefT, HashFunctorT, EqualToFunctorT>;

                explicit ObserverRefSetMulti(ISubjectT& subject) noexcept
						: subjects_refs_{ {std::ref(subject)} } {
                    subject.AttachObserverNSubject(*this);
                }

			protected:
				ObserverRefSetMulti(const ObserverRefSetMulti&) = delete; // C.67	C.21
				ObserverRefSetMulti& operator=(const ObserverRefSetMulti&) = delete;
				ObserverRefSetMulti(ObserverRefSetMulti&&) noexcept = delete;
				ObserverRefSetMulti& operator=(ObserverRefSetMulti&&) noexcept = delete;
			public:
				~ObserverRefSetMulti() override {
					ContainerT::iterator it{ subjects_refs_.begin() };
					//auto it{ subjects_refs_.begin() };
					const auto cend{ subjects_refs_.cend() };
					auto it_prev{ it }; // post increment returns unmutable const_iterator, that is not best choice for mutable Detach
					for (; it != cend; ++it) { // not ranged for, cause elements deleted
						it_prev = it;
						it_prev->get().DetachObserverNSubject(*this);
					}
				};
				//~ObserverWeakMulti() override {
				//	const auto cend{ subjects_.cend() };
				//	for (auto it{ subjects_.begin() }, auto it_prev{ it }; it != cend; ++it) { // not ranged for, cause elements deleted
				//		it_prev = it;
				//		if (auto subject_ptr{ it_prev->lock() }) { // if alive, detach both subject in observer & observer in subject
				//			subject_ptr->DetachObserver(weak_from_this());
				//		}
				//	}
				//};


				/** Update the information about Subject */
				void Update() override {
				};

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				void Update(const ISubjectT& subject) override {
				};
				//virtual void Update(const ISubjectT& subject) = 0;

                /** Add Subject to list of observing. Add Observer to list of Observers in Subject */
                void AttachSubjectNObserver(ISubjectT& subject) override {
					subjects_refs_.emplace(subject);
                    if (!subject.HasObserver(*this)) { subject.AttachObserverNSubject(*this); }
                };

				/**
				 * Delete Subject from set of subjects in Observer, when Subject is destructed.
				 *
				 * @param subject subject, that is destructing.
				 */
				void DetachSubjectNObserver(ISubjectT& subject) override {
					subjects_refs_.erase(std::ref(subject));
					if (subject.HasObserver(*this)) { subject.DetachObserverNSubject(*this); }
				};

				/** Check if there is subject reference in Observer */
                bool HasSubject(const ISubjectT& subject) const noexcept override {
					auto equal_fn = [&subject](const ISubjectRefT& ref) { return &ref.get() == &subject; };
					auto search_result{ std::find_if(subjects_refs_.begin(), subjects_refs_.end(), equal_fn) };
                    return search_result != subjects_refs_.end();
                };

			private:
				/** Subjects for which we will be notified. */
				ContainerT subjects_refs_{};
				// Is not const, cause Attach, Detach functions call.

			};	// !class ObserverRefSetMulti


			/**
			 * Concrete. Observers will be notified on Subject state changes.
			 * One Subject can has many Observers.
			 * Ref Version can be used with stack objects.
			 * Don't use with dynamic objects, cause dangling references.
			 * Observers list is unordered_set.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 * It is better to use set, when the order of Update is not important.
			 */
			class SubjectRefSetMulti : public ISubjectMulti {
			public:
                using IObserverT = IObserverMulti;
				using IObserverRefT = std::reference_wrapper<IObserverT>;
				using HashFunctorT = HashRefWrapperReferent<IObserverT>;
				using EqualToFunctorT = HashRefWrapperEqualTo<IObserverRefT>;
				using ContainerT = std::unordered_set<IObserverRefT, HashFunctorT, EqualToFunctorT>;

				SubjectRefSetMulti() = default;
			protected:
				SubjectRefSetMulti(const SubjectRefSetMulti&) = delete; // C.67	C.21
				SubjectRefSetMulti& operator=(const SubjectRefSetMulti&) = delete;
				SubjectRefSetMulti(SubjectRefSetMulti&&) noexcept = delete;
				SubjectRefSetMulti& operator=(SubjectRefSetMulti&&) noexcept = delete;
			public:
				~SubjectRefSetMulti() override {
					ContainerT::iterator it = observers_refs.begin();
					const auto cend{ observers_refs.cend() };
					auto it_prev{ it }; // post increment returns unmutable const_iterator, that is not best choice for mutable Detach
					while (it != cend) { // not ranged for, cause elements deleted
                        it_prev = it;
                        ++it;
						it_prev->get().DetachSubjectNObserver(*this);
					}
				};

				/** Add Observer to list of notification */
				inline void AttachObserverNSubject(IObserverT& observer) override {	// mustn't be const, cause observer may change
					//observers_refs.emplace(std::ref(observer));
					observers_refs.emplace(observer);
					if (!observer.HasSubject(*this)) {
						observer.AttachSubjectNObserver(*this);
					}
				};
				// TODO: Attach Observers() initializer_list, vector. Other operations with multiple observers.

				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserverNSubject(IObserverT& observer) override {
					observers_refs.erase(std::ref(observer));
					if (observer.HasSubject(*this)) { observer.DetachSubjectNObserver(*this); }
				};

				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (IObserverRefT observer_ref : observers_refs) {
						observer_ref.get().Update();
					}
				};

                /** Update all attached observers with multiple subjects sending Subject& */
                inline void NotifyObserversMulti() const {
                    for (IObserverRefT observer : observers_refs) {
                        observer.get().Update(*this);
                    }
                };

				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept {
					observers_refs.clear();
					DetachAllObservers();
				};

				/** Check if there is observer reference in Subject */
				bool HasObserver(const IObserverT& observer) const noexcept override {
					auto equal_fn = [&observer](const auto& ref) { return &ref.get() == &observer; };
					auto search_result{ std::find_if(observers_refs.begin(), observers_refs.end(), equal_fn) };
					return search_result != observers_refs.end();
				};

			private:
				/** Delete this subject from sets in all attached observers  */
				inline void DetachAllObservers() {
                    auto it = observers_refs.begin();
                    while (it != observers_refs.end()) {
                        it++->get().DetachSubjectNObserver(*this);
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
				ContainerT observers_refs{};
				/*
				* observers_refs maybe unordered_set for quicker search and detach
				* List is used, when the order of observers is important.
				*/

			};	// !class SubjectRefSetMulti



			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 * One Observer may has many Subjects.
			 * Subjects list is unordered_set.
			 * It is better to use List, when the order of Update is important.
			 */
			class ObserverListMulti : public IObserverMulti {
			public:
				using ISubjectT = ISubjectMulti;
				using ISubjectRefT = std::reference_wrapper<ISubjectT>;

                explicit ObserverListMulti(ISubjectT& subject) noexcept
                    : subjects_refs_{ {std::ref(subject)} } {
                    subject.AttachObserverNSubject(*this);
                }
			protected:
				ObserverListMulti(const ObserverListMulti&) = delete; // C.67	C.21
				ObserverListMulti& operator=(const ObserverListMulti&) = delete;
				ObserverListMulti(ObserverListMulti&&) noexcept = delete;
				ObserverListMulti& operator=(ObserverListMulti&&) noexcept = delete;
			public:
				~ObserverListMulti() override {
					for (ISubjectRefT subject_ref : subjects_refs_) {
						subject_ref.get().DetachObserverNSubject(*this);
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
				void Update(const ISubjectT& subject) override {
				};

				/**
				 * Delete Subject from set of subjects in Observer, when Subject is destructed.
				 *
				 * @param subject subject, that is destructing.
				 */
				void DetachSubjectNObserver(ISubjectT& subject) override {
					subjects_refs_.remove_if([&subject](const ISubjectRefT& current) {
                        return &current.get() == &subject;
                        });
				};

			private:
				/** Subjects for which we will be notified. */
				std::forward_list<ISubjectRefT> subjects_refs_{};
				// Is not const, cause Attach, Detach functions call.

			};	// !class ObserverListMulti


			/**
			 * Concrete. Observers will be notified on Subject state changes.
			 * Ref Version can be used with stack objects.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 * One Subject can has many Observers.
			 * Observers list is unordered_set.
			 * It is better to use List, when the order of Update is important.
			 */
			 //template<typename HashFunctorT>
			class SubjectListMulti : public ISubjectMulti {
			public:
				using IObserverT = IObserverMulti;
				using IObserverRefT = std::reference_wrapper<IObserverT>;

				SubjectListMulti() = default;
			protected:
				SubjectListMulti(const SubjectListMulti&) = delete; // C.67	C.21
				SubjectListMulti& operator=(const SubjectListMulti&) = delete;
				SubjectListMulti(SubjectListMulti&&) noexcept = delete;
				SubjectListMulti& operator=(SubjectListMulti&&) noexcept = delete;
			public:
				~SubjectListMulti() override {
					for (IObserverRefT observer_ref : observers_refs) {
						observer_ref.get().DetachSubjectNObserver(*this);
					}
				};

				/** Add Observer to list of notification. Used in constructor of Observer */
				inline void AttachObserverNSubject(IObserverT& observer) override {
					observers_refs.emplace_front(std::ref(observer));
				};


				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserverNSubject(IObserverT& observer) override {
                    observers_refs.remove_if([&observer](const IObserverRefT& current) {
                        return &current.get() == &observer;
                        });
				};


				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (IObserverRefT observer_ref : observers_refs) {
						observer_ref.get().Update();
					}
				};

				/** Update all attached observers with multiple subjects sending Subject& */
				inline void NotifyObserversMulti() const {
					for (IObserverRefT observer : observers_refs) {
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
					for (IObserverRefT observer_ref : observers_refs) {
						observer_ref.get().DetachSubjectNObserver(*this);
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
				std::forward_list<IObserverRefT> observers_refs{};
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
                explicit ObserverMsgMulti(ISubjectT& subject) noexcept
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
				void Update(const ISubjectT& subject) override {
				};

				/** Update Observer, using event message */
                virtual void Update(const ISubjectT& subject, const std::string& msg) {
                };

                /** Update Observer, using event message */
                template<typename EnumT>
				requires std::is_enum_v<EnumT>
                void Update(const ISubjectT& subject, const EnumT event) {
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
                using ISubjectT = ISubjectMulti;
                using ISubjectRef = std::reference_wrapper<ISubjectT>;

                explicit ObserverAspect(ISubjectT& subject) noexcept
                    : subjects_refs_{ {std::ref(subject)} } {
                    subject.AttachObserverNSubject(*this);
                }
			protected:
                ObserverAspect(const ObserverAspect&) = delete; // C.67	C.21
                ObserverAspect& operator=(const ObserverAspect&) = delete;
                ObserverAspect(ObserverAspect&&) noexcept = delete;
				ObserverAspect& operator=(ObserverAspect&&) noexcept = delete;
            public:
                ~ObserverAspect() override {
                    for (ISubjectRef subject_ref : subjects_refs_) {
                        subject_ref.get().DetachObserverNSubject(*this);
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
				void Update(const ISubjectT& subject) override {
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
				void Update(const ISubjectT& subject,
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
				void DetachSubjectNObserver(ISubjectT& subject) override {
					subjects_refs_.remove_if([&subject](const ISubjectRef& current) {
						return &current.get() == &subject;
						});
				};

			private:
				/** Subjects for which we will be notified. */
				std::forward_list<ISubjectRef> subjects_refs_{};
				// Is not const, cause Attach, Detach functions call.

			};	// !class ObserverAspect


			/**
			 * Abstract.With State. Observers will be notified on AbstractSubjectRef state changes.
			 * Ref Version can be used with stack objects.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 * One Subject can has many Observers.
			 */
			template<typename SubjectPartT>//, typename NotifyingListT = std::unordered_set<IObserver>>
			class AbstractSubjectAspect : public ISubjectMulti {
			public:
				using IObserverT = IObserverMulti;
				using IObserverRef = std::reference_wrapper<IObserverT>;

				AbstractSubjectAspect() = default;
			protected:
				AbstractSubjectAspect(const AbstractSubjectAspect&) = delete; // C.67	C.21
				AbstractSubjectAspect& operator=(const AbstractSubjectAspect&) = delete;
				AbstractSubjectAspect(AbstractSubjectAspect&&) noexcept = delete;
				AbstractSubjectAspect& operator=(AbstractSubjectAspect&&) noexcept = delete;
			public:
				~AbstractSubjectAspect() override {
					for (IObserverRef observer_ref : observers_refs) {
						observer_ref.get().DetachSubject(*this);
					}
				};

				/** Add Observer to list of notification */
				inline void AttachObserverNSubject(IObserverT& observer) override {
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
				void AttachObserverNSubject(IObserverT& observer, SubjectPartT&& subject_part) {
					//observers_refs.emplace_front(std::ref(observer));
				};
				// TODO: How to notify only those observers, that need changing aspect of subject?


				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserverNSubject(IObserverT& observer) override {
					observers_refs.remove_if([&observer](const IObserverRef& current) {
						return &current.get() == &observer;
						});
				};


				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (IObserverRef observer_ref : observers_refs) {
						observer_ref.get().Update();
					}
				};

				/** Update all attached observers with multiple subjects sending Subject& */
				inline void NotifyObserversMulti() const {
					for (IObserverRef observer : observers_refs) {
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
					for (IObserverRef observer_ref : observers_refs) {
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
				std::forward_list<IObserverRef> observers_refs{};
				/*
				* observers_refs maybe unordered_set for quicker search and detach
				* List is used, when the order of observers is important.
				*/

			};	// !class AbstractSubjectAspect




            struct State {
                int a_{ 0 };
                int b_{ 0 };
            };

            class MySubject : public SubjectRefSetMulti {
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

			class MyObserver : public ObserverRefSetMulti {
            public:
                explicit MyObserver(MySubject& observer) noexcept
                    : ObserverRefSetMulti{ dynamic_cast<ObserverRefSetMulti::ISubjectT&>(observer) } {
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
				void Update(const ISubjectT& subject) override {
					//state_observer_ = subject
				};

				State state_{};
			};



		} // !namespace observer_ref_1



		namespace observer_ptr_old {
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

		} // !namespace observer_ptr_old



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
					for (const auto& x : callbacks)
					{
						x.get().update(*this);
					}
				}

				// Add an observer_ref_1
				void attach(const Observer& observer) {
					callbacks.push_front(observer);
				}

				// Remove an observer_ref_1
				void detach(Observer& observer) {
					callbacks.remove_if([&observer](const RefObserver& obj)
						{
							return &obj.get() == &observer;
						});
				}

			private:
				std::forward_list<RefObserver> callbacks;
			};

		} // namespace observer_wiki_version



		namespace observer_delegate {
// https://eliogubser.com/posts/2021-04-cpp-observer/

#include <memory>
#include <forward_list>
#include <functional>


			// Simple observer pattern in modern C++ without pointers.
			// Register functions with make_observer() and call notify() to call them.
			// Note: Functions are called from the thread that calls notify().
			template <typename... Args>
			class Subject
			{
			public:
				typedef std::shared_ptr<std::function<void(Args...)>> Observer;

				Subject() {}

				void notify(const Args &...args)
				{
					bool cleanup = false;
					for (const auto& observer_weak : m_observers)
					{
						if (const auto& observer_function = observer_weak.lock())
						{
							(*observer_function)(args...);
						}
						else
						{
							// weak pointer expired, do a cleanup round
							cleanup = true;
						}
					}

					if (cleanup)
					{
						m_observers.remove_if([](auto observer) { return observer.expired(); });
					}
				}


				// Register a function as observer. Keep the returned shared_ptr around as long as you want
				// the function to be called.
				Observer makeObserver(std::function<void(Args...)> observerFunction)
				{
					auto observer = std::make_shared<std::function<void(Args...)>>(observerFunction);
					m_observers.emplace_front(observer);
					return observer;
				}

			private:
				std::forward_list<std::weak_ptr<std::function<void(Args...)>>> m_observers;
			};

		} // !observer_delegate




		namespace observer_weak_ptr {
			//======================================Trash===================================================

		// Not necessary. Minimal interface better
		/**
			* Abstract. Stateless. Subject with weak_ptr dependencies.
			* Alternative name - Publisher.
			* weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			*/
			//template<typename EnumT = ObserverEmptyEnum>
			//class ISubjectWeakExtended : public ISubjectWeak {
			//public:
			//	// Cause of name hiding
			//	using ISubjectWeak::NotifyObservers;
			//	using ISubjectWeak::AttachObserver;
			//	using ISubjectWeak::DetachObserver;

			//	using WeakPtrIObserverWeak = ISubjectWeak::WeakPtrIObserverWeak;

			//protected:
			//	ISubjectWeakExtended() = default;
			//	ISubjectWeakExtended(const ISubjectWeakExtended&) = delete; // C.67	C.21
			//	ISubjectWeakExtended& operator=(const ISubjectWeakExtended&) = delete;
			//	ISubjectWeakExtended(ISubjectWeakExtended&&) noexcept = delete;
			//	ISubjectWeakExtended& operator=(ISubjectWeakExtended&&) noexcept = delete;
			//public:
			//	~ISubjectWeakExtended() override = default;


			//	/** Update all attached observers */
			//	virtual void NotifyObservers() const = 0;

			//	/** Update all attached observers */
			//	virtual void NotifyObservers(const EnumT message) const = 0;

			//	/**
			//	 * Add Observer to list of notification in Subject.
			//	 * There is shared_ptr<ObserverT> outside of class. Subject stores weak_ptr to that Observer.
			//	 */
			//	virtual void AttachObserver(const WeakPtrIObserverWeak observer_ptr) = 0;
			//	// not const, cause must attach subject in observer ptr

			//	/** Detach observer from notifying list */
			//	virtual void DetachObserver(const WeakPtrIObserverWeak observer_ptr) = 0;
			//};

			//=====================Try to solve problem of sending policy in interface function=======================
			// These problems is static language limitation. Compiler must define all types at compile time.
			// Solutions: 1) invariant return in GetExecPolicy
			// 2) Dervied Template classes.
			// 3) GetExecPolicy with void return, std::function parameter as callback.
			//		Inside of GetExecPolicy bind funciton and return std::function<void()>. So bind policy with function and arguments.

			/*enum class ExecPolicyE {
				seq = 0,
				par,
				par_unseq,
				unseq
			};

			template<ExecPolicyE policy>
			struct Policy {
				static constexpr auto value{ std::execution::seq };
			};
			template<>
			struct Policy<ExecPolicyE::par> {
				static constexpr auto value{ std::execution::par };
			};
			template<>
			struct Policy<ExecPolicyE::par_unseq> {
				static constexpr auto value{ std::execution::par_unseq };
			};
			template<>
			struct Policy<ExecPolicyE::unseq> {
				static constexpr auto value{ std::execution::unseq };
			};

			template<ExecPolicyE policy>
			using Policy_v = Policy<policy>::value;


			template<ExecPolicyE policy>
			constexpr auto GetExecPolicy() { return Policy_v<policy>; }*/


			/** Constexpr template for abstract interface class member function. Declare enum param in function. */
			//template<typename T>
			//constexpr T GetExecPolicy(const ExecPolicyEnum policy) noexcept {

			/*constexpr auto GetExecPolicy(const ExecPolicyEnum policy) noexcept {
				switch (policy) {
					case ExecPolicyEnum::seq:			return std::execution::seq;
					case ExecPolicyEnum::par:			return std::execution::par;
					case ExecPolicyEnum::par_unseq:		return std::execution::par_unseq;
					case ExecPolicyEnum::unseq:			return std::execution::unseq;
					default:							return std::execution::seq;
				}
			}*/



			//===============================Giga Chat Multithread Observer==================================================
			//You can use concurrent containers as Intel TBB and Boost.Lockfree
			//
			//#include <iostream>
			//#include <thread>
			//#include <mutex>
			//#include <memory>
			//#include <list>
			//#include <future>
			//
			//// Базовый класс наблюдателя
			//			class Observer {
			//			public:
			//				virtual ~Observer() {};
			//				virtual void update(Subject* s) = 0;
			//			};
			//
			//			// Класс субъекта
			//			class Subject {
			//			public:
			//				void addObserver(std::shared_ptr<Observer> ob) {
			//					std::lock_guard<std::mutex> lk(mutex_);
			//					observers_.push_back(ob);
			//				}
			//
			//				void removeObserver(std::shared_ptr<Observer> ob) {
			//					std::lock_guard<std::mutex> lk(mutex_);
			//					observers_.remove(ob);
			//				}
			//
			//				void notifyObservers() {
			//					// Создание локальной копии активных наблюдателей
			//					std::list<std::weak_ptr<Observer>> active_observers;
			//					{
			//						std::lock_guard<std::mutex> lk(mutex_);
			//						active_observers.assign(observers_.begin(), observers_.end());
			//					}
			//
			//					// Фильтрация активной копии (параллельно)
			//					std::list<std::shared_ptr<Observer>> valid_observers;
			//					std::for_each(active_observers.begin(), active_observers.end(), [&valid_observers](const auto& wp) {
			//						if (auto sp = wp.lock()) {
			//							valid_observers.push_back(sp);
			//						}
			//						});
			//
			//					// Шаг 1: асинхронная рассылка уведомлений
			//					std::vector<std::future<void>> futures;
			//					for (auto& observer : valid_observers) {
			//						futures.emplace_back(std::async([this, observer]() {
			//							observer->update(this);
			//							}));
			//					}
			//
			//					// Ждем завершения всех заданий
			//					for (auto& f : futures) {
			//						f.wait();
			//					}
			//
			//					// Шаг 2: очистка устаревших ссылок (после завершения обработчиков)
			//					{
			//						std::lock_guard<std::mutex> lk(mutex_);
			//						observers_.assign(valid_observers.begin(), valid_observers.end());
			//					}
			//				}
			//
			//			private:
			//				std::list<std::weak_ptr<Observer>> observers_; // Контейнер слабых ссылок
			//				std::mutex mutex_;                             // Мьютекс для защиты доступа к списку
			//			};
			//
			//			// Реализация наблюдателя
			//			class ConcreteObserver : public Observer {
			//			public:
			//				void update(Subject* subj) override {
			//					std::cout << "ConcreteObserver updated by Subject\n";
			//				}
			//			};
			//
			//			int main() {
			//				Subject sub;
			//				auto observer1 = std::make_shared<ConcreteObserver>();
			//				auto observer2 = std::make_shared<ConcreteObserver>();
			//
			//				sub.addObserver(observer1);
			//				sub.addObserver(observer2);
			//
			//				sub.notifyObservers();
			//
			//				return 0;
			//			}


						//==========================Deprecated Experiment with autoclean======================================================
			template<typename IteratorT>
			using CleanupListT = std::vector<IteratorT>;


			/** Erase duplicates in container */
			template<typename ContainerT, typename ExecPolicyT>
			inline void EraseDuplicates_Cleanup(ContainerT& container, ExecPolicyT policy = std::execution::seq) {
				auto begin{ container.begin() };
				auto end{ container.end() };
				std::sort(policy, begin, end); // make groups of equal elements
				container.erase(std::unique(policy, begin, end), end);
			}

			template<typename ValueT, typename IteratorsContainerT>
			//inline bool IsValueInIterators(const typename ContainerT::value_type& value,
			inline bool IsValueInIterators_Cleanup(const ValueT& value, const IteratorsContainerT& iterators_list) {
				for (const auto& it : iterators_list) {
					if (&(*it) == &value) { return true; }
				}
				return false;
			}

			/** Erase cleanup list of iterators to elements from input container */
			template<typename ContainerT, typename IteratorsContainerT, typename ExecPolicyT>
			inline void EraseElementsByIterators_Cleanup(ContainerT& container,
				IteratorsContainerT& iterators_list,
				ExecPolicyT policy = std::execution::seq) {
				//static_assert(std::is_same_v<typename ContainerT::iterator, typename IteratorsContainerT::value_type>);
				//EraseDuplicates(container, policy);

				// Iterators in iterators_list can be easyly invalidated by erase operation
				if (iterators_list.empty()) { return; }

				auto container_end{ container.end() };
				if constexpr (std::is_same_v<ContainerT, std::forward_list<typename ContainerT::value_type>>) { // for Forward_list
					container.remove_if([&iterators_list](const auto& value) {
						return IsValueInIterators(value, iterators_list);
						});
				}
				else { // All other containers, except forward_list
					// all iterator elements will be moved to end
					// container_logic_end is new end of container. first iterator element at the end of container, after remove.
					auto container_logic_end{ container.end() };
					auto remove_iterators_fn = [&container, &policy, &container_end, &container_logic_end](auto& it) {
						if (it != container_end) {
							// move iterator element to end of container
							container_logic_end = std::remove(policy, it, container_logic_end, it);
						}
						};

					// Move each iterator element of ContainerT& container to the end of container
					std::for_each(policy, iterators_list.begin(), iterators_list.end(), remove_iterators_fn);
					container.erase(container_logic_end, container_end);
				}
			}


			/**
			 * Compares if two weak_ptr are alive and have the same stored pointers.
			 *
			 * @param searched_ptr
			 * @param current_ptr
			 * @return result of equality check
			 */
			template<typename ValueT>
			inline bool IsEqualWeakPtr_Cleanup(const std::shared_ptr<ValueT>& searched_shared,
				const std::weak_ptr<ValueT>& current_ptr) {
				auto current_shared = current_ptr.lock();
				return searched_shared && current_shared && searched_shared.get() == current_shared.get();
			}

			/**
			 * Find first weak_ptr, that is alive and has same stored pointer.
			 *
			 * @return	First = of pair of iterator to equal weak_ptr or to and.
			 *			Second = size_t count of expired weak_ptr for cleanup.
			 */
			template<typename ValueT, typename ContainerT, typename ExecPolicyT>
			inline auto FindEqualWeakPtr_Cleanup(ContainerT& container,
				const std::weak_ptr<ValueT>& searched_ptr,
				ExecPolicyT policy = std::execution::seq) {
				/*static_assert(std::is_same_v<std::weak_ptr<ValueT>, typename ContainerT::value_type>,
							  "The type mismatch between container elements and weak_ptr");*/
							  //using iterator = typename ContainerT::const_iterator;
				using iterator = decltype(container.end());
				//std::pair<iterator, CleanupListT<iterator>> result{};
				/*auto& it_equal		{ result.first };
				auto& cleanup_list	{ result.second };*/
				iterator it_equal{};
				CleanupListT<iterator> cleanup_list{};

				if (auto searched_shared = searched_ptr.lock()) {
					auto it_current{ container.begin() };	// for cleanup list
					auto equal_owner = [&searched_shared, &cleanup_list, &it_current](const auto& current_ptr) {
						if (current_ptr.expired()) { cleanup_list.push_back(it_current); }
						++it_current;
						return IsEqualWeakPtr(searched_shared, current_ptr);
						};
					it_equal = std::find_if(policy, container.begin(), container.end(), equal_owner);
				}
				else { // searched_ptr is expired
					it_equal = container.end();
				}
				// Cleanup expired weak_ptr
				EraseElementsByIterators(container, cleanup_list, ExecPolicyT());
				return it_equal;
			}

			/**
			 * Erase first weak_ptr in container, that is alive and has same stored pointer. Container stores weak_ptr..
			 *
			 * \param container
			 * \param searched_ptr
			 * \param policy
			 * @return	count of expired weak_ptr
			 */
			template<typename ContainerT, typename ExecPolicyT>
			inline void EraseEqualWeakPtr_Cleanup(ContainerT& container, typename ContainerT::value_type searched_ptr,
				ExecPolicyT policy = std::execution::seq) {
				//static_assert(std::is_same_v<std::weak_ptr<ValueT>, typename ContainerT::value_type>);

				//using iterator = typename ContainerT::const_iterator;
				using iterator = decltype(container.end());
				std::pair<iterator, CleanupListT<iterator>> result{};
				auto& it_equal{ result.first };
				auto& cleanup_list{ result.second };

				if (auto searched_shared = searched_ptr.lock()) {
					auto end{ container.end() };
					if constexpr (std::is_same_v<ContainerT, std::forward_list<typename ContainerT::value_type>>) {
						auto it_current{ container.begin() };	// for cleanup list
						auto equal_owner = [&searched_shared, &cleanup_list, &it_current](const auto& current_ptr) {
							if (current_ptr.expired()) { cleanup_list.push_back(it_current); }
							++it_current;
							return IsEqualWeakPtr(searched_shared, current_ptr);
							};
						container.remove_if(equal_owner);
					}
					else {
						result = FindEqualWeakPtr(container, searched_ptr, policy);
						if (it_equal != end) { container.erase(it_equal); }
					}
				}
				// Cleanup expired weak_ptr
				EraseElementsByIterators(container, cleanup_list, ExecPolicyT());
			}

		} // !namespace observer_weak_ptr


	} // !namespace behavioral

} // !namespace pattern

#endif // !OBSERVER_OTHERS_HPP
