#!/bin/sh

T1=$(mktemp)
T2=$(mktemp)

$FIND files | sort > $T1
find files | sort > $T2

diff -q $T1 $T2 && rm $T1 $T2
