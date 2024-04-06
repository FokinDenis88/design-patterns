#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace iterator {
			// https://en.wikipedia.org/wiki/Iterator_pattern
			// Iterator-Cursor
			// Stable-Iterator can work with aggregate, changing size

			class IIterator;

			/** Abstract. */
			class IAggregate {
			protected:
				IAggregate() = default;
				IAggregate(const IAggregate&) = default; // C.67	C.21
				IAggregate& operator=(const IAggregate&) = default;
				IAggregate(IAggregate&&) noexcept = default;
				IAggregate& operator=(IAggregate&&) noexcept = default;
			public:
				virtual ~IAggregate() = default;

				virtual std::unique_ptr<IIterator> CreateIterator() const = 0;
			};

			class ConcreteAggregate : public IAggregate {
			public:
				std::unique_ptr<IIterator> CreateIterator() const override {
				};
			};

			/** Abstract. */
			class IIterator {
			protected:
				IIterator() = default;
				IIterator(const IIterator&) = default; // C.67	C.21
				IIterator& operator=(const IIterator&) = default;
				IIterator(IIterator&&) noexcept = default;
				IIterator& operator=(IIterator&&) noexcept = default;
			public:
				virtual ~IIterator() = default;

				virtual void First() = 0;
				virtual bool HasNext() = 0;
				virtual void Next() = 0;
				virtual void Current() = 0;
			};

			class ConcreteIterator : public IIterator {
			public:
				virtual void First() override {};
				virtual bool HasNext() override {};
				virtual void Next() override {
					if (HasNext()) {

					}
				};
				virtual void Current() override {};

			private:
				int index_{};
			};

		} // !namespace iterator

	} // !namespace behavioral
} // !namespace pattern

#endif // !ITERATOR_HPP
