﻿#ifndef ADAPTER_HPP
#define ADAPTER_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace structural {

		namespace adapter_boost {
			// https://www.boost.org/doc/libs/master/libs/iterator/doc/iterator_adaptor.html
		}

		namespace adapter {
			// https://en.wikipedia.org/wiki/Adapter_pattern
			// https://www.modernescpp.com/index.php/the-adapter-pattern/

			/** Adaptee Class */
			class A {
			public:
				void AFunc() {

				};
			};

			/** Target of Adaptation */
			class B {
			public:
				void BFunc() {

				};
			};


			/** Interface of Adapter */
			class IAdapter {
			protected:
				IAdapter() = default;
				IAdapter(const IAdapter&) = delete; // C.67	C.21
				IAdapter& operator=(const IAdapter&) = delete;
				IAdapter(IAdapter&&) noexcept = delete;
				IAdapter& operator=(IAdapter&&) noexcept = delete;
			public:
				virtual ~IAdapter() = default;

				virtual void SomeInterfaceFunction() = 0;
			};

			/** Run-Time Adapter */
			class RunTimeAdapter : public IAdapter {
			public:
				void SomeInterfaceFunction() override {
					adaptee_->AFunc();
				};

			private:
				std::unique_ptr<A> adaptee_{};
			};

			/** Compile Time Adapter */
			class CompileTimeAdapter : public IAdapter, private A {
			public:
				void SomeInterfaceFunction() override {
					AFunc();
				};
			};

		} // !namespace adapter

	} // !namespace structural
} // !namespace pattern

#endif // !ADAPTER_HPP
