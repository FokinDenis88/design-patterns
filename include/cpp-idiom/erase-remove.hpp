#ifndef ERASE_REMOVE_HPP
#define ERASE_REMOVE_HPP

#include <algorithm> // remove_if
#include <execution> // policy

/** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace erase_remove {
			// https://en.wikipedia.org/wiki/Erase%E2%80%93remove_idiom

			template<typename ContainerType,typename PredicateType, typename ExecutionPolicyType>
			inline ContainerType::iterator
				EraseRemoveIf(ContainerType& container, PredicateType predicate, ExecutionPolicyType policy = std::execution::seq)
			{ //container.erase(std::remove_if(policy, container.begin(), container.end(), predicate), container.end())
				auto end{ container.end() };
				return container.erase(std::remove_if(policy, container.begin(), end, predicate), end);
			}

			template<typename ContainerType, typename PredicateType, typename ExecutionPolicyType>
			inline ContainerType::const_iterator
				EraseRemoveIf(const ContainerType& container, PredicateType predicate, ExecutionPolicyType policy = std::execution::seq)
			{
				return container.erase(std::remove_if(policy, container.cbegin(), container.cend(), predicate), container.cend());
			}

		} // !namespace erase_remove

	} // !namespace cpp_idiom
} // !namespace pattern

#endif // !ERASE_REMOVE_HPP
