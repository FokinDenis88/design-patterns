﻿#ifndef FLYWEIGHT_HPP
#define FLYWEIGHT_HPP

#include <iostream>
#include <unordered_map>
//#include <algorithm>

/** Software Design Patterns */
namespace pattern {
	namespace structural {
		namespace flyweight {
			// http://cpp-reference.ru/patterns/structural-patterns/flyweight/

			template<typename extrinsicStateT>
			class IFlyweight {
			protected:
				IFlyweight() = default;
				IFlyweight(const IFlyweight&) = delete; // C.67	C.21
				IFlyweight& operator=(const IFlyweight&) = delete;
				IFlyweight(IFlyweight&&) noexcept = delete;
				IFlyweight& operator=(IFlyweight&&) noexcept = delete;
			public:
				virtual ~IFlyweight() = default;

				virtual void operation(extrinsicStateT extrinsicState) = 0;

				;
			};

			class FlyweightShared : public IFlyweight<int> {
			public:
				void operation(int extrinsicState) override {

				};

			private:
				int intrinsicState_;
			};


			/** Flyweight must not always has shared part. Is used sometimes as a base for FlyweightShared */
			class FlyweightUnShared {

			};

			/** Flyweight Cache Fabric class */
			template<typename CacheKeyT, typename CachedFlyweightT, typename extrinsicStateT>
			requires std::is_base_of_v<IFlyweight<extrinsicStateT>, CachedFlyweightT>
			class FlyweightFabric {
			public:
				// Output can be weak or shared. Depends on who owns cache.
				std::shared_ptr<CachedFlyweightT> GetFlyweight(CacheKeyT cache_key) {
					if (cache_.contains(cache_key) && !cache_[cache_key].expired()) { // item has been created
						return cache_[cache_key].lock();
					} else {
						// Deleter, that will clean cache
						auto deleter = [this, &cache_key](CachedFlyweightT* cached_flyweight_ptr) {
							this->cache_.erase(cache_key);
							delete cached_flyweight_ptr;
						};

						//std::shared_ptr<CachedFlyweightT> ptr{ std::make_shared<CachedFlyweightT>() };
						std::shared_ptr<CachedFlyweightT> ptr(new CachedFlyweightT(), deleter);
						cache_[cache_key] = ptr;
						return ptr;
					}
				};

			private:
				// Can be unoredered_set
				// Can include option of removing unused cache. CachedFlyweightT = weak_ptr<Object> ???
				// Without option of removing unused cache. CachedFlyweightT = unique_ptr<Object>
				// Can be weak or shared
				std::unordered_map<CacheKeyT, std::weak_ptr<CachedFlyweightT>> cache_{};
			};


			// Deleter for cleaning cache
			//auto deleter = [this](CachedFlyweightT* cached_flyweight_ptr) {
			//	auto search_ptr = std::find_if(this->cache_.begin(), this->cache_.end(),
			//		[cached_flyweight_ptr](auto& elem) {
			//			// weak_ptr is expired on the moment of deleter call
			//			if (elem.second.lock().get() == cached_flyweight_ptr) {
			//				return true;
			//			}
			//			else {
			//				return false;
			//			}
			//		});
			//	if (search_ptr != this->cache_.end()) { // clear cache
			//		cache_.erase(search_ptr);
			//	}
			//	delete cached_flyweight_ptr;
			//};

		} // !namespace flyweight

	} // !namespace structural
} // !namespace pattern

#endif // !FLYWEIGHT_HPP
