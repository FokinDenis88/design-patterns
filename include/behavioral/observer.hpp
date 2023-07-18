#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <forward_list>
#include <memory>


/** Software Design Patterns */
namespace pattern {
	namespace behavioral {
		namespace observer {
			// TODO: Maybe it is impossible to change observer.
			// Create interface IObserver
			// Remake DetachObserver
			// Classes Observer and Observable depends on each other by state

			// REMAKE THIS SHIT!!!!


			class IObservable;

			/** Abstract. */
			class IObserver {
			public:
				virtual ~IObserver() = default;

				/** Change the subject of which state we will be notified */
				virtual void SetObservable(const std::shared_ptr<IObservable>& new_observable) = 0;

				/** Update the information about state of observable object */
				virtual void Update() = 0;
			};

			/** Abstract. */
			class IObservable {
			public:
				virtual ~IObservable() = default;

				virtual int GetUpdateInfo() const = 0;

				/** Add Observer to list of notification */
				virtual void AttachObserver(const std::shared_ptr<IObserver>& observer) = 0;

				/** Detach observer from notifying list */
				virtual void DetachObserver(const IObserver& observer) = 0;

				/** Detach all attached observers */
				//virtual inline void ClearAllObservers() = 0;

				/** Update all attached observers */
				virtual void Notify() const = 0;
			};



			class Observable;

			class Observer : public IObserver {
			public:
				/*explicit Observer(const std::shared_ptr<IObservable>& new_observable)
					: observable_{ new_observable } {

				}
				Observer(const Observer&) = delete;
				Observer& operator=(const Observer&) = delete;*/


				/** Change the subject of which state we will be notified */
				inline void SetObservable(const std::shared_ptr<IObservable>& new_observable) override {
					if (observable_.get()) {
						observable_->DetachObserver(*this);
					}
					observable_ = new_observable;
				};

				/** Update the information about state of observable object */
				inline void Update() override {
					observable_->GetUpdateInfo();
				};

			private:
				/** Object, whose state we would be notified */
				std::shared_ptr<IObservable> observable_{};
			};

			class Observable : public IObservable {
			public:
				/*explicit Observable(const std::shared_ptr<IObserver>& new_observer) :
				{
					notifying_list_.push_front(std::make_shared<IObserver>(new_observer));
				}
				Observable(const Observable&) = delete;
				Observable& operator=(const Observable&) = delete;*/

				inline int GetUpdateInfo() const override { return state_; }

				/** Add Observer to list of notification */
				inline void AttachObserver(const std::shared_ptr<IObserver>& observer) override {
					notifying_list_.push_front(observer);
					observer->SetObservable(std::make_shared<Observable>(*this));
				};

				/** Detach observer from notifying list */
				inline void DetachObserver(const IObserver& observer) override {
					notifying_list_.remove_if([&observer](const std::shared_ptr<IObserver>& current) { return current.get() == &observer; });
				};

				/** Detach all attached observers */
				inline void ClearAllObservers() {

					notifying_list_.clear();
				};

				/** Update all attached observers */
				inline void Notify() const override {
					for (const auto& observer : notifying_list_) {
						observer->Update();
					}
				};

			private:
				/** List of observers, that will be attach to observable object */
				std::forward_list<std::shared_ptr<IObserver>> notifying_list_{};

				/** The state of Observable object */
				int state_{};
			};


			void Run();


		} // !namespace observer
	} // !namespace behavioral
} // !namespace pattern

#endif // !OBSERVER_HPP
