UNIX=$(shell uname)

export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-pthread -lm
export UNIXMAKE=make
