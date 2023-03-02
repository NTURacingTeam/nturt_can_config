#!/bin/bash

# declare text styles
COLOR_REST='\e[0m'
HIGHLIGHT='\e[0;1m'
REVERSE='\e[0;7m'
COLOR_RED='\e[1;31m'
COLOR_YELLOW='\e[1;93m'

print_usage() {
    echo "Generate c code from can database file from <can_data_base_file>, the generated code will be in \"generated_code\" directory"
    echo ""
    echo "Usage: ./generate_c_code.sh [OPTIONS] <can_database_file>"
    echo "Options:"
    echo "    -f --float      Configure the generated code to use float for physical value instead of double, only relavent when conbined with '-p' flag"
    echo "    -h --help       Print this help message and exit"
    echo "    -p --physical   Configure the generated code to use physical values instead of raw data"
}

# default argument/option
DBC_FILE_WITH_PATH=""
USE_FLOAT=false
USE_PHYSICAL_VALUE=false

PARAM=$(getopt -o fhp -l float,help,physical -n "$0" -- "$@")

if [ $? != 0 ]; then
    print_usage
    exit 1
fi

set -e

eval set -- "${PARAM}"

while true; do
    case "$1" in
        -f|--float)
            USE_FLOAT=true
            shift
            ;;

        -h|--help)
            print_usage
            exit
            shift
            ;;
        
        -p|--physical)
            USE_PHYSICAL_VALUE=true
            shift
            ;;
        --)
            shift
            if [[ -z $* ]]; then
                echo -e "${COLOR_RED}Error:${HIGHLIGHT} no argument given.${COLOR_REST} Accept one argument as can database file"
                print_usage
                exit 1
            else
                while [[ -n $* ]]; do
                    if [[ -n ${DBC_FILE_WITH_PATH} ]]; then
                        echo -e "${COLOR_RED}Error:${HIGHLIGHT} multiple arguments given.${COLOR_REST} Only accept one argument as can database file"
                        print_usage
                        exit 1
                    else
                        DBC_FILE_WITH_PATH=$1
                        shift
                    fi
                done
            fi
            break
            ;;
    esac
done

# get absolute path
ABSOLUTE_PATH=$(dirname $(realpath ${BASH_SOURCE:-$0}))

# build the code generator if it's not already built
if ! [[ -x ${ABSOLUTE_PATH}/c-coderdbc/build/coderdbc ]]; then
    echo "Software for generating c code not built, building..."
    cmake -S ${ABSOLUTE_PATH}/c-coderdbc/src -B ${ABSOLUTE_PATH}/c-coderdbc/build 1>/dev/null
    cmake --build ${ABSOLUTE_PATH}/c-coderdbc/build --config release 1>/dev/null
    echo "Software for generating c code finished building"
fi

# get dbc file base name
DBC_FILE=${DBC_FILE_WITH_PATH##*/}
DBC_FILE_BASENAME=${DBC_FILE%.*}

DBC_FILE_UPPER_CASE=${DBC_FILE_BASENAME^^}

if [[ -d ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME} ]]; then
    if [[ -d ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}.bak ]]; then
        echo -e "${COLOR_YELLOW}Warning:${HIGHLIGHT} backup for \"${DBC_FILE_BASENAME}\" already exist.${COLOR_REST} Removing"
        rm -r ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}.bak
    fi

    echo "Directory \"${DBC_FILE_BASENAME}\" already exits, move to \"${DBC_FILE_BASENAME}.bak\""
    mv ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME} ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}.bak
fi

mkdir -p ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}

# generate code
${ABSOLUTE_PATH}/c-coderdbc/build/coderdbc -rm -dbc ${DBC_FILE_WITH_PATH} -out ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME} \
    -drvname ${DBC_FILE_BASENAME} -nodeutils 1>/dev/null

# fix generated code
${ABSOLUTE_PATH}/fixfile.sh ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME} 1>/dev/null

# configure to use physical values
if [ "${USE_PHYSICAL_VALUE}" = true ]; then
    sed -i "/${DBC_FILE_UPPER_CASE}_USE_SIGFLOAT/c\#define ${DBC_FILE_UPPER_CASE}_USE_SIGFLOAT" \
        ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}/include/${DBC_FILE_BASENAME}-config.h
    if [ "${USE_FLOAT}" = true ]; then
        sed -i "/typedef double sigfloat_t;/c\typedef float sigfloat_t;" \
            ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}/include/dbccodeconf.h
    else
        sed -i "/typedef double sigfloat_t;/c\typedef double sigfloat_t;" \
            ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}/include/dbccodeconf.h
    fi
fi

# configure to store frame timestamp when receive can siganl and monitor frame timeout
sed -i "/${DBC_FILE_UPPER_CASE}_USE_DIAG_MONITORS/c\#define ${DBC_FILE_UPPER_CASE}_USE_DIAG_MONITORS" \
    ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}/include/${DBC_FILE_BASENAME}-config.h
sed -i "/#define GetSystemTick() __get__tick__()/c\#define GetSystemTick() __get__tick__()" \
    ${ABSOLUTE_PATH}/generated_code/${DBC_FILE_BASENAME}/include/dbccodeconf.h

echo "Generate code completed"
