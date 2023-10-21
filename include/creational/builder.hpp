#ifndef BUILDER_HPP
#define BUILDER_HPP

#include <memory>
#include <utility>

/** Software Design Patterns */
namespace pattern {
	namespace creational {
		namespace builder {
			struct Engine {
				int value1{};
				int value2{};
			};
			struct Wheel {
				int value1{};
				int value2{};
				int value3{};
			};
			struct Door {
				int value1{};
				int value2{};
				int value3{};
			};
			struct Bumper {
				int value1{};
			};

			class Car {
			public:
				Car() = default;
				explicit Car(Engine engine, Wheel wheel, Door door, Bumper bumper) noexcept
					:	engine_{ engine }, wheel_{ wheel },
						door_{ door }, bumper_{ bumper } {
				};

				Engine engine_{};
				Wheel wheel_{};
				Door door_{};
				Bumper bumper_{};
			};


			/** Abstract */
			class CarBuilderAbstract {
			public:
				virtual ~CarBuilderAbstract() = default;

				// Functions can be empty in Derived classes
				virtual void BuildEngine() {};
				virtual void BuildWheel() {};
				virtual void BuildDoor() {};
				virtual void BuildBumper() {};

				virtual std::unique_ptr<Car> GetResult() = 0;
			};

			/** Not all functions will be overriden */
			class BrokenCarBuilder : public CarBuilderAbstract {
			public:
				void BuildWheel() override {
					broken_car_ptr_ = std::make_unique<Car>();
					broken_car_ptr_->wheel_.value1 = 66;
					broken_car_ptr_->wheel_.value2 = 66;
					broken_car_ptr_->wheel_.value3 = 66;
				};
				std::unique_ptr<Car> GetResult() noexcept override {
					return std::move(broken_car_ptr_);
				};
			private:
				std::unique_ptr<Car> broken_car_ptr_{};
			};

			class ToyotaCarBuilder : public CarBuilderAbstract {
			public:
			};

			class CarDirector {
			public:
				CarDirector() = delete;
				CarDirector(const CarDirector&) = delete;
				CarDirector& operator=(const CarDirector&) = delete;

				explicit CarDirector(std::unique_ptr<CarBuilderAbstract>&& car_builder_p) noexcept
					: car_builder_{ std::move(car_builder_p) } {
				};

				std::unique_ptr<Car> ConstructCar(std::unique_ptr<CarBuilderAbstract>&& new_car_builder = nullptr) {
					if (new_car_builder) {
						car_builder_ = std::move(new_car_builder);
					}
					if (car_builder_) {
						car_builder_->BuildEngine();
						car_builder_->BuildWheel();
						car_builder_->BuildDoor();
						car_builder_->BuildBumper();
						return car_builder_->GetResult();
					} else {
						return nullptr;
					}
				};

				/*inline const std::shared_ptr<CarBuilderAbstract> car_builder() const noexcept { return car_builder_; };
				inline void SetBuilder(std::shared_ptr<CarBuilderAbstract> builder_p) noexcept {
					car_builder_ = std::move(builder_p);
				};*/

			private:
				std::unique_ptr<CarBuilderAbstract> car_builder_{};
			};

			/** Is using Director */
			class Client {};

			inline void Run() {
			};
		}
	}
}

#endif // !BUILDER_HPP
