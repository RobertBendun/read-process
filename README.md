# read-process
Goal of this library is to provide cross platform definitions of reading process stdout using std::istream and FILE* conventions and abstractions.

## C
Coming soon as cross platform [``popen``](https://linux.die.net/man/3/popen).

## C++
Library provides only read stream **``ipstream``** derived from ``std::istream``.
Current implementation is POSIX & libstdc++ only.

The simplest example:
```c++
#include <iostream>
#include <string>

#include "ipstream.hpp"

int main()
{
  ipstream stream("ls -la");
  for (std::string line; std::getline(stream, line); ++i)
      std::cout << line << '\n';
}
```
