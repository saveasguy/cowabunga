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
```
# Create new file in the project's root directory
echo "a * (c + d) + 32; (3); (a+(c)); (a * ((a) + (c)));" > main.cb
# Run compiler on the created file:
./build/tools/cbc/cbc main.cb
```
### Examples
Few examples are provided in this repository. They are placed in *examples/* directory.
Exaples are built by default.
```
# To run example PrintAST, run the following command:
./build/examples/PrintAST/PrintAST
```
```
# To run example PrintTokens, run the following command:
./build/examples/PrintTokens/PrintTokens
```
