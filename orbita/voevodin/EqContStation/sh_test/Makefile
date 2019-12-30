CC=$(CROSS_COMPILE)gcc
objects=  sh_test.o ../sh_ccydalib/sh_ccydalib.o -lrt
sh_test:  $(objects)
	${CC} -o sh_test  $(objects) /usr/usera/voevodin/rt-data/ccydalib/ccydalib.a
sh_test.o: ../sh_ccydalib/sh_ccydalib.h 

