#ifndef API_WD
#define API_WD
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

static int api_watchdog_fd = -1;

int api_watchdog_open(const char * watchdog_device);
int api_watchdog_hwfeed(void);
int api_watchdog_init(const char *pcDevice);
int api_watchdog_gettimeout(int *timeout);
int api_watchdog_gettimeleft(int *timeout);
int api_watchdog_settimeout(int *timeout);

#endif
