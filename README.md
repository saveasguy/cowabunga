# Kaleidoc
## About
Kaleidoc is a compiler based on the compiler from LLVM Kaledoscope tutorial (https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html). Language behind this compiler differs from the one described in tutorial.

## Getting Started
To get started, see [Requirements](#requirements) and [Installation](#installation). See [Usage](#usage) for an example.

## Requirements
The following required to use "Kaleidoc" compiler:
* Git
* Any C++ compiler (Clang, GCC, etc)
* CMake 3.0

## Installation
This describes the installation process using **cmake**.    
```
# Clone repository:
git clone https://gitlab.com/al-romanov/kaleidoc.git
# Go to the project root directory:
cd kaleidoc
# Generate build files:
cmake -S . -B build
# Build the project:
cmake --build build
```

## Usage
To play with compiler. *main.ks* file is provided.   
To run compiler on *main.ks*, execute following command:
```
# Go to project root directory
./build/kaleidoc main.ks
```
