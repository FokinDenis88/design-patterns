#ifndef IOBSERVER_HPP
#define IOBSERVER_HPP


#include <memory>		// weak_ptr
#include <string>
#include <type_traits>	// is_enum_v
#include <functional>	// function


namespace pattern {
	namespace behavioral {

		namespace iobserver {

			/*
			* ChatGPT 4.5 | DeepSeek | Midjourney | Telegram | GigaChat
			* Design Choices:					Update() function.
			* 1. No Parameters:					void update()
			* 2. State Information:				void update(int state)
			* 3. Complex State Information:		void update(const StateT& state)
			* 4. Context Information:			void update(const Context& context)		Contextual information related to the update
			* 5. Multiple Parameters:			void update(int value, const std::string& message)
			* 6. Update Returning Status:		bool update()
			* 7. Callback Function:				void update(std::function<void()> callback)
			*
			* 8. Template-based Update Function:	template <typename T> class Observer { void update(const T& data); }
			* 9. Priorities or Importance Levels:	void update(int priority, const std::string& message)
			* 10. Asynchronous:						std::future<void> update()
			* 11. Subject reference/pointer:		void update(Subject* subject)
			* 12. EventData:						void update(const EventData& event_data)
			*		The notification may contain the specific event type, the value of the new state, and other useful data.
			* 13. Generic Interface:				void update(IEvent* event)
			*	class StateChangeEvent : public IEvent {
				public:
					int new_state;
				};

				void MyObserver::update(IEvent* event) {
					if(auto change_event = dynamic_cast<StateChangeEvent*>(event)) {}
				}
			* 14. Signal: Boost library
			*
			class Subject {
			private:
				boost::signals2::signal<void()> signal_;

			public:
				void notifyObservers() {
					signal_();
				}

				boost::signals2::connection connect(boost::signals2::slot<void()> slot) {
					return signal_.connect(slot);
				}
			};
			*
			*
			*/


//=================================IObserver Interfaces============================================

			/**
			* Abstract. Stateless. Interface of Observer.
			* Alternative name - Subscriber.
			*/
			class IObserver {
			protected:
				IObserver() = default;
				IObserver(const IObserver&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserver& operator=(const IObserver&) = delete;
				IObserver(IObserver&&) noexcept = delete;
				IObserver& operator=(IObserver&&) noexcept = delete;
			public:
				virtual ~IObserver() = default;

				/** Update the information about Subject */
				virtual void Update() = 0;
			};


			/**
			* Abstract. Stateless. Interface of Observer with message in Update method.
			* Alternative name - Subscriber.
			*/
			class IObserverMsg {
			protected:
				IObserverMsg() = default;
				IObserverMsg(const IObserverMsg&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverMsg& operator=(const IObserverMsg&) = delete;
				IObserverMsg(IObserverMsg&&) noexcept = delete;
				IObserverMsg& operator=(IObserverMsg&&) noexcept = delete;
			public:
				virtual ~IObserverMsg() = default;

				/** Update the information about Subject */
				virtual void Update(const std::string& message) = 0;
			};

			/**
			* Abstract. Stateless. Interface of Observer with message in Update method.
			* Alternative name - Subscriber.
			*/
			template<typename EnumT>
			class IObserverEnum {
			public:
				static_assert(std::is_enum_v<EnumT>, "EnumT must be an enum type.");

			protected:
				IObserverEnum() = default;
				IObserverEnum(const IObserverEnum&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverEnum& operator=(const IObserverEnum&) = delete;
				IObserverEnum(IObserverEnum&&) noexcept = delete;
				IObserverEnum& operator=(IObserverEnum&&) noexcept = delete;
			public:
				virtual ~IObserverEnum() = default;

				/** Update the information about Subject */
				virtual void Update(const EnumT code) = 0;
			};

			/**
			* Abstract. Stateless. Interface of Observer with message in Update method.
			* Alternative name - Subscriber.
			*/
			template<typename StateT>
			class IObserverState {
			protected:
				IObserverState() = default;
				IObserverState(const IObserverState&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverState& operator=(const IObserverState&) = delete;
				IObserverState(IObserverState&&) noexcept = delete;
				IObserverState& operator=(IObserverState&&) noexcept = delete;
			public:
				virtual ~IObserverState() = default;

				/** Update the information about Subject */
				virtual void Update(const StateT& state) = 0;
				//virtual void Update(StateT&& state) = 0;	// universal reference
			};


			/**
			* Abstract. Stateless. Interface of Observer with message in Update method.
			* Alternative name - Subscriber.
			*/
			template<typename ContextT>
			class IObserverContext {
			protected:
				IObserverContext() = default;
				IObserverContext(const IObserverContext&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverContext& operator=(const IObserverContext&) = delete;
				IObserverContext(IObserverContext&&) noexcept = delete;
				IObserverContext& operator=(IObserverContext&&) noexcept = delete;
			public:
				virtual ~IObserverContext() = default;

				/**
				 * Update the information about Subject with additional context.
				 *
				 * @param context Contextual information.
				 */
				virtual void Update(const ContextT& context) = 0;
			};

			/**
			* Abstract. Stateless. Interface of Observer with message in Update method.
			* Alternative name - Subscriber.
			*/
			class IObserverCallback {
			public:
				using CallbackT = std::function<void()>;

			protected:
				IObserverCallback() = default;
				IObserverCallback(const IObserverCallback&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverCallback& operator=(const IObserverCallback&) = delete;
				IObserverCallback(IObserverCallback&&) noexcept = delete;
				IObserverCallback& operator=(IObserverCallback&&) noexcept = delete;
			public:
				virtual ~IObserverCallback() = default;

				/** Update the information about Subject */
				virtual void Update(const CallbackT& callback) = 0;
			};


			/**
			 * Abstract. Stateless. Interface of Observer.
			 * Alternative name - Subscriber.
			 */
			template<typename UpdateDataT>
			class IObserverGeneric {
			protected:
				IObserverGeneric() = default;
				IObserverGeneric(const IObserverGeneric&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverGeneric& operator=(const IObserverGeneric&) = delete;
				IObserverGeneric(IObserverGeneric&&) noexcept = delete;
				IObserverGeneric& operator=(IObserverGeneric&&) noexcept = delete;
			public:
				virtual ~IObserverGeneric() = default;

				/**
				 * Generic Update the information about Subject.
				 *
				 * @param data_params	Arguments forwarded to the observer.
				 */
				virtual void Update(UpdateDataT&& data) = 0;
			};

			/**
			 * Abstract. Stateless. Interface of Observer.
			 * Alternative name - Subscriber.
			 */
			template<typename... UpdateDataTypes>
			class IObserverGenericTypes {
			protected:
				IObserverGenericTypes() = default;
				IObserverGenericTypes(const IObserverGenericTypes&) = delete; // C.67	C.21 Abstract suppress Copy & Move
				IObserverGenericTypes& operator=(const IObserverGenericTypes&) = delete;
				IObserverGenericTypes(IObserverGenericTypes&&) noexcept = delete;
				IObserverGenericTypes& operator=(IObserverGenericTypes&&) noexcept = delete;
			public:
				virtual ~IObserverGenericTypes() = default;

				/** Generic Update the information about Subject */
				virtual void Update(UpdateDataTypes&&... data_params) = 0;
			};


			class IObserverFeedback {
			protected:
				IObserverFeedback() = default;
				IObserverFeedback(const IObserverFeedback&) = delete; // C.67 C.21 Abstract suppress Copy & Move
				IObserverFeedback& operator=(const IObserverFeedback&) = delete;
				IObserverFeedback(IObserverFeedback&&) noexcept = delete;
				IObserverFeedback& operator=(IObserverFeedback&&) noexcept = delete;
			public:
				virtual ~IObserverFeedback() = default;

				/**
				 * Provide feedback after the update.
				 *
				 * @return Feedback information.
				 */
				virtual std::string GetFeedback() const = 0;
			};

//=================================ISubject Interfaces============================================

			/**
			 * Abstract. Stateless. Stores observers inside.
			 * Alternative name - Publisher.
			 */
			template<typename IObserverT>
			class ISubject {
			protected:
				ISubject() = default;
				ISubject(const ISubject&) = delete; // C.67	C.21
				ISubject& operator=(const ISubject&) = delete;
				ISubject(ISubject&&) noexcept = delete;
				ISubject& operator=(ISubject&&) noexcept = delete;
			public:
				virtual ~ISubject() = default;

				// can be not abstract, but smart pointer to abstract
				//static_assert(std::is_abstract_v<IObserverT>, "IObserverT must be an abstract interface to observer.");


				/** Update all attached observers */
				virtual void NotifyObservers() const = 0;

				/** Add Observer to list of notification in Subject. */
				virtual void AttachObserver(const IObserverT& observer) = 0;
				// not const, cause must attach subject in observer ptr

				/** Detach observer from notifying list */
				virtual void DetachObserver(const IObserverT& observer) = 0;
			};


			/**
			 * Abstract. Stateless. Stores observers inside.
			 * Alternative name - Publisher.
			 *
			 * @tparam IObserverT Interface of observer. Maybe
			 */
			template<typename IObserverT>
			class ISubjectMsg {
			protected:
				ISubjectMsg() = default;
				ISubjectMsg(const ISubjectMsg&) = delete; // C.67	C.21
				ISubjectMsg& operator=(const ISubjectMsg&) = delete;
				ISubjectMsg(ISubjectMsg&&) noexcept = delete;
				ISubjectMsg& operator=(ISubjectMsg&&) noexcept = delete;
			public:
				virtual ~ISubjectMsg() = default;

				/** Update all attached observers */
				virtual void NotifyObservers(const std::string& message) const = 0;

				/** Add Observer to list of notification in Subject. */
				virtual void AttachObserver(const IObserverT& observer) = 0;

				/** Detach observer from notifying list */
				virtual void DetachObserver(const IObserverT& observer) = 0;
			};

			/**
			 * Abstract. Stateless. Subject with weak_ptr dependencies.
			 * Alternative name - Publisher.
			 * weak_ptr helps to prevent dangling pointers and lapsed listener problem.
			 */
			class ISubjectWeakMsg {
			public:
				using WeakPtrIObserverMsg = std::weak_ptr<IObserverMsg>;

			protected:
				ISubjectWeakMsg() = default;
				ISubjectWeakMsg(const ISubjectWeakMsg&) = delete; // C.67	C.21
				ISubjectWeakMsg& operator=(const ISubjectWeakMsg&) = delete;
				ISubjectWeakMsg(ISubjectWeakMsg&&) noexcept = delete;
				ISubjectWeakMsg& operator=(ISubjectWeakMsg&&) noexcept = delete;
			public:
				virtual ~ISubjectWeakMsg() = default;


				/** Update all attached observers */
				virtual void NotifyObservers(const std::string& message) const = 0;

				/**
				 * Add Observer to list of notification in Subject.
				 * There is shared_ptr<ObserverT> outside of class. Subject stores weak_ptr to that Observer.
				 */
				virtual void AttachObserver(const WeakPtrIObserverMsg observer_ptr) = 0;
				// not const, cause must attach subject in observer ptr

				/** Detach observer from notifying list */
				virtual void DetachObserver(const WeakPtrIObserverMsg observer_ptr) = 0;
			};
			/*
			* Design choices. Params in functions. Weak_ptr vs shared_ptr:
			* Best) weak_ptr in params gives guaranty, that we won't use destructed objects.
			* 2) If we use const shared_ptr& outside code must guaranty, that object will be alive, while function is working.
			* 3) If we use shared_ptr we increment strong counter of shared_ptr, increasing life cycle of object.
			*/


			/**
			 * Abstract. Stateless. Generic Subject, that can use any update method.
			 * Alternative name - Publisher.
			 */
			template<typename IObserverT, typename UpdateFnT, typename... UpdateArgsT>
			class ISubjectGeneric {
			protected:
				ISubjectGeneric() = default;
				ISubjectGeneric(const ISubjectGeneric&) = delete; // C.67	C.21
				ISubjectGeneric& operator=(const ISubjectGeneric&) = delete;
				ISubjectGeneric(ISubjectGeneric&&) noexcept = delete;
				ISubjectGeneric& operator=(ISubjectGeneric&&) noexcept = delete;
			public:
				virtual ~ISubjectGeneric() = default;


				/** Update all attached observers */
				virtual void NotifyObservers(const UpdateFnT& update_fn, const UpdateArgsT&... args) const = 0;

				/** Add Observer to list of notification in Subject. */
				virtual void AttachObserver(const IObserverT& observer) = 0;

				/** Detach observer from notifying list */
				virtual void DetachObserver(const IObserverT& observer) = 0;
			};


		} // !namespace iobserver

	} // !namespace behavioral

} // !namespace pattern

#endif // !IOBSERVER_HPP
