#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <cstdlib>
#include <iostream>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>


#define DAEMON_NAME "VoblerCtrl"
#define RUN_AS_USER "root"
#define LOCKFILE  "/var/lock/" DAEMON_NAME
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#include "hw.h"



TDtBehav *device;





int main(int argc, char *argv[])
{
  try{

  QCoreApplication a(argc,argv);
  device=new TDtBehav("/var/www/html/test/");

}
    catch(...)
    {
      syslog( LOG_ERR, "Global Exeption" );
      closelog();
    }
}

