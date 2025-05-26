#ifndef DEPENDENCY_INJECTION_HPP
#define DEPENDENCY_INJECTION_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		// https://en.wikipedia.org/wiki/Dependency_injection
		// https://www.bogotobogo.com/DesignPatterns/Dependency_Injection_Design_Pattern_Inversion_Of_Control_IoC.php


		namespace dependency_injection_boost {
			// https://boost-ext.github.io/di/
		}

		namespace dependency_injection {
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


		namespace dependency_injection_gigachat {
			/*class IService {
			public:
				virtual ~IService() = default;
				virtual void performService() const = 0;
			};

			// Реализация сервиса
			class ConcreteService : public IService {
			public:
				void performService() const override { std::cout << "Concrete service performed\n"; }
			};

			// Класс, использующий сервис
			class Client {
			private:
				IService* _service;

			public:
				// Инъекция зависимости через конструктор
				explicit Client(IService* service) : _service(service) {}

				void doWork() const {
					if (_service != nullptr)
						_service->performService();
				}
			};

			int main_1() {
				auto concreteService = new ConcreteService(); // Создаем реализацию сервиса
				Client client(concreteService);               // Передаем зависимость клиенту
				client.doWork();                              // Используем клиент
				delete concreteService;                      // Освобождаем память
			}


			class IService {
			public:
				virtual ~IService() = default;
				virtual void performService() const = 0;
			};

			class ConcreteService : public IService {
			public:
				void performService() const override { std::cout << "Concrete service performed\n"; }
			};

			class Client {
			private:
				IService* _service;

			public:
				Client() : _service(nullptr) {}           // Конструктор без аргументов
				~Client() { delete _service; }            // Уничтожение зависимости

				void setService(IService* service) {      // Установка зависимости через setter
					delete _service;
					_service = service;
				}

				void doWork() const {
					if (_service != nullptr)
						_service->performService();
				}
			};


			int main() {
				auto concreteService = new ConcreteService();
				Client client;
				client.setService(concreteService);
				client.doWork();
				delete concreteService;
			}


			Автоматизация инъекций с помощью фабрики сервисов

				Можно создать фабрику для управления созданием объектов - зависимостей и упростить работу с ними :

#include <memory>

			class IService {
			public:
				virtual ~IService() = default;
				virtual void performService() const = 0;
			};

			class ConcreteService : public IService {
			public:
				void performService() const override { std::cout << "Concrete service performed\n"; }
			};

			template<typename T>
			std::unique_ptr<T> createService() {
				return std::make_unique<T>();
			}

			class ServiceFactory {
			public:
				template<typename T>
				static std::unique_ptr<IService> makeService() {
					return createService<T>();
				}
			};

			class Client {
			private:
				std::unique_ptr<IService> _service;

			public:
				explicit Client(std::unique_ptr<IService>&& service) : _service(std::move(service)) {}

				void doWork() const {
					if (_service.get())
						_service->performService();
				}
			};

		Использование:

			int main() {
				auto service = ServiceFactory::makeService<ConcreteService>();
				Client client(std::move(service));
				client.doWork();
			}




#include <boost/di.hpp>
			namespace di = boost::di;

			struct IService {
				virtual ~IService() = default;
				virtual void performService() const = 0;
			};

			struct ConcreteService : IService {
				void performService() const override { std::cout << "Concrete service performed\n"; }
			};

			struct Client {
				Client(const IService& service) : _service(service) {}
				void doWork() const { _service.performService(); }

			private:
				const IService& _service;
			};

			auto injector = [] {
				return di::make_injector(
					di::bind<IService>().to<ConcreteService>()
				);
				};

			int main() {
				auto container = injector();
				auto client = container.create<Client>();
				client.doWork();
			}
			*/

		}

	} // !namespace creational
} // !namespace pattern

#endif // !DEPENDENCY_INJECTION_HPP
