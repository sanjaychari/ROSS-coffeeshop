LIBDIR  := ../../ross
CC	:= $(shell $(LIBDIR)/ross-config --cc)
CFLAGS  := -g -Wall -I.
OPTIONS := $(shell $(LIBDIR)/ross-config --cflags)
LDFLAGS := $(shell $(LIBDIR)/ross-config --ldflags)

all: coffeeshop
clean:
	rm -f coffeeshop
	rm -f *.o *~ core*


coffeeshop: coffeeshop.c $(LIBDIR)/libROSS.a $(LIBDIR)/ross-config
	$(CC) $(CFLAGS) $(OPTIONS) coffeeshop.c -o coffeeshop $(LDFLAGS)
