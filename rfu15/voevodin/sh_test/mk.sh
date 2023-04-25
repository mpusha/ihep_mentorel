#!/bin/bash
CC=/opt/gcc-linaro-6.4.1-2017.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
make ARCH=arm CROSS_COMPILE=${CC}
