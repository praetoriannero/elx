
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
- [ ] struct to string method (aka \_\_str\_\_)

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

# Symbol Types

## Struct
ex:
```
struct Foo {
    a: u32;
    b: *bool;

    fn __init__(&mut self, a: u32, b: bool) -> Self {
        self.a = a;
        self.b = new(bool(b));
    }

    fn __deinit__(self) -> None {
        free(self.b);
    }
}
```


## Function


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


# Object Internals
Objects store both inherited struct and implemented trait data in their headers.

Elamite definition:
```
struct Foo<U, T>: Bar<U, T> {

}
```

# Memory Management

## Builtin Functions
`alloc` creates a heap allocated contiguous memory region at least of size `T`. `alloc` will never return a pointer to `null` as out-of-memory (OOM) errors are handled by a call to `panic` resulting in program termination.
ex:
```
let x: *T = alloc(sizeof(T));
```

`free` deallocates a heap allocated region of memory defined by the size of the originally allocated region. Once `free` is called on a pointer type, the pointer is set to `null` automatically.
ex:
```
free(x);
```

Calling `free` on a stack allocated pointer type results in a call to `panic`:
```
let x: *T;
free(x)     // panics
```

