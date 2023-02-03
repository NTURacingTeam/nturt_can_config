#!/bin/bash

# declare text styles
COLOR_REST='\e[0m'
HIGHLIGHT='\e[0;1m'
REVERSE='\e[0;7m'
COLOR_GREEN='\e[0;32m'
COLOR_RED='\e[1;31m'

print_usage() {
    echo "Restruct the file structure of the generated code from <directory> to standard c file structure \"include, src\""
    echo ""
    echo "Usage: ./fixfile.sh [OPTIONS] <directory>"
    echo "Options:"
    echo "    -h --help            Print this help message and exit"
}

# default argument
DIRECTORY=""
SUBDIRECTORY=("butl" "conf" "inc" "lib" "usr")

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
                echo -e "${COLOR_RED}Error:${HIGHLIGHT} no argument given.${COLOR_REST} Accept one argument as directory"
                print_usage
                exit 1
            else
                while [[ -n $* ]]; do
                    if [[ -n ${DIRECTORY} ]]; then
                        echo -e "${COLOR_RED}Error:${HIGHLIGHT} multiple arguments given.${COLOR_REST} Only accept one argument as directory"
                        print_usage
                        exit 1
                    else
                        DIRECTORY=$1
                        shift
                    fi
                done
            fi
            break
            ;;
    esac
done

if ! [[ -d ${DIRECTORY}/include ]]; then
    mkdir ${DIRECTORY}/include
fi

if ! [[ -d ${DIRECTORY}/src ]]; then
    mkdir ${DIRECTORY}/src
fi

# move header files to "include", source file to "src"
for subdirectory in ${SUBDIRECTORY[@]}; do
    if compgen -G ${DIRECTORY}/${subdirectory}/*.h > /dev/null; then
        mv ${DIRECTORY}/${subdirectory}/*.h ${DIRECTORY}/include
    fi
    if compgen -G ${DIRECTORY}/${subdirectory}/*.c > /dev/null; then
        mv ${DIRECTORY}/${subdirectory}/*.c ${DIRECTORY}/src
    fi

    rm -r ${DIRECTORY}/${subdirectory}
done

rm -r ${DIRECTORY}/000

echo "Restructed the generated file completed"
