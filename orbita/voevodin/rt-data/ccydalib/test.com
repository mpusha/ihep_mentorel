#! /bin/sh
CCE=${CC}gcc
LD=${CC}ld
CC=$CCE

$CC -o atest test.c ccydalib.a ccydalib.a
