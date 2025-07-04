#ifndef GAMEDEV_HEADERS_HPP
#define GAMEDEV_HEADERS_HPP


// Book: Robert Nistrem. Game Programming Patterns
// https://gameprogrammingpatterns.com/

/*
Creational
1. Prototype
2. Singleton
3. ObjectPool
4. Lazy initialization

Structural
1. Flyweight

Behavioral
1. Command
2. State
3. Strategy
4. Observer


Sequencing Patterns
8. Double Buffer
9. Game Loop
10. Update Method

Behavioral Patterns
11. Bytecode
12. Subclass Sandbox
13. T Object

Decoupling Patterns
14. Component
15. Event Queue
16. Service Locator

Optimization Patterns
17. Data Locality
18. Dirty Flag
19. Object Pool
20. Spatial Partition
*/



// Sequencing Patterns
#include "gamedev/double-buffer.hpp"
#include "gamedev/game-loop.hpp"
#include "gamedev/update-method.hpp"

// Behavioral Patterns
#include "gamedev/bytecode.hpp"
#include "gamedev/subclass-sandbox.hpp"
#include "gamedev/type-object.hpp"

// Decoupling Patterns
#include "gamedev/component.hpp"
#include "gamedev/event-queue.hpp"
#include "gamedev/service-locator.hpp"

// Optimization Patterns
#include "gamedev/data-locality.hpp"
#include "gamedev/dirty-flag.hpp"
#include "gamedev/spatial-partition.hpp"


#endif // !GAMEDEV_HEADERS_HPP
