#!/bin/bash

# declare text styles
COLOR_REST='\e[0m'
HIGHLIGHT='\e[0;1m'
REVERSE='\e[0;7m'
COLOR_GREEN='\e[0;32m'
COLOR_RED='\e[1;31m'

print_usage() {
    echo "Generate c code from can database file from <can_data_base_file>, the generated code will be in \"generated_code\" directory"
    echo ""
    echo "Usage: ./generate_c_code.sh [OPTIONS] <can_database_file>"
    echo "Options:"
    echo "    -h --help            Print this help message and exit"
}

# default argument
DBC_FILE_WITH_PATH=""

PARAM=$(getopt -o h -l help -n "$0" -- "$@")

if [ $? != 0 ]; then
    print_usage
    exit 1
fi

set -e

eval set -- "${PARAM}"

while true; do
    case "$1" in
        -h|--help)
            print_usage
            exit
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

# build the code generator if it's not already built
if ! [[ -x c-coderdbc/build/coderdbc ]]; then
    echo "Software for generating c code not built, building..."
    cmake -S c-coderdbc/src -B c-coderdbc/build 1>/dev/null
    cmake --build c-coderdbc/build --config release 1>/dev/null
    echo "Software for generating c code finished building"
fi

DBC_FILE_BASENAME=${DBC_FILE_WITH_PATH%.*}

if [[ -d generated_code/${DBC_FILE_BASENAME} ]]; then
    echo "Directory \"${DBC_FILE_BASENAME}\" already exits, move to \"${DBC_FILE_BASENAME}.bak\""
    mv generated_code/${DBC_FILE_BASENAME} generated_code/${DBC_FILE_BASENAME}.bak
fi

mkdir -p generated_code/${DBC_FILE_BASENAME}

# generate code
./c-coderdbc/build/coderdbc -dbc ${DBC_FILE_WITH_PATH} -out generated_code/${DBC_FILE_BASENAME} -drvname ${DBC_FILE_BASENAME} -nodeutils 1>/dev/null

# fix generated code
./fixfile.sh generated_code/${DBC_FILE_BASENAME} 1>/dev/null

echo "Generate code completed"
