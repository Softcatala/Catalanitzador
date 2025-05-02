
# Introduction

A Microsoft Windows & Mac OS program that makes your system Catalan language friendly by installing language packages and changing configuration settings.

**Project:** [Catalanitzador de Softcatalà](https://www.softcatala.org/projectes/catalanitzador-de-softcatala/)

---

# Development

## Windows Requirements

- Microsoft Visual Studio 2010 Express Edition
- Google Test 1.6 ([Google Test](https://code.google.com/p/googletest/))
- Google Mocks 1.6 ([Google Mocks](https://code.google.com/p/googlemock/))

Download and unzip Google Mocks 1.6 at `libs/gmock` and Google Test 1.6 at `libs/gtest`.

Before building the libraries, follow these steps for all projects within the gmock solution:

1. Go to project properties → **Configuration Properties** → **C++ Code Generation**
2. Select **Multithreaded Debug Dll**

For gtest, this should be the default configuration. If it isn't, repeat the same steps.

To build gtest, use the `gtest-md.sln` solution.

## Mac Requirements

- Code X 

# Git Configuration for Contributing

Git should be set up to use Unix line endings:

```bash
$ git config --global core.autocrlf input
```

# Project technical documentation

All technical documentation is in the [docs](docs/) directory
---

# Contact Information

Jesús Corrius [jesus@softcatala.org](mailto:jesus@softcatala.org)