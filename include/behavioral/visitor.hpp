#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace visitor {
			// https://en.wikipedia.org/wiki/Visitor_pattern
			// Separate algorithm from object structure
			// Class Structure mustn't change frequently.
			// Operations on Class Structure added frequently.

			//
			// Standard library type is std::variant. It is realization of Visitor Pattern.
			//



			class IVisitor;
			class ElementA;
			class ElementB;

			/** Abstract. */
			class IElement {
			protected:
				IElement() = default;
				IElement(const IElement&) = delete; // C.67	C.21
				IElement& operator=(const IElement&) = delete;
				IElement(IElement&&) noexcept = delete;
				IElement& operator=(IElement&&) noexcept = delete;
			public:
				virtual ~IElement() = default;

				virtual void Accept(IVisitor& visitor_p) = 0;
			};

			/** Abstract. */
			class IVisitor {
			protected:
				IVisitor() = default;
				IVisitor(const IVisitor&) = delete; // C.67	C.21
				IVisitor& operator=(const IVisitor&) = delete;
				IVisitor(IVisitor&&) noexcept = delete;
				IVisitor& operator=(IVisitor&&) noexcept = delete;
			public:
				virtual ~IVisitor() = default;

				virtual void VisitElementA(ElementA& element_p) = 0;
				virtual void VisitElementB(ElementB& element_p) = 0;
			};


			class ElementA : public IElement {
			public:
				void Accept(IVisitor& visitor_p) override {
					visitor_p.VisitElementA(*this);
				};

				void OperationA() {
				};
			};

			class ElementB : public IElement {
			public:
				void Accept(IVisitor& visitor_p) override {
					visitor_p.VisitElementB(*this);
				};

				void OperationB() {
				};
			};


			class Visitor1 : public IVisitor {
			public:
				/* You can use std::visit for realization */

				void VisitElementA(ElementA& element_p) override {
					element_p.OperationA();
				};

				void VisitElementB(ElementB& element_p) override {
					element_p.OperationB();
				};
			};

		} // !namespace visitor

	} // !namespace behavioral
} // !namespace pattern

#endif // !VISITOR_HPP
