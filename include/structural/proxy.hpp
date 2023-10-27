#ifndef PROXY_HPP
#define PROXY_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace proxy {
			// https://en.wikipedia.org/wiki/Proxy_pattern
			// 1) Remote proxy(acts as an intermediary for a remote service)
			// 2) Virtual proxy(creates an object lazily on request)
			// 3) Security proxy(adds security to a request)
			// 4) Caching proxy(delivers cached requests)


			class ISubject {
			public:
				virtual ~ISubject() = default;

				virtual void Action1() = 0;
				virtual void Action2() = 0;
			};

			class RealSubject : public ISubject {
			public:
				void Action1() override {};
				void Action2() override {};
			};

			class Proxy : public ISubject {
			public:
				Proxy() = delete;
				Proxy(const Proxy&) = delete;
				Proxy& operator=(const Proxy&) = delete;

				explicit Proxy(std::unique_ptr<ISubject>&& real_subject_p) noexcept
						: real_subject_{ std::move(real_subject_p) } {
				};

				void Action1() override {
					// Some Additional Code
					if (real_subject_) {
						real_subject_->Action1();
					}
					// Some Additional Code
				};
				void Action2() override {};

				/*
				* Can be Virtual Proxy with possibility of data Caching
				* Can be Protected Proxy with possibility of controlling and checking rights of access to real object
				* Can consists of additional Data & Functions
				*/

			private:
				std::unique_ptr<ISubject> real_subject_;
			};

		} // !namespace proxy

	} // !namespace creational
} // !namespace pattern

#endif // !PROXY_HPP
