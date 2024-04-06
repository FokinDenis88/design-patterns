#ifndef NULL_OBJECT_HPP
#define NULL_OBJECT_HPP

#include <memory>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace null_object {
			// https://en.wikipedia.org/wiki/Null_object_pattern
			// https://iq.opengenus.org/null-object-design-pattern-cpp/

			/** Abstract. */
			class ISubject {
			protected:
				ISubject() = default;
				ISubject(const ISubject&) = default; // C.67	C.21
				ISubject& operator=(const ISubject&) = default;
				ISubject(ISubject&&) noexcept = default;
				ISubject& operator=(ISubject&&) noexcept = default;
			public:
				virtual ~ISubject() = default;

				virtual void Action() = 0;
			};

			class RealSubject : public ISubject {
			public:
				void Action() override {
					// Do something
					int a = 0;
				};
			};

			class NullSubject : public ISubject {
			public:
				void Action() override {
					// Empty function body
				};
			};

		} // !namespace null_object

	} // !namespace behavioral
} // !namespace pattern

#endif // !NULL_OBJECT_HPP
