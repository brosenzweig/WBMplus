UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-pthread -lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-pthread -lm
export UNIXMAKE=make
endif
ifeq ($(UNIX),SunOS)
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -pthread -fsigned-char -D_GNU_SOURCE
export UNIXLIBS=-pthread -lm
export UNIXMAKE=gmake
endif
