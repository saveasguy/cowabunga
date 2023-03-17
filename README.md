# Cowabunga
## About
**Cowabunga** is a programming language inspired by LLVM Kaleidoscope tutorial (https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html). **cbc** is a compiler for **Cowabunga** language.

## Getting Started
To get started, see [Requirements](#requirements) and [Installation](#installation). See [Usage](#usage) for an example.

## Requirements
The following required to use "Kaleidoc" compiler:
* git
* clang++-15
* cmake 3.13+

## Installation
This describes the installation process using **cmake**.    
```
# Clone repository:
git clone https://github.com/al-romanov/cowabunga.git
# Go to the project root directory:
cd cowabunga
# Generate build files:
cmake -S . -B build
# Build the project:
cmake --build build
```

## Usage
To play with compiler, *main.cb* file is provided.   
To run compiler on *main.cb*, execute following command:
```
# Go to project root directory
./build/cbc/cbc main.cb
```
