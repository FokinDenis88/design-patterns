﻿#ifndef CLASSIC_HEADERS_HPP
#define CLASSIC_HEADERS_HPP

// https://en.wikipedia.org/wiki/Software_design_pattern
// https://sourcemaking.com/design_patterns


// Creational
#include "creational/factory-method.hpp"
#include "creational/abstract-factory.hpp"
#include "creational/builder.hpp"
#include "creational/prototype.hpp"
#include "creational/dependency-injection.hpp"
#include "creational/object-pool.hpp"
#include "creational/singleton.hpp"
#include "creational/lazy-initialization.hpp"

// Structural
#include "structural/adapter.hpp"
#include "structural/facade.hpp"
#include "structural/decorator.hpp"
#include "structural/composite.hpp"
#include "structural/bridge.hpp"
#include "structural/proxy.hpp"
#include "structural/flyweight.hpp"
#include "structural/marker.hpp"
#include "structural/delegation.hpp"

// Behavioral
#include "behavioral/strategy.hpp"
#include "behavioral/command.hpp"

#include "behavioral/observer/generic-observer.hpp"
#include "behavioral/observer/iobserver.hpp"
#include "behavioral/observer/observer-others.hpp"
#include "behavioral/observer/weak-callback-subject.hpp"
#include "behavioral/observer/observer-weak-msg.hpp"
#include "behavioral/observer/observer-weak-multi.hpp"

#include "behavioral/state.hpp"
#include "behavioral/mediator.hpp"
#include "behavioral/chain-of-responsibility.hpp"
#include "behavioral/interpreter.hpp"
#include "behavioral/iterator.hpp"
#include "behavioral/memento.hpp"
#include "behavioral/null-object.hpp"



#endif // !CLASSIC_HEADERS_HPP
