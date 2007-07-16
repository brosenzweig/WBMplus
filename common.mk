UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export MATHLIB=-lm
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-Wall -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export MATHLIB=-lm
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-Wall -fsigned-char -D_GNU_SOURCE
export UNIXMAKE=make
endif
