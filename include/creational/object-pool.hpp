#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include <memory>
#include <utility>
#include <concepts>
#include <functional>

#include <stack>
#include <forward_list>
#include <vector>
#include <list>


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
			struct ObjectPoolDeleterFunctor {
				void operator()(IObjectPoolResource* used_resource) const {
					ObjectPoolType::GetInstance().ReturnResourceToPool(used_resource);
				}
			};

			/** Abstract */
			template<typename ObjectPoolType>
			class IObjectPool {
			public:
				using DeleterType = ObjectPoolDeleterFunctor<ObjectPoolType>;
				using ResourcePtrType = std::unique_ptr<IObjectPoolResource, DeleterType>;

				virtual ~IObjectPool() = default;

				/** Return resource. For Deleter function. */
				virtual void ReturnResourceToPool(IObjectPoolResource* returning_resource) = 0;

				/** Return resource from object pool */
				virtual void ReturnResourceToPool(ResourcePtrType&& returning_resource) = 0;


				/** Get resource from object pool */
				virtual ResourcePtrType GetResourceFromPool() = 0;
			};


			/** Example class */
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


			/** Metafunction for defining Type of Object Pool Container */
			template<typename ContainerType, bool kIsVectorFlag>
			struct VectorOrList {
				using Type = std::vector<ContainerType>;
			};
			template<typename ContainerType>
			struct VectorOrList<ContainerType, false> {
				using Type = std::list<ContainerType>;
			};


			/**
			 * Object pool template.
			 * Singleton pattern.
			 * Invariant: resources must be gotten from Object Pool and returned to pool without destructing.
			 *
			 * @param kIsVector indicates the type of resource storage container
			 */
			template<typename ObjectPoolResourceType, bool kIsVector = true>
			requires std::derived_from<ObjectPoolResourceType, IObjectPoolResource>
			class ObjectPool : public IObjectPool<ObjectPool<ObjectPoolResourceType, kIsVector>> {
			public:
				using IObjectPoolType = IObjectPool<ObjectPool<ObjectPoolResourceType, kIsVector>>;
				/** ObjectPoolDeleterFunctor<ObjectPoolType>; */
				using DeleterType = IObjectPoolType::DeleterType;
				/** std::unique_ptr<IObjectPoolResource, DeleterType>; */
				using ResourcePtrType = IObjectPoolType::ResourcePtrType;

				using IObjectPoolType::ReturnResourceToPool;
				using IObjectPoolType::GetResourceFromPool;

				using PoolContainerType = VectorOrList<ResourcePtrType, kIsVector>::Type;


				/**
				 * Return singleton instance.
				 *
				 * @param initial_new_objects_limit first initial count of resources, that can be allocated.
				 */
				static ObjectPool& GetInstance(const size_t initial_new_objects_limit = 0) {
					static ObjectPool instance{ ObjectPool(initial_new_objects_limit) };
					return (instance);
				}

				void ReturnResourceToPool(IObjectPoolResource* used_resource) override {
					resources_.emplace_back(used_resource, DeleterType());
				};

				void ReturnResourceToPool(ResourcePtrType&& used_resource) override {
					resources_.emplace_back(std::move(used_resource));
				};

				ResourcePtrType GetResourceFromPool() override {
					if (resources_.empty() && new_objects_limit_ > 0) { // There is opportunity to add new resource
						resources_.emplace_back(CreateNewResource());
						--new_objects_limit_;
					}

					ResourcePtrType resource_ptr{ (!resources_.empty()) ? std::move(resources_.back()) : nullptr };
					if (!resources_.empty()) {
						resources_.pop_back();
					}

					return resource_ptr;
				};


				/**
				 * Max count of all available resources.
				 * MaxAvailable = resources_.size + new_objects_limit_.
				 */
				inline size_t SizeMaxAvailableResources() const noexcept {
					return resources_.size() + new_objects_limit_;
				}

				/** Size of resources_ vector */
				inline size_t SizeAllocatedResources() const noexcept {
					return resources_.size();
				}

				/**
				 * Allocate memory for all available objects. Available = resources_.size + new_objects_limit_..
				 * If the resources storage type is list, then doing nothing.
				 */
				inline void ReserveMaxAvailable() {
					if constexpr (kIsVector) {
						resources_.reserve(SizeMaxAvailableResources());
					}
				}

				/**
				 * Allocate and initialize memory for all available objects.
				 * Count of allocation = resources.size + objects_limit_.
				 * new_objects_limit_ will be zero.
				 */
				inline void ResizeMaxAvailable() {
					resources_.resize(SizeMaxAvailableResources());
					new_objects_limit_ = 0;
				}


				inline void set_new_objects_limit(const size_t new_objects_limit_p) noexcept {
					new_objects_limit_ = new_objects_limit_p;
				};

				inline size_t new_objects_limit() const noexcept { return new_objects_limit_; };

			private:
				// Singleton Constructors
				explicit ObjectPool(const size_t new_objects_limit_p) noexcept
						:	new_objects_limit_{ new_objects_limit_p } {
				}
				ObjectPool(const ObjectPool&) = delete;
				ObjectPool& operator=(const ObjectPool&) = delete;


				/** Creating new resource, that can be stored in resources_ vector */
				inline ResourcePtrType CreateNewResource() const noexcept {
					return ResourcePtrType(new ObjectPoolResourceType, DeleterType());
				}


				/** To solve the curse of Fragmentation object pool can be stored in contiguous container */
				PoolContainerType resources_{};

				/**
				 * How many objects you can get from Object Pool.
				 * If this number is reached 0 and a new item is requested, an exception may be thrown, or the thread will
				 * be blocked until an object is released back into the pool.
				 * You can change limit any time.
				 */
				size_t new_objects_limit_{ 0 };
			};

		} // !namespace object_pool

	} // !namespace creational
} // !namespace pattern

#endif // !OBJECT_POOL_HPP
