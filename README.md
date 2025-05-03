# `tsnl/intstr`

Interned strings for C++.

```cpp
#include <cassert>

#include <tsnl/intstr.hpp>

assert(intstr("Hello") == intstr("Hello"));
assert(intstr("Hello") != intstr("World"));

assert(uint32_t(intstr("Hello")) == uint32_t(intstr("Hello")));
assert(uint32_t(intstr("Hello")) != uint32_t(intstr("World")));

using namespace std::literals;
assert(std::string(intstr("Hello")) == "Hello"s);
```
