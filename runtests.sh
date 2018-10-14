#!/bin/sh

export FIND=../myfind

RED='\033[0;31m'
GREEN='\033[0;32m'
END='\e[0m'

cd tests

for TEST in *.sh ; do
  if ./$TEST ; then
    echo "$GREEN$TEST passed$END"
  else
    echo "$RED$TEST failed$END"
    exit 1
  fi
done
