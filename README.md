# Purpose
This document currently reflects the design intentions of the Elamite language. It is not meant to currently reflect the 

# Features in v0.1.0
- [x] lexer
- [ ] parser (in-progress)
- [ ] analyzer
- [ ] transpiler
- [ ] compiler
- [ ] struct member methods

# Features in v0.2.0
- [ ] enums
- [ ] unions
- [ ] type to string method (aka \_\_str\_\_)
- [ ] templates

# Compilation Stages

## Discovery
During the discovery phase, `elx.config` is used to determine the location of all source files.

## Lexing
Source code is lexed into individual tokens.

## Parsing
An abstract syntax tree (AST) is generated from the tokens.

## Analysis
An analysis pass occurs over the AST to ensure correctness to the Elamite language standard.

## Transpilation
The AST is converted into C source code.

## Compilation
The generated C source code is compiled into the resulting executable or library.

# Keywords
| Keyword | Usage | Meaning |
| --- | --- | --- |
| `test` | `test { ... }` | defines a module that, when imported in a project, will be called with `elx test` |


# Symbol Types

## Struct
ex:
```
struct Foo {
    a: u32;
    b: *bool;

    fn __init__(&mut self, a: u32, b: bool) -> Self {
        // __init__ defines a constructor, like `T()`
        self.a = a;
        self.b = new(b);
    }

    fn __deinit__(self) -> None {
        // __deinit__ is called when a variable exits the scope it is defined in
        del(self.b);
    }
}
```


## Function
ex:
```
fn foo(x: &u32) -> () {
    println("the value of x is {}", *x);
}

fn bar(x: &mut u32) -> () {
    x += 4;
    println("the value of x changed to {}", *x);
}

fn baz(x: &u32) -> u32 {
    return *x;              // returns a copy of *x
}
```

## Enum
ex:
```
enum Bar {
    A,
    B(u32),
    C(str),
}
```


## Module


## Trait


## Impl


## Union


## Bitfield
```
bitfield Waldo {
    x: i8: 4,
    y: f64: 7,
}
```

# Mutable and Immutable Variables
All variables are `const` by default.
```
let x = 0;
x += 1;         // panics; variable `x` is immutable
```

The keyword `var` is used to describe mutable types.
```
var x = 0;
x += 1;         // okay

var y = &x;
let z = 4;      // a variable may be assigned to a constant (i.e., `let`) value
y = &z;         // okay

let m = 3;
var y = &m; // panics; a mutable reference cannot be created from a constant
```

# Memory Management

## Creating objects on the heap
`new` creates a heap allocated contiguous memory region at least of size `T`. `new` will never return a pointer to `null` as out-of-memory (OOM) errors result in a call to `panic` resulting in program termination.
ex:
```
let x: *bool = new(true);
```

## Freeing objects on the heap
`del` deallocates a heap allocated region of memory defined by the size of the originally allocated region. Once `del` is called on a pointer type, the pointer is set to `null` automatically.
ex:
```
del(x);
```

Calling `del` on a stack allocated pointer type results in a call to `panic`:
```
let y: *T = new(T{});
del(y);                 // does not panic, successfully frees memory

let x: *T;
del(x);                 // panics
```



# Builtin Types
- u8
- i8
- u16
- i16
- u32
- i32
- u64
- i64
- f32
- f64
- uchar
- ichar
- str
- ()
- usize
- isize
- bool


# References and Pointers

A reference is defined as a memory address to either a heap or stack allocated object.
ex:
```
let y: u32 = 0;
let x: &u32 = &y;
```

Only one mutable reference may exist for an object at a time.
```
let mut i: u32 = 42;
let j: &mut u32 = &mut i;
let k: &mut u32 = &mut i;   // panics, more than one mutable reference is defined for i
```

Any number of immutable references may exist for an object, however.
```
let i: u32 = 42;
let j: &u32 = &i;
let k: &u32 = &i;   // this is fine
```

The `*` operator dereferences references and pointers.
```
let x: u32 = 42;
let y: &u32 = &x;   // equals an immutable pointer to a stack allocated value
let z: u32 = *y;    // equals 42
```

A function which takes in a reference type must explicitly be passed a reference type. No implicit conversions occur when attempting to pass value types as reference types.
ex:
```
fn foo(x: &u32) -> u32 {
    return *x;
}

let x = 42;
// foo(x);                     // panics; wrong type error

let y = &x;
foo(y);                        // okay
foo(&x);                       // okay
foo(&mut x);                   // warning; keyword `mut` not required

let z = new(42);

// foo(z);                     // panics; wrong type error
foo(&*z);                      // okay
```

A function may not manipulate immutable reference types or their contents.
```
fn foo(x: &u32) {
    (*x) += 2;                     // panics; the reference and its contents cannot be mutated
}
```

Pointer types refer to heap memory addresses.
```
let x: *u8 = new(u8{42});       // allocates a 64-bit pointer
// or use shorthand
let x = new(42);
```

Pointer types can create memory leaks if not freed after creation.
```
del(x);                         // deletes/frees the allocated memory for x and sets x to `null`
```

Similar to reference types, a function which takes in a pointer type cannot implicitly accept a value type.
```
fn foo(x: *u32) -> u32 {
    println("The memory address of x is {}", x);
    println("The value of x is {}", *x);
}
```

# Helpful Builtin Types
- Box<T>
- Option<T>
