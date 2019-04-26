\mainpage
[TOC]
# Deterministic finite automata learning library
The `libgsjj` is an open-source framework for learning deterministic finite automata. This project was done by Jonathan Joertz and Gaëtan Staquet (we named this library after our initials in our humility) for our Master's project at the University of Mons (UMONS).

This README uses Doxygen's features. Please see the [online documentation](https://libgsjj.github.io/libgsjj/index.html) for a easier to read README.

## Learning deterministic finite automata
This library focuses on passive learning algorithms. In other words, the learning task is the following:
  1. We have a sample \f$S = (S_+, S_-)\f$ over an alphabet \f$\sigma\f$ where \f$S_+\f$ is the finite set of words the DFA must accept and \f$S_-\f$ is the finite set of words the DFA must reject.
  2. We must build the smallest DFA consistent with \f$S\f$. A DFA \f$A\f$ is consistent with \f$S\f$ if \f$S_+ \subseteq L(A) \land S_- \cap L(A) = \emptyset\f$.

To build such a DFA, we use what we call passive learning algorithms. The following algorithms are implemented in `libgsjj`:
  - Biermann and Feldman's algorithm
  - Grinchtein, Leucker and Piterman's unary and binary algorithms
  - Heule and Verwer's algorithm
  - Neider and Jansen's algorithm

## How to use libgsjj
Please note that the benchmarks will only be correct under Linux because the way the CPU time for a thread is retrieved is only working under that family of OS. The Linux kernel's version must be at least 2.6.26.

### Dependencies to install
To be able to compile and use our library, you will need the install the following dependencies (we also give the Debian and Ubuntu package's name in parenthesis):
  - GMP (libgmp-dev)
  - CVC4 (libcvc4-dev)
  - zlib (zlib1g-dev)
  - Boost (libboost-all-dev)
  - Bison (bison)
  - Flex (flex)
  - PkgConfig (pkg-config or pkgconfig), optional
  - Doxygen and dot (doxygen and graphviz), optional, to build to documentation

The other dependencies are given in this repository.

### How to compile
When you are in the root directory of this library:
```bash
mkdir build && cd build && cmake .. && make
```

This will build the library. If you want to install it, use `make install`. You can change the installation folder with `cmake -DCMAKE_INSTALL_PREFIX=your/path ..` (or use a graphical interface for CMake).

To enable building the benchmarks, instead of `cmake ..` use `cmake -DBUILD_BENCHMARKS=TRUE ..`.

To enable building the tests, instead of `cmake ..` use `cmake -DBUILD_TESTS=TRUE ..`. You can of course enable the benchmarks and the tests at the same time with `cmake -DBUILD_BENCHMARKS=TRUE -DBUILD_TESTS=TRUE ..`

### How to use in a project
If you use CMake, it's easy. The building stage described before also creates a CMake Config file. Therefore, in your `CMakeLists.txt`, you only have to add the line
```cmake
find_library(gsjj)
```
and `gsjj` will be available a target you can link against. It's possible the user must set the `gsjj_DIR` CMake parameter to be able to build your program if the files are not installed in the standard location.

To construct a method, please see the gsjj::passive::constructMethod functions. Here is the complete list of built-in methods with the names used in the program:
  * Biermann and Feldman: `biermann`
  * Neider and Jansen: `neider`
  * Grinchtein, Leucker and Piterman unary:
    * CNF: `unary`
    * Non-CNF: `unaryNonCNF`
  * Grinchtein, Leucker and Piterman binary:
    * CNF: `binary`
    * Non-CNF: `binaryNonCNF`
  * Heule and Verwer:
    * CNF: `heule`
    * Non-CNF: `heuleNonCNF`

#### How to add a new method
Let's say we want to create a new method called `TestMethod`. It's really simple. We just have to create a new class, makes it inheriting gsjj::passive::Method and gsjj::passive::RegisterInFactory<TestMethod>. Here is a very simple (and stupid) example:
```cpp
class TestMethod :
  public gsjj::passive::Method,
  public gsjj::passive::RegisterInFactory<TestMethod> {
  public:
      bool solve() override {
        // Every method inheriting RegisterInFactory must have a line using s_registered
        s_registered = s_registered;
        return true;
      }
      std::unique_ptr<gsjj::DFA<char>> constructDFA() override {
        return nullptr;
      }
      void setStopTrigger(const std::chrono::seconds &timeLimit, std::atomic_bool &stopTrigger, const bool *stopPointer) override {
        // If the method relies on CVC4, use timeLimit to set the engine's timeLimit
        // If the method relies on Maple, use stopTrigger
        // If the method relies on Limboole, use stopPointer
      }
      static std::string getFactoryName() {
        return "test";
      }

  protected:
      TestMethod(
        const std::set<std::string> &SpSet,
        const std::set<std::string> &SmSet,
        const std::set<std::string> &SSet,
        const std::set<std::string> &prefixesSet,
        const std::set<char> &alphabetSet,
        unsigned int n) :
          Method(SpSet, SmSet, SSet, prefixesSet, alphabetSet, n) {}
};
```

After that, creating a new instance of this method is as easy as for the other methods:
```cpp
std::set<std::string> Sp = {...}, Sm = {...};
auto method = gsjj::passive::constructMethod("test", Sp, Sm);
```

### Unit tests
Once build, you can launch the unit tests by starting the program `tests` (built in the subfolder `tests` in `build`).

### Benchmarks
Once build, you can launch the benchmarks by starting the program `benchmarks` (built in the subfolder `benchmarks` in `build`).

#### User manual
The `benchmarks` program can be used to benchmark a method on a sample or to simply execute a method.

To execute a method (let's say the Heule and Verwer method) on a randomly generated sample (with default parameters) and get the optimal DFA in a DOT format, we write:
```bash
./benchmarks --to-dot heule
```
By default, the output file is `out.dot`. This can be changed with the option `--output-file`.

To execute a method on an input-file describing a sample, we write:
```bash
./benchmarks --to-dot --input-file /path/to/file heule
```

To execute a method on an input-file describing a loop-free DFA, we write:
```bash
./benchmarks --to-dot --input-file /path/to/file --loop-free heule
```

We can also ask the program to execute a method on a fixed DFA size. Let's say we want to use the binary method, use a randomly generated sample and test if there exists a consistent DFA of size 3:
```bash
./benchmarks --to-dot binary 3
```

To benchmark a method on a sample, we write:
```bash
./benchmarks --benchmarks --input-file /path/to/file biermann
```
The program will print the time taken by the method and return 0 if and only if the method could find the optimal DFA.

You can get the complete list of options with
```bash
./benchmarks --help
```

## License
Due to bcsat and the modifications done on this dependency, our library is licensed under the GNU GPL v2.

CVC4 is distributed both under BSD 3-Clause and GNU GPL v3. If you want to distribute binaries based on libgsjj, be sure to use a BSD 3-Clause distribution for CVC4 since GNU GPL v2 and v3 are incompatible.

### Changes made on bcsat
We give here the modifications done on bcsat. The base files can be found at https://github.com/scrippie/bcsat.

  * On 6 March 2019:
    * Modification of `bcminisat220_solve.cc` to remove log messages.
    * Removing folders `minisat-2.2.0` et `zchaff.2008.10.12`.
  * On 10 March 2019:
    * Modification of `bc.hh`, `bc.cc` and `bcminisat220_solve.cc` to allow the functions to stop when the time limit is reached (the information is propagated through a `std::atomic_bool` object).
  * On 25 March 2019:
    * Modification of `bcminisat220_solve.c`, `lexer.lex`, `lexer11.lex`, `parser.y` and `parser11.y` to change the include paths