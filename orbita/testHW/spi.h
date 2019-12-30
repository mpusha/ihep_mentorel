#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


#ifdef __cplusplus
extern "C" {
#endif
void pabort(const char *s);
void transfer(int fd, u_int32_t *dataWr, u_int32_t *dataRd, int bytescount);
int setSpi(void);
void closeSpi(int fd);

#ifdef __cplusplus
}
#endif

#endif
