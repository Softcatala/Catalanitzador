# Overview of the system

The whole system is a client / server application composed of the following components:

- The Windows client application that executes in the user, performs selected actions, and sends the results to a central server
- A server that process the information sent in XML and stores the data in the database backend
- Any user using a web browser can see the statistics of the systems that are generated by the server processing the data in the SQL server

# Technical design decisions

These are some design decisions:

- A single binary for all architectures (32 and 64 bits)
- The client is written in C++ since the application need to run in all Windows operating systems starting from Windows XP
- The ability to use unit tests was one of the early principles that drove several architectural decisions
- The application should be easily extensible by third parties. Just by implementing a new class the whole system (user interface, serialization, etc) should support a new action

# Windows client architecture

# Common patterns in code

## Dependency injection

Many classes use [dependency injection](http://en.wikipedia.org/wiki/Dependency_injection) on their constructors like in the following example:

```cpp
ConfigureDefaultLanguageAction::ConfigureDefaultLanguageAction(IOSVersion* OSVersion, IRegistry* registry, IRunner* runner)
{
    ....
}
```

The objective is to make more easy to unit test classes using [mock objects](http://en.wikipedia.org/wiki/Mock_object).

## Protected members and unit test

In C++ classes (except for friend classes) can only access other classes public methods. Sometimes we need to be able to unit test methods that we do not want to part of the public API. The pattern using in this case is to make them protected:

```cpp
class _APICALL FirefoxAction : public Action
{
public:
    ...
    
protected:
    virtual void _getProfileRootDir(wstring &location);
    bool _readVersionAndLocale();
```

To be able to change their visibility in child classes consumed by the unit tests.

```cpp
class FirefoxActionForTest : public FirefoxAction
{
    ...
public: 
    using FirefoxAction::_readVersionAndLocale;
```

## CatalanitzadorPerAlWindows.lib

The CatalanitzadorPerAlWindows.lib is a library that contains all the classes to which the unit testing application links to be able to exercise the classes to unit test.

The applications classes are compiled using *dllimport* when creating the library, this achieved by the _APICALL Macro:

```cpp
#ifdef CATALANITZADOR_CREATE_SHARED_LIBRARY
#define _APICALL __declspec(dllexport)
#else // Using shared lib
#define _APICALL __declspec(dllimport)
#endif
```