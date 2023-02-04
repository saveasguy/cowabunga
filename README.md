# Kaleidoscope compiler
## Table of contents

[TOC]

## About
Kaleidoscope is a programming language from LLVM tutorial (https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html). This repository contains an implementatation of Kaleidoscope compiler.

## Building
This describes the installation process using cmake. As pre-requisites, you'll need git and cmake installed.   
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
