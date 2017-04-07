TARGET	    = mycc
CC	    = gcc
CFLAGS	    = -Wall -g
INCLUDES    = -I./inc -I../logger/inc
SRCS	    = $(wildcard src/*.c) \
	      $(wildcard tst/*.c) \
	      $(wildcard ../logger/src/*.c)
OBJS	    = $(SRCS:.c=.o)
LIBS        = -lm

all:    $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) $(OBJS) $(TARGET) *~

.PHONY: depend clean

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
