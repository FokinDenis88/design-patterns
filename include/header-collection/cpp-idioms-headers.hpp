#ifndef CPP_IDIOMS_HEADERS_HPP
#define CPP_IDIOMS_HEADERS_HPP

//https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms

/*
1. Acyclic Visitor Pattern TODO
2. Address Of
3. Algebraic Hierarchy
4. Attach by Initialization
5. Attorney - Client
6. Barton - Nackman trick
7. Base - from - Member
8. Boost mutant
9. Calling Virtuals During Initialization
10. Capability Query
11. Checked delete
12. Clear - and -minimize
13. Coercion by Member Template
14. Computational Constructor
15. Concrete Data T
16. Construct On First Use
17. Construction Tracker
18. Copy - and -swap
19. Copy - on - write
20. Thread - safe Copy - on - write
21. Intrusive reference counting(Counted Body)
22. Covariant Return Ts TODO
23. Curiously Recurring Template Pattern(CRTP)
24. Deprecate and Delete TODO
25. Empty Base Optimization(EBO)
26. enable - if
27. Erase - Remove
28. Execute - Around Pointer
29. Exploding Return T TODO
30. Export Guard Macro TODO
31. Expression - template
32. Fake Vtable TODO
33. Fast Pimpl  TODO
34. Final Class
35. Free Function Allocators
36. Function Object TODO
37. Function Poisoning TODO
38. Generic Container Idioms
39. Hierarchy Generation TODO
40. Implicit conversions TODO
41. Include Guard Macro
42. Inline Guard Macro
43. Inner Class
44. Int - To - T
45. Interface Class
46. Iterator Pair
47. Making New Friends
48. Metafunction
49. Move Constructor
50. Multi - statement Macro
51. Member Detector
52. Named Constructor
53. Named External Argument TODO
54. Named Loop(labeled loop)
55. Named Parameter
56. Named Template Parameters TODO
57. Nifty Counter(Schwarz Counter)
58. Non - copyable Mixin
59. Non - member Non - friend Function  TODO
60. Non - throwing swap
61. Non - Virtual Interface(NVI, Public Overloaded Non - Virtuals Call Protected Non - Overloaded Virtuals)
62. nullptr
63. Object Generator
64. Object Template TODO
65. Parameterized Base Class(Parameterized Inheritance)
66. Pimpl(Handle Body, Compilation Firewall, Cheshire Cat)
67. Policy Clone(Metafunction wrapper)
68. Policy - based Design TODO
69. Polymorphic Exception
70. Polymorphic Value Ts  TODO
71. Recursive T Composition TODO
72. Requiring or Prohibiting Heap - based Objects
73. Resource Acquisition Is Initialization(RAII, Execute - Around Object, Scoped Locking)
74. Resource Return
75. Return T Resolver
76. Runtime Static Initialization Order Idioms
77. Safe bool
78. Scope Guard
79. Substitution Failure Is Not An Error(SFINAE)
80. Shortening Long Template Names TODO
81. Shrink - to - fit
82. Small Object Optimization TODO
83. Smart Pointer
84. Storage Class Tracker TODO
85. Tag Dispatching  TODO
86. Temporary Base Class
87. Temporary Proxy
88. The result_of technique TODO
89. Thin Template
90. Thread - Safe Interface TODO
91. Traits  TODO
92. T Erasure
93. T Generator(Templated Tdef)
94. T Safe Enum
95. T Selection
96. Virtual Constructor
97. Virtual Friend Function
*/


// Cpp-Idioms
#include "cpp-idiom/attorney-client.hpp"
#include "cpp-idiom/copy-n-swap.hpp"
#include "cpp-idiom/crtp.hpp"
#include "cpp-idiom/d-pointer.hpp"
#include "cpp-idiom/named-parameter.hpp"
#include "cpp-idiom/non-virtual-interface.hpp"
#include "cpp-idiom/opaque-pointer.hpp"
#include "cpp-idiom/pimpl.hpp"
#include "cpp-idiom/pimpl-macros.hpp"
#include "cpp-idiom/sfinae.hpp"

#endif // !CPP_IDIOMS_HEADERS_HPP
