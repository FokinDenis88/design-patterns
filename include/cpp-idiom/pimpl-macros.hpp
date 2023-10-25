#ifndef PIMPL_MACROS_HPP
#define PIMPL_MACROS_HPP

#include <memory>

/**
 * Only declare, not define.Default Destructor and Constructors for Class
 * with pImpl(Pointer to Implementation). Use in header.hpp file, in class declaration.
 */
#define DECLARE_PIMPL_CONSTRUCTORS(ClassName)	public:													\
													~ClassName();										\
													ClassName(const ClassName& obj);					\
													ClassName& operator=(const ClassName& rhs);			\
													ClassName(ClassName&& obj) noexcept;				\
													ClassName& operator=(ClassName&& rhs) noexcept;

/**
 * Declare all private data and non virtual functions of Class with pImpl idiom.
 * Use in header.hpp file, in class declaration..
 */
#define DECLARE_PIMPL_POINTER	private:												\
									struct Impl;										\
									std::unique_ptr<Impl> pImpl_;

/**
 * Declare Constructors and private part of pImpl idiom.
 * Use in header.hpp file, in class declaration..
 */
#define DECLARE_PIMPL(ClassName)	DECLARE_PIMPL_CONSTRUCTORS(ClassName)	\
									DECLARE_PIMPL_POINTER

/**
 * Define all Constructors and Default Destructor.
 * Use in source.cpp file after defining Impl structure..
 */
#define DEFINE_PIMPL(ClassName)	ClassName::~ClassName() = default;										\
								ClassName::ClassName(const ClassName& obj) : pImpl_{ nullptr } {		\
									if (obj.pImpl_) {													\
										pImpl_ = std::make_unique<ClassName::Impl>(*obj.pImpl_);		\
									}																	\
								};																		\
								ClassName& ClassName::operator=(const ClassName& rhs) {					\
									if (this != &rhs) {													\
										CopyUniquePtrOwnedObjects(rhs.pImpl_, pImpl_);					\
									}																	\
									return *this;														\
								};																		\
								ClassName::ClassName(ClassName&& obj) noexcept = default;				\
								ClassName& ClassName::operator=(ClassName&& rhs) noexcept = default;

/** Make owned object of two unique_ptr pointers equal. */
template<typename ObjectType>
inline void CopyUniquePtrOwnedObjects(const std::unique_ptr<ObjectType>& source_ptr,
										std::unique_ptr<ObjectType>& destination_ptr) {
	if (!source_ptr) {
		destination_ptr.reset();
	} else {
		if (destination_ptr) {
			*destination_ptr = *source_ptr;
		} else {
			destination_ptr = std::make_unique<ObjectType>(*source_ptr);
		}
	}
}

#endif // !PIMPL_MACROS_HPP
