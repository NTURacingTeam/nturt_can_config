# NTURT CAN Config

## Introduction

This repository includes the can database(dbc) format used in nturt and c code generator that generate code from dbc files.

### CAN database format

CAN database format(or .dbc format) is the de facto standard for can config file documenting how pyhsical numbers can be parsed from a can signal, see [DBC](https://github.com/iDoka/awesome-canbus#dbc) for some explaination and [DBC File Format Documentation](http://mcu.so/Microcontroller/Automotive/DBC_File_Format_Documentation.pdf) for complete documentation.

### Code generator

There are plenty of software support for dbc format, and even dozens of code generators that can generate c code from it, see [cantools](https://cantools.readthedocs.io/en/latest/#the-generate-c-source-subcommand) for possible options. However, none of the options was adopted here, instead [coderdbc](https://github.com/astand/c-coderdbc/blob/39611296382a3a222fa80c2b0bd95871d2c39701/README.md) was chosen since it provides functions to receive can signals which uses binary search for the best performance and other utilities such as debugging tools.

## Usage

The usage of coderdbc can be found in the [README.md](https://github.com/astand/c-coderdbc/blob/39611296382a3a222fa80c2b0bd95871d2c39701/README.md), but for automation purposes, two shell scripts are provided here.

### fixfile.sh

By default, the code generated from `coderdbc` can not directory be used in a c project as it is not in the standard c file structure with `include` and `src`, instead, the header and source files are scattered in `butl`, `conf`, `inc`, `lib` and `usr` directory. Hence, this shell script converted them back to the normal c file structure.

Usage: 

```shell=
./fixfile.sh [OPTIONS] \<directory\>
```

### generate_c_code.sh

This shell script is inteded for automating code generation that can be included in prebuild steps of stm32cube ide or cmake. It generate code to `generated_code` directory and use `fixfile.sh` to make the generated code readily available to be included in a c/c++ project.

Usage:

```shell=
./generate_c_code.sh [OPTIONS] <can_database_file>
```

### Use in stm32cubeide

The generated code can be used in a stm32cubeide project with the following steps:

1. Clone this repository in your STM32CubeIDE project folder
2. Go to `Project -> Properties -> C/C++ Build -> Settings -> Build Steps Tab` and in `Pre-build steps` tab, add:
   ```shell=
   ${workspace_loc:/${ProjName}}/nturt_can_config/generate_c_code.sh -p ${workspace_loc:/${ProjName}}/nturt_can_config/nturt_can_config.dbc
   ```
3. Go to `Project -> Properties -> C/C++ General -> Paths and Symbols` and in `Includes` tab, add:
   ```shell=
   nturt_can_config/generated_code/nturt_can_config/include
   ```
4. Then in `Source Location` tab, add:
   ```shell=
   nturt_can_config/generated_code/nturt_can_config/src
   ```
5. Build and run your project
