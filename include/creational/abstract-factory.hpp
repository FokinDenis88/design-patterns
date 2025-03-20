#ifndef ABSTRACT_FACTORY_HPP
#define ABSTRACT_FACTORY_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace abstract_factory {
			// https://en.wikipedia.org/wiki/Abstract_factory_pattern
			// Patterns Friends: Factory Method (CreateFunctions), Prototype (CreateFunctions), Singleton (Factory Instance)

			class IProduct {
			public:
				virtual ~IProduct() = default;

				virtual void foo() = 0;
			};

			class ProductA : public IProduct {
			public:
				void foo() override {};
			};

			class ProductB : public IProduct {
			public:
				void foo() override {};
			};

			class ProductC : public IProduct {
			public:
				void foo() override {};
			};


			/** Abstract. May include inside factory method, prototype, builder */
			class IAbstractFactory {
			protected:
				IAbstractFactory() = default;
				IAbstractFactory(const IAbstractFactory&) = delete; // C.67	C.21
				IAbstractFactory& operator=(const IAbstractFactory&) = delete;
				IAbstractFactory(IAbstractFactory&&) noexcept = delete;
				IAbstractFactory& operator=(IAbstractFactory&&) noexcept = delete;
			public:
				virtual ~IAbstractFactory() = default;

				virtual std::unique_ptr<IProduct> CreateProductA() = 0;
				virtual std::unique_ptr<IProduct> CreateProductB() = 0;
				virtual std::unique_ptr<IProduct> CreateProductC() = 0;
			};

			class ConcreteFactory : public IAbstractFactory {
			public:
				inline std::unique_ptr<IProduct> CreateProductA() override {
					return std::make_unique<ProductA>();
				};
				inline std::unique_ptr<IProduct> CreateProductB() override {
					return std::make_unique<ProductB>();
				};
				inline std::unique_ptr<IProduct> CreateProductC() override {
					return std::make_unique<ProductC>();
				};
			};

		} // !namespace abstract_factory

	} // !namespace creational
} // !namespace pattern

#endif // !ABSTRACT_FACTORY_HPP
