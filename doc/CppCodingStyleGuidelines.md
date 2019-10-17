# FCCSW Guidelines for C++ Code Style

Clashing coding styles are avoided by broadly following the [LHCb](https://twiki.cern.ch/twiki/bin/view/LHCb/LHCbCodingGuidelines) / Gaudi style guides.

One particular exception is that we extended the 80 characters per line requirement to 120 characters per line.

## Goal

Give guidelines on naming conventions and how to structure code.

General tips on how to write good code can be found [here](./GeneralCppGuidelines.md).

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

## Automatic Checking

Some basic formatting checks can be done with the `checkformat` script which calls the SAS format checker based on clang (and sets up the required LLVM suite temporarily). There are two different ways of using it:

- Show diff only:

~~~
./checkformat Examples/src/CreateSampleJet.cpp Examples/src/CreateSampleJet.h
~~~

  It will display a diff in case of mismatches between the format defined in a FCCSW format file that is automatically created for you (`.clang-format`).

- Apply changes in-place:

~~~
./checkformat -i Examples/src/CreateSampleJet.cpp Examples/src/CreateSampleJet.h
~~~

  All mismatches between the format and the implementation / header files are applied in place. **This will override your files. Make sure you commit your changes first, then use the formatter and amend the changes to your commit after checking everything still compiles and tests.**

Note that this does not check for naming conventions at the moment but only for correct indentation, trailing whitespaces, line length, etc. The script can be called with a list of files separated by whitespace or on a directory.


## Naming Conventions

### Variable Names
Names should be as descriptive as possible.

Do not worry about space; Worry about how understandable the name is. Avoid unclear abbreviations.

**Examples**:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
int trackParameter;  // no abbrevations
int numErrors;       // num is unambigous
// Do not:
int n;               // meaningless
~~~~~~~~~~~~~~~~~~~~~~~~

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

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
class Foo {
public:
  // ctor + dtor should be here
  int bar() const { return m_bar; }
  void setBar(int bar) { m_bar = bar; }
private:
  int m_bar;
};
~~~~~~~~~~~~~~~~~~~~~~~~

### Namespace names

Namespaces are all lower case.

### Enumerator names

Enumerators should follow the same rules as constants (start with k).

**Example**

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
enum class ErrorCode {
  kSuccess, kFileNotFound, kError
};
~~~~~~~~~~~~~~~~~~~~~~~~

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
  - tests (contains files used for unit-testing)
    - options (contains configuration files only used for tests)
    - scripts (contains python scripts used for testing)
  - data (contains input text files needed for this package)

### File Names

Header files end with `.h`, source files end with `.cpp`, standalone executable implementations end with `.cxx`.

When an additional "implementation header file" is needed (e.g. in order to not clutter the header file of a templated class) an additional file `.incl` can be included in the header file.

***

## General Header Rules

### Include Guard

Avoid multiple includes with include guards.

A guard should follow the naming convention of `PACKAGE_FILE_H` where `PACKAGE` is the package name and `FILE` is the file name (without `.h`). Both written in all capital letters. The corresponding if is closed at the end of the header file and has a trailing comment with the name of the constant.

**Example**

In file `BarPackage/BarPackage/Foo.h`:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
#ifndef BARPACKAGE_FOO_H
#define BARPACKAGE_FOO_H
// header body
#endif /* BARPACKAGE_FOO_H */
~~~~~~~~~~~~~~~~~~~~~~~~

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

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
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
~~~~~~~~~~~~~~~~~~~~~~~~

### One Class per Header

Each header file should only contain the declaration of one class.

***

## Code Formatting

### Indentation
Indentations should be done with spaces, 2 spaces per indentation step.

- Indent by scope with the exception of namespaces and access modifiers (`public`, `private` and `protected`).
- Indent in case of line continuation.
- Indent twice for member initialiser lists

**Example**:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
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
~~~~~~~~~~~~~~~~~~~~~~~~

Member initialiser list:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
Foo::Foo() :
    m_bar(1),
    m_foo(1) {
  // ctor body
}
~~~~~~~~~~~~~~~~~~~~~~~~

### Curly braces

Opening curly braces should be placed in the same line; Closing curly braces should be in a new line.

**Examples**:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
class Foo() {
  // my class definition
};
~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
if (condition) {
  // doSomething
}
~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
if (condition) {
  // doSomething
} else {
  // doSomethingElse
}
~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
for (int a = 10; a < 20; ++a) {
  // doSomethingRepeatedly
}
~~~~~~~~~~~~~~~~~~~~~~~~

### Comments

Comments should be abundant and non-trivial.

Both `/* */` and `//` style comments are OK. Inline comments should be short, prefer to comment before the relevant code block.

Keep in mind doxygen pages are created and read through the [how-to for doxygen comments](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html):

- Use `@` for [commands](http://www.stack.nl/%7Edimitri/doxygen/manual/commands.html#cmd_intro).
- Member functions documentation depends on the detail level:
  - Short descriptions on the line above the function definition with `///` starting the comment.
  - Complex descriptions using [return cmd](http://www.stack.nl/%7Edimitri/doxygen/manual/commands.html#cmdreturn) and [params cmd](http://www.stack.nl/%7Edimitri/doxygen/manual/commands.html#cmdparam) should follow the same style as class descriptions (see example).
- Member variable documentation depends on the length of the comment:
  - A short description of a member may be put on the same line starting with `///<`.
  - Longer descriptions are put above the member definition starting with `///`.
- A class description is put above the namespace region with author information and a description of the class. A date may optionally be added.

**Example**

*NOTE* this example is not rendered correctly in Doxygen, have a look at [github](https://github.com/jlingema/FCCSW/blob/master/doc/CppCodingStyleGuidelines.md#comments).

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
#include "OtherClass.h"

/** @class foo::MyClass MyPackage/MyPackage/MyClass.h MyClass.h
 *
 *  Short description of what your class does
 *
 *  @author Main Author
 *  @author Other Contributor
 *  @date yyyy-mm-dd
 */
namespace foo {
class MyClass : public OtherClass {
public:
  /** @enum foo::MyEnum
   *  Description of the enum
   */
  enum class MyEnum {
    kVal1, kVal2
  };

  /// Description about ctor
  MyClass(int a, int b);

  /** doSomething: does something
    * @param[in] f is used to do...
    * @returns an integer
    */
  int doSomething(const float& f);

private:
  int m_a;  ///< short member a documentation
  /// a rather long description for member b that needs a whole line
  int m_b
};
}
~~~~~~~~~~~~~~~~~~~~~~~~

