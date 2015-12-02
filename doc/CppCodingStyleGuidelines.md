# FCCSW Guidelines for C++ Code Style

Clashing coding styles are avoided by broadly following the [LHCb](https://twiki.cern.ch/twiki/bin/view/LHCb/LHCbCodingGuidelines) / Gaudi style guides.

One particular exception is that we extended the 80 characters per line requirement to 120 characters per line.

## Goal
Give guidelines on naming conventions and how to structure code.

General tips on how to write good code can be found in ... (add link)

## Contents
* [Naming Conventions](#naming-conventions)
   * [Variable Names](#variable-names)
      * [Names of Member Variables](#names-of-member-variables)
      * [Names of Constants](#names-of-constants)
      * [Additional Considerations](#additional-considerations)
   * [Type Names](#type-names)
   * [Function Names](#function-names)
      * [Member Functions](#member-function)
   * [Namespace Names](#namespace-names)
   * [Enumerator Names](#enumerator-names)
* [Class Declaration](#class-declaration)
   * [Declaration Order](#declaration-order)
   * [Inline Functions](#inline-functions)
* [Function Declaration](#function-declaration)
   * [Parameter Order](#parameter-order)
* [Code Structure](#code-structure)
   * [Folder Structure of a Package](#folder-structure-of-a-package)
   * [File Names](#file-names)
* [General Header Rules](#general-header-rules)
   * [Include Guard](#include-guard)
   * [Include Ordering](#include-ordering)
   * [Forward Declaration](#forward-declaration)
   * [One Class per Header](#one-class-per-header)
* [Code Formatting](#code-formatting)
   * [Indentation](#indentation)
   * [Curly Braces](#curly-braces)
   * [Comments](#comments)

## Naming Conventions
### Variable Names
Names should be as descriptive as possible.

Do not worry about space; Worry about how understandable the name is. Avoid unclear abbreviations.

**Examples**:
```C++
int trackParameter;  // no abbrevations
int numErrors;       // num is unambigous
// Do not:
int n;               // meaningless
```

#### Names of Member Variables
Member variables should be prefixed with `m_` otherwise the same rules as above apply.

This does not apply to (public) member variables of structs.

#### Names of Constants
Constant variables should start with `k`.

#### Additional Considerations
It might be clearer to the reader if variables are prefixed with a letter corresponding to its scope / type. This is not a strict rule, but consider to follow:
- Function arguments can be prefixed with `a`
- variables with local scope can be prefixed with `l`

### Type Names
Types start with an upper case letter, capital letter for each new word. Generally they should be a noun and singular. Consider using plural for collection names.

This applies for all types: Classes, enums, typedefs, structs and type template parameters

### Function Names
Functions (both member or non-member) start with lower case and capital letter for each new word.

#### Member Functions
As noted above, member functions follow the same naming convention as non-member functions.

Additionally, getter functions do *not* start with get. Instead, the related variable name (omitting the `m_`) is used with lower case for the first word.

Setter functions start with `set` and continue with the related variable name (omitting the `m_`).

**Example**
```C++
class Foo {
public:
  // ctor + dtor should be here
  int bar() const { return m_bar; }
  void setBar(int bar) { m_bar = bar; }
private:
  int m_bar;
};
```

### Namespace names
Namespaces are all lower case.

### Enumerator names
Enumerators should follow the same rules as constants (start with k).

**Example**
```C++
enum class ErrorCode {
  kSuccess, kFileNotFound, kError
};
```

***

## Class Declaration
All entities should be contained in a namespace.
### Declaration Order
Follow this order: `public` before `protected` before `private`; methods before data members. Omit empty sections.

More specifically, within each section follow this order:
- Typedef + Enums
- Constants (static const data members)
- Constructors
- Destructors
- Methods
- Data members

### Inline Functions
Only trivial implementations should be inlined in the class definition.

Consider additional `.impl` files for performance critical `inline` functions that are longer than one line or templated functions.

*Note*: Functions should only be inlined if they are proven to help the compiler to optimise the code.

***

## Function Declaration
All entities should be contained in a namespace.
### Parameter Order
Parameter order is: constant inputs, non-const inputs, outputs.

Inputs should generally be const references or values. Inputs that can be changed are references. Outputs are pointers.

***
## Code Structure
### Folder Structure of a Package
The repository is (or will be) divided into main topics like Reconstruction, Simulation, etc. The packages in these main topics will start with a three letter prefix indicating the topic they belong to, `Rec`, `Sim`, etc.

Packages may again be split in packages, too much nesting should be avoided.

The last child package should have the folder structure:
- PrePackage (main package folder, "Pre" according to topic)
  - PrePackage (contains all header files that are part of the package library)
  - src
    - Lib (contains all implementation files that are part of the package library)
    - components (contains all implementation and header files not part of the package library)
  - options (contains all configuration files)
  - dict (contains files needed to build the lcg library)
  - doc (contains readme files documenting this specific package)
  - test (contains files used for unit-testing)

### File Names
Header files end with `.h`, source files end with `.cpp`, standalone executable implementations end with `.cxx`.

When an additional "implementation header file" is needed (e.g. in order to not clutter the header file of a templated class) an additional file `.impl` can be included in the header file.

***

## General Header Rules
### Include Guard
Avoid multiple includes with include guards.

A guard should follow the naming convention of `PACKAGE_FILE_H` where `PACKAGE` is the package name and `FILE` is the file name (without `.h`). Both written in all capital letters. The corresponding if is closed at the end of the header file and has a trailing comment with the name of the constant.

**Example**

In file `BarPackage/BarPackage/Foo.h`:
```C++
#ifndef BARPACKAGE_FOO_H
#define BARPACKAGE_FOO_H
// header body
#endif /* BARPACKAGE_FOO_H */
```

### Include Ordering
Standard order for readability: Related header, C library, C++ library, other libraries, project headers, e.g. for class `Foo` in `Foo.cpp`:

1. Related header: `#include "Package/Foo/Foo.h"` or `#include Foo.h`
2. C-system files: `#include <sys/types.h>`
3. C++ system files: `#include <vector>`
4. Other libraries: `#include "boost/ptree.h"
5. Other project headers: `#include "Package/Foo/Bar.h"`

### Forward Declaration
Prefer forward declaration over including the corresponding header-files.

**Examples**
```C++
// Forward declaration of class in the same namespace:
class Foo;
// Forward declaration of a class in another namespace:
namespace foo {
class Bar;
}
// Forward declaration of a class in nested namespaces:
namespace foo {
namespace bar {
class FooBar;
}
}
```

### One Class per Header
Each header file should only contain the declaration of one class.

***

## Code Formatting
### Indentation
Indentations should be done with spaces, 2 spaces per indentation step.

- Indent by scope with the exception of namespaces and access modifiers (`public`, `private` and `protected`).
- Indent in case of line continuation.
- Indent for member initialiser lists

**Example**:
```C++
namespace foo {
class Foo {
public:
  Foo();
  ~Foo();
private:
  int m_bar;
  int m_foo;
};
}
```
Member initialiser list:
```C++
Foo::Foo() :
  m_bar(1),
  m_foo(1) {
  // ctor body
}
```

### Curly braces
Opening curly braces should be placed in the same line; Closing curly braces should be in a new line.

**Examples**:
```C++
class Foo() {
  // my class definition
};
```

```C++
if (condition) {
  // doSomething
}
```

```C++
if (condition) {
  // doSomething
} else {
  // doSomethingElse
}
```

```C++
for (int a = 10; a < 20; ++a) {
  // doSomethingRepeatedly
}
```

### Comments
Comments should be abundant and non-trivial.

Both `/* */` and `//` style comments are OK. Inline comments should be short, prefer to comment before the relevant code block.

Keep in mind doxygen pages are created and read through the [how-to for doxygen comments](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html).
