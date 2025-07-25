﻿#ifndef PIMPL_MACROS_HPP
#define PIMPL_MACROS_HPP

#include <memory>



/**
 * Only declare, not define.Default Destructor and Constructors for Class
 * with pImpl(Pointer to Implementation). Use in header.hpp file, in class declaration.
 */
#define DECLARE_PIMPL_CONSTRUCTORS(ClassName)				\
	public:													\
		ClassName();										\
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
#define DEFINE_PIMPL(ClassName)												\
	ClassName::ClassName() = default;										\
	ClassName::~ClassName() = default;										\
	ClassName::ClassName(const ClassName& obj) : pImpl_{ nullptr } {		\
		if (obj.pImpl_) {													\
			pImpl_ = std::make_unique<ClassName::Impl>(*obj.pImpl_);		\
		}																	\
	};																		\
	ClassName& ClassName::operator=(const ClassName& rhs) {					\
		using pattern::cpp_idiom::pimpl::CopyUniquePtrOwnedObjects;			\
		if (this != &rhs) {													\
			CopyUniquePtrOwnedObjects(rhs.pImpl_, pImpl_);					\
		}																	\
		return *this;														\
	};																		\
	ClassName::ClassName(ClassName&& obj) noexcept = default;				\
	ClassName& ClassName::operator=(ClassName&& rhs) noexcept = default;


 /** C++ Idioms */
namespace pattern {
	namespace cpp_idiom {
		namespace pimpl {


			/** Make owned object of two unique_ptr pointers equal. */
			template<typename ObjectT>
			inline void CopyUniquePtrOwnedObjects(const std::unique_ptr<ObjectT>& source_ptr,
				std::unique_ptr<ObjectT>& destination_ptr) {
				if (!source_ptr) { // source_ptr is nullptr
					destination_ptr.reset();
				}
				else { // source_ptr is not nullptr
					if (destination_ptr) {
						*destination_ptr = *source_ptr; // Copy assignment of stored object
					}
					else { // memory is not allocated for destination_ptr. No stored object
						destination_ptr = std::make_unique<ObjectT>(*source_ptr);
					}
				}
			}


		} // !namespace pimpl
    } // !namespace cpp_idiom
} // !namespace pattern

#endif // !PIMPL_MACROS_HPP
