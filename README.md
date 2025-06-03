# awCore - (andriiwho Core library)

![C++](https://img.shields.io/badge/C%2B%2B-23-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)

Welcome to the awCore repository. This repository contains a C++23 library, which is a collection of core stuff from most of my 
previous projects until today.

Feel free to use this library as you wish. I hope you'll find it useful.
From now on, I'll be using this library to build my projects instead of reinventing the wheel 😁.

## ✨ Features
### 🧠 Memory Management
- 📦 Paged Memory Pool
- 🎯 Smart Allocators:
    - DefaultAllocator with PagedMemoryPool
    - InlineAllocator with stack buffer
    - StaticAllocator for stack-only allocation

### 🧵 Threading
- ThreadPool for parallel task execution
- TaskGraph for dependency-based scheduling
- ThreadWorker for background processing

### 🔢 Math
- Vector operations (2D, 3D, 4D)
- Matrix operations
- Constexpr calculations
- Not yet SIMD 👎

### 🍕 Goodies
- ```defer``` functionality for C++
- container aliases for using PagedMemoryPool

## 📋 Requirements
- C++23 compatible compiler
- CMake 3.30+
- Your favorite IDE 😎


## 🚀 Quick Start

To build just clone this repository (or add it as a submodule) and do ```add_subdirectory(awCore)``` in your CMakeLists.txt.

To use it you then can include the 'all.h' or separate headers as you wish.
```C++
    #include <aw/core/all.h>    // This will include all of the headers. (You can add it to your pch to build it faster)
```
or
```C++
    #include <aw/core/async/thread_pool.h> // If you want to use ony the memory pool class.
```

If you don't want to build and run tests, run this command in your CMakeLists before adding it as a subdirectory:
```cmake
    set(AWCORE_BUILD_TESTS OFF CACHE BOOL "")
```
This way tests won't be built and won't clutter your workspace.

## 🤝 Contributing
Feel free to:
- 🐛 Report bugs
- 💡 Suggest features
- 🔧 Send pull requests

## 📬 Contact
Got questions? Feel free to reach out!

## 🤔 FAQ
Q: Why C++23? A: Because we love living on the edge! If you want C++20, it's probably pretty easy to adapt it.

Q: Can I use this in my project? A: Absolutely! That's what it's here for!

## 📄 License
This project is licensed under the MIT License - see the LICENSE file for details
