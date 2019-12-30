/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "spi.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
int rep=1;
void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *deviceSPI;
static uint8_t mode;
static uint8_t bits = 24;
static uint32_t speed = 1000000;
static uint16_t delay;

void transfer(int fd,u_int32_t *dataWr,u_int32_t *dataRd,int bytescount)
{
	int ret;

    //u_int32_t *tx =dataWr;

    //u_int32_t *rx=dataRd; // = {0, };
	struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)dataWr,
        .rx_buf = (unsigned long)dataRd,
        .len = bytescount,
//        .speed_hz = speed,
//        .delay_usecs = 300,
        .bits_per_word = 24,
//        .cs_change = 2,
	};

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1) pabort("can't send spi message");
}


int setSpi(void)
{
 /* "  -D --device   device to use (default /dev/spidev1.1)\n"
    "  -s --speed    max speed (Hz)\n"
    "  -d --delay    delay (usec)\n"
    "  -b --bpw      bits per word \n"
    "  -l --loop     loopback\n"
    "  -H --cpha     clock phase\n"
    "  -O --cpol     clock polarity\n"
    "  -L --lsb      least significant bit first\n"
    "  -C --cs-high  chip select active high\n"
    "  -3 --3wire    SI/SO signals shared\n"
 */
  int ret = 0;
  int fd;

  deviceSPI =  "/dev/spidev1.0";
  speed = 2000000; // MHz
  delay = 0; // delay in us
  bits = 24;
  mode=0;
// uncomment if must be set
//mode |= SPI_LOOP;
//mode |= SPI_CPHA;
//mode |= SPI_CPOL;
//mode |= SPI_LSB_FIRST;
//mode |= SPI_CS_HIGH;
//mode |= SPI_3WIRE;
//mode |= SPI_NO_CS;
//mode |= SPI_READY;


  fd = open(deviceSPI, O_RDWR);
  if (fd < 0)
    pabort("can't open device");

/*
 * spi mode
 */
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
    pabort("can't set spi mode");

  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret == -1)
    pabort("can't get spi mode");

/*
	 * bits per word
*/
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't set bits per word");

  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't get bits per word");

/*
 * max speed hz
*/
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't set max speed hz");

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't get max speed hz");

  if(ret<0) return ret;
  else
    return(fd);
}
void closeSpi(int fd)
{
  close(fd);
}
