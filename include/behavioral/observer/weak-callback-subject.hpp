#ifndef WEAK_CALLBACK_SUBJECT_HPP
#define WEAK_CALLBACK_SUBJECT_HPP

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

// Container variants

#include "containers-library/generic-container.hpp"
#include "diagnostics-library/custom-exception.hpp"
#include "general-utilities-library/functional/weak-method-invoker.hpp"


/** Software Design Patterns */
namespace pattern {
	namespace behavioral {

		namespace weak_callback_subject {
			/**
			 * Pattern Observer. Subject holds weak method calls to observers.
			 * Call can be done to any function in observer with any signature.
			 * Maybe stored in hash table.
			 *
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
			template<typename ContainerT = std::unordered_set<::common::MethodActionWrap> >
			class WeakCallbackSubject {
			public:
				using MethodActionWrap = ::common::MethodActionWrap;
				using value_type       = typename ContainerT::value_type;
				using iterator         = ContainerT::iterator;//decltype(callbacks_.end());
				using const_iterator   = ContainerT::const_iterator;//decltype(callbacks_.cend());

				static_assert(std::is_same_v<value_type, MethodActionWrap>, "Container elements must be MethodAction.");


				// Container Ts Alternatives
				//using ContainerSet			= std::set<MethodActionWrap, std::owner_less<MethodActionWrap>>;
				//using ContainerList			= std::list<MethodActionWrap>;
				//using ContainerForwardList	= std::forward_list<MethodActionWrap>;
				//using ContainerVector		= std::vector<MethodActionWrap>;
				//using ContainerDeque		= std::deque<MethodActionWrap>;

				WeakCallbackSubject() = default;
				// Subject may be constructed without observable Subjects

				template<typename IteratorT, typename ExecPolicyT>
				WeakCallbackSubject(IteratorT begin, IteratorT end, ExecPolicyT policy = std::execution::seq) {
					AttachObserver(begin, end, policy);
				}

				template<typename ExecPolicyT>
				explicit WeakCallbackSubject(const std::initializer_list<MethodActionWrap>& init_list,
											ExecPolicyT policy = std::execution::seq) {
					//: observers_{ init_list.begin(), init_list.end() } {
					AttachObserver(init_list.begin(), init_list.end(), policy);
				};

				/** Callbacks are stored in any ContainerT. */
				template<typename ContainerT, typename ExecPolicyT>
				explicit WeakCallbackSubject(const ContainerT& container,
											ExecPolicyT policy = std::execution::seq) {
					static_assert(std::is_same_v<typename ContainerT::value_type, MethodActionWrap>,
								"Elements of container, when AttachObserver must be MethodActionWrap");
					AttachObserver(container.begin(), container.end(), policy);
				};

				template<typename CallbackT, typename ExecPolicyT>
				explicit WeakCallbackSubject(CallbackT callback,
											ExecPolicyT policy = std::execution::seq) {
					static_assert(std::is_same_v<std::remove_cvref_t<CallbackT>, MethodActionWrap>,
									"Callback must be MethodAction.");
					AttachObserver(std::forward<CallbackT>(callback), policy); // No differrence with std::move(callback)
				};


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
									const bool run_in_new_thread = true,
									ExecPolicyT policy = std::execution::seq) const {
					auto update_n_clean_fn = [](MethodActionWrap& callback) {
						if (!callback()) { return true; } // if call success -> not to remove
						return false;
					};

					// foreach callback: invoke and clean expired callback
					std::unique_lock lock{ observers_shared_mtx_ };							// write
					generic::GenericRemoveIf(callbacks_, update_n_clean_fn, policy);		// O(k), k - count of expired callbacks
				};

				/*
				 * Update all attached observers in main thread.
				 *
				 * Complexity: set = O(log n), Other containers = O(n).
				 *
				 * @param message		message with information needed for Update.
				 * @param new_thread	one thread from thread pool.
				 */
				//template<typename ExecPolicyT>
				//void NotifyObservers(std::thread& new_thread,
				//					const std::string& message = "",
				//					ExecPolicyT policy = std::execution::seq) const {
				//	auto observer_update_fn = [&message](MethodActionWrap callback) {
				//		// callback in lambda is shared_ptr, cause in GenericNotify it is locked
				//		callback->Update(message);
				//		};
				//	MutexNotifyObserversNClean(observer_update_fn, policy);
				//};


				// TODO: Notification in another thread. Wait the end of notification process
				// TODO: bool run_in_new_thread = false
				// In new std::thread
				// Async variant


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
					static_assert(std::is_same_v<decltype(*attachable_begin), MethodActionWrap>,
									"Iterator must be dereferencable to MethodActionWrap");

					auto attach_observer_fn = [&callbacks_, &policy](const auto& callback) { // callback from attachable range
						if (callback.expired()) { return; }

						if (!HasCallback(callback, policy)) { // Duplicate control. Mustn't duplicate weak_ptr
							generic::GenericAddElement(callbacks_, callback);		// O(1)
						}
						//UpdateExpiredObserversCount(has_observer.second);
					}; // !lambda
					{
						std::unique_lock lock{ observers_shared_mtx_ };
						std::for_each(policy, attachable_begin, attachable_end, attach_observer_fn); // write	O(n)
					} // !lock
					//CleanFoundExpiredObservers(policy);							// O(n)
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT>
				inline void AttachObserver(const std::initializer_list<MethodActionWrap>& init_list,
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
					static_assert(std::is_same_v<typename ContainerT::value_type, MethodActionWrap>,
									"Elements of container, when AttachObserver must be MethodActionWrap");

					AttachObserver(container.begin(), container.end(), policy);
				};

				/**
				 * Add Observer to list of Observers. Wrapper.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 *
				 * Complexity: O(n)
				 *
				 * @param callback	weak pointer to observer.
				 */
				template<typename ExecPolicyT>
				inline void AttachObserver(const MethodActionWrap callback,
											ExecPolicyT policy = std::execution::seq) {
					if (callback.expired()) { return; }	// Precondition
					AttachObserver({ callback }, policy);	// for HasCallback
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
					static_assert(std::is_same_v<decltype(*erasable_begin), MethodActionWrap>,
									"Iterator must be dereferencable to MethodActionWrap");

					//size_t expired_count{};
					auto detach_observer_fn = [&callbacks_, &policy](const auto& callback) {
						generic::GenericEraseFirst(callbacks_, callback, policy);

						// Can Detach only alive objects
						//expired_count = EraseEqualWeakPtr(callbacks_, callback, policy);			// O(n)
						//UpdateExpiredObserversCount(expired_count);
					}; // !lambda

					{
						std::unique_lock lock{ observers_shared_mtx_ }; // write
						std::for_each(policy, erasable_begin, erasable_end, detach_observer_fn); // write	O(k*n)
						//CleanFoundExpiredObservers(policy);												// O(n)
					} // !lock
				};

				/**
				 * Detach Observer. Wrapper.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT>
				inline void DetachObserver(const std::initializer_list<MethodActionWrap>& init_list,
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
					static_assert(std::is_same_v<typename ContainerT::value_type, MethodActionWrap>,
									"Elements of container, when DetachObserver must be MethodActionWrap");

					DetachObserver(container.begin(), container.end(), policy);
				};

				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 *
				 * Complexity: O(n)
				 *
				 * @param callback weak pointer to observer.
				 */
				template<typename ExecPolicyT>
				inline void DetachObserver(const MethodActionWrap& callback,
											ExecPolicyT policy = std::execution::seq) {
					if (callback.expired()) { return; }	// precondition

					std::unique_lock lock{ observers_shared_mtx_ }; // write
					generic::GenericEraseFirst(callbacks_, callback, policy);
					// Can Detach only alive objects
					//size_t expired_count{ EraseEqualWeakPtr(callbacks_, callback, policy) };	// O(n)
					//UpdateCountNCleanExpiredObservers(expired_count, policy);
				};


				/**
				 * Detach all expired weak_ptr objects in container
				 *
				 * Complexity: O(n)
				 */
				template<typename ExecPolicyT>
				inline void CleanupAllExpired(ExecPolicyT policy = std::execution::seq) const {
					std::unique_lock lock{ observers_shared_mtx_ };		// write
					auto expired = [](const auto& callback) { return callback.expired(); };
					GenericRemoveIf(callbacks_, expired, policy);		// O(n)
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
				/*template<typename ExecPolicyT>
				inline bool HasObserverNClean(const MethodActionWrap callback,
										ExecPolicyT policy = std::execution::seq) const {
					std::pair<bool, size_t> has_observer{ Find(callback, policy) };
					UpdateCountNCleanExpiredObservers(has_observer.second, policy);
					return has_observer.first;
				};*/

			private:
				/*
				* Find callback in container.
				*
				* Complexity: // O(n) || O(log n) || O(1)
				*/
				template<typename ExecPolicyT>
				inline const_iterator Find(const MethodActionWrap& callback, ExecPolicyT policy = std::execution::seq) const {
					std::shared_lock lock{ observers_shared_mtx_ };			// read
					return GenericFind(callbacks_, callback, policy);		// O(n) || O(log n) || O(1)
					// TODO: iterator category is const ?
				};

				/*
				* Checks if there is callback in container.
				*
				* Complexity: // O(n) || O(log n) || O(1)
				*/
				template<typename ExecPolicyT>
				inline bool HasCallback(const MethodActionWrap& callback, ExecPolicyT policy = std::execution::seq) const noexcept {
					return Find(callback, policy) != callbacks_.end();
				}
				// TODO: autoclean, when find. Add in weak-ptr.hpp equal func, that indicate expired state

				/**
				 * Detach all expired weak_ptr objects in container. Concurrent sync by mutex.
				 * Helps to minimize count of calls Cleanup in concurrent usage of this class.
				 * Cause many blocked threads may found equal expired weak_ptr.
				 * But cleanup is only done by counter found_expired_observers_.
				 * Locked with unique mutex.
				 *
				 * Complexity: O(n)
				 */
				//template<typename ExecPolicyT>
				//inline void CleanFoundExpiredObservers(ExecPolicyT policy = std::execution::seq) const {
				//	if (found_expired_observers_.load(std::memory_order_relaxed) > 0) { // precondition
				//		{
				//			std::unique_lock lock{ observers_shared_mtx_ };
				//			// Cleanup expired weak_ptr
				//			EraseNExpiredWeakPtr(callbacks_, found_expired_observers_, policy); // write	O(n)
				//		} // !lock
				//		ResetExpiredObserversCount();
				//	}
				//}
				// TODO: Decrease the Complexity of cleaning to make attach & detach complexity = O(log n).

				/** Modify count of expired observers, if new count bigger */
				/*inline void UpdateExpiredObserversCount(const size_t new_count) const noexcept {
					if (new_count > found_expired_observers_) {
						found_expired_observers_ = new_count;
					}
                }*/
				/** Set count of expired observer to 0 */
				/*inline void ResetExpiredObserversCount() const noexcept {
					found_expired_observers_ = 0;
				}*/
				// TODO: replace assign of atomic with quicker ops = load(relaxed) + compare_exchange_weak

				/**
				 * Update count of expired observers and clean observers container.
				 * Locked with mutex.
				 *
				 * Complexity: O(n)
				 */
				/*template<typename ExecPolicyT>
				inline void UpdateCountNCleanExpiredObservers(const size_t new_count,
															ExecPolicyT policy = std::execution::seq) const {
					UpdateExpiredObserversCount(new_count);
					CleanFoundExpiredObservers(policy);
				}*/

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
						expired_count = GenericNotifyWeakObservers(callbacks_, observer_update_fn, policy); // read	O(n)
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
					std::thread notify_thread(&WeakCallbackSubject::MutexNotifyObserversNClean, this, observer_update_fn, policy);
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
					std::future result{ std::async(std::launch::async, &WeakCallbackSubject::MutexNotifyObserversNClean,
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
				mutable ContainerT callbacks_{}; // mutable is for const fn notify function cleaning of expired weak_ptr
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



			//template<typename ExecPolicyT = std::execution::sequenced_policy>
			//class MySubjectInheritance : public SubjectWeak<ExecPolicyT> {
			//public:
			//	using SybjectT = SubjectWeak<ExecPolicyT>;
			//	using WeakPtrIObserverWeak = SybjectT::WeakPtrIObserverWeak;

			//	MySubjectSingle() = default;
			//protected:
			//	MySubjectSingle(const MySubjectSingle&) = delete; // C.67	C.21
			//	MySubjectSingle& operator=(const MySubjectSingle&) = delete;
			//	MySubjectSingle(MySubjectSingle&&) noexcept = delete;
			//	MySubjectSingle& operator=(MySubjectSingle&&) noexcept = delete;
			//public:
			//	~MySubjectSingle() override = default;

			//	StateSingle state_{};	// is public for testing
			//};


			//class MyObserverInheritance : public ObserverWeak<> {
			//public:
			//	/*explicit MyObserverSingle(WeakPtrISubjectT subject_ptr) : ObserverWeakMulti(subject_ptr) {
			//	}*/

			//	MyObserverInheritance() = default;
			//protected:
			//	MyObserverInheritance(const MyObserverInheritance&) = delete; // C.67	C.21
			//	MyObserverInheritance& operator=(const MyObserverInheritance&) = delete;
			//	MyObserverInheritance(MyObserverInheritance&&) noexcept = delete;
			//	MyObserverInheritance& operator=(MyObserverInheritance&&) noexcept = delete;
			//public:
			//	~MyObserverInheritance() override = default;

			//	/**
			//	 * Update the information about Subject.
			//	 */
			//	void Update(const std::string& message = "") override {
			//		// Some Actions
			//	};

			//	StateSingle state_{};	// is public for testing
			//};

		} // !weak_callback_subject


	} // !namespace behavioral
} // !namespace pattern

#endif // !WEAK_CALLBACK_SUBJECT_HPP
