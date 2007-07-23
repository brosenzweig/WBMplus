UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCC=gcc
export UNIXCCOPS=-Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXCC=gcc
export UNIXCCOPS=-Wall -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-lm
export UNIXMAKE=make
endif
