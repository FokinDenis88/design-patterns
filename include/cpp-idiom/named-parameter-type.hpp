#ifndef NAMED_PARAMETER_TYPE_HPP
#define NAMED_PARAMETER_TYPE_HPP

#include <memory>
#include <utility>

#include <iostream>

/** C++ Idioms */
namespace pattern {
    namespace cpp_idiom {
        namespace named_parameter_type {
            // https://www.fluentcpp.com/2018/12/14/named-arguments-cpp/
            // https://www.fluentcpp.com/2016/12/08/strong-types-for-strong-interfaces/

            template <typename T, typename Parameter>
            class NamedT {
            public:
                explicit NamedT(T const& value) : value_(value) {}
                explicit NamedT(T&& value) : value_(std::move(value)) {}

                T& get() { return value_; }
                T const& get() const { return value_; }

            private:
                T value_;
            }; // class NamedT

            using Width = NamedT<double, struct WidthParameter>;
            // struct WidthParameter - is phantom template argumate, cause it is not used in NamedT

        } // !namespace named_parameter_type

    } // !namespace cpp_idiom
} // !namespace pattern

#endif // !NAMED_PARAMETER_TYPE_HPP
