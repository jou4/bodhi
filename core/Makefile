CC			= gcc
CFLAGS		= -O3 -Wall -I/usr/local/include -I../include
DFLAGS		= -g -Wall -I/usr/local/include -I../include
DEST		=
LDFLAGS		=
LIBS		=
OBJS		= core.o primitives.o value.o gc.o
PROGRAM		= libbdcore.a
AR			= ar


all:		$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(AR) rv $(PROGRAM) $(OBJS)

debug:	CFLAGS = $(DFLAGS)
debug:	$(OBJS)
	$(AR) rv $(PROGRAM) $(OBJS)

test:	CFLAGS = $(DFLAGS)
test:	$(OBJS) test.o
	$(CC) $(OBJS) test.o ../lib/libbdutil.a

clean:
	rm -f *.o *~ $(PROGRAM)
