# NTURT CAN Config

## Introduction

This repository includes the can database(dbc) format used in nturt and c code generator that generate code from dbc files.

### CAN database format

CAN database format(or .dbc format) is the de facto standard for can config file documenting how pyhsical numbers can be parsed from a can signal, see [DBC](https://github.com/iDoka/awesome-canbus#dbc) for some explaination and [DBC File Format Documentation](http://mcu.so/Microcontroller/Automotive/DBC_File_Format_Documentation.pdf) for complete documentation.

### Code generator

There are plenty of software support for dbc format, and even dozens of code generators that can generate c code from it, see [cantools](https://cantools.readthedocs.io/en/latest/#the-generate-c-source-subcommand) for possible options. However, none of the options was adopted here, instead [coderdbc](c-coderdbc/README.md) was chosen since it provides functions to receive can signals which uses binary search for the best performance and other utilities such as debugging tools.

## Usage

The usage of coderdbc can be found in the [README.md](c-coderdbc/README.md), but for automation purposes, two shell scripts are provided here.

### fixfile.sh

By default, the code generated from `coderdbc` can not directory be used in a c project as it is not in the standard c file structure with `include` and `src`, instead, the header and source files are scattered in `butl`, `conf`, `inc`, `lib` and `usr` directory. Hence, this shell script converted them back to the normal c file structure.

Usage: fixfile.sh [OPTIONS] \<directory\>

### generate_c_code.sh

This shell script is inteded for automating code generation that can be included in prebuild steps of stm32cube ide or cmake. It generate code to `generated_code` directory and use `fixfile.sh` to make the generated code readily available to be included in a c/c++ project.

Usage: ./generate_c_code.sh [OPTIONS] <can_database_file>
