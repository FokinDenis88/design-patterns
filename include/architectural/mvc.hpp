#ifndef MVC_HPP
#define MVC_HPP

#include <memory>
#include <utility>
//#include <iostream>


#include "behavioral/observer.hpp"
#include "behavioral/strategy.hpp"
#include "structural/composite.hpp"

//#include "creational/factory-method.hpp"
//#include "structural/decorator.hpp"


/** Software Design Patterns */
namespace pattern {
	namespace architectural {
		namespace mvc {
			// https://en.wikipedia.org/wiki/Model%E2%80%93view%E2%80%93controller

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
			 */


			using pattern::behavioral::observer_ref::ObserverRef;
			using pattern::behavioral::observer_ref::SubjectRef;

			using pattern::structural::composite::NonTerminalComponent;
			using pattern::structural::composite::AbstractComponent;

			using pattern::behavioral::strategy::IStrategy;

			/**
			 * It directly manages the data, logic and rules of the application.
			 * The model is responsible for managing the data of the application. It receives user input from the controller.
			 */
			class Model {
			public:
			};


			/**
			 * Any representation of information such as a chart, diagram or table.
			 * The view renders presentation of the model in a particular format.
			 */
			class MainView : public NonTerminalComponent {
			public:
			};

			class SubView_1 : public AbstractComponent {
			public:
				void Operation() override {
				};
			};

			class SubView_2 : public AbstractComponent {
			public:
				void Operation() override {
				};
			};


			/**
			 * Accepts input and converts it to commands for the model or view.
			 * At any given time, each controller has one associated view and model, although one model object may
			 * hear from many different controllers.
			 *
			 * The controller responds to the user input and performs interactions on the data model objects.
			 * The controller receives the input, optionally validates it and then passes the input to the model.
			 */
			class Controller : public IStrategy {
			public:
				void Algorithm() override {
					//std::cout << "Hello World";
				}
			};

			// Controller_1
			// Controller_2


			class User {
			public:
				void Request() {
				}
			};

		} // !namespace mvc

	} // !namespace architectural
} // !namespace pattern

#endif // !MVC_HPP
