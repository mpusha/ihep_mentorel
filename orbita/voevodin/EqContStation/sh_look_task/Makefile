CC=$(CROSS_COMPILE)gcc
objects=  shMemLook.o ../sh_ccydalib/sh_ccydalib.o -lrt
shMemLook:  $(objects)
	${CC} -o shMemLook  $(objects) /usr/usera/voevodin/rt-data/ccydalib/ccydalib.a
shMemLook.o: ../sh_ccydalib/sh_ccydalib.h /usr/usera/voevodin/rt-data/ccydalib/ccydalib.h

