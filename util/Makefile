CC			= gcc
CFLAGS		= -O3 -Wall -I/usr/local/include
DFLAGS		= -g -Wall -I/usr/local/include
DEST		=
LDFLAGS		=
LIBS		=
OBJS		= exception.o list.o map.o mem.o set.o string_buffer.o treemap.o vector.o
PROGRAM		= libbdutil.a
AR			= ar


all:		$(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(AR) rv $(PROGRAM) $(OBJS)

debug:	CFLAGS = $(DFLAGS)
debug:	$(OBJS)
	$(AR) rv $(PROGRAM) $(OBJS)

clean:
	rm -f *.o *~ $(PROGRAM)
