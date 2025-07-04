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

#include "behavioral/observer/iobserver.hpp"


// TODO: maybe separate classes to different files to make less includes

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
			using namespace pattern::behavioral::iobserver;

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
			 * [[Testlevel(0)]]
			 */
			template<typename UpdateDataT,
					 typename ContainerT = std::list< std::weak_ptr<IObserverMsg> > >
			class SubjectWeakMsg : public ISubjectWeakMsg
				//public std::enable_shared_from_this<SubjectWeak<typename ExecPolicyT, typename ContainerT_t>>
			{
			public:
				using value_type = typename ContainerT::value_type;
				using WeakPtrIObserverMsg = std::weak_ptr<IObserverMsg>;
				//using IteratorNExpiredCount = std::pair<decltype(observers_.end()), size_t>;

				static_assert(std::is_object_v<value_type>, "The C++ Standard forbids containers of non-object types "
															"because of [container.requirements].");

				// Container Ts Alternatives
				using ContainerList = std::list<WeakPtrIObserverMsg>;
				//using ContainerVector		= std::vector<WeakPtrIObserverWeakHub>;
				//using ContainerSet			= std::set<WeakPtrIObserverWeakHub, std::owner_less<WeakPtrIObserverWeakHub>>;
				//using ContainerForwardList	= std::forward_list<WeakPtrIObserverWeakHub>;
				//using ContainerDeque		= std::deque<WeakPtrIObserverWeakHub>;


				SubjectWeakHub() = default;
				// Subject may be constructed without observable Subjects

				template<typename IteratorT, typename ExecPolicyT>
				SubjectWeakHub(IteratorT begin, IteratorT end, ExecPolicyT policy = std::execution::seq) {
					AttachObserver(begin, end, policy);
				}

				template<typename ExecPolicyT>
				explicit SubjectWeakHub(const std::initializer_list<WeakPtrIObserverMsg>& init_list,
										ExecPolicyT policy = std::execution::seq) {
					//: observers_{ init_list.begin(), init_list.end() } {
					AttachObserver(init_list.begin(), init_list.end(), policy);
				};

				template<typename ContainerT, typename ExecPolicyT>
				explicit SubjectWeakHub(const ContainerT& container,
										ExecPolicyT policy = std::execution::seq) {
					static_assert(std::is_same_v<typename ContainerT::value_type, WeakPtrIObserverMsg>,
								"Elements of container, when AttachObserver must be weak_ptr to observer interface");
					AttachObserver(container.begin(), container.end(), policy);
				};

				template<typename ExecPolicyT>
				explicit SubjectWeakHub(const WeakPtrIObserverMsg observer_ptr,
										ExecPolicyT policy = std::execution::seq) {
					AttachObserver(observer_ptr, policy); // No differrence with std::move(observer_ptr)
				};

			protected:
				SubjectWeakHub(const SubjectWeakHub&) = delete; // C.67	C.21
				SubjectWeakHub& operator=(const SubjectWeakHub&) = delete;
				SubjectWeakHub(SubjectWeakHub&&) noexcept = delete;
				SubjectWeakHub& operator=(SubjectWeakHub&&) noexcept = delete;
			public:
				~SubjectWeakHub() override = default;

				// TODO: write notify methods for all stubs in ObserverHub class. With all variants of Update fn of observer.

				/*
				 * Update all attached observers in main thread.
				 *
				 * Complexity: set = O(log n), Other containers = O(n).
				 *
				 * @param message	message with information needed for Update.
				 */
				template<typename ExecPolicyT>
				void NotifyObservers(const std::string& message = "",
									ExecPolicyT policy = std::execution::seq) const {
					auto observer_method = [&message](std::shared_ptr<IObserverMsg> observer_ptr) {
						// observer_ptr in lambda is shared_ptr, cause in GenericNotify it is locked
						observer_ptr->Update(message);
					};


				};

				template<typename ContainerType, typename UpdateFunctionType, typename ExecPolicyType>
				inline size_t GenericNotifyWeakObservers(const ContainerType& observers,
														UpdateFunctionType observer_method,
														ExecPolicyType policy = std::execution::seq) {
					if (observers.empty) { return expired_count; } // precondition

					auto update_fn = [&observer_method](const auto& observer_ptr) { // return true, when observer expired
						if (auto observer_shared{ observer_ptr.lock() }) {
							observer_method(observer_shared);
							return false;
						}
						return true;
					}; // !lambda
					std::unique_lock lock{ observers_shared_mtx_ };
					generic::GenericRemoveIf(observers_, update_fn, policy);
				}

				// TODO: Notification in another thread. Wait the end of notification process
				// TODO: bool run_in_new_thread = false
				// In new std::thread
				// Async variant

				/*void Update(UpdateDataT data) override {};
				void Update(const UpdateDataT& data) override {};
				void Update(UpdateDataT&& data) override {};*/



				/**
				* Add Multiple Observer to list of Observers.
				* Only alive weak_ptr can be attached to container and only that is not duplicates.
				*
				* Complexity: O(K), K - count of attachable observers.
				* Complexity of adding each observer: maybe O(1), but is O(n), cause CleanOps.
				*
				* @param attachable_begin		start of range of observers, that will be added to Subject
				* @param attachable_end			end of range of observers, that will be added to Subject
				*/
				template<typename IteratorT, typename ExecPolicyT>
				void AttachObserver(IteratorT attachable_begin, IteratorT attachable_end,
									ExecPolicyT policy = std::execution::seq)
				{ // Main logic in this function to decrease count of function call, so lower overhead resources for call of fn.
					if (attachable_begin == attachable_end) { return; }	// Precondition
					static_assert(std::is_same_v<decltype(*attachable_begin), WeakPtrIObserverMsg>,
									"Iterator be dereferencable to weak_ptr to observer interface");

					auto attach_observer_fn = [this, &policy](const auto& observer_ptr) {
						auto has_observer{ HasObserver(observer_ptr, policy) };
						if (!has_observer.first) { // Duplicate control. Mustn't duplicate weak_ptr
							GenericAddElement(observers_, observer_ptr);		// O(1)
						}
						UpdateExpiredObserversCount(has_observer.second);
					}; // !lambda
					{
						std::unique_lock lock{ observers_shared_mtx_ };
						std::for_each(policy, attachable_begin, attachable_end, attach_observer_fn); // write	O(n)
					} // !lock
					CleanFoundExpiredObservers(policy);							// O(n)
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT>
				inline void AttachObserver(const std::initializer_list<WeakPtrIObserverMsg>& init_list,
											ExecPolicyT policy = std::execution::seq) {
					if (init_list.size() == 0) { return; }	// Precondition
					AttachObserver(init_list.begin(), init_list.end(), policy);
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 *
				 * Complexity: O(n)
				 */
				template<typename ContainerT, typename ExecPolicyT>
				inline void AttachObserver(const ContainerT& container, ExecPolicyT policy = std::execution::seq) {
					if (container.empty()) { return; }	// Precondition
					static_assert(std::is_same_v<typename ContainerT::value_type, WeakPtrIObserverMsg>,
						"Elements of container, when AttachObserver must be weak_ptr to observer interface");

					AttachObserver(container.begin(), container.end(), policy);
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 *
				 * Complexity: O(n)
				 *
				 * @param observer_ptr	weak pointer to observer.
				 */
				template<typename ExecPolicyT>
				inline void AttachObserver(const WeakPtrIObserverMsg observer_ptr,
											ExecPolicyT policy = std::execution::seq) {
					if (observer_ptr.expired()) { return; }	// Precondition
					AttachObserver({ observer_ptr }, policy);	// for HasObserver
				};
				// not const, cause must attach subject in observer ptr


				/**
				 * Detach Multiple Observers.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 *
				 * Complexity: O(k*n), k - count of detachable observers.
				 * Complexity of detaching each observer: maybe O(n), but is O(n), cause CleanOps.
				 *
				 * @param erasable_begin	begin of container with observers, that must be detached
				 * @param erasable_end		end of container with observers, that must be detached
				 */
				template<typename IteratorT, typename ExecPolicyT>
				inline void DetachObserver(IteratorT erasable_begin, IteratorT erasable_end,
											ExecPolicyT policy = std::execution::seq) {
					if (erasable_begin == erasable_end) { return; }	// Precondition
					static_assert(std::is_same_v<decltype(*erasable_begin), WeakPtrIObserverMsg>,
									"Iterator be dereferencable to weak_ptr to observer interface");

					size_t expired_count{};
					auto detach_observer_fn = [this, expired_count](const auto& observer_ptr) {
						// Can Detach only alive objects
						expired_count = EraseEqualWeakPtr(observers_, observer_ptr, policy);			// O(n)
						UpdateExpiredObserversCount(expired_count);
					}; // !lambda

					{
						std::unique_lock lock{ observers_shared_mtx_ }; // write
						std::for_each(policy, erasable_begin, erasable_end, detach_observer_fn); // write	O(k*n)
						CleanFoundExpiredObservers(policy);												// O(n)
					} // !lock
				};

				/**
				 * Detach Observer. Wrapper.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT>
				inline void DetachObserver(const std::initializer_list<WeakPtrIObserverMsg>& init_list,
											ExecPolicyT policy = std::execution::seq) {
					if (init_list.size() == 0) { return; }	// Precondition
					DetachObserver(init_list.begin(), init_list.end(), policy);
				};

				/**
				 * Detach Observer. Wrapper.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 *
				 * Complexity: O(n)
				 */
				template<typename ContainerT, typename ExecPolicyT>
				inline void DetachObserver(const ContainerT& container,
											ExecPolicyT policy = std::execution::seq) {
					if (container.empty()) { return; }	// Precondition
					static_assert(std::is_same_v<typename ContainerT::value_type, WeakPtrIObserverMsg>,
						"Elements of container, when AttachObserver must be weak_ptr to observer interface");

					DetachObserver(container.begin(), container.end(), policy);
				};

				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 *
				 * Complexity: O(n)
				 *
				 * @param observer_ptr weak pointer to observer.
				 */
				template<typename ExecPolicyT>
				inline void DetachObserver(const WeakPtrIObserverMsg observer_ptr,
											ExecPolicyT policy = std::execution::seq) {
					std::unique_lock lock{ observers_shared_mtx_ }; // write
					// Can Detach only alive objects
					size_t expired_count{ EraseEqualWeakPtr(observers_, observer_ptr, policy) };	// O(n)
					UpdateCountNCleanExpiredObservers(expired_count, policy);
				};


				/**
				 * Detach all expired weak_ptr objects in container
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT>
				inline void CleanupAllExpiredObservers(ExecPolicyT policy = std::execution::seq) const {
					std::unique_lock lock{ observers_shared_mtx_ }; // write
					EraseAllExpired(observers_, policy);
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
				template<typename ExecPolicyT>
				inline bool HasObserverNClean(const WeakPtrIObserverMsg observer_ptr,
												ExecPolicyT policy = std::execution::seq) const {
					std::pair<bool, size_t> has_observer{ HasObserver(observer_ptr, policy) };
					UpdateCountNCleanExpiredObservers(has_observer.second, policy);
					return has_observer.first;
				};

			private:
				/* Complexity: O(n) */
				template<typename ExecPolicyT>
				inline std::pair<bool, size_t> HasObserver(const WeakPtrIObserverMsg observer_ptr,
															ExecPolicyT policy = std::execution::seq) const {
					std::pair<decltype(observers_.end()), size_t> find_result{};
					{
						std::shared_lock lock{ observers_shared_mtx_ }; // read
						find_result = FindEqualWeakPtr(observers_, observer_ptr, policy);	// O(n)
					} // !lock

					bool has_observer = find_result.first != observers_.end();
					return std::make_pair(has_observer, find_result.second); // iterators maybe invalidated after cleanup
				};

				/**
				 * Detach all expired weak_ptr objects in container. Concurrent sync by mutex.
				 * Helps to minimize count of calls Cleanup in concurrent usage of this class.
				 * Cause many blocked threads may found equal expired weak_ptr.
				 * But cleanup is only done by counter found_expired_observers_.
				 * Locked with unique mutex.
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT>
				inline void CleanFoundExpiredObservers(ExecPolicyT policy = std::execution::seq) const {
					if (found_expired_observers_.load(std::memory_order_relaxed) > 0) { // precondition
						{
							std::unique_lock lock{ observers_shared_mtx_ };
							// Cleanup expired weak_ptr
							EraseNExpiredWeakPtr(observers_, found_expired_observers_, policy); // write	O(n)
						} // !lock
						ResetExpiredObserversCount();
					}
				}
				// TODO: Decrease the Complexity of cleaning to make attach & detach complexity = O(log n).

				/** Modify count of expired observers, if new count bigger */
				inline void UpdateExpiredObserversCount(const size_t new_count) const noexcept {
					if (new_count > found_expired_observers_) {
						found_expired_observers_ = new_count;
					}
				}
				/** Set count of expired observer to 0 */
				inline void ResetExpiredObserversCount() const noexcept {
					found_expired_observers_ = 0;
				}
				// TODO: replace assign of atomic with quicker ops = load(relaxed) + compare_exchange_weak

				/**
				 * Update count of expired observers and clean observers container.
				 * Locked with mutex.
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT>
				inline void UpdateCountNCleanExpiredObservers(const size_t new_count,
																ExecPolicyT policy = std::execution::seq) const {
					UpdateExpiredObserversCount(new_count);
					CleanFoundExpiredObservers(policy);
				}

				/**
				 * Thread safe. Notify all observers of Subject. In same thread.
				 * Used mutex to block shared resource.
				 *
				 * Complexity: O(n)
				 */
				template<typename UpdateFunctionT, typename ExecPolicyT>
				void MutexNotifyObserversNClean(UpdateFunctionT observer_update_fn,
												ExecPolicyT policy = std::execution::seq) const {
					size_t expired_count{};
					{
						std::shared_lock lock{ observers_shared_mtx_ };
						expired_count = GenericNotifyWeakObservers(observers_, observer_update_fn, policy); // read	O(n)
					} // !lock

					UpdateCountNCleanExpiredObservers(expired_count, policy);						// O(n)
				}

				/**
				 * Thread safe. Notify all observers of Subject in new thread of execution.
				 * Used mutex to block shared resource.
				 */
				template<typename ThreadPoolT, typename UpdateFunctionT, typename ExecPolicyT>
				void ThreadNotifyObserversNClean(ThreadPoolT& thread_pool,
												UpdateFunctionT observer_update_fn,
												ExecPolicyT policy = std::execution::seq) const {
					//thread_pool.enqueue(&SubjectWeakHub::MutexNotifyObserversNClean, this, observer_update_fn, policy);
					thread_pool.enqueue([this, &observer_update_fn, &policy]() {
						MutexNotifyObserversNClean(observer_update_fn, policy);
						});
				}
				/**
				 * Thread safe. Notify all observers of Subject in new thread of execution.
				 * Used mutex to block shared resource.
				 */
				template<typename ThreadPoolT, typename UpdateFunctionT, typename ExecPolicyT>
				void ThreadNotifyObserversNClean(UpdateFunctionT observer_update_fn,
												ExecPolicyT policy = std::execution::seq) const {
					// try catch may be deleted, cause it is critical error with termination
					std::thread notify_thread(&SubjectWeakHub::MutexNotifyObserversNClean, this, observer_update_fn, policy);
					notify_thread.detach();	// Thread will work separately
					// instead of detach thread object can be global, so you can monitor it
				}

				/**
				 * Thread safe. Notify all observers of Subject in new thread of execution.
				 * Used mutex to block shared resource.
				 */
				template<typename ThreadPoolT, typename UpdateFunctionT, typename ExecPolicyT>
				void ThreadNotifyObserversNCleanAsync(UpdateFunctionT observer_update_fn,
														ExecPolicyT policy = std::execution::seq) const {
					std::future result{ std::async(std::launch::async, &SubjectWeakHub::MutexNotifyObserversNClean,
											this, observer_update_fn, policy) };
				}


				//-----------------------SubjectWeakHub Data-------------------------------------------------

				/**
				* List of observers, that will be attach to Subject.
				* Subject is not interested in owning of its Observers.
				* So can be used weak_ptr, created from shared_ptr.
				*
				* Design: If there is too many subjects with few observers you can use hash table.
				*/
				mutable ContainerT observers_{}; // mutable is for const fn notify function cleaning of expired weak_ptr
				// Is not const, cause Attach, Detach functions call.

				/**
				 * Mutex for thread safety class operations.
				 * Helps to read data without lock. And to write data with lock.
				 */
				mutable std::shared_mutex observers_shared_mtx_{};
				/* Maybe order of concurent thread calls must be detach, attach, notify. First delete, then add, then notify */

				/**
				 * Atomic variable for concurrent auto cleaning of found by read operation expired observers.
				 * Necessary for decreasing number of calls cleanup function.
				 */
				mutable std::atomic_size_t found_expired_observers_{};

				/** Execution policy, that will work for all functions */
				//ExecPolicyT policy_{};

			};	// !class SubjectWeakHub
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



			struct StateSingle {
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



			class MySubject : public SubjectWeakHub<ExecPolicyT> {
			public:
				using SybjectT = SubjectWeak<ExecPolicyT>;
				using WeakPtrIObserverWeak = SybjectT::WeakPtrIObserverWeak;

				MySubjectSingle() = default;
			protected:
				MySubjectSingle(const MySubjectSingle&) = delete; // C.67	C.21
				MySubjectSingle& operator=(const MySubjectSingle&) = delete;
				MySubjectSingle(MySubjectSingle&&) noexcept = delete;
				MySubjectSingle& operator=(MySubjectSingle&&) noexcept = delete;
			public:
				~MySubjectSingle() override = default;

				StateSingle state_{};	// is public for testing
			};


			class MyObserver : public ObserverWeak<> {
			public:
				/*explicit MyObserverSingle(WeakPtrISubjectT subject_ptr) : ObserverWeakMulti(subject_ptr) {
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
				 */
				void Update(const std::string& message = "") override {
					// Some Actions
				};

				StateSingle state_{};	// is public for testing
			};

		} // !namespace observer_weak

	} // !namespace behavioral

} // !namespace pattern

#endif // !WEAK_OBSERVER_HPP
