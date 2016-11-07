# FCCSW Guidelines for C++ Code

The first sentences of each section is an executive summary of the section.

## Goal

Some common sense guidelines on how to write code that is easier to maintain. Please keep in mind that such a document can never be complete but only a starting point.

## Contents

- [Repository Rules](#repository-rules)
- [General Remarks](#general-remarks)
- [Scope and Initialisation](#scope-and-initialisation)
- [Namespaces](#namespaces)
- [Pointers and References](#poiners-and-references)
- [Constness](#constness)
- [Lifetime](#lifetime)
- [Best Practices in Parallelism](#best-practices-in-parallelism)
- [Useful C++11 Features](#auto)
- [Other Things to Avoid](#other-things-to-avoid)
- [Further Reading](#further-reading)

## Repository Rules

As mentioned in the README, the contributions to the code are reviewed in pull-requests.

### Code in the master-branch:

Code that is committed and pushed to the master branch has to compile without warnings.

### Code in topic / development branches:

Code pushed in dedicated topic branches that are used by a sub-group has to compile.

## General Remarks

- Try to write code that is easy to understand
  - Use inheritance only where it is needed
    - Avoid complicated inheritance diagrams
    - Avoid multiple inheritance
    - Avoid friend classes
  - Use templates with care
  - Consider whether aggregation or composition is needed
  - If something can be written in one line instead of four does not mean it is better
  - Avoid multiply branching code (nested `if` or loop constructs)
- Document your code
  - Document your interfaces (Doxygen compatible)
    - Make clear what are the inputs and what are the outputs
  - Document the purpose of your class
  - If multiple known solutions exist, mention why you chose the one you did
- Make things explicit
  - Read about constness below
  - Try to choose function names that already describe what it does

## Scope and Initialisation

Place variables / objects in the narrowest scope possible and initialise variables where you declare them.

For class members use the member initialiser list instead of constructor-body to initialise.

**Examples**

Initialise where you declare:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
auto vec = std::vector<int>{1, 2}; // C++11
int i = f();
~~~~~~~~~~~~~~~~~~~~~~~~

Member initialiser list:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
Foo::Foo(int bar, int foo) :
  m_bar(bar),
  m_foo(foo) {
}
~~~~~~~~~~~~~~~~~~~~~~~~

Variables needed for loops should be declared within the statements to confine to scope:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
for (int i = 0; i < 1000; ++i) {
  doSomething(i);
}
~~~~~~~~~~~~~~~~~~~~~~~~

Caveat: Objects needed in the loop should not be created on each iteration:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
Foo f();
for (int i = 0; i < 1000; ++i) {
  f.doSomething(i);
}
~~~~~~~~~~~~~~~~~~~~~~~~

## Namespaces

All entities are contained in a namespace. The only exception are classes that are plugins / components (i.e. classes that are not used by other code).

_Warning_: Do not include header files within the namespace region but before!

### Non-member Functions and Namespaces

If non-member functions are associated to a specific class they belong in the same namespace as the class.

Non-member functions that are not associated to specific classes are grouped in namespaces by functionality.

### Avoid Using `using namespace`

Never write `using namespace`.

If the statement is used in a header file, the statement holds in any code that includes that header file. While the effect may be less detrimental in implementation files, consider that it hides information from future maintainers.

Similar to python's importing rules, only use the statement for single entities from a namespace so it is clear where it is defined and to avoid name hiding:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
using std::cout;
using std::endl;
using mathpackage::sqrt;

void printSqrt(float value) {
  cout << sqrt(value) << endl;
}
~~~~~~~~~~~~~~~~~~~~~~~~

## Pointers and References

The code should not contain `new` outside of braces. Avoid raw-pointers where possible.

Raw C++ pointers should be avoided. Only use them when you are certain that you have to. Consider using `std::unique_ptr`, `std::shared_ptr` and references instead.

Pass by const reference instead of const pointer.

Avoid dangling handles (references or pointers) by avoiding to return references or pointers.

### When Pointers

Special cases may arise where pointers are needed (e.g. interface to external packages). In that case **document!** Please document _why_ you use a pointer, _who_ is responsible for the pointer's deletion, _when_ the pointer is deleted.

_If you **really** need to_ declare a pointer without instantiating the corresponding object, use `nullptr`. Avoid null-pointers wherever possible.

**Examples**

Unique pointers: They hold sole ownership over the contained object. It is not possible to have two `unique_ptr` managing the same object.

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
{
  std::unique_ptr<Foo> myPtr(new Foo);
  // Smart pointers allow normal pointer semantics:
  myPtr->doSomething();  // Calls the member function of Foo
  f(*myPtr);             // Passes a reference to Foo (or value, depending on the function argument)
} // myPtr goes out of scope and the instance of Foo is deleted automatically
~~~~~~~~~~~~~~~~~~~~~~~~

Shared pointers: Shared ownership of the object across several instances of the pointers. The object is deleted when the last `shared_ptr` managing it is destroyed or the last managing `shared_ptr` is assigned another object to hold.

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
{
  // Let's assume Bar is derived from Foo:
  std::shared_ptr<Foo> myPtr1 = std::make_shared<Bar>();
  {
    // This myPtr2 points to the same instance of Bar:
    std::shared_ptr<Foo> myPtr2 = myPtr1;
  } // myPtr2 goes out of scope, but myPtr1 stays in scope, the instance still exists
} // Here, the instance of Bar is deleted: The last pointer, myPtr1, goes out of scope
~~~~~~~~~~~~~~~~~~~~~~~~

## Constness
Use `const` wherever you can.

If `const` is used correctly, the compiler helps to identify potential problems.

Functions without side-effects are preferable and should be marked as such. Declare member-functions that do not change the state of the object as `const`.

If your function is only "an observer" of an argument, make it explicit. This way you get compiler errors when a function unexpectedly does change the object's state (if the above rule is followed).

Getter functions should return const values or references. _Note_ that when returning a reference and you cannot guarantee the object's validity after the function call, you risk a dangling reference.

**Example**

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
class Foo {
public:
  // Omitting ctor and dtor

  // The getter does not change the state of Foo
  const Vector3 position() const {
    return m_position;
  }

  // This function does not change the state of Foo or the argument
  const Vector3 offset(const Vector3& otherPosition) const {
    return m_position - otherPosition;
  }

  // This set-function does change the state of Foo not of the argument
  void setPosition(const Vector3& newPosition) {
    m_position = newPosition;
  }

  // This function does change the argument but not Foo
  void addPositionTo(Vector3& otherPosition) const {
    otherPosition += m_position;
  }
private:
 Vector3 m_position;
};
~~~~~~~~~~~~~~~~~~~~~~~~

## Lifetime

Always consider what the lifetime of an entity is and restrict it to the shortest period that makes sense.

Sometimes it is not inherently clear what the lifetime of an object in memory is. While names only exist in the scope in which they are defined, the objects they refer to can exist longer:
- Objects created with `new` live until they are destroyed using `delete`.
- The lifetime of static objects is from the time they are defined the first time until the program exits

If objects exist longer than their name, the corresponding memory is "leaked" (i.e. practically, it becomes unusable). This directly translates into performance issues. Therefore
- Use smart-pointers instead of raw pointers
- Avoid static objects

## Best Practices in Parallelism

### Statelessness

Write stateless functions and classes wherever possible.

Practically speaking, stateless functions are functions that always produce the same result given the same inputs. Examples are implementations of mathematical functions or pure functions.  Note that statelessness is a stricter concept than constness: A const function may still depend on a member of the object (i.e. the state of the object) that can be altered between calls.

Stateless objects do not have any member fields that can be altered. I.e. they only have compile-time constants as members. Note that being stateless is a stricter requirement than immutable: An immutable object has only `const` members but they may depend on the initialisation.

The benefit in terms of parallelism is that the function / object can be safely used in different threads and will still produce the desired result, as there is no shared information / state that can be altered between the threads.

See also [const-expressions](#const-expressions) below.

### Re-entrancy

If a function cannot be stateless, the state should be local.

Re-entrant functions can be interrupted at any point and re-entered without altering results. They may depend on the state of the object but the state cannot change between calls (e.g. const members of an object). Especially the use of non-const static or global data breaks re-entrancy. For a function to be re-entrant, it can only call re-entrant code.

## Useful C++11 Features

### Auto

Where the type of an object can be inferred by the compiler, `auto` can be used.

It can be useful to decouple from specific implementations of data-structures. To take constness into account, use `const auto`. Similarly you can get a reference with `const auto&`.

**Examples**

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
// An iterator:
auto vectorIterator = myVector.begin();
// A const iterator:
auto vectorConstIterator = myVector.cbegin();
// A reference to the collection of tracks:
auto& trackCollection = getTracks();
~~~~~~~~~~~~~~~~~~~~~~~~

### For Loops

Use the C++11 for-loop syntax to avoid common mistakes.

In combination with `auto`, iterators are even easier to use in C++ and should be preferred over index loops when looping over container contents. C++11 also defines a syntax for a loop that iterates automatically over each object in a container.

**Examples**

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
std::vector<int> myVec = {1, 2, 3, 4, 5};
// Iterate over all contents
for (const auto& cntr : myVec) {
  std::cout << cntr << std::endl;
}

// Using auto and iterators in loops
for (auto it = myVec.begin(); it != myVec.end(); ++it) {
  std::cout << *it << std::endl;
}
~~~~~~~~~~~~~~~~~~~~~~~~

### Const-Expressions
Use `constexpr` if an expression can be calculated at compile time.

Const expressions are evaluated at compile time (i.e. no computation during run time). You can declare functions, variables and even constructors as `constexpr`.

**Example**

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
constexpr int addTwo(int arg) {
  return arg+2;
}

constexpr int a(4);
constexpr int b = addTwo(a);
int c = addTwo(b);
~~~~~~~~~~~~~~~~~~~~~~~~

### Move semantics
A destructive(!) variant of copy which can optimise performance.

Sometimes it makes sense not to duplicate the data contained in an object but just to adjust pointers instead. This makes only sense in certain cases:
- You want to swap objects
- The object that is moved somewhere else cannot be used afterwards

Example use case: Storing objects instead of pointers in `std::vector` can be made efficient when implementing the move constructor:

~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
Foo(Foo&& other);
~~~~~~~~~~~~~~~~~~~~~~~~

## Other Things to Avoid

- Do not write singletons
- Do not define global variables, any entity in the global namespace
- Never have non-const static functions or member data

### Avoid C syntax

While most C-code compiles as C++, it is strongly discouraged.

An incomplete list:
- Use const-expressions instead of macros (see [above](#const-expressions) for details)
- Use enums or `static const` variables instead of constants
- Forget about `malloc`, etc.
- Prefer C++ style casts: `static_cast<int>(variable)` instead of `(int) variable`


## Further Reading

There are many good books and open material about best practices in C++. An incomplete list:
- Scott Meyers, _Effective C++_ and _More Effective C++_
- to be extended...
