#ifndef OBSERVER_WEAK_MULTI_HPP
#define OBSERVER_WEAK_MULTI_HPP

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
#include "memory-management-library/weak-ptr/weak-ptr.hpp"
//#include "error/error.hpp"


// TODO: maybe separate classes to different files to make less includes

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {

		namespace observer_weak_multi {
			//=============================Multi Observer Interfaces===========================================
			// !!!!Very, very rare use case of ISubjectWeakMulti, IObserverWeakMulti interfaces.
			// Use only for very unique situation, when One Observer can observ to many Subjects.

			/*
			* 1. Increased complexity of state management. If an observer is subscribed to multiple objects, it has to track changes from
			* multiple sources. Each update will require checking the state of different objects, which complicates the processing logic
			* and increases the likelihood of errors.
			*
			* 2. Performance issues. When one event occurs on one object, it must go through a queue of handlers associated with it.
			* The more objects are associated with an observer, the more difficult it becomes to determine the source of the event
			* and choose the correct behavior.
			*
			* 3. Opacity of relationships. Implementation of the multiple subscription mechanism increases the number of connections
			* between system components, which makes it difficult to understand the program architecture and support it.
			*
			* 4. State dependency. Objects often depend on each other, and subscribing to multiple objects at the same time can
			* lead to conflicts and cyclic dependencies that violate data integrity.
			*
			* While it is possible to implement an observer that tracks multiple objects, this solution makes the system more complex
			* and error-prone. Most often, a simple model where one observer tracks one object is sufficient. This approach helps
			* to keep the structure clear and reduces the risk of side effects and synchronization issues.
			*/

			using namespace util;

			class ISubjectWeakMulti;

			/**
			 * Abstract. Stateless. Interface for Observer weak_ptr with multiple subjects.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 * Use this class with shared_ptr arguments.
			 */
			class IObserverWeakMulti {
			public:
				using ISubjectT = ISubjectWeakMulti;
				using WeakPtrISubjectT = std::weak_ptr<ISubjectT>;
				// T will make subject unmutable
				using WeakPtrConstISubjectT = const std::weak_ptr<const ISubjectWeakMulti>;

			protected:
				IObserverWeakMulti() = default;
				IObserverWeakMulti(const IObserverWeakMulti&) = delete; // C.67	C.21
				IObserverWeakMulti& operator=(const IObserverWeakMulti&) = delete;
				IObserverWeakMulti(IObserverWeakMulti&&) noexcept = delete;
				IObserverWeakMulti& operator=(IObserverWeakMulti&&) noexcept = delete;
			public:
				virtual ~IObserverWeakMulti() = default;

				/** Update the information about Subject */
				virtual void Update() = 0;

				/**
				 * Update the information about Subject.
				 * The task of Observer to get notification and to react on it. Not to change the state of Subject.
				 * Subject mustn't change state while updating observers, cause updatable state of subject must
				 * be equal for all observers, in notification process.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				virtual void Update(const WeakPtrConstISubjectT subject_ptr) = 0;

				/**
				 * Add pair Subject in Observer & Observer in Subject.
				 * In Observer Add Subject to list of observing. In Subject Add Observer to list of Observers.
				 *
				 * @param subject
				 * @param recursion_depth	0 is default. 0 recursion depth = Attach Subject & Observer.
				 *							1 recursion depth = Attach only Subject.
				 * @return
				 */
				virtual void AttachSubject(WeakPtrISubjectT subject_ptr, size_t recursion_depth) = 0;
				// not const, cause must attach observer in subject ptr

				/**
				 * Detach pair Subject in Observer & Observer in Subject.
				 * In Observer Detach Subject, when Subject is destructed. In Subject Detach Observer.
				 *
				 * @param subject				subject, that is destructing.
				 * @param recursion_depth		0 is default. 0 recursion depth = Attach Subject & Observer.
				 *								1 recursion depth = Attach only Subject.
				 */
				virtual void DetachSubject(WeakPtrISubjectT subject_ptr, size_t recursion_depth) = 0;
				// subject mustn't be const, cause mutate deleting observer in subject

				/**
				 * Detach n expired weak_ptr in container.
				 *
				 * \param expired_count
				 * \param to_erase_all_expired	if true, every expired object in container will be Detached
				 */
				virtual void DetachNExpired(const size_t expired_count, bool to_erase_all_expired = false) = 0;

				/** Check if there is subject reference in Observer */
				virtual bool HasSubject(const WeakPtrISubjectT subject_ptr) = 0;
			};

			/**
			 * Abstract. Stateless. Subject with weak_ptr dependencies.
			 * Alternative name - Publisher.
			 * May be used for IObserverWeak, IObserverWeakMulti.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 */
			class ISubjectWeakMulti {
			public:
				using IObserverT = IObserverWeakMulti;
				using WeakPtrIObserverT = std::weak_ptr<IObserverT>;

			protected:
				ISubjectWeakMulti() = default;
				ISubjectWeakMulti(const ISubjectWeakMulti&) = delete; // C.67	C.21
				ISubjectWeakMulti& operator=(const ISubjectWeakMulti&) = delete;
				ISubjectWeakMulti(ISubjectWeakMulti&&) noexcept = delete;
				ISubjectWeakMulti& operator=(ISubjectWeakMulti&&) noexcept = delete;
			public:
				virtual ~ISubjectWeakMulti() = default;

				/** Update all attached observers */
				virtual void NotifyObservers() = 0;
				// not const, cause cleanup operations

				/**
				 * Add pair Observer in Subject & Subject in Observer.
				 * There is shared_ptr<ObserverT> outside of class. Subject stores weak_ptr to that Observer.
				 *
				 * @param recursion_depth		0 is default. 0 recursion depth = Attach Observer & Subject.
				 *								1 recursion depth = Attach only Observer.
				 */
				virtual void AttachObserver(WeakPtrIObserverT observer_ptr, size_t recursion_depth) = 0;
				// not const, cause must attach subject in observer ptr

				/**
				 * Detach pair Observer in Subject & Subject in Observer.
				 *
				 * @param observer_ptr
				 * @param recursion_depth		0 is default. 0 recursion depth = Attach Observer & Subject.
				 *								1 recursion depth = Attach only Observer.
				 */
				virtual void DetachObserver(WeakPtrIObserverT observer_ptr, size_t recursion_depth) = 0;

				/**
				 * Detach n expired weak_ptr in container.
				 *
				 * \param expired_count
				 * \param to_erase_all_expired	if true, every expired object in container will be Detached
				 */
				virtual void DetachNExpired(const size_t expired_count, bool to_erase_all_expired = false) = 0;

				/** Check if there is observer in Subject */
				virtual bool HasObserver(const WeakPtrIObserverT observer_ptr) = 0;
			};
			/*
			* Design choices:
			* 1) Maybe return bool of Attach & Detach, cause expired ptrs?
			*/


			//======================================================================================
			// Very very very rare, when you need observer with many observable subjects and with list of Subjects.
			// ObserverWeakMulti, SubjectWeakMulti are not main classes


			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 * One Observer may has many Subjects.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 * Don't use unordered_set with weak_ptr, cause may give undefined behavior.
			 * Not recommend to use vector, as bad performance of remove_if.
			 *
			 * Invariant: don't attach expired weak_ptr. Value type must be weak_ptr. Mustn't duplicate weak_ptr.
			 *
			 * [[Testlevel(35, Hand debug)]]
			 */
			template<typename ExecPolicyT = std::execution::sequenced_policy,
				typename ContainerT_t = std::list<std::weak_ptr<ISubjectWeakMulti>>>
			requires std::is_execution_policy_v<ExecPolicyT>
			class [[deprecated("Very rare use case of class. Old code.")]]
				ObserverWeakMulti : public IObserverWeakMulti,
									public std::enable_shared_from_this<ObserverWeakMulti<typename ExecPolicyT,
																						typename ContainerT_t>>
				// weak_from_this is for creating shared_ptr from
				// this without doubled control block of shared_ptr with the outside shared_ptr
			{
			public:
				using value_type = typename ContainerT_t::value_type;
				using container_type = ContainerT_t;

				using ISubjectT = ISubjectWeakMulti;
				using WeakPtrISubjectT = std::weak_ptr<ISubjectT>;
				using IObserverT = IObserverWeakMulti;
				// T will make subject unmutable
				using WeakPtrConstISubjectT = IObserverWeakMulti::WeakPtrConstISubjectT;

				//static_assert(std::is_object_v<ISubjectT_t>, "The C++ Standard forbids containers of non-object types "
																//"because of [container.requirements].");

				// Container Ts Alternatives
				using ContainerForwardListT = std::forward_list<WeakPtrISubjectT>;
				//using ContainerListT = std::list<WeakPtrISubjectT>;
				//using ContainerVectorT = std::vector<WeakPtrISubjectT>;
				//using ContainerDequeT = std::deque<WeakPtrISubjectT>;

				ObserverWeakMulti() = default;
				// Observer may be constructed without observable Subjects

				/**
				 * Constructs Observer from Subject.
				 * Class mustn't attach expired weak_ptr and spam internal container.
				 *
				 * @param subject from which Observer is constructed
				 * @param self_ptr self pointer, that owns Observer and for which Observer is constructed.
				 */
				 /*explicit ObserverWeakMulti(WeakPtrISubjectT subject_ptr) {
					 if (auto subject_shared{ subject_ptr.lock() }) {
						 if constexpr (std::is_same_v<ContainerT_t, ContainerForwardListT>) {
							 subjects_.emplace_after(subjects_.cbefore_begin(), std::move(subject_ptr));
						 } else {
							 subjects_.emplace(std::move(subject_ptr));
						 }
						 subject_shared->AttachObserverNSubject(this->weak_from_this(), kRecursDepthForSingleOperation);
					 }
				 }*/ // Problems with this->weak_from_this(), cause object is not constructed

			protected:
				ObserverWeakMulti(const ObserverWeakMulti&) = delete; // C.67	C.21
				ObserverWeakMulti& operator=(const ObserverWeakMulti&) = delete;
				ObserverWeakMulti(ObserverWeakMulti&&) noexcept = delete;
				ObserverWeakMulti& operator=(ObserverWeakMulti&&) noexcept = delete;
			public:
				/** Needs to Detach this Observer in all subjects */
				~ObserverWeakMulti() override { // weak_from_this is already expired
					auto detach_fn = [](auto& subject_ptr) {
						if (subject_ptr.expired()) return;
						if (auto subject_shared{ subject_ptr.lock() }) {
							// Don't need to detach Subjects in Observer, cause Observer will be destructed
							subject_shared->DetachNExpired(1);
						}
						};
					std::for_each(ExecPolicyT(), subjects_.begin(), subjects_.end(), detach_fn);
				};


				/** Update the information about Subject */
				void Update() override {
				};

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				void Update(const WeakPtrConstISubjectT subject_ptr) override {
				};


				/**
				 * Add pair Subject & Observer.
				 * In Observer Add Subject to list of observing. In Subject Add Observer to list of Observers.
				 * Only alive weak_ptr can be attached to container and only that is not in container already.
				 *
				 * @param subject_ptr
				 * @param add_observer_in_subj	if true, Observer will be added in Subject
				 */
				void AttachSubject(WeakPtrISubjectT subject_ptr, size_t recursion_depth = 0) override {
					if (subject_ptr.expired()) { return; } // precondition

					if (!HasSubject(subject_ptr)) { // Duplicate control. Mustn't duplicate weak_ptr
						generic::Emplace(subjects_, subject_ptr);
						//if constexpr (std::is_same_v<ContainerT_t, ContainerForwardListT>) {
						//	subjects_.emplace_after(subjects_.cbefore_begin(), subject_ptr);
						//}
						//else { // All other types of containers, except forward_list
						//	subjects_.emplace(subject_ptr);
						//}
					}
					if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
						if (auto subject_shared{ subject_ptr.lock() }) {
							subject_shared->AttachObserver(this->weak_from_this(), ++recursion_depth);
						}
					}
				};
				// not const, cause must attach subject in observer ptr

				/**
				 * Detach pair Subject & Observer.
				 * In Observer Detach Subject, when Subject is destructed. In Subject Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality define on alive objects.
				 *
				 * @param subject subject, that is destructing.
				 */
				inline void DetachSubject(WeakPtrISubjectT subject_ptr, size_t recursion_depth = 0) override {
					if (subject_ptr.expired()) { return; } // precondition Can Detach only alive objects

					// First Detach Observer in Subject
					if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
						if (auto subject_shared{ subject_ptr.lock() }) {
							subject_shared->DetachObserver(this->weak_from_this(), ++recursion_depth);
						}
					}
					EraseEqualOwner(subjects_, subject_ptr, ExecPolicyT());

					// Cleanup expired weak_ptr
					//EraseNExpired(container, expired_count, policy);
				};

				/**
				 * Detach n expired weak_ptr in container.
				 *
				 * \param expired_count
				 * \param to_erase_all_expired	if true, every expired object in container will be Detached
				 */
				inline void DetachNExpired(const size_t expired_count, bool to_erase_all_expired = false) override {
					if (to_erase_all_expired) { EraseAllExpired(subjects_, ExecPolicyT()); }
					else { EraseNExpired(subjects_, expired_count, ExecPolicyT()); }
					// if subject is expired, it is deleted, so we don't need to detach observer in subject
				};

				/**
				 * Check if there is subject reference in Observer.
				 * Cleanup expired weak_ptr subjects in container.
				 *
				 * \param subject_ptr
				 * \param policy
				 * \return
				 */
				inline bool HasSubject(const WeakPtrISubjectT subject_ptr) override {
					return FindEqualOwner(subjects_, subject_ptr, ExecPolicyT()) != subjects_.cend();
				};
				// not const, cause autoclean

			private:
				/**
				 * Depth of recursion function for Attach, Detach funcs for Single operations.
				 * Single operation - f.e. attach not pair Observer-Subject, but only Observer.
				 */
				static constexpr size_t kRecursDepthForSingleOperation{ 1 };

				/** Subjects for which we will be notified. */
				container_type subjects_{};
				// Is not const, cause Attach, Detach functions call.

			};	// !class ObserverWeakMulti
			/*
			* https://stackoverflow.com/questions/13695640/how-to-make-a-c11-stdunordered-set-of-stdweak-ptr
			* weak_ptr is not stable element of container and is the worst choice for std::unordered_set.
			* Can work in pair with stable element f.e. std::map<string, weak_ptr>.
			* hash of locked weak_ptr may be changed if stored object is destructed.
			* lock of expired weak_ptr returns shared_ptr() empty pointer with zero hash.
			* So undefined behaviour is possible.
			*
			* Design choices of recursive AttachObserver:
			* 1) friend AttachSubject in Observer gives access to private AttachSubject of Observer in Subject AttachObserver().
			* Advantages: High performance, clean code.
			* Disadvantages: Huge dependency of Observer and Subject Classes.
			* 2) Recursive AttachObserver with recurse depth parameter.
			* Advantages: Normal performance, no dependency.
			* Disadvantages: change interface with additional parameter. User of class can add only Observer in Subject &
			* forget to add Subject in Observer. So there can be Subject, Observer without pair.
			* 3) Only by conditional if in recursion to exit out of recursion.
			* Advantages: no dependency.
			* Disadvantages: Bad performance, cause of additional checks in HasObserver, HasSubjects with multiple locks
			* of weak_ptr in container.
			* Best) is 2), cause no dependency & performance is very important for Observer pattern
			*
			* Design choices. Params in functions. Weak_ptr vs shared_ptr:
			*
			* Best) weak_ptr in params gives guaranty, that we won't use destructed objects.
			* 2) If we use const shared_ptr& outside code must guaranty, that object will be alive, while function is working.
			*/



			// TODO: Broken auto clean of expired weak_ptr functionality.

			/**
			 * Concrete. Observers will be notified on Subject state changes.
			 * One Subject can has many Observers.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 * Not recommend to use vector, as bad performance of remove_if.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 * Invariant: don't attach expired weak_ptr. Value type must be weak_ptr. Mustn't duplicate weak_ptr.
			 *
			 * [[Testlevel(35, Hand debug)]]
			 */
			template<typename ExecPolicyT = std::execution::sequenced_policy,
				typename ContainerT_t = std::list<std::weak_ptr<IObserverWeakMulti>> >
				requires std::is_execution_policy_v<ExecPolicyT>
			class SubjectWeakMulti : public ISubjectWeakMulti,
				public std::enable_shared_from_this<SubjectWeakMulti<typename ExecPolicyT,
				typename ContainerT_t>> {
			public:
				using value_type = typename ContainerT_t::value_type;
				using container_type = ContainerT_t;
				using iterator = typename container_type::iterator;

				using IObserverT = IObserverWeakMulti;
				using WeakPtrIObserverT = std::weak_ptr<IObserverT>;
				using ISubjectT = ISubjectWeakMulti;

				//static_assert(std::is_object_v<ISubjectT_t>, "The C++ Standard forbids containers of non-object types "
																//"because of [container.requirements].");

				// Container Ts Alternatives
				using ContainerForwardListT = std::forward_list<WeakPtrIObserverT>;
				//using ContainerListT = std::list<WeakPtrIObserverT>;
				//using ContainerVectorT = std::vector<WeakPtrIObserverT>;
				//using ContainerDequeT = std::deque<WeakPtrIObserverT>;

				SubjectWeakMulti() = default;
				// Subject may be constructed without observable Subjects

			protected:
				SubjectWeakMulti(const SubjectWeakMulti&) = delete; // C.67	C.21
				SubjectWeakMulti& operator=(const SubjectWeakMulti&) = delete;
				SubjectWeakMulti(SubjectWeakMulti&&) noexcept = delete;
				SubjectWeakMulti& operator=(SubjectWeakMulti&&) noexcept = delete;
			public:
				/** Needs to Detach this Subject in all subjects */
				~SubjectWeakMulti() override {
					auto detach_fn = [](auto& observer_ptr) {
						if (observer_ptr.expired()) return;
						if (auto observer_shared{ observer_ptr.lock() }) {
							// Don't need to detach Observers in Subject, cause Subject will be destructed
							observer_shared->DetachNExpired(1);
						}
						};
					std::for_each(ExecPolicyT(), observers_.begin(), observers_.end(), detach_fn);
				};


				/** Update all attached observers */
				inline void NotifyObservers() override { // not const, cause cleanup operations
					size_t expired_count{};
					auto update_fn = [&expired_count](const auto& observer_ptr) {
						if (observer_ptr.expired()) { ++expired_count; }
						else if (auto observer_shared{ observer_ptr.lock() }) {
							observer_shared->Update();
						}
						};
					std::for_each(ExecPolicyT(), observers_.begin(), observers_.end(), update_fn);
					// Cleanup expired weak_ptr
					EraseNExpired(observers_, expired_count, ExecPolicyT());

				};

				/** Update all attached observers */
				virtual inline void NotifyObserversMulti() { // not const, cause cleanup operations
					size_t expired_count{};
					auto weak_this{ this->weak_from_this() };
					auto update_fn = [&weak_this, &expired_count](const auto& observer_ptr) {
						if (observer_ptr.expired()) { ++expired_count; }
						else if (auto observer_shared{ observer_ptr.lock() }) {
							observer_shared->Update(weak_this);
						}
						};
					std::for_each(ExecPolicyT(), observers_.begin(), observers_.end(), update_fn);
				};

				// TODO: Attach Observers() initializer_list, vector. Other operations with multiple observers.

				/**
				 * Add pair Observer & Subject.
				 * In Observer Add Subject to list of observing. In Subject Add Observer to list of Observers.
				 * Only alive weak_ptr can be attached to container and only that is not in container already.
				 *
				 * @param subject_ptr
				 * @param add_observer_in_subj	if true, Observer will be added in Subject
				 */
				void AttachObserver(WeakPtrIObserverT observer_ptr, size_t recursion_depth = 0) override {
					if (observer_ptr.expired()) { return; } // precondition

					if (!HasObserver(observer_ptr)) { // Duplicate control. Mustn't duplicate weak_ptr
						generic::Emplace(observers_, observer_ptr);
						//if constexpr (std::is_same_v<ContainerT_t, ContainerForwardListT>) { // if ForwardList
						//	observers_.emplace_after(observers_.cbefore_begin(), observer_ptr);
						//}
						//else { // All other types of containers, except forward_list
						//	observers_.emplace(observer_ptr);
						//}
					}

					if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
						if (auto observer_shared{ observer_ptr.lock() }) { // observer_ptr is not expired
							observer_shared->AttachSubject(this->weak_from_this(), ++recursion_depth);
						}
					}
				};
				// not const, cause must attach subject in observer ptr

				/**
				 * Detach pair Subject & Observer.
				 * In Observer Detach Subject, when Subject is destructed. In Subject Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality define on alive objects.
				 *
				 * @param subject subject, that is destructing.
				 */
				void DetachObserver(WeakPtrIObserverT observer_ptr, size_t recursion_depth = 0) override {
					if (observer_ptr.expired()) { return; } // precondition

					// First Detach Observer in Subject
					if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
						// Can Detach only alive objects
						if (auto observer_shared{ observer_ptr.lock() }) {
							observer_shared->DetachSubject(this->weak_from_this(), ++recursion_depth);
						}
					}
					EraseEqualOwner(observers_, observer_ptr, ExecPolicyT());

					// Cleanup expired weak_ptr
					//EraseNExpired(container, expired_count, policy);
				};

				/**
				 * Detach n expired weak_ptr in container.
				 *
				 * \param expired_count
				 * \param to_erase_all_expired	if true, every expired object in container will be Detached
				 */
				inline void DetachNExpired(const size_t expired_count, bool to_erase_all_expired = false) override {
					if (to_erase_all_expired) { EraseAllExpired(observers_, ExecPolicyT()); }
					else { EraseNExpired(observers_, expired_count, ExecPolicyT()); }
					// if subject is expired, it is deleted, so we don't need to detach observer in subject
				}; // TODO: refactor, maybe extract function.

				/**
				 * Check if there is subject reference in Observer.
				 * Cleanup expired weak_ptr subjects in container.
				 *
				 * \param subject_ptr
				 * \param policy
				 * \return
				 */
				inline bool HasObserver(const WeakPtrIObserverT observer_ptr) override {
					return FindEqualOwner(observers_, observer_ptr, ExecPolicyT()) != observers_.end();
				};
				// not const, cause autoclean

			private:
				/**
				 * Depth of recursion function for Attach, Detach funcs for Single operations.
				 * Single operation - f.e. attach not pair Observer-Subject, but only Observer.
				 */
				static constexpr size_t kRecursDepthForSingleOperation{ 1 };

				/**
				 * List of observers, that will be attach to observable object.
				 * Subject is not interested in owning of its Observers.
				 * So can be used weak_ptr, created from shared_ptr.
				 *
				 * Design: If there is too many subjects with few observers you can use hash table.
				 */
				container_type observers_{};
				// Is not const, cause Attach, Detach functions call.

			};	// !class SubjectWeakMulti





			struct State {
				int a_{ 0 };
				int b_{ 0 };
			};

			//class MySubject : public SubjectWeakMulti<std::forward_list<std::weak_ptr<IObserverWeakMulti>>> {
			template<typename ExecPolicyT = std::execution::sequenced_policy>
			class MySubject : public SubjectWeakMulti<typename ExecPolicyT> {
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

			//class MyObserver : public ObserverWeakMulti<std::forward_list<std::weak_ptr<ISubjectWeakMulti>>> {
			template<typename ExecPolicyT = std::execution::sequenced_policy>
			class MyObserver : public ObserverWeakMulti<ExecPolicyT> {
			public:
				using WeakPtrConstISubjectT = ObserverWeakMulti<ExecPolicyT>::WeakPtrConstISubjectT;

				/*explicit MyObserver(WeakPtrISubjectT subject_ptr) : ObserverWeakMulti(subject_ptr) {
				}*/
				MyObserver() = default;
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
				void Update(const WeakPtrConstISubjectT subject_ptr) override {
					//state_observer_ = subject
				};

				State state_{};
			};

		} // !namespace observer_weak_multi

	} // !namespace behavioral

} // !namespace pattern

#endif // !OBSERVER_WEAK_MULTI_HPP
