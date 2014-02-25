#!/bin/sh
if [ $# -ne 1 ]; then
   echo "Usage:" $0 "Compile|Clean|Load|Unload"
   exit 127
fi
if [ $1 = "Compile" ]; then
   make
elif [ $1 = "Clean" ]; then
   make clean
elif [ $1 = "Load" ]; then
   kldload ./mfi.ko
elif [ $1 = "Unload" ]; then
   kldunload mfi
else
  echo "Invalid Option"
  echo "Usage:" $0 "Compile|Load|Unload"
fi
exit 0