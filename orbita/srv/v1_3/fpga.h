#ifndef FPGA_H
#define FPGA_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>


#define TIMEOUT_ERR 100000
#define LOADFPGA_ERR_MEMMAP 1
#define LOADFPGA_ERR_FNAME 2
#define LOADFPGA_ERR_TIMEOUT 3
#define LOADFPGA_OK  0

#define GPIO_OE 0x134
#define GPIO_DATAIN 0x138
#define GPIO_DATAOUT 0x13c
#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190


#define GPIO0_START_ADDR 0x44e07000
#define GPIO0_END_ADDR   0x44e08000
#define GPIO0_SIZE (GPIO0_END_ADDR - GPIO0_START_ADDR)

#define GPIO1_START_ADDR 0x4804c000
#define GPIO1_END_ADDR   0x4804d000
#define GPIO1_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR)

#define GPIO2_START_ADDR 0x481ac000
#define GPIO2_END_ADDR   0x481ad000
#define GPIO2_SIZE (GPIO2_END_ADDR - GPIO2_START_ADDR)

#define GPIO3_START_ADDR 0x481ae000
#define GPIO3_END_ADDR   0x481af000
#define GPIO3_SIZE (GPIO3_END_ADDR - GPIO3_START_ADDR)

#define PIN_DCLK     (1<<9)    // 1    - DCLK      P1.33 GPIO0_9  HOST  OUT
#define PIN_NSTAT    (1<<8)    //      - nSTATUS   P1.35 GPIO0_8  FPGA  INPUT

#define PIN_CONFDONE (1<<17)   // 3    - CONF_D    P1.34 GPIO2_17 FPGA  INPUT
#define PIN_NCONFIG  (1<<16)   // 5    - nCONFIG   P1.36 GPIO2_16 HOST  OUT
#define PIN_DATA0    (1<<15)   // 7    - Data0     P1.38 GPIO2_15 HOST  OUT
#define PIN_NCE    (1<<14)     //      - nCE       P1.37 GPIO2_14 HOST  OUT
#define PIN_NCS0    (1<<12)    //      - nCS0      P1.39 GPIO2_12 HOST  INPUT
#define PIN_ASD    (1<<13)     //      - ASD       P1.40 GPIO2_13 HOST  INPUT


#define GPIO_SETDIRECTIONOUT(gpio,value) *(u_int32_t *)(gpio+GPIO_OE) &=~((u_int32_t)value) //1 - out
#define GPIO_SETDIRECTIONIN(gpio,value) *(u_int32_t *)(gpio+GPIO_OE) |=((u_int32_t)value) //1 - in
#define GPIO_SETBIT32(gpio,value) *(u_int32_t *)(gpio+GPIO_SETDATAOUT)=(u_int32_t)value
#define GPIO_CLEARBIT32(gpio,value) *(u_int32_t *)(gpio+GPIO_CLEARDATAOUT)=(u_int32_t)value
#define GPIO_WRITEDATA32(gpio,value) *(u_int32_t*)(gpio+GPIO_DATAOUT)=(u_int32_t)value
#define GPIO_READDATA32(gpio) *(u_int32_t*)(gpio+GPIO_DATAIN)
#define WRITEREG32(reg,value) *(u_int32_t *)(reg)=(u_int32_t)value
#define READREG32(reg) *(u_int32_t*)(reg)

#endif
