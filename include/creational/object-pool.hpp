#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include <iostream>
#include <forward_list>
#include <memory>
#include <utility>
#include <concepts>


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

			/** Abstract */
			class IObjectPool {
			public:
				virtual ~IObjectPool() = default;

				/** Get resource from object pool */
				virtual std::shared_ptr<IObjectPoolResource> GetResourceFromPool() = 0;
				/** Return resource from object pool */
				virtual void ReturnResourceToPool(std::shared_ptr<IObjectPoolResource>&& returning_resource) = 0;
			};

			inline auto object_pool_deleter = [](IObjectPoolResource* used_resource) {
				//ObjectPool::GetInstance()->ReturnResourceToPool(std::make_unique<IObjectPoolResource>(used_resource));
				if (used_resource) {
					used_resource->Reset();
				}
			};
			using DeleterType = decltype(object_pool_deleter);


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


			/** Deleter for concrete object pool, returning resource to pool */
			/*template<typename ObjectPoolType>
			requires std::is_base_of_v<IObjectPool, ObjectPoolType>
			auto object_pool_deleter = [](IObjectPoolResource* used_resource) {
				ObjectPoolType::GetInstance()->ReturnResourceToPool(std::make_unique<IObjectPoolResource>(used_resource));
			};*/
			//void (IObjectPoolResource* used_resource) object_pool_deleter;



			/** Singleton pattern. */
			class ObjectPool : public IObjectPool {
			public:
				//deleter_type = object_pool_deleter<ObjectPool>;
				//using DeleterType = decltype(object_pool_deleter);

				/** Return singleton instance */
				static ObjectPool& GetInstance() {
					static ObjectPool instance{};
					return (instance);
				}

				std::shared_ptr<IObjectPoolResource> GetResourceFromPool() override {
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

					std::shared_ptr<IObjectPoolResource> resource_ptr{ std::move(resources_.front()) };
					resources_.pop_front();
					return resource_ptr;

				};

				/** Adding resource, that has been used to pool */
				void ReturnResourceToPool(std::shared_ptr<IObjectPoolResource>&& used_resource) override {
					/*if (auto resource_ptr{ used_resource.lock() }) {
						resource_ptr->Reset();
					}
					resources_.emplace_front(std::move(used_resource));*/
				};

			private:
				static const size_t kPoolMaxSize{ 100 };
				inline static bool is_pool_initialized{ false };


				//std::size_t max_resource_count_{10};
				// stack container for object pool?
				// To solve the curse of Fragmentation object pool can be stored in contiguous container
				std::forward_list<std::shared_ptr<IObjectPoolResource>> resources_{};
				//std::forward_list<std::unique_ptr<IObjectPoolResource>, object_pool_deleter<ObjectPool>> resources_{};
			};



			

		} // !namespace pattern
	} // !namespace creational
} // !namespace object_pool

#endif // !OBJECT_POOL_HPP
