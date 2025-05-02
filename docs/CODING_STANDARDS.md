
# Catalanitzador Coding Standards

## Naming

- **Classes, interfaces, enums, and structures**: Use **UpperCamelCase**
  **Example**:
  ```cpp
  class SystemRestore;
  ```

- **Variables**: Use **lowerCamelCase**
  **Example**:
  ```cpp
  int systemRestore;
  ```

### Prefixes

- `m_` for member variables
  Example: `m_processes`

- `g_` for global variables
  Example: `g_currentObject`

- **Methods**:
  - Public methods: **UpperCamelCase**
  - Private/protected methods: Prefix with `_` and use **lowerCamelCase**

  **Example**:
  ```cpp
  public:
      void FinishExecution(ExecutionProcess process);

  protected:
      void _enumVersions(vector<wstring>& versions);
  ```

- **Constants**: Use **UPPER_CASE**

  **Example**:
  ```cpp
  #define APPLICATON_WEBSITE L"http://catalanitzador.softcatala.org"
  ```

---

## Declaring and Initializing

- Declare constants and macros at the beginning of `.cpp` files.
- Always initialize arrays, especially `char` and `wchar_t` arrays, to `0`.
  - Note: In debug mode, the compiler initializes them automatically, but **not** in release mode.

  **Example**:
  ```cpp
  char szTmpA[1024] = {0};
  wchar_t szTmpB[1024] = {0};
  ```

---

## Namespaces

- **Avoid `using namespace std;`** globally, as it can cause conflicts between libraries.

  **Problem Example**:
  ```cpp
  // <vector>
  namespace std {
      template <class T, class Allocator = allocator<T>>
      class vector {
          // ...
      };
  }

  // "mylib.h"
  template <class T>
  class vector {
      // ...
  };

  // file.cpp
  #include <vector>
  #include "mylib.h"

  int main() {
      vector<int> x; // my own vector (mylib.h)
      using namespace std;
      vector<int> y; // ambiguous!
  }
  ```

- Especially **avoid `using namespace` in header files**.

### Preferred Alternatives

- Use **explicit symbol imports**:
  ```cpp
  using std::cout;
  using std::vector;

  cout << "blah blah";
  vector<int> v(10);
  ```

- Or even better, use **explicit namespace scope**:
  ```cpp
  std::cout << "blah blah";
  std::vector<int> v(10);
  ```

