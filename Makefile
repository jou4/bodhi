CC			= gcc
CFLAGS		= -O3 -Wall -I/usr/local/include
DFLAGS		= -g -Wall -I/usr/local/include
DEST		=
LDFLAGS		=
LIBS		=
OBJS		= exception.o list.o map.o mem.o set.o string_buffer.o treemap.o vector.o
PROGRAM		= bodhi
BIN			= ./bin


all:
	$(MAKE) dir
	cd util; $(MAKE) all
	cp util/libbdutil.a lib/libbdutil.a
	cd src; $(MAKE) all
	cp src/a.out bin/bodhi
	cd core; $(MAKE) all
	cp core/libbdcore.a lib/libbdcore.a

debug:
	$(MAKE) dir
	cd util; $(MAKE) debug
	cp util/libbdutil.a lib/libbdutil.a
	cd src; $(MAKE) debug
	cp src/a.out bin/bodhi
	cd core; $(MAKE) debug
	cp core/libbdcore.a lib/libbdcore.a

dir:
	mkdir -p lib
	mkdir -p bin

clean:
	cd util; $(MAKE) clean
	cd src; $(MAKE) clean
	cd core; $(MAKE) clean
	rm -f lib/*
	rm -f bin/*
	rmdir lib
	rmdir bin

runtest:
	bin/bodhi test/hello.bd
	./a.out
	bin/bodhi test/char.bd
	./a.out
	bin/bodhi test/fib.bd
	./a.out
	bin/bodhi test/args.bd
	./a.out aa bb cc dd ee
	bin/bodhi test/fold.bd
	./a.out
	bin/bodhi test/ref.bd
	./a.out
	bin/bodhi test/array.bd
	./a.out
	bin/bodhi test/closure.bd
	./a.out
	bin/bodhi test/file.bd
	./a.out
	bin/bodhi test/tuple.bd
	./a.out
	bin/bodhi test/float.bd
	./a.out

