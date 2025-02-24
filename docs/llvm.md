# How to LLVM

If you got here, you either use Windows or some other system where no prebuilt versions of LLVM exist. Because the
original LLVM documentation is way too complicated to go through if you are inexperienced, here a short guide on how to
build LLVM by yourself.

The prerequisites are the same as for the NewJS compiler, so take a look at
the [main documentation](../README.md#building-newjs) if you have not done so far.

**IMPORTANT**: If you are using Windows with MSVC, you most likely want to run all of this in a Visual Studio Developer
Powershell,which adds the benefit of having all tools you need for building in your shell path. This also includes the
standard library path.

First thing you do is to check out the official GitHub repository for the llvm-project. For that, open a terminal, go to
the directory where you want to clone the project to (choose a short path without spaces to minimize problems like
Windows max-path-length or something like that). When you are happy with where you are, execute this command to clone
the repository:

```shell
git clone --depth 1 --branch release/18.x https://github.com/llvm/llvm-project.git
```

Next step is to enter the directory:

```shell
cd llvm-project
```

And configure the CMake project. This is where the command will differ depending on what system, generator or other
configuration you use. For example, this is what I used on Windows with Ninja and a MinGW toolchain:

```shell
cmake -S llvm -B build -G Ninja '-DCMAKE_INSTALL_PREFIX=C:\Program Files (x86)\LLVM' -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DLLVM_PARALLEL_COMPILE_JOBS=10 -DLLVM_PARALLEL_LINK_JOBS=2 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20 -DLLVM_TARGETS_TO_BUILD=X86
```

But there are so many possibilities on how to configure a CMake project, so I'll leave the documentation from llvm here
if you are not sure of what parameters to use:

* [LLVM Documentation - Getting Started](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
* [LLVM Documentation - Getting Started with CMake](https://llvm.org/docs/CMake.html)
* [LLVM Documentation - Getting Started with Microsoft Visual Studio](https://llvm.org/docs/GettingStartedVS.html)

**Essentially**, what you really need are the compiler (if CMake does not automatically detect it right away), you want
to set the llvm targets to build to your own platform (e.g. X86, but you can find a list of targets in the llvm
documentation, link above), maybe set the parallel compile and link jobs to speed things up if you have a beefy
computer, and if you want to debug the application later on you want to set the cmake build type to Debug instead of
Release. The installation prefix can be set to any directory in your system, but remember, for some of them you need to
execute the installation process from an elevated process or shell.

When the configuration process is done, you proceed by building the project. To do so, inside the llvm-project directory
run:

```shell
cmake --build build
```

If configured correctly, this process will run through about 2500 steps flawlessly, and you get a finished build of
llvm. Now you can install llvm by running:

```shell
cmake --install build
```

**BUT**: remember to use an elevated shell (sudo or run as administrator) if you want to install to a (for a normal
user) write protected directory.

Last thing to do is to add the binary installation directory to your path if not yet done so.

If everything worked, and you open a new shell, and type:

```shell
llvm-config --version
```

You should see some version number starting with '18', like '18.x.x'.

If you had any issues with building llvm, take a look at the official documentation or ask me, I don't know, just open
an issue or something. I'll be gladly extending this quick guide on how to build llvm if you have any suggestions on how
to improve it.

But now back to [building NewJS](../README.md#building-newjs), if you still want to do this to yourself.
