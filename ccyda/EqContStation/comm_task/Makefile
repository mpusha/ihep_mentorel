CC=$(CROSS_COMPILE)gcc
objects=  CommTask.o ../sh_ccydalib/sh_ccydalib.o -lrt
CommTask:  $(objects)
	${CC} -o CommTask  $(objects) ../../rt-data/ccydalib/ccydalib.a
CommTask.o: 
	CommTask.h ../sh_ccydalib/sh_ccydalib.h ../../rt-data/ccydalib/ccydalib.h

