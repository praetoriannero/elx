
# Symbol Types:


## Struct
ex:
```
struct Foo<T, U>(Bar<T, U>): Baz<T> {
    a: T,
    b: U,

    fn __init__(&mut self, a: T, b: U) -> Self {
        self.a = a;
        self.b = b;
    }
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

## Function


## Module


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

