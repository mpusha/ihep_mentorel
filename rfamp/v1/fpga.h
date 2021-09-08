

#include <stdio.h>
#include <stdlib.h>
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

#define GPIO2_START_ADDR 0x481ac000
#define GPIO2_END_ADDR   0x481ad000
#define GPIO2_SIZE (GPIO2_END_ADDR - GPIO2_START_ADDR)
#define GPIO3_START_ADDR 0x481ae000
#define GPIO3_END_ADDR   0x481af000
#define GPIO3_SIZE (GPIO3_END_ADDR - GPIO3_START_ADDR)

#define PIN_DCLK     (1<<12)   // 1    - DCLK      P1.39 GPIO2_12 HOST
#define PIN_CONFDONE (1<<13)   // 3    - CONF_DONE P1.40 GPIO2_13 FPGA
#define PIN_NCONFIG  (1<<11)   // 5    - nCONFIG   P1.42 GPIO2_11 HOST
#define PIN_DATA0    (1<<9)    // 7    - Data0     P1.44 GPIO2_9  HOST
#define PIN_NSTAT    (1<<17)   //      - nSTATUS   P2.28 GPIO3_17 FPGA


#define GPIO_SETDIRECTION(gpio,value) *(u_int32_t *)(gpio+GPIO_OE) &=~((u_int32_t)value) //1 - out
#define GPIO_SETBIT32(gpio,value) *(u_int32_t *)(gpio+GPIO_SETDATAOUT)=(u_int32_t)value
#define GPIO_CLEARBIT32(gpio,value) *(u_int32_t *)(gpio+GPIO_CLEARDATAOUT)=(u_int32_t)value
#define GPIO_WRITEDATA32(gpio,value) *(u_int32_t*)(gpio+GPIO_DATAOUT)=(u_int32_t)value
#define GPIO_READDATA32(gpio) *(u_int32_t*)(gpio+GPIO_DATAIN)
#define WRITEREG32(reg,value) *(u_int32_t *)(reg)=(u_int32_t)value
#define READREG32(reg) *(u_int32_t*)(reg)


