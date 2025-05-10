#ifndef OBSERVER_HPP
#define OBSERVER_HPP

// TODO: compress include list
#include <algorithm> // remove_if
#include <execution> // execution policies
#include <forward_list>
#include <functional>
#include <memory>
#include <iterator>
#include <string>
#include <type_traits>
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



		namespace observer_weak_ptr {
			// TODO:
			// command pattern instead Update
			// composition over inheritance

			using ExpiredFnType = decltype([](const auto& value_weak_ptr) { return value_weak_ptr.expired(); });


			/** Erase custom n number of expired weak_ptr from container */
			template<typename ContainerType, typename ExecPolicyType>
			inline void EraseNExpired(ContainerType& container,
										const size_t expired_count,
										ExecPolicyType policy = std::execution::seq) {
				if (expired_count == 0) { return; }
				size_t find_count{};
				auto expired = [&expired_count, &find_count](const typename ContainerType::value_type& value_ptr) {
					if (find_count < expired_count && value_ptr.expired()) {
                        ++find_count;
						return true;
					}
					return false;
				};
				if constexpr (std::is_same_v<ContainerType, std::forward_list<typename ContainerType::value_type>>) { // for Forward_list
					container.remove_if(expired); // erase-remove idiom is not for forward_list
				} else { // All other containers
					container.erase(std::remove_if(policy, container.begin(),
													container.end(), expired), container.end());
				}
			};


			/**
			 * Compares if two weak_ptr are alive and have the same stored pointers.
			 *
			 * @param searched_ptr
			 * @param current_ptr
			 * @return result of equality check
			 */
			template<typename ValueType>
			inline bool IsEqualWeakPtr(const std::shared_ptr<ValueType>& searched_shared,
									   const std::weak_ptr<ValueType>& current_ptr) {
                auto current_shared = current_ptr.lock();
				return searched_shared && current_shared && searched_shared.get() == current_shared.get();
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
										const std::weak_ptr<ValueType>& searched_ptr,
										ExecPolicyType policy = std::execution::seq) {
				/*static_assert(std::is_same_v<std::weak_ptr<ValueType>, typename ContainerType::value_type>,
							  "The type mismatch between container elements and weak_ptr");*/

				//using iterator = typename ContainerType::const_iterator;
				using iterator = decltype(container.end());
				iterator it_equal{ container.end() };
				size_t count_expired{};

				if (auto searched_shared = searched_ptr.lock()) {
					auto equal_owner = [&searched_shared, &count_expired](const auto& current_ptr) {
						if (current_ptr.expired()) { ++count_expired; }
						return IsEqualWeakPtr(searched_shared, current_ptr);
					};
					it_equal = std::find_if(policy, container.begin(), container.end(), equal_owner);
				}
				// Cleanup expired weak_ptr
				EraseNExpired(container, count_expired, ExecPolicyType());
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
			inline void EraseEqualWeakPtr(ContainerType& container, typename ContainerType::value_type searched_ptr,
																		ExecPolicyType policy = std::execution::seq) {
				//static_assert(std::is_same_v<std::weak_ptr<ValueType>, typename ContainerType::value_type>);

				//using iterator = typename ContainerType::const_iterator;
				using iterator = decltype(container.end());
				auto it_equal{ container.end() };
				size_t count_expired{};

				if (auto searched_shared = searched_ptr.lock()) {
					if constexpr (std::is_same_v<ContainerType, std::forward_list<typename ContainerType::value_type>>) { // forward_list
						auto equal_owner = [&searched_shared, &count_expired](const auto& current_ptr) {
							if (current_ptr.expired()) { ++count_expired; }
							return IsEqualWeakPtr(searched_shared, current_ptr);
						}; // !lambda end
						container.remove_if(equal_owner);
					} else { // All other types of containers
						it_equal = FindEqualWeakPtr(container, searched_ptr, policy);
						if (it_equal != container.end()) { container.erase(it_equal); }
					}
				}
				// Cleanup expired weak_ptr
				EraseNExpired(container, count_expired, ExecPolicyType());
			}


			/** Erase all expired weak_ptr from container */
			template<typename ContainerType, typename ExecPolicyType>
			//requires std::same_as<ValueType, typename ContainerType::value_type>
			inline void EraseAllExpired(ContainerType& container,
															ExecPolicyType policy = std::execution::seq) {
				//static_assert(std::is_same_v<ValueType, typename ContainerType::value_type>);

				auto expired = [](const typename ContainerType::value_type&value_ptr) { return value_ptr.expired(); };
				if constexpr (std::is_same_v<ContainerType, std::forward_list<typename ContainerType::value_type>>) { // for Forward_list
					container.remove_if(expired); // erase-remove idiom is not for forward_list
				} else { // All other containers
					container.erase(std::remove_if(policy, container.begin(),
															container.end(), expired), container.end());
				}
			};


//============================Single Observer Interfaces====================================

			/**
			 * Abstract. Stateless. Interface of Observer with weak_ptr dependencies.
			 * Alternative name - Subscriber.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 */
			class IObserverWeak {
			protected:
				IObserverWeak() = default;	// Must be created with Subject reference inside
				IObserverWeak(const IObserverWeak&) = delete; // C.67	C.21
				IObserverWeak& operator=(const IObserverWeak&) = delete;
				IObserverWeak(IObserverWeak&&) noexcept = delete;
				IObserverWeak& operator=(IObserverWeak&&) noexcept = delete;
			public:
				virtual ~IObserverWeak() = default;

				/** Update the information about Subject */
				virtual void Update() = 0;
			};

			/**
			 * Abstract. Stateless. Subject with weak_ptr dependencies.
			 * Alternative name - Publisher.
			 * May be used for IObserverWeak, IObserverWeakMulti.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 */
			class ISubjectWeak {
			public:
				using IObserverType = IObserverWeak;
				using WeakPtrIObserverType = std::weak_ptr<IObserverType>;

			protected:
				ISubjectWeak() = default;
				ISubjectWeak(const ISubjectWeak&) = delete; // C.67	C.21
				ISubjectWeak& operator=(const ISubjectWeak&) = delete;
				ISubjectWeak(ISubjectWeak&&) noexcept = delete;
				ISubjectWeak& operator=(ISubjectWeak&&) noexcept = delete;
			public:
				virtual ~ISubjectWeak() = default;

				/** Update all attached observers */
				virtual void NotifyObservers() const = 0;

				/**
				 * Add Observer to list of notification in Subject.
				 * There is shared_ptr<ObserverT> outside of class. Subject stores weak_ptr to that Observer.
				 */
				virtual bool AttachObserver(WeakPtrIObserverType observer_ptr) = 0;
				// not const, cause must attach subject in observer ptr

				/** Detach observer from notifying list */
				virtual bool DetachObserver(WeakPtrIObserverType observer_ptr) = 0;
			};


//=============================Multi Observer Interfaces===========================================

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
				 * Add pair Subject & Observer.
				 * In Observer Add Subject to list of observing. In Subject Add Observer to list of Observers.
				 *
				 * @param subject
				 * @param recursion_depth	0 is default. 0 recursion depth = Attach Subject & Observer.
				 *							1 recursion depth = Attach only Subject.
				 * @return
				 */
				virtual void AttachSubjectNObserver(WeakPtrISubjectType subject_ptr,
													size_t recursion_depth) = 0;
				// not const, cause must attach observer in subject ptr

				/**
				 * Detach pair Subject & Observer.
				 * In Observer Detach Subject, when Subject is destructed. In Subject Detach Observer.
				 *
				 * @param subject				subject, that is destructing.
				 * @param recursion_depth		0 is default. 0 recursion depth = Attach Subject & Observer.
				 *								1 recursion depth = Attach only Subject.
				 */
				virtual void DetachSubjectNObserver(WeakPtrISubjectType subject_ptr,
													size_t recursion_depth) = 0;
				// subject mustn't be const, cause mutate deleting observer in subject

				/** Check if there is subject reference in Observer */
				virtual bool HasSubject(const WeakPtrISubjectType subject_ptr) = 0;

				/** Detach all expired weak_ptr in container */
				virtual void DetachAllExpired() = 0;
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
				 * Add Observer to list of notification in Subject.
				 * There is shared_ptr<ObserverT> outside of class. Subject stores weak_ptr to that Observer.
				 *
				 * @param recursion_depth		0 is default. 0 recursion depth = Attach Observer & Subject.
				 *								1 recursion depth = Attach only Observer.
				 */
				virtual void AttachObserverNSubject(WeakPtrIObserverType observer_ptr,
													size_t recursion_depth) = 0;
				// not const, cause must attach subject in observer ptr

				/**
				 * Detach observer from notifying list.
				 *
				 * @param observer_ptr
				 * @param recursion_depth		0 is default. 0 recursion depth = Attach Observer & Subject.
				 *								1 recursion depth = Attach only Observer.
				 */
				virtual void DetachObserverNSubject(WeakPtrIObserverType observer_ptr,
															size_t recursion_depth) = 0;

				/** Check if there is observer in Subject */
				virtual bool HasObserver(const WeakPtrIObserverType observer_ptr) = 0;

				/** Detach all expired weak_ptr in container */
				virtual void DetachAllExpired() = 0;
			};
			/*
			* Design choices:
			* 1) Maybe return bool of Attach & Detach, cause expired ptrs?
			*/


			//======================================================================================
			// TODO: Realize pull version of Update


			/**
			 * Concrete. Simple observer class realize Upate() and Update(Subject&) functions.
			 * One Observer may has many Subjects.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 * Don't use unordered_set with weak_ptr, cause may give undefined behavior.
			 * Not recommend to use vector, as bad performance of remove_if.
			 * Invariant: don't attach expired weak_ptr. Value type must be weak_ptr. Mustn't duplicate weak_ptr.
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
					for (const auto& subject_ptr : subjects_) {
						if (auto subject_shared{ subject_ptr.lock() }) {
							//subject_shared->DetachObserverNSubject(this->weak_from_this(), kRecursDepthForSingleOperation);
							// Don't need to detach Subjects in Observer, cause Observer will be destructed
							// TODO: parallel policy in Detach
							subject_shared->DetachAllExpired();
						}
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
				void AttachSubjectNObserver(WeakPtrISubjectType subject_ptr,
													size_t recursion_depth = 0) override {
					if (auto subject_shared{ subject_ptr.lock() }) {
						if (!HasSubject(subject_shared)) { // Duplicate control. Mustn't duplicate weak_ptr
							if constexpr (std::is_same_v<ContainerType_t, ContainerForwardListType> ) {
								subjects_.emplace_after(subjects_.cbefore_begin(), std::move(subject_ptr));
							} else { // All other types of containers, except forward_list
								subjects_.emplace(std::move(subject_ptr));
							}
						}
						if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
							subject_shared->AttachObserverNSubject(this->weak_from_this(), ++recursion_depth);
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
				inline void DetachSubjectNObserver(WeakPtrISubjectType subject_ptr,
												   size_t recursion_depth = 0) override {
					// Can Detach only alive objects
					if (auto subject_shared{ subject_ptr.lock() }; subject_shared && HasSubject(subject_shared)) {
						// First Detach Observer in Subject
						if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
							subject_shared->DetachObserverNSubject(this->weak_from_this(), ++recursion_depth);
						}
						EraseEqualWeakPtr(subjects_, std::move(subject_ptr), ExecPolicyType());
					}
				};


				/** Detach all expired weak_ptr subjects in container */
				inline void DetachAllExpired() {
					EraseAllExpired(subjects_, ExecPolicyType());
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
					for (const auto& observer_ptr : observers_) {
						if (auto observer_shared{ observer_ptr.lock() }) {
							//observer_shared->DetachSubjectNObserver(this->weak_from_this(), kRecursDepthForSingleOperation);
							// TODO: parallel policy in Detach
							// Don't need to detach Observers in Subject, cause Subject will be destructed
							observer_shared->DetachAllExpired();
						}
					}
				};


				/** Update all attached observers */
				inline void NotifyObservers() override { // not const, cause cleanup operations
					std::vector<iterator> cleanup_list{};
					auto current_it{ observers_.begin() };
					auto update_fn = [&cleanup_list, &current_it](auto& observer_ptr) {
						if (auto observer_shared{ observer_ptr.lock() }) {
							observer_shared->Update();
						} else { // clean expired
							cleanup_list.emplace_back(current_it);
						}
						++current_it;
					};
					std::for_each(ExecPolicyType(), observers_.begin(), observers_.end(), update_fn);
				};

				/** Update all attached observers */
				virtual inline void NotifyObserversMulti() { // not const, cause cleanup operations
					/*auto weak_this{ this->weak_from_this() };
					auto update_fn = [&weak_this](const auto& observer_ptr) {
						if (auto observer_shared{ observer_ptr.lock() }) {
							observer_shared->Update(weak_this);
						}
					};
					std::for_each(ExecPolicyType(), observers_.begin(), observers_.end(), update_fn);*/
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
				void AttachObserverNSubject(WeakPtrIObserverType observer_ptr,
													size_t recursion_depth = 0) override {
					if (auto observer_shared{ observer_ptr.lock() }) { // observer_ptr is not expired
						if (!HasObserver(observer_shared)) { // Duplicate control. Mustn't duplicate weak_ptr
							if constexpr (std::is_same_v<ContainerType_t, ContainerForwardListType>) { // if ForwardList
								observers_.emplace_after(observers_.cbefore_begin(), std::move(observer_ptr));
							} else { // All other types of containers, except forward_list
								observers_.emplace(std::move(observer_ptr));
							}
						}
						if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
							observer_shared->AttachSubjectNObserver(this->weak_from_this(), ++recursion_depth);
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
				void DetachObserverNSubject(WeakPtrIObserverType observer_ptr,
											size_t recursion_depth = 0) override {
					// Can Detach only alive objects
					if (auto observer_shared{ observer_ptr.lock() }; observer_shared && HasObserver(observer_shared)) {
						// First Detach Observer in Subject
						if (recursion_depth < kRecursDepthForSingleOperation) { // pair operation Observer-Subject
							observer_shared->DetachSubjectNObserver(this->weak_from_this(), ++recursion_depth);
						}
						EraseEqualWeakPtr(observers_, std::move(observer_ptr), ExecPolicyType());
					}
				};

				/** Detach all expired weak_ptr subjects in container */
				inline void DetachAllExpired() {
					EraseAllExpired(observers_, ExecPolicyType());
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
				inline bool HasObserver(const WeakPtrIObserverType observer_ptr) override {
					return FindEqualWeakPtr(observers_, observer_ptr, ExecPolicyType()) != observers_.cend();
				};

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
