﻿#ifndef OBSERVER_HPP
#define OBSERVER_HPP

// TODO: compress include list
#include <algorithm> // remove_if
#include <execution> // execution policies
#include <forward_list>
#include <functional>
#include <memory>
#include <iterator>
#include <initializer_list>	// for AttachObservers
#include <string>
#include <type_traits>
#include <list>
#include <utility> // pair
#include <unordered_set>
#include <vector>


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



		namespace observer_weak_ptr {
			// TODO:
			// command pattern instead Update
			// SubjectWeakMultiShared - class with shared_ptr
			// std::set implementation to all functions. all operations O(log n)
			// add ranges from c++ 20
			//
			// refactoring: Function extraction from member methods of ObserverWeakMulti
			//		and SubjectWeakMulti. Many equal code in functions.


//=============================Helper Functions for ObserverMulti & SubjectMulti=====================================

			using ExpiredFnType = decltype([](const auto& value_weak_ptr) { return value_weak_ptr.expired(); });


			/** Erase custom n number of expired weak_ptr from container */
			template<typename ContainerType, typename ExecPolicyType>
			inline void EraseNExpired(ContainerType& container,
										const size_t expired_count,
										ExecPolicyType policy = std::execution::seq) {
				if (container.empty() || expired_count == 0) { return; } // Precondition
				//static_assert(std::is_same_v<typename ContainerType::value_type, std::weak_ptr<ValueType>>,
								//"The type mismatch between container elements and weak_ptr");

				size_t find_count{};
				auto expired_fn = [&expired_count, &find_count](const typename ContainerType::value_type& value_ptr) {
					if (find_count < expired_count && value_ptr.expired()) { // after N expired found, value_ptr.expired() is not called
                        ++find_count;
						return true;
					}
					return false;
				};
				if constexpr (std::is_same_v<ContainerType, std::forward_list<typename ContainerType::value_type>>) { // for Forward_list
					container.remove_if(expired_fn); // erase-remove idiom is not for forward_list
				} else { // All other containers
					container.erase(std::remove_if(policy, container.begin(),
													container.end(), expired_fn), container.end());
				}
			};


			/**
			 * Compares if two weak_ptr are alive and have the same stored pointers.
			 *
			 * @param shared_ptr search ptr. Is shared for decreasing number of locks.
			 * @param current_ptr
			 * @return result of equality check
			 */
			template<typename ValueType>
			inline bool IsEqualWeakPtr(const std::shared_ptr<ValueType>& searched_shared,
									   const std::weak_ptr<ValueType>& current_ptr) noexcept {
				if (current_ptr.expired() || !searched_shared) { return false; }
                auto current_shared = current_ptr.lock();
				return current_shared && searched_shared.get() == current_shared.get();
			}

			/**
             * Find first weak_ptr, that is alive and has same stored pointer.
             * With auto clean of expired weak_ptrs.
             *
             * @return	First = of pair of iterator to equal weak_ptr or to and.
             *			Second = size_t count of expired weak_ptr for cleanup.
             */
			template<typename ValueType, typename ContainerType, typename ExecPolicyType>
			inline auto FindEqualWeakPtr(ContainerType& container,
										std::weak_ptr<ValueType> searched_ptr,
										ExecPolicyType policy = std::execution::seq)
					-> decltype(container.end()) {
				if (container.empty() || searched_ptr.expired()) { return container.end(); }
				static_assert(std::is_same_v<typename ContainerType::value_type, std::weak_ptr<ValueType>>,
							  "The type mismatch between container elements and weak_ptr");

				auto it_equal{ container.end() };
				size_t expired_count{};

				// mustn't lock for long time, cause of resource use and life time
				// But better to lock for lowering number of locks in IsEqualWeakPtr
				if (auto searched_shared = searched_ptr.lock()) {
					auto equal_owner = [&searched_shared, &expired_count](const auto& current_ptr) {
						if (current_ptr.expired()) { ++expired_count; }
						return IsEqualWeakPtr(searched_shared, current_ptr);
					};
					it_equal = std::find_if(policy, container.begin(), container.end(), equal_owner);
				}
				// Cleanup expired weak_ptr
				EraseNExpired(container, expired_count, ExecPolicyType());
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
			template<typename ContainerType, typename ExecPolicyType>
			inline void EraseEqualWeakPtr(ContainerType& container,
											typename ContainerType::value_type searched_ptr,
											ExecPolicyType policy = std::execution::seq) {
				if (container.empty() || searched_ptr.expired()) { return; }
				//static_assert(std::is_same_v<typename ContainerType::value_type, std::weak_ptr<ValueType>,
								// "Value of container must be weak_ptr");

				auto it_equal{ container.end() };
				size_t expired_count{};

				if constexpr (std::is_same_v<ContainerType, std::forward_list<typename ContainerType::value_type>>) { // forward_list
					if (auto searched_shared = searched_ptr.lock()) {
						auto equal_owner = [&searched_shared, &expired_count](const auto& current_ptr) {
							if (current_ptr.expired()) { ++expired_count; }
							return IsEqualWeakPtr(searched_shared, current_ptr);
						}; // !lambda end

						container.remove_if(equal_owner);
					}
				} else { // All other types of containers
					// std::move(searched_ptr) is no necessary, cause weak_ptr hold 2 pointers.
					// Copy or Move operations are equal in performance.
					it_equal = FindEqualWeakPtr(container, searched_ptr, policy);
					if (it_equal != container.end()) { container.erase(it_equal); }
				}

				// Cleanup expired weak_ptr
				EraseNExpired(container, expired_count, ExecPolicyType());
			}


			/** Erase all expired weak_ptr from container */
			template<typename ContainerType, typename ExecPolicyType>
			inline void EraseAllExpired(ContainerType& container, ExecPolicyType policy = std::execution::seq) {
				if (container.empty()) { return; }

				auto expired = [](const auto& value_ptr) { return value_ptr.expired(); };
				if constexpr (std::is_same_v<ContainerType, std::forward_list<typename ContainerType::value_type>>) { // for Forward_list
					container.remove_if(expired); // erase-remove idiom is not for forward_list
				} else { // All other containers
					container.erase(std::remove_if(policy, container.begin(),
															container.end(), expired), container.end());
				}
			};


//============================Single Observer Interfaces====================================

			/**
			 * Abstract. Stateless. Interface of Observer.
			 * Alternative name - Subscriber.
			 */
			class IObserverWeak {
			protected:
				IObserverWeak() = default;
				IObserverWeak(const IObserverWeak&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverWeak& operator=(const IObserverWeak&) = delete;
				IObserverWeak(IObserverWeak&&) noexcept = delete;
				IObserverWeak& operator=(IObserverWeak&&) noexcept = delete;
			public:
				virtual ~IObserverWeak() = default;

				/** Update the information about Subject */
				virtual void Update(const std::string& message) = 0;
			};


			/** Empty messages code enum for default template arguments */
			enum class ObserverEmptyEnum {};

			/**
			 * Abstract. Stateless. Interface of Observer.
			 * Alternative name - Subscriber.
			 */
			template<typename EnumType = ObserverEmptyEnum>
			class IObserverWeakExtended : public IObserverWeak {
			public:
				using IObserverWeak::Update; // cause name hiding

			protected:
				IObserverWeakExtended() = default;
				IObserverWeakExtended(const IObserverWeakExtended&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverWeakExtended& operator=(const IObserverWeakExtended&) = delete;
				IObserverWeakExtended(IObserverWeakExtended&&) noexcept = delete;
				IObserverWeakExtended& operator=(IObserverWeakExtended&&) noexcept = delete;
			public:
				~IObserverWeakExtended() override = default;


				/** Update the information about Subject */
				virtual void Update() = 0;

				/** Update the information about Subject */
				virtual void Update(const EnumType message) = 0;	// more convenient for switch statement
			};


			/**
			 * Abstract. Stateless. Subject with weak_ptr dependencies.
			 * Alternative name - Publisher.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 */
			class ISubjectWeak {
			public:
				using WeakPtrIObserverWeak = std::weak_ptr<IObserverWeak>;

			protected:
				ISubjectWeak() = default;
				ISubjectWeak(const ISubjectWeak&) = delete; // C.67	C.21
				ISubjectWeak& operator=(const ISubjectWeak&) = delete;
				ISubjectWeak(ISubjectWeak&&) noexcept = delete;
				ISubjectWeak& operator=(ISubjectWeak&&) noexcept = delete;
			public:
				virtual ~ISubjectWeak() = default;


				/** Update all attached observers */
				virtual void NotifyObservers(const std::string& message) const = 0;

				/**
				 * Add Observer to list of notification in Subject.
				 * There is shared_ptr<ObserverT> outside of class. Subject stores weak_ptr to that Observer.
				 */
				virtual void AttachObserver(const WeakPtrIObserverWeak observer_ptr) = 0;
				// not const, cause must attach subject in observer ptr

				/** Detach observer from notifying list */
				virtual void DetachObserver(const WeakPtrIObserverWeak observer_ptr) = 0;
			};
			/*
			* Design choices. Params in functions. Weak_ptr vs shared_ptr:
			* Best) weak_ptr in params gives guaranty, that we won't use destructed objects.
			* 2) If we use const shared_ptr& outside code must guaranty, that object will be alive, while function is working.
			* 3) If we use shared_ptr we increment strong counter of shared_ptr, increasing life cycle of object.
			*/


			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 *
			 * Invariant: don't attach expired weak_ptr. Value type must be weak_ptr. Mustn't duplicate weak_ptr.
			 * @tparam EnumType is enum class for messages for one of realization of Update.
			 *
			 * [[Testlevel(0)]]
			 */
			template<typename EnumType = ObserverEmptyEnum>
			//requires std::is_enum_v<EnumType>	// c++20 requires
			class ObserverWeak : public IObserverWeakExtended<EnumType> {
			public:
				ObserverWeak() = default;
				// Observer may be constructed without observable Subjects

			protected:
				ObserverWeak(const ObserverWeak&) = delete; // C.67	C.21
				ObserverWeak& operator=(const ObserverWeak&) = delete;
				ObserverWeak(ObserverWeak&&) noexcept = delete;
				ObserverWeak& operator=(ObserverWeak&&) noexcept = delete;
			public:
				~ObserverWeak() override = default;

				/**
				* Update the information about Subject
				*
				* @param message	use in switch statements
				*/
				void Update(const std::string& message = "") override {
					// Implement specific update logic here
					// For example, logging or state update
					// For now, empty implementation
					//(void)message;  // suppress unused parameter warning
				};

				/** Update the information about Subject */
				void Update() override {
				};

				/** Update the information about Subject */
				void Update(const EnumType message) override {
				};

			};	// !class ObserverWeak
			// Design choices: subject weak_ptr is necessary, when you need get more data for updating process.
			// In most cases subject weak_ptr is not needed.


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
			 * @tparam ExecPolicyType Execution policy (e.g., std::execution::sequenced_policy)
			 * @tparam ContainerType_t Container type holding weak_ptrs to observers
			 *
			 * [[Testlevel(0)]]
			 */
			template<typename ExecPolicyType = std::execution::sequenced_policy, typename EnumType = ObserverEmptyEnum,
					 typename ContainerType = std::list<std::weak_ptr<IObserverWeak>> >
			//requires std::is_execution_policy_v<ExecPolicyType> && std::is_enum_v<EnumType>	// c++20 requires
			class SubjectWeak : public ISubjectWeak
								//public std::enable_shared_from_this<SubjectWeak<typename ExecPolicyType, typename ContainerType_t>>
			{
			public:
                using value_type	= typename ContainerType::value_type;
				//using iterator		= typename ContainerType::iterator;

				using WeakPtrIObserverWeak = std::weak_ptr<IObserverWeak>;	// not from derived for clarity

				//using container_type = ContainerType;
				//using iterator = std::iterator<std::input_iterator_tag, WeakPtrIObserverWeak>;

				static_assert(std::is_object_v<value_type>, "The C++ Standard forbids containers of non-object types "
															"because of [container.requirements].");

				// Container Types Alternatives
				using ContainerList			= std::list<WeakPtrIObserverWeak>;
				using ContainerForwardList	= std::forward_list<WeakPtrIObserverWeak>;
				//using ContainerSet		= std::set<WeakPtrIObserverWeak, std::owner_less<WeakPtrIObserverWeak>>;
				//using ContainerVector		= std::vector<WeakPtrIObserverWeak>;
				//using ContainerDeque		= std::deque<WeakPtrIObserverWeak>;

				SubjectWeak() = default;
				// Subject may be constructed without observable Subjects

				template<typename IteratorType>
				SubjectWeak(IteratorType begin, IteratorType end) {
					AttachObserver(begin, end);
				}

				explicit SubjectWeak(const std::initializer_list<WeakPtrIObserverWeak>& init_list) {
						//: observers_{ init_list.begin(), init_list.end() } {
					AttachObserver(init_list.begin(), init_list.end());
				};

				template<typename ContainerType>
				explicit SubjectWeak(const ContainerType& container) {
					AttachObserver(container.begin(), container.end());
				};

				explicit SubjectWeak(const WeakPtrIObserverWeak observer_ptr) {
					AttachObserver(observer_ptr); // No differrence with std::move(observer_ptr)
				};


			protected:
				SubjectWeak(const SubjectWeak&) = delete; // C.67	C.21
				SubjectWeak& operator=(const SubjectWeak&) = delete;
				SubjectWeak(SubjectWeak&&) noexcept = delete;
				SubjectWeak& operator=(SubjectWeak&&) noexcept = delete;
			public:
				~SubjectWeak() override = default;


				/*
				 * Update all attached observers.
				 * Complexity: O()
				 *
				 * @param message	message with information needed for Update.
				 */
				void NotifyObservers(const std::string& message = "") const override { // not const, cause cleanup operations
					size_t expired_count{};
					auto update_fn = [&message, &expired_count](const auto& observer_ptr) {
						if (observer_ptr.expired()) { // expired is less expensive, then lock. Thats why it is first
							++expired_count;
							return;
						}
						if (auto observer_shared{ observer_ptr.lock() }) {
							observer_shared->Update(message);
						}
					};
					std::for_each(ExecPolicyType{}, observers_.begin(), observers_.end(), update_fn);
					// Cleanup expired weak_ptr
					EraseNExpired(observers_, expired_count, ExecPolicyType{});
				};


				/**
				* Add Observer to list of Observers.
				* Only alive weak_ptr can be attached to container and only that is not duplicates.
				*
				* @param begin		start of range of observers, that will be added to Subject
				* @param end		end of range of observers, that will be added to Subject
				*/
				template<typename IteratorType>
				void AttachObserver(IteratorType begin, IteratorType end)
				{ // Main logic in this function to decrease count of function call, so lower overhead resources for call of fn.
					if (begin == end) { return; }	// Precondition
					static_assert(std::is_same_v<decltype(*begin), WeakPtrIObserverWeak>,
									"Iterator be dereferencable to weak_ptr to observer interface");

					auto attach_observer_fn = [this](const auto& observer_ptr) {
						if (!HasObserver(observer_ptr)) { // Duplicate control. Mustn't duplicate weak_ptr
							if constexpr (std::is_same_v<ContainerType, ContainerForwardList>) { // forward_list
								observers_.emplace_after(observers_.before_begin(), observer_ptr);
							} else { // All other types of containers, except forward_list
								observers_.emplace(observer_ptr);
							} // TODO: std::set implementation
						}
					};
					std::for_each(ExecPolicyType{}, begin, end, attach_observer_fn);
				};

				/**
				 * Add Observer to list of Observers.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 */
				inline void AttachObserver(const std::initializer_list<WeakPtrIObserverWeak>& init_list) {
					if (init_list.size() == 0) { return; }	// Precondition
					AttachObserver(init_list.begin(), init_list.end());
				};

				/**
				 * Add Observer to list of Observers.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 */
				template<typename ContainerType>
				inline void AttachObserver(const ContainerType& container) {
					if (container.empty()) { return; }	// Precondition
					static_assert(std::is_same_v<typename ContainerType::value_type, WeakPtrIObserverWeak>,
									"Elements of container, when AttachObserver must be weak_ptr to subject");

					AttachObserver(container.begin(), container.end());
				};

				/**
				 * Add Observer to list of Observers.
				 * Only alive weak_ptr can be attached to container and only that is not duplicates.
				 *
				 * @param observer_ptr	weak pointer to observer.
				 */
				inline void AttachObserver(const WeakPtrIObserverWeak observer_ptr) override {
					if (observer_ptr.expired()) { return; }	// Precondition
					AttachObserver({ observer_ptr });
				};
				// not const, cause must attach subject in observer ptr


				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 */
				template<typename IteratorType>		// TODO: add exec policy to templ param?
				inline void DetachObserver(IteratorType begin, IteratorType end) {
					if (begin == end) { return; }	// Precondition
					static_assert(std::is_same_v<decltype(*begin), WeakPtrIObserverWeak>,
									"Iterator be dereferencable to weak_ptr to observer interface");

					auto detach_observer_fn = [this](const auto& observer_ptr) {
						// Can Detach only alive objects
						EraseEqualWeakPtr(observers_, observer_ptr, ExecPolicyType{});
					};
					std::for_each(ExecPolicyType{}, begin, end, detach_observer_fn);
				};

				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 */
				inline void DetachObserver(const std::initializer_list<WeakPtrIObserverWeak>& init_list) {
					if (init_list.size() == 0) { return; }	// Precondition
					DetachObserver(init_list.begin(), init_list.end());
				};

				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 */
				template<typename ContainerType>
				inline void DetachObserver(const ContainerType& container) {
					if (container.empty()) { return; }	// Precondition
					static_assert(std::is_same_v<typename ContainerType::value_type, WeakPtrIObserverWeak>,
									"Elements of container, when AttachObserver must be weak_ptr to subject");

					DetachObserver(container.begin(), container.end());
				};

				/**
				 * Detach Observer.
				 * Can Detach only not expired weak_ptr, cause equality defined on alive objects.
				 *
				 * @param observer_ptr weak pointer to observer.
				 */
				inline void DetachObserver(const WeakPtrIObserverWeak observer_ptr) override {
					// Can Detach only alive objects
					EraseEqualWeakPtr(observers_, observer_ptr, ExecPolicyType{});
				};


				/** Detach all expired weak_ptr objects in container */
				inline void CleanupAllExpiredObservers() const {
					EraseAllExpired(observers_, ExecPolicyType{});
					// if subject is expired, it is deleted, so we don't need to detach observer in subject
				};

				/**
				 * Check if there is observer in Subject.
				 * Cleanup expired weak_ptr subjects in container.
				 *
				 * \param subject_ptr
				 * \param policy
				 * \return
				 */
				inline bool HasObserver(const WeakPtrIObserverWeak observer_ptr) const {
					return FindEqualWeakPtr(observers_, observer_ptr, ExecPolicyType{}) != observers_.cend();
				};

			private:
				/**
				 * List of observers, that will be attach to Subject.
				 * Subject is not interested in owning of its Observers.
				 * So can be used weak_ptr, created from shared_ptr.
				 *
				 * Design: If there is too many subjects with few observers you can use hash table.
				 */
				mutable ContainerType observers_{}; // mutable is for const fn notify function cleaning of expired weak_ptr
				// Is not const, cause Attach, Detach functions call.


				/** Execution policy, that will work for all functions */
				ExecPolicyType policy_{};

			};	// !class SubjectWeak
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

			// TODO: Partial notification of observers (rare use case?)
			// Normal Cleanup
			// Execution policy, different types of policy
			//



			struct StateSingle {
				int a_{ 0 };
				int b_{ 0 };
			};


			// Design Choices: Aggregation is better in the meaning of design of software.

			//template<typename ExecPolicyType = std::execution::sequenced_policy>
			//class MySubjectAggregation {
			//public:
			//	using WeakPtrIObserverWeak		= SubjectWeak<ExecPolicyType>::WeakPtrIObserverWeak;
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



			//template<typename ExecPolicyType = std::execution::sequenced_policy>
			//class MySubjectInheritance : public SubjectWeak<ExecPolicyType> {
			//public:
			//	using SybjectType = SubjectWeak<ExecPolicyType>;
			//	using WeakPtrIObserverWeak = SybjectType::WeakPtrIObserverWeak;

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
			//	/*explicit MyObserverSingle(WeakPtrISubjectType subject_ptr) : ObserverWeakMulti(subject_ptr) {
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


//=============================Multi Observer Interfaces===========================================
			// Very, very rare use case of ISubjectWeakMulti, IObserverWeakMulti interfaces.
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


			class ISubjectWeakMulti;

			/**
			 * Abstract. Stateless. Interface for Observer weak_ptr with multiple subjects.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 * Use this class with shared_ptr arguments.
			 */
			class IObserverWeakMulti {
			public:
				using ISubjectType = ISubjectWeakMulti;
				using WeakPtrISubjectType = std::weak_ptr<ISubjectType>;
				// Type will make subject unmutable
				using WeakPtrConstISubjectType = const std::weak_ptr<const ISubjectWeakMulti>;

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
				virtual void Update(const WeakPtrConstISubjectType subject_ptr) = 0;

				/**
				 * Add pair Subject in Observer & Observer in Subject.
				 * In Observer Add Subject to list of observing. In Subject Add Observer to list of Observers.
				 *
				 * @param subject
				 * @param recursion_depth	0 is default. 0 recursion depth = Attach Subject & Observer.
				 *							1 recursion depth = Attach only Subject.
				 * @return
				 */
				virtual void AttachSubject(WeakPtrISubjectType subject_ptr, size_t recursion_depth) = 0;
				// not const, cause must attach observer in subject ptr

				/**
				 * Detach pair Subject in Observer & Observer in Subject.
				 * In Observer Detach Subject, when Subject is destructed. In Subject Detach Observer.
				 *
				 * @param subject				subject, that is destructing.
				 * @param recursion_depth		0 is default. 0 recursion depth = Attach Subject & Observer.
				 *								1 recursion depth = Attach only Subject.
				 */
				virtual void DetachSubject(WeakPtrISubjectType subject_ptr, size_t recursion_depth) = 0;
				// subject mustn't be const, cause mutate deleting observer in subject

				/**
				 * Detach n expired weak_ptr in container.
				 *
				 * \param expired_count
				 * \param to_erase_all_expired	if true, every expired object in container will be Detached
				 */
				virtual void DetachNExpired(const size_t expired_count, bool to_erase_all_expired = false) = 0;

				/** Check if there is subject reference in Observer */
				virtual bool HasSubject(const WeakPtrISubjectType subject_ptr) = 0;
			};

			/**
			 * Abstract. Stateless. Subject with weak_ptr dependencies.
			 * Alternative name - Publisher.
			 * May be used for IObserverWeak, IObserverWeakMulti.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 */
			class ISubjectWeakMulti {
			public:
				using IObserverType = IObserverWeakMulti;
				using WeakPtrIObserverType = std::weak_ptr<IObserverType>;

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
				virtual void AttachObserver(WeakPtrIObserverType observer_ptr, size_t recursion_depth) = 0;
				// not const, cause must attach subject in observer ptr

				/**
				 * Detach pair Observer in Subject & Subject in Observer.
				 *
				 * @param observer_ptr
				 * @param recursion_depth		0 is default. 0 recursion depth = Attach Observer & Subject.
				 *								1 recursion depth = Attach only Observer.
				 */
				virtual void DetachObserver(WeakPtrIObserverType observer_ptr, size_t recursion_depth) = 0;

				/**
				 * Detach n expired weak_ptr in container.
				 *
				 * \param expired_count
				 * \param to_erase_all_expired	if true, every expired object in container will be Detached
				 */
				virtual void DetachNExpired(const size_t expired_count, bool to_erase_all_expired = false) = 0;

				/** Check if there is observer in Subject */
				virtual bool HasObserver(const WeakPtrIObserverType observer_ptr) = 0;
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
			template<typename ExecPolicyType = std::execution::sequenced_policy,
					 typename ContainerType_t = std::forward_list<std::weak_ptr<ISubjectWeakMulti>>>
			requires std::is_execution_policy_v<ExecPolicyType>
			class ObserverWeakMulti : public IObserverWeakMulti,
									  public std::enable_shared_from_this<ObserverWeakMulti<typename ExecPolicyType,
																							typename ContainerType_t>>
				// weak_from_this is for creating shared_ptr from
				// this without doubled control block of shared_ptr with the outside shared_ptr
			{
			public:
				using value_type = typename ContainerType_t::value_type;
				using container_type = ContainerType_t;

				using ISubjectType = ISubjectWeakMulti;
				using WeakPtrISubjectType = std::weak_ptr<ISubjectType>;
				using IObserverType = IObserverWeakMulti;
				// Type will make subject unmutable
				using WeakPtrConstISubjectType = IObserverWeakMulti::WeakPtrConstISubjectType;

				//static_assert(std::is_object_v<ISubjectType_t>, "The C++ Standard forbids containers of non-object types "
																//"because of [container.requirements].");

				// Container Types Alternatives
				using ContainerForwardListType = std::forward_list<WeakPtrISubjectType>;
				//using ContainerListType = std::list<WeakPtrISubjectType>;
				//using ContainerVectorType = std::vector<WeakPtrISubjectType>;
				//using ContainerDequeType = std::deque<WeakPtrISubjectType>;

				ObserverWeakMulti() = default;
				// Observer may be constructed without observable Subjects

				/**
				 * Constructs Observer from Subject.
				 * Class mustn't attach expired weak_ptr and spam internal container.
				 *
				 * @param subject from which Observer is constructed
				 * @param self_ptr self pointer, that owns Observer and for which Observer is constructed.
				 */
				/*explicit ObserverWeakMulti(WeakPtrISubjectType subject_ptr) {
					if (auto subject_shared{ subject_ptr.lock() }) {
						if constexpr (std::is_same_v<ContainerType_t, ContainerForwardListType>) {
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
					std::for_each(ExecPolicyType(), subjects_.begin(), subjects_.end(), detach_fn);
				};


				/** Update the information about Subject */
				void Update() override {
				};

				/**
				 * Update the information about Subject.
				 *
				 * @param subject concrete subject from subjects_refs_, that called Update.
				 */
				void Update(const WeakPtrConstISubjectType subject_ptr) override {
				};


				/**
				 * Add pair Subject & Observer.
				 * In Observer Add Subject to list of observing. In Subject Add Observer to list of Observers.
				 * Only alive weak_ptr can be attached to container and only that is not in container already.
				 *
				 * @param subject_ptr
				 * @param add_observer_in_subj	if true, Observer will be added in Subject
				 */
				void AttachSubject(WeakPtrISubjectType subject_ptr, size_t recursion_depth = 0) override {
					if (subject_ptr.expired()) { return; } // precondition

					if (!HasSubject(subject_ptr)) { // Duplicate control. Mustn't duplicate weak_ptr
						if constexpr (std::is_same_v<ContainerType_t, ContainerForwardListType> ) {
							subjects_.emplace_after(subjects_.cbefore_begin(), subject_ptr);
						} else { // All other types of containers, except forward_list
							subjects_.emplace(subject_ptr);
						}
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
				inline void DetachSubject(WeakPtrISubjectType subject_ptr, size_t recursion_depth = 0) override {
					if (subject_ptr.expired()) { return; } // precondition Can Detach only alive objects

					// First Detach Observer in Subject
					if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
						if (auto subject_shared{ subject_ptr.lock() }) {
							subject_shared->DetachObserver(this->weak_from_this(), ++recursion_depth);
						}
					}
					EraseEqualWeakPtr(subjects_, subject_ptr, ExecPolicyType());
				};

				/**
				 * Detach n expired weak_ptr in container.
				 *
				 * \param expired_count
				 * \param to_erase_all_expired	if true, every expired object in container will be Detached
				 */
				inline void DetachNExpired(const size_t expired_count, bool to_erase_all_expired = false) override {
					if (to_erase_all_expired) { EraseAllExpired(subjects_, ExecPolicyType()); }
					else { EraseNExpired(subjects_, expired_count, ExecPolicyType()); }
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
				inline bool HasSubject(const WeakPtrISubjectType subject_ptr) override {
					return FindEqualWeakPtr(subjects_, subject_ptr, ExecPolicyType()) != subjects_.cend();
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



			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 * One Observer may has many Subjects.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 * Don't use unordered_set with weak_ptr, cause may give undefined behavior.
			 * Not recommend to use vector, as bad performance of remove_if.
			 * Invariant: don't attach expired weak_ptr. Value type must be weak_ptr. Mustn't duplicate weak_ptr.
			 */

			/**
			 * Concrete. Observers will be notified on Subject state changes.
			 * One Subject can has many Observers.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 * Not recommend to use vector, as bad performance of remove_if.
			 * Don't forget to Notify Observers, where it is necessary, when Subject state changes.
			 * [[Testlevel(35, Hand debug)]]
			 */
			template<typename ExecPolicyType = std::execution::sequenced_policy,
					 typename ContainerType_t = std::forward_list<std::weak_ptr<IObserverWeakMulti>> >
			requires std::is_execution_policy_v<ExecPolicyType>
			class SubjectWeakMulti : public ISubjectWeakMulti,
									 public std::enable_shared_from_this<SubjectWeakMulti<typename ExecPolicyType,
																						  typename ContainerType_t>> {
			public:
				using value_type = typename ContainerType_t::value_type;
				using container_type = ContainerType_t;
				using iterator = typename container_type::iterator;

				using IObserverType = IObserverWeakMulti;
				using WeakPtrIObserverType = std::weak_ptr<IObserverType>;
				using ISubjectType = ISubjectWeakMulti;

				//static_assert(std::is_object_v<ISubjectType_t>, "The C++ Standard forbids containers of non-object types "
																//"because of [container.requirements].");

				// Container Types Alternatives
				using ContainerForwardListType = std::forward_list<WeakPtrIObserverType>;
				//using ContainerListType = std::list<WeakPtrIObserverType>;
				//using ContainerVectorType = std::vector<WeakPtrIObserverType>;
				//using ContainerDequeType = std::deque<WeakPtrIObserverType>;

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
					std::for_each(ExecPolicyType(), observers_.begin(), observers_.end(), detach_fn);
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
					std::for_each(ExecPolicyType(), observers_.begin(), observers_.end(), update_fn);
					// Cleanup expired weak_ptr
					EraseNExpired(observers_, expired_count, ExecPolicyType());

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
					std::for_each(ExecPolicyType(), observers_.begin(), observers_.end(), update_fn);
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
				void AttachObserver(WeakPtrIObserverType observer_ptr, size_t recursion_depth = 0) override {
					if (observer_ptr.expired()) { return; } // precondition

					if (!HasObserver(observer_ptr)) { // Duplicate control. Mustn't duplicate weak_ptr
						if constexpr (std::is_same_v<ContainerType_t, ContainerForwardListType>) { // if ForwardList
							observers_.emplace_after(observers_.cbefore_begin(), observer_ptr);
						} else { // All other types of containers, except forward_list
							observers_.emplace(observer_ptr);
						}
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
				void DetachObserver(WeakPtrIObserverType observer_ptr, size_t recursion_depth = 0) override {
					if (observer_ptr.expired()) { return; } // precondition

					// First Detach Observer in Subject
					if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
						// Can Detach only alive objects
						if (auto observer_shared{ observer_ptr.lock() }) {
							observer_shared->DetachSubject(this->weak_from_this(), ++recursion_depth);
						}
					}
					EraseEqualWeakPtr(observers_, observer_ptr, ExecPolicyType());
				};

				/**
				 * Detach n expired weak_ptr in container.
				 *
				 * \param expired_count
				 * \param to_erase_all_expired	if true, every expired object in container will be Detached
				 */
				inline void DetachNExpired(const size_t expired_count, bool to_erase_all_expired = false) override {
					if (to_erase_all_expired) { EraseAllExpired(observers_, ExecPolicyType()); }
					else { EraseNExpired(observers_, expired_count, ExecPolicyType()); }
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
				inline bool HasObserver(const WeakPtrIObserverType observer_ptr) override {
					return FindEqualWeakPtr(observers_, observer_ptr, ExecPolicyType()) != observers_.cend();
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
			template<typename ExecPolicyType = std::execution::sequenced_policy>
			class MySubject : public SubjectWeakMulti<typename ExecPolicyType> {
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
			template<typename ExecPolicyType = std::execution::sequenced_policy>
			class MyObserver : public ObserverWeakMulti<ExecPolicyType> {
			public:
				using WeakPtrConstISubjectType = ObserverWeakMulti<ExecPolicyType>::WeakPtrConstISubjectType;

				/*explicit MyObserver(WeakPtrISubjectType subject_ptr) : ObserverWeakMulti(subject_ptr) {
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
				void Update(const WeakPtrConstISubjectType subject_ptr) override {
					//state_observer_ = subject
				};

				State state_{};
			};



//======================================Trash===================================================

			// Not necessary. Minimal interface better
			/**
			 * Abstract. Stateless. Subject with weak_ptr dependencies.
			 * Alternative name - Publisher.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 */
			 //template<typename EnumType = ObserverEmptyEnum>
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
			 //	virtual void NotifyObservers(const EnumType message) const = 0;

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
			template<typename IteratorType>
			using CleanupListType = std::vector<IteratorType>;


			/** Erase duplicates in container */
			template<typename ContainerType, typename ExecPolicyType>
			inline void EraseDuplicates_Cleanup(ContainerType& container, ExecPolicyType policy = std::execution::seq) {
				auto begin{ container.begin() };
				auto end{ container.end() };
				std::sort(policy, begin, end); // make groups of equal elements
				container.erase(std::unique(policy, begin, end), end);
			}

			template<typename ValueType, typename IteratorsContainerType>
			//inline bool IsValueInIterators(const typename ContainerType::value_type& value,
			inline bool IsValueInIterators_Cleanup(const ValueType& value, const IteratorsContainerType& iterators_list) {
				for (const auto& it : iterators_list) {
					if (&(*it) == &value) { return true; }
				}
				return false;
			}

			/** Erase cleanup list of iterators to elements from input container */
			template<typename ContainerType, typename IteratorsContainerType, typename ExecPolicyType>
			inline void EraseElementsByIterators_Cleanup(ContainerType& container,
														IteratorsContainerType& iterators_list,
														ExecPolicyType policy = std::execution::seq) {
				//static_assert(std::is_same_v<typename ContainerType::iterator, typename IteratorsContainerType::value_type>);
				//EraseDuplicates(container, policy);

				// Iterators in iterators_list can be easyly invalidated by erase operation
				if (iterators_list.empty()) { return; }

				auto container_end{ container.end() };
				if constexpr (std::is_same_v<ContainerType, std::forward_list<typename ContainerType::value_type>>) { // for Forward_list
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

					// Move each iterator element of ContainerType& container to the end of container
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
			template<typename ValueType>
			inline bool IsEqualWeakPtr_Cleanup(const std::shared_ptr<ValueType>& searched_shared,
				const std::weak_ptr<ValueType>& current_ptr) {
				auto current_shared = current_ptr.lock();
				return searched_shared && current_shared && searched_shared.get() == current_shared.get();
			}

			/**
			 * Find first weak_ptr, that is alive and has same stored pointer.
			 *
			 * @return	First = of pair of iterator to equal weak_ptr or to and.
			 *			Second = size_t count of expired weak_ptr for cleanup.
			 */
			template<typename ValueType, typename ContainerType, typename ExecPolicyType>
			inline auto FindEqualWeakPtr_Cleanup(ContainerType& container,
				const std::weak_ptr<ValueType>& searched_ptr,
				ExecPolicyType policy = std::execution::seq) {
				/*static_assert(std::is_same_v<std::weak_ptr<ValueType>, typename ContainerType::value_type>,
							  "The type mismatch between container elements and weak_ptr");*/
							  //using iterator = typename ContainerType::const_iterator;
				using iterator = decltype(container.end());
				//std::pair<iterator, CleanupListType<iterator>> result{};
				/*auto& it_equal		{ result.first };
				auto& cleanup_list	{ result.second };*/
				iterator it_equal{};
				CleanupListType<iterator> cleanup_list{};

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
				EraseElementsByIterators(container, cleanup_list, ExecPolicyType());
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
			template<typename ContainerType, typename ExecPolicyType>
			inline void EraseEqualWeakPtr_Cleanup(ContainerType& container, typename ContainerType::value_type searched_ptr,
				ExecPolicyType policy = std::execution::seq) {
				//static_assert(std::is_same_v<std::weak_ptr<ValueType>, typename ContainerType::value_type>);

				//using iterator = typename ContainerType::const_iterator;
				using iterator = decltype(container.end());
				std::pair<iterator, CleanupListType<iterator>> result{};
				auto& it_equal{ result.first };
				auto& cleanup_list{ result.second };

				if (auto searched_shared = searched_ptr.lock()) {
					auto end{ container.end() };
					if constexpr (std::is_same_v<ContainerType, std::forward_list<typename ContainerType::value_type>>) {
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
				EraseElementsByIterators(container, cleanup_list, ExecPolicyType());
			}
//==========================================================================================



		} // !namespace observer








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
				using IObserverType = IObserver;

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
				virtual void AttachObserver(IObserverType& observer) = 0;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserver(IObserverType& observer) = 0;

				/** Update all attached observers */
				virtual void NotifyObservers() const = 0;
			};

//==================================Multi Interfaces=====================================

			class ISubjectMulti;

			/** Abstract. Stateless. Interface for Observer with multiple subjects */
			class IObserverMulti : public IObserver {
            public:
                using ISubjectType = ISubjectMulti;

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
				virtual void Update(const ISubjectType& subject) = 0;

				/** Add Subject to list of observing. Add Observer to list of Observers in Subject */
				virtual void AttachSubjectNObserver(ISubjectType& subject) = 0;

				/**
				 * Detach Subject from Observer, when Subject is destructed..
				 *
				 * @param subject subject, that is destructing.
				 */
				virtual void DetachSubjectNObserver(ISubjectType& subject) = 0;

				/** Check if there is subject reference in Observer */
				virtual bool HasSubject(const ISubjectType& subject) const noexcept = 0;
			};


			/**
			 * Abstract. Stateless.
			 * Alternative name - Publisher.
			 * May be used for IObserver, IObserverMulti.
			 */
			class ISubjectMulti {
			public:
				using IObserverMultiType = IObserverMulti;

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
				virtual void AttachObserverNSubject(IObserverMultiType& observer) = 0;

				/** Detach observer_ref_1 from notifying list */
				virtual void DetachObserverNSubject(IObserverMultiType& observer) = 0;

				/** Update all attached observers */
				virtual void NotifyObservers() const = 0;

                /** Check if there is observer reference in Subject */
                virtual bool HasObserver(const IObserverMultiType& observer) const noexcept = 0;
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
                using ISubjectType = ISubjectMulti;
				using ISubjectRefType = std::reference_wrapper<ISubjectType>;
				using HashFunctorType = HashRefWrapperReferent<ISubjectType>;
				using EqualToFunctorType = HashRefWrapperEqualTo<ISubjectRefType>;
				using ContainerType = std::unordered_set<ISubjectRefType, HashFunctorType, EqualToFunctorType>;

                explicit ObserverRefSetMulti(ISubjectType& subject) noexcept
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
					ContainerType::iterator it{ subjects_refs_.begin() };
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
				void Update(const ISubjectType& subject) override {
				};
				//virtual void Update(const ISubjectType& subject) = 0;

                /** Add Subject to list of observing. Add Observer to list of Observers in Subject */
                void AttachSubjectNObserver(ISubjectType& subject) override {
					subjects_refs_.emplace(subject);
                    if (!subject.HasObserver(*this)) { subject.AttachObserverNSubject(*this); }
                };

				/**
				 * Delete Subject from set of subjects in Observer, when Subject is destructed.
				 *
				 * @param subject subject, that is destructing.
				 */
				void DetachSubjectNObserver(ISubjectType& subject) override {
					subjects_refs_.erase(std::ref(subject));
					if (subject.HasObserver(*this)) { subject.DetachObserverNSubject(*this); }
				};

				/** Check if there is subject reference in Observer */
                bool HasSubject(const ISubjectType& subject) const noexcept override {
					auto equal_fn = [&subject](const ISubjectRefType& ref) { return &ref.get() == &subject; };
					auto search_result{ std::find_if(subjects_refs_.begin(), subjects_refs_.end(), equal_fn) };
                    return search_result != subjects_refs_.end();
                };

			private:
				/** Subjects for which we will be notified. */
				ContainerType subjects_refs_{};
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
                using IObserverType = IObserverMulti;
				using IObserverRefType = std::reference_wrapper<IObserverType>;
				using HashFunctorType = HashRefWrapperReferent<IObserverType>;
				using EqualToFunctorType = HashRefWrapperEqualTo<IObserverRefType>;
				using ContainerType = std::unordered_set<IObserverRefType, HashFunctorType, EqualToFunctorType>;

				SubjectRefSetMulti() = default;
			protected:
				SubjectRefSetMulti(const SubjectRefSetMulti&) = delete; // C.67	C.21
				SubjectRefSetMulti& operator=(const SubjectRefSetMulti&) = delete;
				SubjectRefSetMulti(SubjectRefSetMulti&&) noexcept = delete;
				SubjectRefSetMulti& operator=(SubjectRefSetMulti&&) noexcept = delete;
			public:
				~SubjectRefSetMulti() override {
					ContainerType::iterator it = observers_refs.begin();
					const auto cend{ observers_refs.cend() };
					auto it_prev{ it }; // post increment returns unmutable const_iterator, that is not best choice for mutable Detach
					while (it != cend) { // not ranged for, cause elements deleted
                        it_prev = it;
                        ++it;
						it_prev->get().DetachSubjectNObserver(*this);
					}
				};

				/** Add Observer to list of notification */
				inline void AttachObserverNSubject(IObserverType& observer) override {	// mustn't be const, cause observer may change
					//observers_refs.emplace(std::ref(observer));
					observers_refs.emplace(observer);
					if (!observer.HasSubject(*this)) {
						observer.AttachSubjectNObserver(*this);
					}
				};
				// TODO: Attach Observers() initializer_list, vector. Other operations with multiple observers.

				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserverNSubject(IObserverType& observer) override {
					observers_refs.erase(std::ref(observer));
					if (observer.HasSubject(*this)) { observer.DetachSubjectNObserver(*this); }
				};

				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (IObserverRefType observer_ref : observers_refs) {
						observer_ref.get().Update();
					}
				};

                /** Update all attached observers with multiple subjects sending Subject& */
                inline void NotifyObserversMulti() const {
                    for (IObserverRefType observer : observers_refs) {
                        observer.get().Update(*this);
                    }
                };

				/** Detach all attached observers */
				inline void ClearAllObservers() noexcept {
					observers_refs.clear();
					DetachAllObservers();
				};

				/** Check if there is observer reference in Subject */
				bool HasObserver(const IObserverType& observer) const noexcept override {
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
				ContainerType observers_refs{};
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
				using ISubjectType = ISubjectMulti;
				using ISubjectRefType = std::reference_wrapper<ISubjectType>;

                explicit ObserverListMulti(ISubjectType& subject) noexcept
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
					for (ISubjectRefType subject_ref : subjects_refs_) {
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
				void Update(const ISubjectType& subject) override {
				};

				/**
				 * Delete Subject from set of subjects in Observer, when Subject is destructed.
				 *
				 * @param subject subject, that is destructing.
				 */
				void DetachSubjectNObserver(ISubjectType& subject) override {
					subjects_refs_.remove_if([&subject](const ISubjectRefType& current) {
                        return &current.get() == &subject;
                        });
				};

			private:
				/** Subjects for which we will be notified. */
				std::forward_list<ISubjectRefType> subjects_refs_{};
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
				using IObserverType = IObserverMulti;
				using IObserverRefType = std::reference_wrapper<IObserverType>;

				SubjectListMulti() = default;
			protected:
				SubjectListMulti(const SubjectListMulti&) = delete; // C.67	C.21
				SubjectListMulti& operator=(const SubjectListMulti&) = delete;
				SubjectListMulti(SubjectListMulti&&) noexcept = delete;
				SubjectListMulti& operator=(SubjectListMulti&&) noexcept = delete;
			public:
				~SubjectListMulti() override {
					for (IObserverRefType observer_ref : observers_refs) {
						observer_ref.get().DetachSubjectNObserver(*this);
					}
				};

				/** Add Observer to list of notification. Used in constructor of Observer */
				inline void AttachObserverNSubject(IObserverType& observer) override {
					observers_refs.emplace_front(std::ref(observer));
				};


				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserverNSubject(IObserverType& observer) override {
                    observers_refs.remove_if([&observer](const IObserverRefType& current) {
                        return &current.get() == &observer;
                        });
				};


				/** Update all attached observers */
				inline void NotifyObservers() const override {
					for (IObserverRefType observer_ref : observers_refs) {
						observer_ref.get().Update();
					}
				};

				/** Update all attached observers with multiple subjects sending Subject& */
				inline void NotifyObserversMulti() const {
					for (IObserverRefType observer : observers_refs) {
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
					for (IObserverRefType observer_ref : observers_refs) {
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
				std::forward_list<IObserverRefType> observers_refs{};
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
                explicit ObserverMsgMulti(ISubjectType& subject) noexcept
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
				void Update(const ISubjectType& subject) override {
				};

				/** Update Observer, using event message */
                virtual void Update(const ISubjectType& subject, const std::string& msg) {
                };

                /** Update Observer, using event message */
                template<typename EnumT>
				requires std::is_enum_v<EnumT>
                void Update(const ISubjectType& subject, const EnumT event) {
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
                using ISubjectType = ISubjectMulti;
                using ISubjectRef = std::reference_wrapper<ISubjectType>;

                explicit ObserverAspect(ISubjectType& subject) noexcept
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
				void Update(const ISubjectType& subject) override {
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
				void Update(const ISubjectType& subject,
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
				void DetachSubjectNObserver(ISubjectType& subject) override {
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
				using IObserverType = IObserverMulti;
				using IObserverRef = std::reference_wrapper<IObserverType>;

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
				inline void AttachObserverNSubject(IObserverType& observer) override {
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
				void AttachObserverNSubject(IObserverType& observer, SubjectPartT&& subject_part) {
					//observers_refs.emplace_front(std::ref(observer));
				};
				// TODO: How to notify only those observers, that need changing aspect of subject?


				/** Detach observer_ref_1 from notifying list */
				inline void DetachObserverNSubject(IObserverType& observer) override {
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
                    : ObserverRefSetMulti{ dynamic_cast<ObserverRefSetMulti::ISubjectType&>(observer) } {
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
				void Update(const ISubjectType& subject) override {
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


	} // !namespace behavioral
} // !namespace pattern

#endif // !OBSERVER_HPP
