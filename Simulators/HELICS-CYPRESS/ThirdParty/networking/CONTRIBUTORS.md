# Contributors

This file describes the contributors to the Utilities library and the software used as part of this project It is part of the GMLC-TDC project and used in the HELICS library. HELICS is a joint project between PNNL, LLNL, and NREL, with contributions from many other national Labs
If you would like to contribute to the Utilities or HELICS project see [CONTRIBUTING](CONTRIBUTING.md)

## Individual contributors

### Pacific Northwest National Lab

### Lawrence Livermore National Lab

- Ryan Mast
- Philip Top

### National Renewable Energy Lab

- Dheepak Krishnamurthy

### Argonne National Lab

## Used Libraries or Code

### [HELICS](https://github.com/GMLC-TDC/HELICS)

Some of the original code for this library was pulled from use inside HELICS. It was pulled out as the networking libraries are not core to HELICS and it was useful as a standalone library so it could have better testing and separation of concerns. HELICS now uses the networking library instead of maintaining its own code. HELICS is released with a BSD-3-Clause license.

### [googleTest](https://github.com/google/googletest)

The tests are written to use google test and mock frameworks and is pulled in as a subproject via git clone or CMake FetchContent. Googletest is released with a BSD-3-clause licensed

### CMake scripts

Several cmake scripts came from other sources and were either used or modified for use in HELICS.

- Lars Bilke [CodeCoverage.cmake](https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake)
- CLI11 [CLI11](https://github.com/CLIUtils/CLI11) while CLI11 was not used directly many of the CI scripts and structure were borrowed to set up the CI builds.
