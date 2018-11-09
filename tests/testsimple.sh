#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
END='\e[0m'

# Check that output is equivalent to that of the real find.

T1=$(mktemp)
T2=$(mktemp)
VAL='valgrind --leak-check=full --show-leak-kinds=all'

mkdir -p to_del/bonjour/salut
touch to_del/fichier
touch to_del/bonjour/hey.txt
mv to_del/ files/
while read args ; do
  $VAL $FIND $args | sort > $T1
  find $args | sort > $T2

  if diff $T1 $T2 ; then
    echo "$GREEN$args$END"
  else
    echo "$RED$args$END"
    exit 1
  fi

done <<EOF
doesnotexist
files
files -type d
files -name a
files -print
files -exec echo {} ;
files -execdir echo {} ;
files -exec ls ;
files -execdir ls ;
files -perm 755
files -perm /755
files -perm -5
files -user jonathan
files -user duncan
files -group jonathan
files -group duncan
files -newer ./testsimple.sh
files -true
files/to_del -delete
EOF

rm $T1 $T2
