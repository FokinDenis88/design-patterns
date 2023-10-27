#include "cpp-idiom/pimpl.hpp"

namespace pattern {
	namespace cpp_idiom {

		namespace pimpl_1 {
			struct Widget::Impl {
				// Widget private data and functions

			};

			Widget::Widget() = default;
			Widget::~Widget() = default;
			Widget::Widget(const Widget& obj) : pImpl_{ nullptr } {
				if (obj.pImpl_) {
					pImpl_ = std::make_unique<Widget::Impl>(*obj.pImpl_);
				}
			};
			Widget& Widget::operator=(const Widget& rhs) {
				if (this != &rhs) {
					//CopyUniquePtrOwnedObjects(rhs.pImpl_, pImpl_);
					if (!rhs.pImpl_) {	//
						pImpl_.reset();
					} else if (!pImpl_) {
						pImpl_ = std::make_unique<Impl>(*rhs.pImpl_);
					} else {
						*pImpl_ = *rhs.pImpl_;
					}
				}
				return *this;
			};
			Widget::Widget(Widget&& obj) noexcept = default;
			Widget& Widget::operator=(Widget&& rhs) noexcept = default;



		} // !namespace pimpl_1

		namespace pimpl_2 {
			struct Widget::Impl {
				// Widget private data and functions

			};

			DEFINE_PIMPL(Widget)

		} // !namespace pimpl_2

	} // !namespace cpp_idiom
} // !namespace pattern
