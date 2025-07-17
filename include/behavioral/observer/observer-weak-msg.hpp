#ifndef WEAK_OBSERVER_HPP
#define WEAK_OBSERVER_HPP

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

// Externals
#include "diagnostics-library/custom-exception.hpp"
#include "memory-management-library/weak-ptr/weak-ptr.hpp"


#include "behavioral/observer/iobserver.hpp"
#include "behavioral/observer/generic-observer.hpp"


/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		// https://en.wikipedia.org/wiki/Observer_pattern
		// https://eliogubser.com/posts/2021-04-cpp-observer/	Simple observer pattern using modern C++
		//		eliogubser Used Function wrapper for Update command.
		//
		// Friend patterns: Mediator. ChangeManager act like mediator between Observers and Subjects,
		//		encapsulate complex mechanics of Update.
		// Singleton is used in ChangeManager.
		// Command. Realization of Observer may store Delegates in Subject to Observer's functions.
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

		/**
		* Update function may be called in Subject's Notify() in mode Sync or Async (Message, Events Queue).
		* You can use parallel threads, if Observer's Update() is huge, slow, resource eating code.
		* Or if there is huge count of observers.
		*
		* Lapsed Listener Problem - memory leak, if there is strong references, pointers to Observers.
		*
		* Message Queue. By creating a dedicated message queue server (and sometimes an extra message handler
		* object) as an extra stage between the observer and the object being observed, thus decoupling the components.
		*
		* Change Manager - is Mediator Pattern, that helps to move all refs to Observer from Subject to Manager.
		* Change Manager is best solution for complex update logic.
		*/

		// Observers always responsible for controlling relationship between Subjects and Observers. Not by Subjects.
		// Subjects only notify Observers and don't know anything about them.
		/*
		* Most popular realization of Observer Pattern is, when one Observer is observing one Subject.
		* This realization give opportunity to have create of events and decrease dependency.
		*/


		// Shared_ptr on observers in Subject destroy all architecture of application, cause of risk of cycle shared_ptr in system.


		namespace observer_boost {
			// https://www.boost.org/doc/libs/1_63_0/doc/html/signals.html

			/*
			* Boost.Signals. Can be a good alternative to implementing the Observer pattern in C++.
			* They provide a flexible and efficient way to manage event-driven programming and
			* facilitate communication between objects.
			*/
		}


		namespace observer_weak_msg {
			using namespace ::pattern::behavioral::iobserver;
			//using namespace pattern::behavioral::observer;

			class ObserverMsg : public IObserverMsg {
			public:
				ObserverMsg() = default;

			protected:
				ObserverMsg(const ObserverMsg&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				ObserverMsg& operator=(const ObserverMsg&) = delete;
				ObserverMsg(ObserverMsg&&) noexcept = delete;
				ObserverMsg& operator=(ObserverMsg&&) noexcept = delete;
			public:
				~ObserverMsg() override = default;

				/** Update the information about Subject */
				void Update(const std::string& message) override {
				}
			};

			/**
			 * Notify all observers in container by lambda function, that encapsulate observer update function call.
			 * Auto clean expired observers from container.
			 * Can freeze main thread, if it is long to Update observer.
			 *
			 * Complexity: O(n) + O(k), k - count of expired callbacks
			 * Mutex: write
			 *
			 * @param observers					observers container
			 * @param observer_method			lambda that encapsulate observer update function call, f.e. observer->Update()
			 */
			template<typename ContainerType, typename UpdateFunctionType,
					typename ExecPolicyT = std::execution::sequenced_policy>
			inline void NotifyWeakObserversNClean(ContainerType& observers,
													UpdateFunctionType observer_method,
													ExecPolicyT policy = std::execution::seq) {
				if (observers.empty()) { return; } // precondition

				auto update_fn = [&observer_method](const auto& observer_ptr) { // return true, when observer expired
					if (auto observer_shared{ observer_ptr.lock() }) {
						observer_method(observer_shared);
						return false;
					}
					return true;
				}; // !lambda

				generic::RemoveIf(observers, update_fn, policy);		// O(n) + O(k), k - count of expired callbacks
			}


			/**
			 * Concrete. Observers will be notified on Subject state changes.
			 * One Subject can has many Observers.
			 * Gives opportunity to communicate with different layers of the application.
			 * Plus, dependency is weak, cause it is dependency to an abstract object.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 *
			 * Not recommend to use vector, as bad performance Detach.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 *
			 * Invariant: don't attach & store expired weak_ptr. Mustn't duplicate weak_ptr.
			 *
			 * Complexity: maybe best for std::set = O(log n). But CleanOps are O(n)
			 *
			 * @tparam UpdateDataT		type of update data in param of update function in observer
			 * @tparam ExecPolicyT		Execution policy (e.g., std::execution::sequenced_policy)
			 * @tparam ContainerT_t		Container type holding weak_ptrs to observers
			 *
			 * [[Testlevel(35, Hand debug)]]
			 */
			template<typename ContainerT = std::list< std::weak_ptr<IObserverMsg> > >
			class SubjectWeakMsg : public ISubjectWeakMsg
				//public std::enable_shared_from_this<SubjectWeak<typename ExecPolicyT, typename ContainerT_t>>
			{
			public:
				using value_type          = typename ContainerT::value_type;
				using WeakPtrIObserverMsg = std::weak_ptr<IObserverMsg>;

				static_assert(std::is_object_v<value_type>, "The C++ Standard forbids containers of non-object types "
															"because of [container.requirements].");

				// Container Ts Alternatives
				using ContainerList = std::list<WeakPtrIObserverMsg>;
				//using ContainerVector		= std::vector<WeakPtrIObserverWeakHub>;
				//using ContainerSet			= std::set<WeakPtrIObserverWeakHub, std::owner_less<WeakPtrIObserverWeakHub>>;
				//using ContainerForwardList	= std::forward_list<WeakPtrIObserverWeakHub>;
				//using ContainerDeque		= std::deque<WeakPtrIObserverWeakHub>;


				SubjectWeakMsg() = default;
				// Subject may be constructed without observable Subjects

				template<typename IteratorT, typename ExecPolicyT = std::execution::sequenced_policy>
				SubjectWeakMsg(IteratorT begin, IteratorT end, ExecPolicyT policy = std::execution::seq) {
					AttachObserver(begin, end, policy);
				}

				template<typename ExecPolicyT = std::execution::sequenced_policy>
				explicit SubjectWeakMsg(const std::initializer_list<WeakPtrIObserverMsg>& init_list,
										ExecPolicyT policy = std::execution::seq) {
					//: observers_{ init_list.begin(), init_list.end() } {
					AttachObserver(init_list.begin(), init_list.end(), policy);
				};

				template<typename ContainerT, typename ExecPolicyT = std::execution::sequenced_policy>
				explicit SubjectWeakMsg(const ContainerT& container,
										ExecPolicyT policy = std::execution::seq) {
					static_assert(std::is_same_v<typename ContainerT::value_type, WeakPtrIObserverMsg>,
								"Elements of container, when AttachObserver must be weak_ptr to observer interface");
					AttachObserver(container.begin(), container.end(), policy);
				};

				template<typename ExecPolicyT = std::execution::sequenced_policy>
				explicit SubjectWeakMsg(const WeakPtrIObserverMsg observer_ptr,
										ExecPolicyT policy = std::execution::seq) {
					AttachObserver(observer_ptr, policy); // No differrence with std::move(observer_ptr)
				};

				template<typename ExecPolicyT = std::execution::sequenced_policy>
				explicit SubjectWeakMsg(const std::shared_ptr<IObserverMsg> observer_ptr,
										ExecPolicyT policy = std::execution::seq) {
					AttachObserver(std::weak_ptr<IObserverMsg>(observer_ptr), policy); // No differrence with std::move(observer_ptr)
				};

			protected:
				SubjectWeakMsg(const SubjectWeakMsg&) = delete; // C.67	C.21
				SubjectWeakMsg& operator=(const SubjectWeakMsg&) = delete;
				SubjectWeakMsg(SubjectWeakMsg&&) noexcept = delete;
				SubjectWeakMsg& operator=(SubjectWeakMsg&&) noexcept = delete;
			public:
				~SubjectWeakMsg() override = default;

//_____________________________________________________________________________________________________

				// TODO: write notify methods for all stubs in ObserverHub class. With all variants of Update fn of observer.

				/*
				 * Generic Update attached observers using any observer update method.
				 * It is better to call in another thread using thread pool.
				 *
				 * Complexity: set = O(log n), Other containers = O(n).
				 *
				 * @param observer_method	functor with signature: void (std::shared_ptr<IObserverMsg> observer_ptr)
				 */
				template<typename UpdateFunctionType, typename ExecPolicyT = std::execution::sequenced_policy>
				inline void GenericNotifyObservers(UpdateFunctionType observer_method,
													ExecPolicyT policy = std::execution::seq) const {
					std::lock_guard lock{ observers_mtx_ };
					NotifyWeakObserversNClean(observers_, observer_method, policy);	// write = erase expired
				}

				/*
				 * Update attached observers.
				 * It is better to call in another thread using thread pool.
				 *
				 * Complexity: set = O(log n), Other containers = O(n).
				 *
				 * @param message	message with information needed for Update.
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				void NotifyObservers(const std::string& message,
									ExecPolicyT policy = std::execution::seq) const {
					auto observer_method = [&message](std::shared_ptr<IObserverMsg> observer_ptr) {
						// is shared_ptr, cause in GenericNotify it is locked
						observer_ptr->Update(message);
					}; // observer Update method

					GenericNotifyObservers(observer_method, policy);
				};

				/*
				 * Update all attached observers in main thread.
				 * It is better to call in another thread using thread pool.
				 *
				 * Complexity: set = O(log n), Other containers = O(n).
				 *
				 * @param message	message with information needed for Update.
				 */
				void NotifyObservers(const std::string& message = "") const override {
					NotifyObservers(message, std::execution::seq);
				};
				// TODO: Notification in another thread. Wait the end of notification process
				// TODO: bool run_in_new_thread = false
				// In new std::thread
				// Async variant

//_____________________________________________________________________________________________________

				/**
				* Add Multiple Observer to list of Observers.
				* Only alive weak_ptr can be attached to container and only that is not duplicates.
				* Use only IObserverMsg, cause template can't convert implicitly.
				*
				* Complexity: O(K), K - count of attachable observers.
				* Complexity of adding each observer: maybe O(1), but is O(n), cause CleanOps.
				*
				* @param attachable_begin		start of range of observers, that will be added to Subject
				* @param attachable_end			end of range of observers, that will be added to Subject
				*/
				template<typename IteratorT, typename ExecPolicyT = std::execution::sequenced_policy>
				void AttachObserver(IteratorT attachable_begin, IteratorT attachable_end,
									ExecPolicyT policy = std::execution::seq)
				{ // Main logic in this function to decrease count of function call, so lower overhead resources for call of fn.
					static_assert(std::is_same_v< std::remove_cvref_t<decltype(*attachable_begin)>, WeakPtrIObserverMsg>,
									"Iterator be dereferencable to weak_ptr to observer message interface");
					if (attachable_begin == attachable_end) { return; }	// Precondition

					auto attach_observer_fn = [this, policy](const auto& observer_ptr) {
						if ( !util::HasValueNClean(observers_, observer_ptr, policy) ) {
							// Duplicate control. Mustn't duplicate weak_ptr
							generic::Emplace(observers_, observer_ptr);		// O(1)
						}
					}; // !lambda

					std::lock_guard lock{ observers_mtx_ };
					std::for_each(policy, attachable_begin, attachable_end, attach_observer_fn); // write	O(n)
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				inline void AttachObserver(const std::initializer_list<WeakPtrIObserverMsg>& init_list,
											ExecPolicyT policy = std::execution::seq) {
					if (init_list.size() == 0) { return; }	// Precondition
					AttachObserver(init_list.begin(), init_list.end(), policy);
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 */
				template<typename ContainerT, typename ExecPolicyT = std::execution::sequenced_policy>
				inline void AttachObserver(const ContainerT& container, ExecPolicyT policy = std::execution::seq) {
					static_assert(std::is_same_v<typename ContainerT::value_type, WeakPtrIObserverMsg>,
						"Elements of container, when AttachObserver must be weak_ptr to observer interface");
					if (container.empty()) { return; }	// Precondition

					AttachObserver(container.begin(), container.end(), policy);
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 *
				 * @param observer_ptr	weak pointer to observer.
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				inline void AttachObserver(const WeakPtrIObserverMsg observer_ptr,
											ExecPolicyT policy = std::execution::seq) {
					if (observer_ptr.expired()) { return; }	// Precondition
					AttachObserver({ observer_ptr }, policy);	// for HasObserver
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 *
				 * @param observer_ptr	weak pointer to observer.
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				inline void AttachObserver(const std::shared_ptr<IObserverMsg> observer_ptr,
											ExecPolicyT policy = std::execution::seq) {
					AttachObserver(std::weak_ptr<IObserverMsg>(observer_ptr), policy);	// for HasObserver
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 *
				 * Complexity: O(n)
				 *
				 * @param observer_ptr	weak pointer to observer.
				 */
				inline void AttachObserver(const WeakPtrIObserverMsg observer_ptr) override {
					AttachObserver(observer_ptr, std::execution::seq);
				};

//_____________________________________________________________________________________________________

				/**
				 * Detach Multiple Observers.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(k*n), k - count of detachable observers.
				 * Complexity of detaching each observer: maybe O(n), but is O(n), cause CleanOps.
				 *
				 * @param erasable_begin	begin of container with observers, that must be detached
				 * @param erasable_end		end of container with observers, that must be detached
				 */
				template<typename IteratorT, typename ExecPolicyT = std::execution::sequenced_policy>
				inline void DetachObserver(IteratorT erasable_begin, IteratorT erasable_end,
											ExecPolicyT policy = std::execution::seq) {
					static_assert(std::is_same_v< std::remove_cvref_t<decltype(*erasable_begin)>, WeakPtrIObserverMsg>,
								"Iterator be dereferencable to weak_ptr to observer interface");
					if (erasable_begin == erasable_end) { return; }	// Precondition

					auto detach_observer_fn = [this, policy](const auto& observer_ptr) {
						std::lock_guard lock{ observers_mtx_ }; // write
						// Can Detach only alive objects
						util::EraseEqualOwnerNClean(observers_, observer_ptr, policy);			// O(n)
						// TODO: optimize lock for sequential policy. It must be only one time before for_each
						// And parallel policy lock must be inside lambda.
					}; // !lambda

					std::for_each(policy, erasable_begin, erasable_end, detach_observer_fn); // write	O(k*n)
				};

				/**
				 * Detach Observer. Wrapper.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				inline void DetachObserver(const std::initializer_list<WeakPtrIObserverMsg>& init_list,
											ExecPolicyT policy = std::execution::seq) {
					if (init_list.size() == 0) { return; }	// Precondition
					DetachObserver(init_list.begin(), init_list.end(), policy);
				};

				/**
				 * Detach Observer. Wrapper.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 */
				template<typename ContainerT, typename ExecPolicyT = std::execution::sequenced_policy>
				inline void DetachObserver(const ContainerT& container,
											ExecPolicyT policy = std::execution::seq) {
					static_assert(std::is_same_v<typename ContainerT::value_type, WeakPtrIObserverMsg>,
								"Elements of container, when AttachObserver must be weak_ptr to observer interface");
					if (container.empty()) { return; }	// Precondition

					DetachObserver(container.begin(), container.end(), policy);
				};

				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 *
				 * @param observer_ptr weak pointer to observer.
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				inline void DetachObserver(const WeakPtrIObserverMsg observer_ptr,
											ExecPolicyT policy) {
					DetachObserver({ observer_ptr }, policy);
				};

				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 *
				 * @param observer_ptr weak pointer to observer.
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				inline void DetachObserver(const std::shared_ptr<IObserverMsg> observer_ptr,
											ExecPolicyT policy = std::execution::seq) {
					DetachObserver(std::weak_ptr<IObserverMsg>(observer_ptr), policy);
				};

				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 * Use only IObserverMsg, cause template can't convert implicitly.
				 *
				 * Complexity: O(n)
				 *
				 * @param observer_ptr weak pointer to observer.
				 */
				inline void DetachObserver(const WeakPtrIObserverMsg observer_ptr) override {
					DetachObserver(observer_ptr, std::execution::seq);
				};

//_____________________________________________________________________________________________________

				/**
				 * Detach all expired weak_ptr objects in container
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				inline void CleanupAllExpired(ExecPolicyT policy = std::execution::seq) const {
					std::lock_guard lock{ observers_mtx_ }; // write
					util::EraseAllExpired(observers_, policy);
					// if subject is expired, it is deleted, so we don't need to detach observer in subject
				};

				/**
				 * Check if there is observer in Subject.
				 * Cleanup expired weak_ptr subjects in container.
				 *
				 * Complexity: O(n)
				 *
				 * \param subject_ptr
				 * \param policy
				 * \return
				 */
				template<typename ExecPolicyT = std::execution::sequenced_policy>
				inline bool HasObserverNClean(const WeakPtrIObserverMsg searchable_ptr,
												ExecPolicyT policy = std::execution::seq) const {
					std::lock_guard lock{ observers_mtx_ };
					return util::HasValueNClean(observers_, searchable_ptr, policy); // write
				};

			private:
				///**
				// * Thread safe. Notify all observers of Subject in new thread of execution.
				// * Used mutex to block shared resource.
				// */
				//template<typename ThreadPoolT, typename UpdateFunctionT, typename ExecPolicyT = std::execution::sequenced_policy>
				//void ThreadNotifyObserversNClean(ThreadPoolT& thread_pool,
				//								UpdateFunctionT observer_update_fn,
				//								ExecPolicyT policy = std::execution::seq) const {
				//	//thread_pool.enqueue(&SubjectWeakHub::MutexNotifyObserversNClean, this, observer_update_fn, policy);
				//	thread_pool.enqueue([this, &observer_update_fn, &policy]() {
				//		MutexNotifyObserversNClean(observer_update_fn, policy);
				//		}
				//	);
				//}
				///**
				// * Thread safe. Notify all observers of Subject in new thread of execution.
				// * Used mutex to block shared resource.
				// */
				//template<typename ThreadPoolT, typename UpdateFunctionT, typename ExecPolicyT = std::execution::sequenced_policy>
				//void ThreadNotifyObserversNClean(UpdateFunctionT observer_update_fn,
				//								ExecPolicyT policy = std::execution::seq) const {
				//	// try catch may be deleted, cause it is critical error with termination
				//	std::thread notify_thread(&SubjectWeakHub::MutexNotifyObserversNClean, this, observer_update_fn, policy);
				//	notify_thread.detach();	// Thread will work separately
				//	// instead of detach thread object can be global, so you can monitor it
				//}

				///**
				// * Thread safe. Notify all observers of Subject in new thread of execution.
				// * Used mutex to block shared resource.
				// */
				//template<typename ThreadPoolT, typename UpdateFunctionT, typename ExecPolicyT = std::execution::sequenced_policy>
				//void ThreadNotifyObserversNCleanAsync(UpdateFunctionT observer_update_fn,
				//										ExecPolicyT policy = std::execution::seq) const {
				//	std::future result{ std::async(std::launch::async, &SubjectWeakHub::MutexNotifyObserversNClean,
				//							this, observer_update_fn, policy) };
				//}

//___________________________Data______________________________________________________________

				/**
				* List of observers, that will be attach to Subject.
				* Subject is not interested in owning of its Observers.
				* So can be used weak_ptr, created from shared_ptr.
				*
				* Design: If there is too many subjects with few observers you can use hash table.
				*/
				mutable ContainerT observers_{}; // mutable is for const fn notify function cleaning of expired weak_ptr

				/**
				 * Mutex for thread safety class operations.
				 */
				mutable std::mutex observers_mtx_{};
				/* Maybe order of concurent thread calls must be detach, attach, notify. First delete, then add, then notify */

			};	// !class SubjectWeakMsg
			/*
			* list
			* Pros: Efficient in terms of insertion and removal (O(1)), maintains order, can store duplicates.
			* - Good for scenarios where observers frequently join or leave.
			* Cons: Higher memory overhead due to pointers and less cache-friendly compared to arrays.
			* - Accessing elements by index is slower (O(n)).
			*
			* vector
			* Pros: Contiguous memory layout (better cache performance), faster access times using indices.
			* - Simple and straightforward to implement.
			* - If the number of observers is relatively stable and you do not expect frequent additions/removals, this is a good choice.
			* Cons: Inefficient for frequent insertions and removals (elements need to be shifted). Can be costly (O(n))
			*		since elements may need to be shifted.
			* - You will need to manually remove expired weak pointers when notifying observers.
			*
			* forward_list
			* Pros: Lower memory overhead than `std::list` because it only has one pointer per element, efficient
			*	for insertions and removals at the front.
			* Cons: Cannot access elements in the middle without iterating from the start, which makes it less flexible for random removals.
			*
			*
			* set
			* Pros: Automatically manages unique observers, allows for fast insertion and removal (O(log n)), sorted order.
			* Cons: No duplicates allowed, more memory overhead due to tree structure.
			*
			* unordered_set
			* Pros: Automatically handles uniqueness (no duplicates).
			* - Fast average-time complexity for insertions and removals (O(1)).
			* - The std::owner_less comparator allows for comparisons based on the underlying shared_ptr, which is useful for weak pointers.
			* Cons: Does not maintain any order of observers.
			* - Unstable for weak_ptr, cause if expired hash will change, so undefined behavior.
			* - Slightly higher memory overhead due to hash table.
			* - Stability of weak pointers: std::weak_ptr does not have a stable identity over its lifetime because if the object it
			*		points to expires, the internals of the weak_ptr can change, including the hash used in unordered_set. This instability
			*		makes it unreliable as keys in an unordered associative container.
			*
			* Recommendation
			* For scenarios where observers are frequently added and removed, **std::list** or **std::set** are generally the best choices:
			* - Use **std::list** if you need to allow duplicates and maintain insertion order.
			* - Use **std::set** if you need to ensure that observers are unique and you don’t mind the overhead for maintaining sorted order.
			*
			* Set in real cases can give worser performance then list on big size of data, cause additional multiplicators
			* in complexity formula. O(log n) < O(n). But in real cases it can be worser, f. e. O(5*log n) > O(n).
			*
			* Vector has better performance of notification iteration, then list, set.
			*/



			struct MyState {
				int a_{ 0 };
				int b_{ 0 };
			};


			// Design Choices: Aggregation is better in the meaning of design of software.

			//template<typename ExecPolicyT = std::execution::sequenced_policy>
			//class MySubjectAggregation {
			//public:
			//	using WeakPtrIObserverWeak		= SubjectWeak<ExecPolicyT>::WeakPtrIObserverWeak;
			//	using SharedPtrISubjectWeak		= std::shared_ptr<ISubjectWeak>;

			//	void NotifyObservers(const std::string& message = "") const {
			//		if (subject_ptr_) { subject_ptr_->NotifyObservers(message); }
			//	}
			//	inline void AttachObserver(const WeakPtrIObserverWeak observer_ptr) {
			//		if (subject_ptr_) { subject_ptr_->AttachObserver(observer_ptr); }
			//	}
			//	inline void DetachObserver(const WeakPtrIObserverWeak observer_ptr) {
			//		if (subject_ptr_) { subject_ptr_->DetachObserver(observer_ptr); }
			//	}
			//	inline void DetachAllExpired() {
			//		if (subject_ptr_) { subject_ptr_->DetachAllExpired(); }
			//	}
			//	inline bool HasObserver(const WeakPtrIObserverWeak observer_ptr) const {
			//		if (subject_ptr_) { return subject_ptr_->HasObserver(observer_ptr); }
			//		return false;
			//	}


			//	inline void set_subject(const SharedPtrISubjectWeak subject_ptr_p) noexcept { subject_ptr_ = subject_ptr_p; }
			//	inline std::weak_ptr<ISubjectWeak> subject() const noexcept { return subject_ptr_; };

			//	StateSingle state_{};	// is public for testing

			//private:

			//	SharedPtrISubjectWeak subject_ptr_{};
			//};

			//class MyObserverAggregation {
			//public:
			//	using SharedPtrIObserverWeak = std::shared_ptr<IObserverWeak>;

			//	/**
			//	 * Update the information about Subject.
			//	 */
			//	void Update(const std::string& message = "") {
			//		if (observer_ptr_) { observer_ptr_->Update(message); }

			//		// + Some Actions
			//	};

			//	inline void set_observer(const SharedPtrIObserverWeak observer_ptr_p) noexcept {
			//		observer_ptr_ = observer_ptr_p;
			//	}
			//	inline std::weak_ptr<IObserverWeak> observer() const noexcept { return observer_ptr_; };

			//	StateSingle state_{};	// is public for testing

			//private:

			//	SharedPtrIObserverWeak observer_ptr_{};
			//};



			class MySubject : public SubjectWeakMsg<> {
			public:
				MySubject() = default;
			protected:
				MySubject(const MySubject&) = delete; // C.67	C.21
				MySubject& operator=(const MySubject&) = delete;
				MySubject(MySubject&&) noexcept = delete;
				MySubject& operator=(MySubject&&) noexcept = delete;
			public:
				~MySubject() override = default;

				MyState state_{};	// is public for testing
			};


			class MyObserver : public ObserverMsg {
			public:
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
				 */
				void Update(const std::string& message = "") override {
					// Some Actions
				};

				MyState state_{};	// is public for testing
			};

		} // !namespace observer_weak

	} // !namespace behavioral

} // !namespace pattern

#endif // !WEAK_OBSERVER_HPP
