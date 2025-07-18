#ifndef OBJECT_POOL_HPP
#define OBJECT_POOL_HPP

#include <memory>
#include <utility>
#include <concepts>
#include <functional>

#include <vector>
#include <list>


/** Software Design Patterns */
namespace pattern {
	namespace creational {

		namespace object_pool_boost {
			// https://www.boost.org/doc/libs/1_40_0/libs/pool/doc/index.html
		}

		namespace object_pool {
			// https://sourcemaking.com/design_patterns/object_pool/cpp/1
			// https://gameprogrammingpatterns.com/object-pool.html

			/** Abstract */
			class IObjectPoolResource {
			protected:
				IObjectPoolResource() = default;
				IObjectPoolResource(const IObjectPoolResource&) = delete; // C.67	C.21
				IObjectPoolResource& operator=(const IObjectPoolResource&) = delete;
				IObjectPoolResource(IObjectPoolResource&&) noexcept = delete;
				IObjectPoolResource& operator=(IObjectPoolResource&&) noexcept = delete;
			public:
				virtual ~IObjectPoolResource() = default;


				/** Resets values to default value. Needed, when returning resource_ptr to pool */
				virtual void Reset() = 0;
			};

			/** Functor callable object */
			template<typename ObjectPoolT>
			struct ObjectPoolDeleterFunctor {
				void operator()(IObjectPoolResource* used_resource) const {
					ObjectPoolT::GetInstance().ReturnResourceToPool(used_resource);
				}
			};

			/** Abstract. Hybrid version CRTP idiom. */
			template<typename ObjectPoolT>
			class IObjectPool {
			protected:
				IObjectPool() = default;
				IObjectPool(const IObjectPool&) = delete; // C.67	C.21
				IObjectPool& operator=(const IObjectPool&) = delete;
				IObjectPool(IObjectPool&&) noexcept = delete;
				IObjectPool& operator=(IObjectPool&&) noexcept = delete;
			public:
				virtual ~IObjectPool() = default;

				using DeleterT = ObjectPoolDeleterFunctor<ObjectPoolT>;
				using ResourcePtrT = std::unique_ptr<IObjectPoolResource, DeleterT>;


				/** Return resource. For Deleter function. */
				virtual void ReturnResourceToPool(IObjectPoolResource* returning_resource) = 0;

				/** Return resource from object pool */
				virtual void ReturnResourceToPool(ResourcePtrT&& returning_resource) = 0;


				/** Get resource from object pool */
				virtual ResourcePtrT GetResourceFromPool() = 0;
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


			/** Metafunction for defining T of Object Pool Container */
			template<typename ContainerT, bool kIsVectorFlag>
			struct VectorOrList {
				using T = std::vector<ContainerT>;
			};
			template<typename ContainerT>
			struct VectorOrList<ContainerT, false> {
				using T = std::list<ContainerT>;
			};


			/**
			 * Object pool template.
			 * Singleton pattern.
			 * Invariant: resources must be gotten from Object Pool and returned to pool without destructing.
			 *
			 * @param kIsVector indicates the type of resource storage container
			 */
			template<typename ObjectPoolResourceT, bool kIsVector = true>
			requires std::derived_from<ObjectPoolResourceT, IObjectPoolResource>
			class ObjectPool : public IObjectPool<ObjectPool<ObjectPoolResourceT, kIsVector>> {
			public:
				using IObjectPoolT	= IObjectPool<ObjectPool<ObjectPoolResourceT, kIsVector>>;
				/** ObjectPoolDeleterFunctor<ObjectPoolT>; */
				using DeleterT		= IObjectPoolT::DeleterT;
				/** std::unique_ptr<IObjectPoolResource, DeleterT>; */
				using ResourcePtrT	= IObjectPoolT::ResourcePtrT;

				using IObjectPoolT::ReturnResourceToPool;
				using IObjectPoolT::GetResourceFromPool;

				using PoolContainerT	= VectorOrList<ResourcePtrT, kIsVector>::T;


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
					resources_.emplace_back(used_resource, DeleterT());
				};

				void ReturnResourceToPool(ResourcePtrT&& used_resource) override {
					resources_.emplace_back(std::move(used_resource));
				};

				ResourcePtrT GetResourceFromPool() override {
					if (resources_.empty() && new_objects_limit_ > 0) { // There is opportunity to add new resource
						resources_.emplace_back(CreateNewResource());
						--new_objects_limit_;
					}

					ResourcePtrT resource_ptr{ (!resources_.empty()) ? std::move(resources_.back()) : nullptr };
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
				// Mustn't be Copy Constructed, cause singleton
				ObjectPool(const ObjectPool&) = delete;
				ObjectPool& operator=(const ObjectPool&) = delete;
				~ObjectPool() = default;


				/** Creating new resource, that can be stored in resources_ vector */
				inline ResourcePtrT CreateNewResource() const noexcept {
					return ResourcePtrT(new ObjectPoolResourceT, DeleterT());
				}


				/** To solve the curse of Fragmentation object pool can be stored in contiguous container */
				PoolContainerT resources_{};

				/**
				 * How many objects you can get from Object Pool.
				 * If this number is reached 0 and a new item is requested, an exception may be thrown, or the thread will
				 * be blocked until an object is released back into the pool.
				 * You can change limit any time.
				 */
				size_t new_objects_limit_{ 0 };

			}; // !class ObjectPool


		} // !namespace object_pool

	} // !namespace creational
} // !namespace pattern

#endif // !OBJECT_POOL_HPP
