#!/bin/sh

export TEST=0;
RED='\033[0;31m'
GREEN='\033[0.32m'

message()
{
    echo "$0Test $(TEST)"
}

compare()
{
    if [ $(./myfind $0) = $(find $0) ]; then
        message $GREEN
    else
        message $RED
    fi
    ((TEST++))
}

compare '.'
compare 'foo'
compare '/'

