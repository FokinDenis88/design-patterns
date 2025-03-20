#ifndef DEPENDENCY_INJECTION_HPP
#define DEPENDENCY_INJECTION_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace dependency_injection {
			// https://en.wikipedia.org/wiki/Dependency_injection
			// https://www.bogotobogo.com/DesignPatterns/Dependency_Injection_Design_Pattern_Inversion_Of_Control_IoC.php


			/** Abstract. */
			class IServices {
			protected:
				IServices() = default;
				IServices(const IServices&) = delete; // C.67	C.21
				IServices& operator=(const IServices&) = delete;
				IServices(IServices&&) noexcept = delete;
				IServices& operator=(IServices&&) noexcept = delete;
			public:
				virtual ~IServices() = default;

				virtual void RunServices() = 0;
			};

			class ConcreteServices1 : public IServices {
			public:
				void RunServices() override {
				};
			};

			class ConcreteServices2 : public IServices {
			public:
				void RunServices() override {
				};
			};

			enum class ServiceID : unsigned {
				First = 0,
				Second
			};

			/**
			 * Constructor injection
			 * Setter injection
			 * Interface injection
			 */
			class Injector {
			public:
				std::unique_ptr<IServices> CreateServices(ServiceID service_id) {
					switch (service_id) {
						case ServiceID::First:
							return std::make_unique<ConcreteServices1>();
							break;
						case ServiceID::Second:
							return std::make_unique<ConcreteServices2>();
							break;
						default:
							return std::make_unique<ConcreteServices1>();
							break;
					}
				};
			};


			/** Services are injected through class constructor */
			class Client {
			public:
				Client() = delete;
				Client(const Client&) = delete;
				Client operator=(const Client&) = delete;

				Client(Client&&) noexcept = default;
				Client& operator=(Client&&) noexcept = default;

				explicit Client(std::unique_ptr<IServices>&& services_p) noexcept : services_{ std::move(services_p) } {
				};

				//const std::unique_ptr<IServices>& services() const noexcept { return services_; };
				//void set_services(std::unique_ptr<IServices>&& services_p) { services_ = std::move(services_p); };

			private:
				std::unique_ptr<IServices> services_;
			};

		} // !namespace dependency_injection

	} // !namespace creational
} // !namespace pattern

#endif // !DEPENDENCY_INJECTION_HPP
