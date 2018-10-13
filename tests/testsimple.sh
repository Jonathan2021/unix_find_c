#!/bin/sh

# Check that output is equivalent to that of the real find.

T1=$(mktemp)
T2=$(mktemp)

while read args ; do
  $FIND $args | sort > $T1
  find $args | sort > $T2

  diff $T1 $T2 || exit 1
done <<EOF
files
files -type d
files -name a
files -print
files -exec echo hello ;
files -execdir echo bye ;
files -perm 755
files -perm +755
files -perm -5
files -user jonathan
files -user duncan
files -group jonathan
files -group duncan
files -newer ./testsimple.sh
files -true
EOF

rm $T1 $T2
