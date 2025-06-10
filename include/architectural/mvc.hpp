#ifndef MVC_HPP
#define MVC_HPP

#include <memory>
#include <utility>


#include "behavioral/observer.hpp"
#include "behavioral/strategy.hpp"
#include "ieditor.hpp"
#include "structural/composite.hpp"


//#include "creational/factory-method.hpp"
//#include "structural/decorator.hpp"


/** Software Design Patterns */
namespace pattern {
	namespace architectural {
		namespace mvc {
			/**
			 * https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller
			 * https://developer.mozilla.org/en-US/docs/Glossary/MVC
			 */


			/**
             * the model, the internal representations of information
             * the view, the interface that presents information to and accepts it from the user. Can send input from user to controller.
             * the controller, the software linking the two. May send data from model to view.
			 */

			/**
			 * View and Controller can be integrated in one class.
			 * View depends on Model. Model doesn't depend on View.
			 * Segregation of Model and View is important.
			 * Segregation of Controller and View less important. (More important for Web)
			 */

			/**
			 * Friend Patterns. Main: Observer, Composite, Strategy.
			 * Model: Observer(Model-to-View)(Main)
			 *
			 * View: Composite(for nested views), Observer(Model-to-View),Strategy(Controller-View),
			 *		 Factory Method(for defining default Controller), Decorator(for scrolling)
			 *
			 * Controller: Strategy(Controller-View)(Main)
			 *
             * Patterns maybe used: Factory Method.
             * Controller may use factory method for creating instance of model and view. For decoupling.
             * Strategy can be used for controller. And for data processing between model and view.
             * Observer for support, decoupling and for notifying of state changes.
             * Composite for subviews in main view. Data hierarchy in model in view.
             * Decorator for dynamically adding of functionality for model, controller and view.
             * Singleton for sessions with users or databases.
             * Template Method for main actions in controller.
             * Chain or responsibility for complex requests.
			 */

			/**
			 * Interactions.
             * The model is responsible for managing the data of the application. It receives user input from the controller.
             *
             * The view renders presentation of the model in a particular format.
             *
             * The controller responds to the user input and performs interactions on the data model objects.
             * The controller receives the input, optionally validates it and then passes the input to the model.
             * In classic mvc model is not sending data to view directly, but only through controller.
             * So there is no communication between model and view directly.
			 */

			/**
			 * Most important part is to separate Model and View. Model must be independently changeable from View.
			 * Separation Control from View is not such important, as Controller can be integrated in View.
			 * Separating Controller and View will give much more freedom and flexibility.
			 */

			/**
             * The Model layer absolutely should contain functions for editing (creating, reading, updating, and deleting - CRUD)
             * data, as it is responsible for handling the application's data and business logic.
             * Separating data management logic from the view and controller makes the application more maintainable and easier to extend.
			 */

			/**
			 * Where is used Observer in MVC?
			 * Model
			 * In non classic mvc in Mainly Must be in View (Observer) - Model (Subject) to notify about model state changes.
			 * Maybe in Controller (Observer) - Model (Subject).
			 * View
			 * Rare maybe in View (Observer) - Controller (Subject).
			 * Controller
			 * Controller (Observer) - View (Subject).
			 *
			 * Most popular: View (Observer) - Model (Subject)
			 */

			/** In standard realization model and view are not stored in controller. */


			using general::IEditor;
			using pattern::behavioral::observer_ref::ObserverRefSetMulti;
			using pattern::behavioral::observer_ref::SubjectRefSetMulti;

			using pattern::structural::composite::NonTerminalComponent;
			using pattern::structural::composite::TerminalComponent;

			using pattern::behavioral::strategy::IStrategy;

			/**
			 * Manages data and business logic.
			 * It is the application's dynamic data structure, independent of the user interface.
			 * It directly manages the data, logic and rules of the application.
			 * The model is responsible for managing the data of the application.
			 * It receives user input from the controller.
			 * If the state of this data changes, then the model will usually notify the view and, maybe, controller.
			 * Must contain functions for editing data, for handling the application's data and business logic.
			 * Editing (creating, reading, updating, and deleting - CRUD) data.
			 * Business logic: validation, data transformation, and calculations.
			 * One model object may hear from many different controllers.
			 */
			class Model : public IEditor, public SubjectRefSetMulti {
			public:
				Model() = default;
            protected:
                Model(const Model&) = delete; // C.67	C.21
				Model& operator=(const Model&) = delete;
				Model(Model&&) noexcept = delete;
				Model& operator=(Model&&) noexcept = delete;
            public:
                ~Model() override = default;

                /** Adding new data. */
				void Create() override {};

                /** Retrieving existing data. */
				void Read() override {};

                /** Modifying data records. */
				void Update() override {};

                /** Removing data records. */
				void Delete() override {};
			};


			/**
			 * Routes commands to the model and view parts
			 * Accepts input and converts it to commands for the model or view.
			 * At any given time, each controller has one associated view and model, although one model object may
			 * hear from many different controllers.
			 *
			 * The controller responds to the user input and performs interactions on the data model objects.
			 * The controller receives the input, optionally validates it and then passes the input to the model.
			 * May communicate with View to change some windows.
			 * Only one controller, the "active" controller, receives user input at any given time
			 */
			class Controller : public IStrategy, public ObserverRefSetMulti, public SubjectRefSetMulti {
            public:
                explicit Controller(Model& model) noexcept
                    : ObserverRefSetMulti{ model } {
                };
            protected:
                Controller(const Controller&) = delete; // C.67	C.21
                Controller& operator=(const Controller&) = delete;
                Controller(Controller&&) noexcept = delete;
                Controller& operator=(Controller&&) noexcept = delete;
            public:
                ~Controller() override = default;


				void Algorithm() override {
					//std::cout << "Hello World";
				};
			};

            class Controller_2 : public IStrategy {
            public:
				void Algorithm() override {
					//std::cout << "Hello World";
				};
            };


			/**
			 * Handles layout and display
			 * Any representation of information such as a chart, diagram or table.
			 * The view renders presentation of the model in a particular format.
			 * Multiple views of the same information are possible.
			 */
            class View : public NonTerminalComponent, public ObserverRefSetMulti {
            public:
                explicit View(Controller& controller) noexcept
                    : ObserverRefSetMulti{ dynamic_cast<ObserverRefSetMulti::ISubjectType&>(controller) } {
                };
            protected:
                View(const View&) = delete; // C.67	C.21
                View& operator=(const View&) = delete;
                View(View&&) noexcept = delete;
                View& operator=(View&&) noexcept = delete;
            public:
                ~View() override = default;

            };

            class SubView_1 : public TerminalComponent {
            public:
                void Operation() override {
                };
            };

            class SubView_2 : public TerminalComponent {
            public:
                void Operation() override {
                };
            };


			class User {
			public:
				void Request() {
					Model my_model;
					Controller my_controller{ my_model };
					View my_view{ my_controller };
				};
			};

		} // !namespace mvc

	} // !namespace architectural
} // !namespace pattern

#endif // !MVC_HPP
