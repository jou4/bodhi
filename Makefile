CC			= gcc
CFLAGS		= -O3 -Wall -I/usr/local/include
DEST		=
LDFLAGS		=
LIBS		=
OBJS		= exception.o list.o map.o mem.o set.o string_buffer.o treemap.o vector.o
PROGRAM		= bodhi
BIN			= ./bin


all:
	$(MAKE) dir
	cd util; $(MAKE) all
	cp util/libutil.a lib/libutil.a
	cd src; $(MAKE) all
	cp src/a.out bin/bodhi
	cd core; $(MAKE) all
	cp core/libcore.a lib/libcore.a

dir:
	mkdir -p lib
	mkdir -p bin

clean:
	cd util; $(MAKE) clean
	rm lib/libutil.a
	cd src; $(MAKE) clean
	rm bin/bodhi
	cd core; $(MAKE) clean
	rm lib/libcore.a