#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

//#include <iostream>
#include <forward_list>
#include <memory>
#include <utility>
#include <concepts>
#include <functional>


/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace object_pool {
			// https://sourcemaking.com/design_patterns/object_pool/cpp/1
			// https://gameprogrammingpatterns.com/object-pool.html

			/** Abstract */
			class IObjectPoolResource {
			public:
				virtual ~IObjectPoolResource() = default;

				/** Resets values to default value. Needed, when returning resource_ptr to pool */
				virtual void Reset() = 0;
			};


			template<typename ObjectPoolType>
			//requires std::derived_from<ObjectPoolType, IObjectPool<ObjectPoolType>>
			//inline void object_pool_deleter = []<typename ObjectPoolType>(IObjectPoolResource* used_resource) {
			inline void ObjectPoolDeleter(IObjectPoolResource* used_resource) {
				//ObjectPoolType::GetInstance().ReturnResourceToPool(std::make_unique<IObjectPoolResource>(used_resource));
				ObjectPoolType::GetInstance().ReturnResourceToPool(used_resource);
			};

			/** Abstract */
			//template<typename DeleterType>
			//requires std::is_invocable_v<DeleterType>
			template<typename ObjectPoolType>
			class IObjectPool {
			protected:


			public:
				/** Return resource. For Deleter function. */
				virtual void ReturnResourceToPool(IObjectPoolResource* returning_resource) = 0;
				//virtual void DeleterFunc(IObjectPoolResource* used_resource) = 0;
				using DeleterType = decltype(&ObjectPoolDeleter<ObjectPoolType>);

				virtual ~IObjectPool() = default;

				using ResourcePtrType = std::unique_ptr<IObjectPoolResource, DeleterType>;

				/** Return resource from object pool */
				//virtual void ReturnResourceToPool(std::shared_ptr<IObjectPoolResource>&& returning_resource) = 0;
				virtual void ReturnResourceToPool(ResourcePtrType&& returning_resource) = 0;


				/** Get resource from object pool */
				//virtual std::shared_ptr<IObjectPoolResource> GetResourceFromPool() = 0;
				virtual ResourcePtrType GetResourceFromPool() = 0;
			};

			// Deleter for shared_ptr Realization
			//inline auto object_pool_deleter = [](IObjectPoolResource* used_resource) {
			//	//ObjectPool::GetInstance()->ReturnResourceToPool(std::make_unique<IObjectPoolResource>(used_resource));
			//	if (used_resource) {
			//		used_resource->Reset();
			//	}
			//};
			//using DeleterType = decltype(object_pool_deleter);


			//void(IObjectPoolResource* used_resource) del = [](IObjectPoolResource* used_resource) {};
			//void dele(IObjectPoolResource* used_resource);
			//auto dele = [](IObjectPoolResource* used_resource) {};

			class ObjectPoolResource : public IObjectPoolResource {
			public:
				void Reset() override {
					value = 0;
				};

				inline void SetValue(int new_value) noexcept {
					value = new_value;
				}

			private:
				int value{88};
			};




			// TODO: encapsulate different types of Object Pool allocation.
			// Everything allocate at beginning. Or allocate by request.


			/**
			 * Singleton pattern.
			 * Invariant: resources must be gotten from Object Pool and return to pool without destructing.
			 */
			 //class ObjectPool : public IObjectPool<decltype(object_pool_deleter<ObjectPool>)> {
			class ObjectPool : public IObjectPool<ObjectPool> {
			public:
				/** Return singleton instance */
				static ObjectPool& GetInstance() {
					static ObjectPool instance{};
					return (instance);
				}

				void ReturnResourceToPool(IObjectPoolResource* used_resource) override {
					resources_.emplace_front(used_resource, &ObjectPoolDeleter<ObjectPool>);
				};

				/*static void DeleterFunc(IObjectPoolResource* used_resource) {
					ObjectPool::GetInstance().ReturnResourceToPool(used_resource);
				};*/

			protected:


			public:
				/** Deleter for concrete object pool, returning resource to pool */
				//template<typename ObjectPoolType>
				//requires std::derived_from<ObjectPoolType, IObjectPool>

				//std::function<void(IObjectPoolResource* used_resource)> object_pool_deleter = [](IObjectPoolResource* used_resource) {

				/*void (*object_pool_deleter)(IObjectPoolResource* used_resource) = [](IObjectPoolResource* used_resource) {
					ObjectPool::GetInstance().ReturnResourceToPool(std::make_unique<IObjectPoolResource>(used_resource));
					};*/

					/*static auto object_pool_deleter = [](IObjectPoolResource* used_resource) {
						ObjectPool::GetInstance().ReturnResourceToPool(std::make_unique<IObjectPoolResource>(used_resource));
					};*/

					//static void ObjectPoolDeleter(IObjectPoolResource* used_resource) {
					//	//ObjectPool::GetInstance().ReturnResourceToPool(std::make_unique<IObjectPoolResource>(used_resource));
					//}


					//void (IObjectPoolResource* used_resource) object_pool_deleter;

					//deleter_type = object_pool_deleter<ObjectPool>;
					//using DeleterType = decltype(object_pool_deleter<ObjectPool>);





				/** Adding resource, that has been used to pool */
				/*void ReturnResourceToPool(std::shared_ptr<IObjectPoolResource>&& used_resource) override {
					if (auto resource_ptr{ used_resource.lock() }) {
						resource_ptr->Reset();
					}
					resources_.emplace_front(std::move(used_resource));
				};*/

				void ReturnResourceToPool(ResourcePtrType&& used_resource) override {
					resources_.emplace_front(std::move(used_resource));
				};

				//using DeleterType = decltype(object_pool_deleter<ObjectPool>);
				/*using DeleterType = decltype([](IObjectPoolResource* used_resource) {
					ObjectPool::GetInstance().ReturnResourceToPool(used_resource);
					});
				using ResourcePtrType = std::unique_ptr<IObjectPoolResource, DeleterType>;*/

				//std::shared_ptr<IObjectPoolResource> GetResourceFromPool() override {
					//if (!is_pool_initialized) {	// pool allocates & initializes memory
					//	resources_.resize(kPoolMaxSize);
					//	for (auto & resource_shared_ptr : resources_) {
					//		//resource_shared_ptr = std::shared_ptr<ObjectPoolResource>(new ObjectPoolResource(), object_pool_deleter);

					//		/*if (auto ptr{ resource.lock() }) {
					//			ptr = std::shared_ptr<ObjectPoolResource>(new ObjectPoolResource(), object_pool_deleter);
					//		}*/

					//		auto ptr{ resource_shared_ptr.lock() };
					//		ptr = std::shared_ptr<ObjectPoolResource>(new ObjectPoolResource(), object_pool_deleter);
					//		resource_shared_ptr = ptr;
					//	}
					//	is_pool_initialized = true;
					//}

					/*std::shared_ptr<IObjectPoolResource> resource_ptr{std::move(resources_.front())};
					resources_.pop_front();
					return resource_ptr;

				};*/
				ResourcePtrType GetResourceFromPool() override {
					//if (!is_pool_initialized) {	// pool allocates & initializes memory
					//	resources_.resize(kPoolMaxSize);
					//	for (auto & resource_shared_ptr : resources_) {
					//		//resource_shared_ptr = std::shared_ptr<ObjectPoolResource>(new ObjectPoolResource(), object_pool_deleter);

					//		/*if (auto ptr{ resource.lock() }) {
					//			ptr = std::shared_ptr<ObjectPoolResource>(new ObjectPoolResource(), object_pool_deleter);
					//		}*/

					//		auto ptr{ resource_shared_ptr.lock() };
					//		ptr = std::shared_ptr<ObjectPoolResource>(new ObjectPoolResource(), object_pool_deleter);
					//		resource_shared_ptr = ptr;
					//	}
					//	is_pool_initialized = true;
					//}

					//ResourcePtrType resource_ptr{ std::move(resources_.front()) };
					//ResourcePtrType resource_ptr{ (!resources_.empty()) ? std::move(resources_.front()) : nullptr };
					//if (!resources_.empty()) {
					//	//resource_ptr = ;
					//	resources_.pop_front();
					//}
					//else { resource_ptr = nullptr; }

					//return resource_ptr;
					ResourcePtrType resource_ptr = ResourcePtrType(new ObjectPoolResource, &ObjectPoolDeleter<ObjectPool>);
					return resource_ptr;
				};

			private:

				/**
				 * If this number is reached and a new item is requested, an exception may be thrown, or the thread will
				 * be blocked until an object is released back into the pool.
				 */
				static const size_t kPoolMaxSize{ 100 };
				inline static bool is_pool_initialized{ false };


				// stack container for object pool?
				/** To solve the curse of Fragmentation object pool can be stored in contiguous container */
				//std::forward_list<std::shared_ptr<IObjectPoolResource>> resources_{};

				std::forward_list<ResourcePtrType> resources_{};
				/*std::forward_list<std::unique_ptr<IObjectPoolResource, decltype([](IObjectPoolResource* used_resource) {
					ObjectPool::GetInstance().ReturnResourceToPool(used_resource);
					})>> lst_2{};*/

					//std::forward_list<std::unique_ptr<IObjectPoolResource, decltype(object_pool_deleter)>> resources_{};
					//std::forward_list<std::unique_ptr<IObjectPoolResource, std::function<void(IObjectPoolResource* used_resource)>>> resources_{};
					//std::forward_list<std::unique_ptr<IObjectPoolResource, decltype(&ObjectPoolDeleter)>> resources_{};
			};


			/*auto object_pool_deleter_2 = [](IObjectPoolResource* used_resource) {
				ObjectPool::GetInstance().ReturnResourceToPool(std::make_unique<IObjectPoolResource>(used_resource));
			};*/



		} // !namespace pattern
	} // !namespace creational
} // !namespace object_pool

#endif // !OBJECT_POOL_HPP
