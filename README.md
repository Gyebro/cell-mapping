# Cell Mapping library

State-of-the-art implementation of several Cell Mapping methods, for analysing dynamical systems.

**Authors:** Gergely Gyebrószki, Gábor Csernák

Department of Applied Mechanics, Budapest University of Technology and Economics


## Introduction

`CMlib` is a library of cell mapping algorithms and utility functions and classes written in c++. 
Cell mapping methods are suitable for the global analysis of dynamical systems, they can be used to efficiently find state space objects (fixed points, periodic orbits and their corresponding domains of attractions) in a given discretized state space domain. 

The goal of the `CMlib` library is to provide simple and efficient implementations of basic cell mapping algorithms allowing users to quickly utilize them to their problems or to integrate them into other applications independently of the underlying data types or differential equation solvers. To achieve this, `CMlib` mostly provides template base classes for various components, from which application-specific classes can derived.

The author of the `CMlib` library have used cell mapping methods extensively during his PhD studies and currently moving (and rewriting) his private code base to this open-source library.

## Related paper

G. Gyebrószki and G. Csernák, *Clustered Simple Cell Mapping: An extension to the Simple Cell Mapping method*, Communications in Nonlinear Science and Numerical Simulation 42, pp. 607–622, 2017. DOI: [10.1016/j.cnsns.2016.06.020](https://doi.org/10.1016/j.cnsns.2016.06.020).

*Please consider citing this paper if you have found this program useful.*

## Features

- Current version includes Simple Cell Mapping (SCM) and Clustered SCM methods, Generalized Cell Mapping (GCM) will be added.
- Entirely written in \texttt{c++} considering modern language features, with care to code clarity and documentation.
- Contains cross-platform and efficient implementations of cell mapping algorithms.
- Template approach allows replacing certain components (e.g. underlying data types, methods for state space discretization, used solvers) easily and independently from each other.
- Open source (with MIT licence).
- Actively maintained and developed.

## Documentations and Examples

Please see the [docs](https://github.com/Gyebro/cell-mapping/tree/master/docs) folder of this repository.

## Requirements and installation

The `CMlib` library can be used as a stand-alone c++ library built from sources contained in the `cpp/cm` folder of the repository or as a `CMake` library. The only requirement is a modern C++ compiler with `c++11` support. (GCC 4.8.1 or above, on Windows MinGW-w64 is recommended.)

After cloning the repository, the `cpp` folder contains helper scripts for initializing/reloading the `CMake` project and building the demos.

In order to build the demonstrations in Release mode, run:
`reload-project-release.bat|sh`, followed by
`build-demo-release.bat|sh`. 

## Open source licences

`CMlib` uses the open-source `libjpeg` library to generate output jpeg images. For more information and licence, see [jpeg-9a/readme](https://github.com/Gyebro/cell-mapping/tree/master/cpp/libjpeg/jpeg-9a) or [ijg.org](https://www.ijg.org/).