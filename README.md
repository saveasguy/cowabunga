# Cowabunga
## About
**Cowabunga** is a programming language inspired by LLVM Kaleidoscope tutorial (https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html). **cbc** is a compiler for **Cowabunga** language.

## Getting Started
To get started, see [Requirements](#requirements) and [Installation](#installation). See [Usage](#usage) for an example.

## Requirements
The following required to use "Kaleidoc" compiler:
* llvm-dev, libllvm, llvm
* clang
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
echo "a = 2; b = 1; c = 3; d = add(a, b, c); r = add(sub(a, b), d);" > main.cb
# Run compiler on the created file:
./build/tools/cbc/cbc main.cb
# You will get LLVM IR file and executable
# Run executable:
./main
# Output will be:
# 7
```
### Examples
Few examples are provided in this repository. They are placed in *examples/* directory.
Exaples are built by default.
```
# To run example ParenthesesSequence parser, run the following command:
./build/examples/ParenthesesSequence/ParenthesesSequence
```
```
# To run example PrintTokens, run the following command:
./build/examples/PrintTokens/PrintTokens
```
