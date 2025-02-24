# NewJS - WTF is this

NewJS is the hero nobody wants and also nobody needs; it represents the worst of all programming language worlds: a bad
cli experience, JS-like syntax, and even worse compiler errors. The only upside is that it is strictly typed, so no
possible issues on that side... So if you really and completely want to lose your mind and don't know if it is you or
the compiler that does sh**, then use NewJS. If you just stopped by to take a look, well, take a look and then LEAVE.
But if you somehow happen to decide to write some NewJS, be prepared for your work being worthless a few commits down
the line, as this language guaranties no backwards compatability with older compiler versions at all. But anyway, you
can read the rest of this documentation if you want to learn more or just want to know how to NOT write a documentation
for a compiler.

## About NewJS

The NewJS "language standard" features different functionality all hand-picked from already existing languages, like the
overall syntax and destructuring from JavaScript / TypeScript, references are a mix between C++ and Rust, the module
system is similar to the one from Python, but also "syntaxed" like JavaScript, you've got structs, tuples, pointers,
functions, integers and floats, you can create type aliases, you can pass constants into constant reference parameters
for function calls... the list is long and weird, but it sometimes makes a lot more sense when you try to write code
using NewJS instead of only talking about it.

If you want to learn more about the syntax and logic of the language, take a look at
the [language documentation](docs/newjs.md)

## Building NewJS

To build the compiler and standard library (yes, there is a standard library) you need to have a few things installed
beforehand:

* a C/C++ compiler toolchain of your choice (supporting c++ 20 standard with formatting and that stuff; the project was
  tested with gcc-13, gcc-14 and msvc-v143)
* CMake (version 3.20 or higher)
* a build system like Ninja or GNU Make
* LLVM 18

If you do not have LLVM version 18 installed, this will not work. Under Debian / Ubuntu systems you can just install the
packages from [the llvm apt repository](https://apt.llvm.org/), but for any other system (sorry Windows users) you'll
have to build it by yourself. But I'll provide a guide on [how to quickly setup and build LLVM](docs/llvm.md), too

Now building the compiler is a simple as checking out the project:

```shell
git clone https://github.com/Scriptor25/NewJS.git
```

Entering the project directory:

```shell
cd NewJS
```

Configuring CMake:

```shell
cmake -S . -B build 
```

Building:

```shell
cmake --build build 
```

And last but not least, installing:

```shell
cmake --install build
```

If you don't know how to use CMake, it is a bit late now, but
here [the CMake documentation](https://cmake.org/cmake/help/latest/index.html).

## NewJS Code Examples

There are several [simple NewJS code examples](example) included with this repository. Some of them are GNU Make
projects, so if you want to explore them keep in mind you have to have GNU Make installed for them to work -
alternatively you could also enter the cli commands by hand.

Anyway, one example without any project files is the fibonacci number example. Build it using:

```shell
newjs -m fib -o fib.o -t obj fib.njs
```

This spits out the compiled object representation of this program. You can link it using either MSVC's `link` or GNU's
`gcc` against the `stdnewjs` standard library, like this:

```shell
gcc -o fib fib.o -lstdnewjs
```

Very similar is the `test.njs` example. This one needs to be linked against the fibonacci example, but there is a way to
build them both at once:

```shell
newjs -m test -o test.o -t obj test.njs fib.njs
```

As you can see, NewJS has the amazing ability to link multiple source files at compile time, so the only thing you have
to link against now is, again, the standard library:

```shell
gcc -o test test.o -lstdnewjs
```
