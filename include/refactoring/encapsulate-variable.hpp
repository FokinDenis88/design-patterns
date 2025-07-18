#ifndef ENCAPSULATE_VARIABLE_HPP
#define ENCAPSULATE_VARIABLE_HPP

#include "creational/singleton.hpp"

#include <tuple>
#include <type_traits>

namespace refactoring {
	namespace encapsulate_variable{ // experiments classes

        /** Example of data */
		struct GlobalVariablesData {
			int a{};
			double b{};
        };

        enum class GlobalVariablesEnum : size_t {
            a = 0,
            b
        };


        /** Class helps to track mutations of global variables */
        template<typename GlobalVariablesStructT>
        class GlobalVariables {
        public:
            static void set_a(const int new_a) {
                global_variables_.a = new_a;
            }

            static int a() {
                return global_variables_.a;
            };

            static void set_b(const int new_b) {
                global_variables_.b = new_b;
            }

            static int b() {
                return global_variables_.b;
            };

        protected:	// protected for inheritance; private for single class with data
            GlobalVariables() = default; // There must be only one instance of class, so hidden constructor
            GlobalVariables(const GlobalVariables&) = delete;
            GlobalVariables& operator=(const GlobalVariables&) = delete;
            GlobalVariables(GlobalVariables&&) noexcept = delete;
            GlobalVariables& operator=(GlobalVariables&&) noexcept = delete;
            virtual ~GlobalVariables() = default;

        private:
            inline static GlobalVariablesStructT global_variables_{};
        };



        // !Better to use GlobalVariables variant. It is not universal.
        /**
         * Class helps to track mutations of global variables.
         * Variables are stored in tuple and accessed by using of EnumT in set/get operations.
         */
        template<typename EnumT, typename... ArgsT>
        class GlobalVariablesTuple {
        public:
            /** Used for selection of variable for set/get operations */
            //using EnumT = EnumT;

            template<EnumT VariableIndex, typename ArgT>
            static void set_variable(const ArgT&& new_value) {
                //static_assert(std::is_same_v<decltype(std::get<VariableIndex>(global_variables_)), ArgT>);
                constexpr size_t index{ static_cast<size_t>(VariableIndex) };
                std::get<index>(global_variables_) = new_value;
            };

            template<EnumT VariableIndex>
            static const auto& get_variable() {
                constexpr size_t index{ static_cast<size_t>(VariableIndex) };
                return std::get<index>(global_variables_);
            };

        protected:	// protected for inheritance; private for single class with data
            GlobalVariablesTuple() = default; // There must be only one instance of class, so hidden constructor
            GlobalVariablesTuple(const GlobalVariablesTuple&) = delete;
            GlobalVariablesTuple& operator=(const GlobalVariablesTuple&) = delete;
            GlobalVariablesTuple(GlobalVariablesTuple&&) noexcept = delete;
            GlobalVariablesTuple& operator=(GlobalVariablesTuple&&) noexcept = delete;
            virtual ~GlobalVariablesTuple() = default;

        private:
            inline static std::tuple<ArgsT...> global_variables_{};
        };

	} // !namespace encapsulate_variable
} // !namespace refactoring

#endif // !ENCAPSULATE_VARIABLE_HPP
