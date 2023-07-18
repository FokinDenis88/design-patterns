#ifndef FACTORY_METHOD_HPP
#define FACTORY_METHOD_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace factory_method {
			enum class ProductID : unsigned int {
				TypeA = 0,
				TypeB,
				TypeC
			};

			class IProduct {
			public:
				virtual ~IProduct() = default;
			};

			class ProductA : public IProduct {
			public:
			};

			class ProductB : public IProduct {
			public:
			};

			class ProductC : public IProduct {
			public:
			};



			/** Factory Method Class. Allows to redefine what objects will be instantiated in sublasses. Abstract. */
			template<typename IProductType, typename ProductID_Type>
			class IFactoryMethod {
			public:
				virtual ~IFactoryMethod() = default;

				/** Create product of type ProductID */
				virtual std::unique_ptr<IProductType> Create(ProductID_Type product_id) const = 0;
			};



			class CreatorA : public IFactoryMethod<IProduct, ProductID> {
			public:
				std::unique_ptr<IProduct> Create(ProductID product_id) const override {
					switch (product_id) {
						using enum ProductID;
					case TypeA:
						return std::make_unique<ProductA>();
					case TypeB:
						return std::make_unique<ProductB>();
					case TypeC:
						return std::make_unique<ProductC>();
					default:
						return nullptr;
					}
				};
			};


			//=======================Templated Methods===================================
			/*template<typename ProductType, typename ProductID_Type>
			class IFactoryMethod {
			public:
				virtual ~IFactoryMethod() = default;

				virtual std::unique_ptr<ProductType> Create(ProductID_Type product_id) const = 0;
			};

			template<typename ProductType, typename ProductID_Type>
			class CreatorA : public IFactoryMethod<ProductType, ProductID_Type> {
			public:
				std::unique_ptr<ProductType> Create(ProductID_Type product_id) const override {
					switch (product_id) {
						using enum ProductID;
					case TypeA:
						return std::make_unique<ProductA>();
					case TypeB:
						return std::make_unique<ProductB>();
					case TypeC:
						return std::make_unique<ProductC>();
					default:
						return nullptr;
					}
				};
			};*/

			void Run();

		} // !namespace factory_method
	} // !namespace creational
} // !namespace pattern

#endif // !FACTORY_METHOD_HPP
