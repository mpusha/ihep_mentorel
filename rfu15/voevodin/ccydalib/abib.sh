#! /bin/sh
CCE=${CC}gcc
AR=${CC}ar
CC=$CCE
ARCH=arm
$CC  -c adirdbl.c
$CC  -c atrdbl.c
$CC  -c detdbl.c
$CC  -c dirdbl.c
$CC  -c getname.c
$CC  -c opnglb.c
$CC  -c rcldbl.c
$CC  -c rdirdbl.c
$CC  -c reddbl.c
$CC  -c redwri.c
$CC  -c rdwr.c
$CC  -c rlpdbl.c
$CC  -c ropdbl.c
$CC  -c setdbl.c
$CC  -c tdirdbl.c
$CC  -c tupdbl.c
$CC  -c txrdbl.c
$CC  -c inidbl.c
$CC  -c preob.c
$CC  -c gl_glos.c
$CC  -c getname.c
$CC  -c des_show.c
$CC  -c tri.c
$AR r ccydalib.a *.o
$AR t ccydalib.a

