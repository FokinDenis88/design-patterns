#ifndef GENERIC_OBSERVER_HPP
#define GENERIC_OBSERVER_HPP

// TODO: compress include list
//#include <atomic>
#include <algorithm> // remove_if
#include <execution> // execution policies
//#include <forward_list>
//#include <functional>
//#include <future>	// for Async Update
//#include <memory>
//#include <mutex>
//#include <iterator>
//#include <initializer_list>	// for AttachObservers
//#include <string>
//#include <shared_mutex>
//#include <system_error>	// thread execution exception
//#include <set>
//#include <thread>	// concurrency and thread safety SubjectWeakHub
//#include <type_traits>
//#include <list>
//#include <utility> // pair
//#include <unordered_set>
//#include <vector>

// Container variants


#include "memory-management-library/weak-ptr/weak-ptr.hpp"


/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace observer {
			/**
			 * Notify all observers in container by lambda function, that
			 * encapsulate observer update function call.
			 * Can freeze main thread, if it is long to Update observer.
			 *
			 * Complexity: O(n)
			 * Mutex: read
			 *
			 * @param observers
			 * @param observer_update_fn		lambda that encapsulate observer update function call, f.e. observer->Update()
			 * @return							count of expired weak_ptr
			 */
			template<typename ContainerType, typename UpdateFunctionType, typename ExecPolicyType>
			inline size_t GenericNotifyWeakObservers(const ContainerType& observers,
													UpdateFunctionType observer_update_fn,
													ExecPolicyType policy = std::execution::seq) {
				size_t expired_count{};
				if (observers.empty) { return expired_count; } // precondition

				auto update_fn = [&observer_update_fn, &expired_count](const auto& observer_ptr) {
					if (auto observer_shared{ observer_ptr.lock() }) {
						observer_update_fn(observer_shared);
					} else {
						++expired_count;
					}
				}; // !lambda
				std::for_each(policy, observers.begin(), observers.end(), update_fn);
				return expired_count;
			}

			/**
			 * Notify all observers in container by lambda function, that
			 * encapsulate observer update function call.
			 * Can freeze main thread, if it is long to Update observer.
			 *
			 * Complexity: O(n)
			 * Mutex: read + write
			 *
			 * @param observers
			 * @param observer_update_fn		lambda that encapsulate observer update function call, f.e. observer->Update()
			 */
			template<typename ContainerType, typename UpdateFunctionType, typename ExecPolicyType>
			inline void GenericNotifyWeakObserversNClean(ContainerType& observers,
														UpdateFunctionType observer_update_fn,
														ExecPolicyType policy = std::execution::seq) {
				size_t expired_count{ GenericNotifyWeakObservers(observers, observer_update_fn, policy) };
				// Cleanup expired weak_ptr
				EraseNExpiredWeakPtr(observers, expired_count, policy);
			}

		} // !namespace observer

	} // !namespace behavioral

} // !namespace pattern

#endif // !GENERIC_OBSERVER_HPP
