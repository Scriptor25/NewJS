# How to NewJS

This is a short guide on how to write code compatible with the NewJS compiler.

## Basics

Like already mentioned in the [general documentation](../README.md), the syntax of this language is very similar to web
scripting languages like JavaScript or TypeScript. Probably more like TypeScript due to the very strict typed-ness of
the language. Let's start of by writing the simplest program imaginable that still runs and counts as a real program in
NewJS: an empty file. That does work and run because the top level scope of the file itself IS the main function of the
module it represents.

To write a "Hello World" example, you would do something like this:

```
extern function println(message: i8[const], ...)

println("Hello World!")
```

There are no standard header files for NewJS, so you need to declare all functions you want to use yourself at some
point. I'll probably add then sometime, but for now you have to live with writing a lot of stuff yourself.

## Functions

Functions work very similar to the ones in JavaScript / TypeScript:

```
function foo(arg1: type1, arg2: type2): type3 { ... }
```

But this is only the tip of the iceberg. If you need to import a function from another module, you need to prepend the
`export` keyword:

```
export function foo() { ... }
```

Also, if you don't want to implement the function yet for some reason, or it is marked `extern`, you don't have to
provide a function body yet:

```
function foo()

extern function foo()
```

Extern functions are not name-mangled, that means they are compatible with C programs and libraries, so you can use the
C standard library. That is why I did not have to implement a complete one by myself.

Something that has been considered black magic by myself, because I don't know how the f*** I got it to work, are
**FUNCTION AND TYPE TEMPLATES** (*play epic thunder sound now*). If you don't know what they are or what they do, don't
touch them, but else, here's how you use them:

```
function<A, B, C> foo(bar: A): B { ... }
```

Function templates must be implemented and can not be predefined. They provide you with a way of creating functions
using types you don't know yet. That is very useful for something like a generic vector type, where you don't know the
type of the elements yet.

## Function Calls

Function calls are really nothing special, they are just the callee followed by an open-close pair of parenthesis
containing the arguments. Only special thing is, again, function templates. Those are called like this:

```
foo<A, B, C>(bar)
```

The template types are not implicit, and must be specified every time.

## Types

There are a few default types in NewJS:

| Mnemonic | Size (Byte) | Description             |
|----------|-------------|-------------------------|
| i1       | 1           | 1-bit signed integer    |
| i8       | 1           | 8-bit signed integer    |
| i16      | 2           | 16-bit signed integer   |
| i32      | 4           | 32-bit signed integer   |
| i64      | 8           | 64-bit signed integer   |
| u1       | 1           | 1-bit unsigned integer  |
| u8       | 1           | 8-bit unsigned integer  |
| u16      | 2           | 16-bit unsigned integer |
| u32      | 4           | 32-bit unsigned integer |
| u64      | 8           | 64-bit unsigned integer |
| f16      | 2           | 16-bit floating point   |
| f32      | 4           | 32-bit floating point   |
| f64      | 8           | 64-bit floating point   |
| void     | -           | nothing                 |

There are some builtin aliases for types:

| Mnemonic | Description       | Original  |
|----------|-------------------|-----------|
| boolean  | a boolean         | u1        |
| char     | a character       | i8        |
| string   | a constant string | i8[const] |

You can use these primitives directly or combine them in different ways:

| Mnemonic                | Size (Byte)               | Description                                                                          |
|-------------------------|---------------------------|--------------------------------------------------------------------------------------|
| X[]                     | system dependent          | a pointer to type X                                                                  |
| X[const]                | system dependent          | a constant pointer to type X                                                         |
| X[N]                    | size of X * N             | an array of N elements with type X                                                   |
| [ X, Y, ... ]           | dependent on the elements | a tuple of a variable number of different typed elements                             |
| { foo: X, bar: Y, ... } | dependent on the elements | a struct of a variable number of typed members, identifiable uniquely by their names |
| (X) => Y                | system dependent          | a function type taking argument of type X and returning type Y                       |

To create an alias for a type, use

```
type A = B
```

where `A` is the name of the alias and `B` is any type. This can be used for creating aliases for structs, tuples,
arrays, etc., e.g.

```
type my_struct_t = {
    field_a: i32,
    field_b: u32,
    field_c: i8[const],
}
```

And, again, you can make templates, but this time for types. This is demonstrated here for a generic pair type:

```
type<F, S> pair = {
    fst: F,
    snd: S,
}
```

# TODO
