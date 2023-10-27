#ifndef TEMPLATE_METHOD_HPP
#define TEMPLATE_METHOD_HPP

#include <iostream>
#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace template_method {

			/** Template method. Abstract. */
			class TemplateMethodAbstract {
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
