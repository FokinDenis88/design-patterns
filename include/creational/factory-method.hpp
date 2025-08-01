﻿#ifndef FACTORY_METHOD_HPP
#define FACTORY_METHOD_HPP

#include <memory>
#include <utility>
#include <type_traits>

#include <unordered_map>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		// https://en.wikipedia.org/wiki/Factory_method_pattern
		// Delegate creation of object to subclasses
		//
		// Variants of realization:
		// 1) ICreator Interface class is Abstract
		// 2) ICreator Interface class is Concrete with default functions realization
		// 3) Create function in ICreator Interface class is parametrized with enum of ProductID. Parametrized Factory.
		// 4) Create function in ICreator Interface class is a template.
		//
		// Friend patterns: Abstract Factory, Template Method, Prototype


		namespace factory_method_boost {
			// https://www.boost.org/doc/libs/1_81_0/libs/functional/factory/doc/html/index.html
		}

		namespace factory_method {
			enum class ProductID : unsigned int {
				TA = 0,
				TB,
				TC
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


			/** Abstract. Factory Method Class. Allows to redefine what objects will be instantiated in sublasses. */
			template<typename IProductT, typename ProductID_T>
			requires std::is_enum_v<ProductID_T>
			class IFactoryMethod {
			protected:
				IFactoryMethod() = default;
				IFactoryMethod(const IFactoryMethod&) = delete; // C.67	C.21
				IFactoryMethod& operator=(const IFactoryMethod&) = delete;
				IFactoryMethod(IFactoryMethod&&) noexcept = delete;
				IFactoryMethod& operator=(IFactoryMethod&&) noexcept = delete;
			public:
				virtual ~IFactoryMethod() = default;

				/** Create product of type ProductID */
				virtual std::unique_ptr<IProductT> Create(ProductID_T product_id) const = 0;
			};



			class CreatorA : public IFactoryMethod<IProduct, ProductID> {
			public:
				std::unique_ptr<IProduct> Create(ProductID product_id) const override {
					switch (product_id) {
						using enum ProductID;
					case TA:
						return std::make_unique<ProductA>();
						break;
					case TB:
						return std::make_unique<ProductB>();
						break;
					case TC:
						return std::make_unique<ProductC>();
						break;
					default:
						return nullptr;
						break;
					}
				};
			};

		} // !namespace factory_method


		namespace factory_method_book {
			class IProduct {
			public:
				virtual ~IProduct() = default;
			};

			class Product1 : public IProduct {
			public:
				virtual ~Product1() = default;
			};

			class Product2 : public IProduct {
			public:
				virtual ~Product2() = default;
			};

			class Product3 : public IProduct {
			public:
				virtual ~Product3() = default;
			};

			inline auto custom_deleter = [](IProduct* product_ptr) {
				// MakeLog(product_ptr);
				delete product_ptr;
			};

			template<typename... ArgTs>
			std::unique_ptr<IProduct, decltype(custom_deleter)> CreateProduct(ArgTs&&... args) {
			//auto CreateProduct(ArgTs&&... args) {
				std::unique_ptr<IProduct, decltype(custom_deleter)> product_ptr(nullptr, custom_deleter);
				size_t switch_value{ 0 };
				switch (switch_value) {
				case 0:
					product_ptr.reset(new Product1(std::forward<ArgTs>(args)...));
					break;
				case 1:
					product_ptr.reset(new Product2(std::forward<ArgTs>(args)...));
					break;
				case 2:
					product_ptr.reset(new Product3(std::forward<ArgTs>(args)...));
					break;
				default:
					break;

					return product_ptr;
				}
			}
		} // !namespace factory_method_book

		namespace factory_method_book_2 {
			// Scott Mayers Example
			enum class WidgetID {
			};
			class Widget {
			public:
			};

			/** Old fabric method */
			inline std::shared_ptr<const Widget> loadWidget(WidgetID id) {
				return std::make_shared<const Widget>();
			}

			/** Caching fabric method with option of removing cache */
			inline std::shared_ptr<const Widget> fastLoadWidget(WidgetID id) {
				static std::unordered_map<WidgetID, std::weak_ptr<const Widget>> cache;
				auto objPtr = cache[id].lock();
				if (!objPtr) {
					objPtr = loadWidget(id);
					cache[id] = objPtr;
				}
				return objPtr;
			}
		} // !namespace factory_method_book_2


		namespace factory_method_1 {
			class IProduct {
			public:
				virtual ~IProduct() = default;
			};

			class Product1 : public IProduct {
			public:
				virtual ~Product1() = default;
			};

			class Product2 : public Product1 {
			public:
				virtual ~Product2() = default;
			};

			class IFactoryMethod {
			public:
				virtual ~IFactoryMethod() = default;

				virtual std::unique_ptr<IProduct> Create() const = 0;
			};

			class FactoryMethodA : public IFactoryMethod {
			public:
				std::unique_ptr<IProduct> Create() const noexcept override {
					return std::make_unique<Product1>();
				};
			};

			class FactoryMethodB : public FactoryMethodA {
			public:
				std::unique_ptr<IProduct> Create() const noexcept override {
					return std::make_unique<Product2>();
				};
			};
		} // !namespace factory_method_1


		namespace factory_method_2 {
			class IProduct {
			public:
				virtual ~IProduct() = default;
			};

			class Product1 : public IProduct {
			public:
				virtual ~Product1() = default;
			};

			class Product2 : public Product1 {
			public:
				virtual ~Product2() = default;
			};

			class IFactoryMethod {
			public:
				virtual ~IFactoryMethod() = default;

				virtual std::unique_ptr<IProduct> Create() const {
				};
			};

			class FactoryMethodA : public IFactoryMethod {
			public:
				std::unique_ptr<IProduct> Create() const noexcept override {
					return std::make_unique<Product1>();
				};
			};

			class FactoryMethodB : public FactoryMethodA {
			public:
				std::unique_ptr<IProduct> Create() const noexcept override {
					return std::make_unique<Product2>();
				};
			};
		} // !namespace factory_method_2

		namespace factory_method_3 {
			class IProduct {
			public:
				virtual ~IProduct() = default;
			};

			class Product1 : public IProduct {
			public:
				virtual ~Product1() = default;
			};

			class Product2 : public Product1 {
			public:
				virtual ~Product2() = default;
			};

			enum class ProductID : size_t {
				FirstID = 0,
				SecondID
			};

			class IFactoryMethod {
			public:
				virtual ~IFactoryMethod() = default;

				virtual std::unique_ptr<IProduct> Create(ProductID) const = 0;
			};

			class FactoryMethodA : public IFactoryMethod {
			public:
				std::unique_ptr<IProduct> Create(ProductID product_id) const override {
					switch (product_id) {
					case  ProductID::FirstID:
						return std::make_unique<Product1>();
						break;
					case  ProductID::SecondID:
						return std::make_unique<Product2>();
						break;
					default:
						return std::make_unique<Product1>();
						break;
					}
				};
			};

			class FactoryMethodB : public FactoryMethodA {
			public:
				std::unique_ptr<IProduct> Create(ProductID product_id) const override {
					switch (product_id) {
					case  ProductID::FirstID:
						return std::make_unique<Product2>();
						break;
					case  ProductID::SecondID:
						return std::make_unique<Product1>();
						break;
					default:
						return std::make_unique<Product2>();
						break;
					}
				};
			};
		} // !namespace factory_method_3



		namespace factory_method_4 {
			class IProduct {
			public:
				virtual ~IProduct() = default;
			};

			class Product1 : public IProduct {
			public:
				virtual ~Product1() = default;
			};

			class Product2 : public Product1 {
			public:
				virtual ~Product2() = default;
			};

			template<typename ProductT>
			class FactoryMethodA {
			public:
				std::unique_ptr<IProduct> Create() const noexcept {
					return std::make_unique<ProductT>();
				};
			};

		} // !namespace factory_method_4

	} // !namespace creational
} // !namespace pattern

#endif // !FACTORY_METHOD_HPP
