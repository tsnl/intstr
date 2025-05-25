# `tsnl/intstr`

Interned strings for C++.

A `tsnl::intstr` is an integer that is associated with a string.
It enables O(1) string comparisons and hashing at the cost of O(N) creation and some book-keeping.

The implementation is simple, thread-safe, and is contained in [a single header](/include/tsnl/intstr.hpp).

```cpp
#include <cassert>

#include <tsnl/intstr.hpp>

using namespace tsnl::literals;
assert("Hello"_is == "Hello"_is);
assert("Hello"_is != "World"_is);

static_assert(sizeof("Hello"_is) == 4);
assert(uint32_t("Hello"_is) == "Hello"_is.id());
assert(std::string_view(intstr("Hello"_is.id())) == std::string_view("Hello"));

assert(uint32_t(""_is) == 0);
```

---

## FAQ

<details>
<summary>What is string interning?</summary>
<p>
String interning de-duplicates strings in memory. It is useful when dealing with strings that you expect to recur often.
The price for de-duplication is typically an O(N) hashing operation on the string to convert it to some stable, 
string-specific ID.
</p>

<p>
A common use-case for interned strings is to represent syntactic identifiers (variable names) in compilers and 
interpreters. Each variable name is stored as a single interned string. Symbol lookups in symbol tables, field name 
queries on records/structs, and import resolution all rely on interned strings to work more efficiently.
</p>

<p>
Another use-case for interned strings is as a concise identifier for objects in game-engines. Names can be interned to 
turn them into integers. Subsequent queries using these names can supply the interned string, reducing the cost of 
hashing and comparing the strings.
</p>
</details>

<details>
<summary>How does this implementation work?</summary>

<p>
This implementation uses a hash-map to associate every interned string with a unique ID. When an <code>intstr</code> 
instance is constructed, we lock the hash-map and check if the string has been interned already. If so, we return the 
previously returned ID. Otherwise, we mint a new ID, update the global hash-map, and return the new ID.
</p>

<p>
This means that each <code>intstr</code> points to exactly one string. It also allows you to recover the original string
representation from an ID by performing a reverse-lookup on the aforementioned map using a supplementary lookup table.
</p>

<p>
The global hash-map is synchronized by a mutex, making it thread-safe. We acquire shared locks to the global mutex when 
checking if a string is already interned, only taking exclusive locks when we need to update the global state with an 
unencountered string. Since string interning is employed when the strings are expected to occur repeatedly, we expect to
rarely take the global exclusive lock.
</p>

<p>
Finally, we expose the underlying integer handle and ABI as part of the API. We guarantee the integer will increment 
monotonically, that the <code>0</code> ID will always refer to an empty string, and that the specified integer type 
(<code>uint32_t</code> by default) is all each <code>intstr</code> instance will ever contain. This makes our class 
suitable for use in tagged-object interpreters where the integer must be packed and unpacked reliably.
</p>

<p>
The implementation is simple, thread-safe, and is contained in <a href="/include/tsnl/intstr.hpp">a single header</a>. 
It is easy to modify and patch, e.g. changing to 16-bit or 64-bit integers.
<p>

<p>
Happy hacking!
</p>
</details>

