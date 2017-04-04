TARGET	    = mycc
CC	    = gcc
CFLAGS	    = -Wall -g
INCLUDES    = -I./inc
SRCS	    = $(wildcard src/*.c) \
	      $(wildcard tst/*.c)
OBJS	    = $(SRCS:.c=.o)

all:    $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) 

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)

.PHONY: depend clean

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
