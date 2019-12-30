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
static uint32_t speed = 500000;
static uint16_t delay;

void transfer(int fd,u_int32_t *dataWr,u_int32_t *dataRd)
{
	int ret;
    //printf("Idata=%u %u\n",*data,*(data+1));
    u_int32_t *tx =dataWr;
   /* {
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
		0xF0, 0x0D,
    };*/
    //u_int16_t rx[ARRAY_SIZE(tx)] = {0, };
    u_int32_t *rx=dataRd; // = {0, };
	struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,

        //.len = ARRAY_SIZE(tx),
.len = 1024*4,
//        .speed_hz = speed,
//        .delay_usecs = 300,
        .bits_per_word = 24,
//        .cs_change = 2,
	};

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1) pabort("can't send spi message");

//printf("data=%u %u\n",rx[0],rx[1]);
//	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
//		if (!(ret % 6))
//			puts("");
//		printf("%.2X ", rx[ret]);
//	}
//	puts("");
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
  speed = 1000000; // MHz
  delay = 0; // delay in us
  bits = 24;
  mode=0; // uncomment if must be set
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

//  printf("spi mode: %d\n", mode);
//  printf("bits per word: %d\n", bits);
//  printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

  //close(fd);
  if(ret<0) return ret;
  else
    return(fd);
}
void closeSpi(int fd)
{
  close(fd);
}
