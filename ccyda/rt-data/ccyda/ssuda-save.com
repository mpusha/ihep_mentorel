#!/bin/sh
cp /usr/users/base.dat /usr/usera/voevodin/rt-data/ccyda/ssuda.xxx
cp /usr/users/base3.dat /usr/usera/voevodin/rt-data/ccyda/ssuda-3.xxx
cp /usr/users/ssuda/sys/config.dbm /usr/usera/voevodin/rt-data/ccyda/config.xxx
ls -l /usr/usera/voevodin/rt-data/ccyda/*.xxx
