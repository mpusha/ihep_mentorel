cc  -o dsc dsc.c dscstd.o incdsc.o rlprint.o indl.o datrw.o extend.o nam.o numnam.o ioshr.o custom.o tables.o for_ssuda.o

