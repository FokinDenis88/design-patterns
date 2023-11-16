#ifndef TEMPLATE_METHOD_HPP
#define TEMPLATE_METHOD_HPP

#include <iostream>
#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace template_method {

			/** Abstract. Template method. */
			class TemplateMethodAbstract {
			protected:
				TemplateMethodAbstract() = default;
				TemplateMethodAbstract(const TemplateMethodAbstract&) = delete; // C.67	C.21
				TemplateMethodAbstract& operator=(const TemplateMethodAbstract&) = delete;
				TemplateMethodAbstract(TemplateMethodAbstract&&) noexcept = delete;
				TemplateMethodAbstract& operator=(TemplateMethodAbstract&&) noexcept = delete;
			public:
				/** Make class Abstract and give possibility to be override in Derived classes */
				virtual ~TemplateMethodAbstract() = 0 {};

				void TemplateMethod() {

				};

			protected:
				virtual void PrimitiveOperation1() = 0;

				/** Hook Method */
				virtual void PrimitiveOperation2() {

				};
			};

			class TemplateMethodConcrete1 : public TemplateMethodAbstract {
			protected:
				void PrimitiveOperation1() override {

				};
				void PrimitiveOperation2() override {

				};
			};

		} // !namespace template_method

	} // !namespace behavioral
} // !namespace pattern

#endif // !TEMPLATE_METHOD_HPP
