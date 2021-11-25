#target file name : 20161598.out
CC = gcc
OBJS = 20161598.o
TARGET = 20161598.out

.SUFFIXES : .c .o

all: $(TARGET)

$(TARGET): $(OBJS)
	gcc -Wall -O2 -o $@ $(OBJS)
#-Wall : make every warning, -O2 : optimization

clean : 
	rm -f $(OBJS) $(TARGET) *lst *obj

20161598.out : 20161598.c 20161598.h
