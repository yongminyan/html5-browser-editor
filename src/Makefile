CC = gcc

CFLAGS  = -g -Wall
LWS     = -lwebsockets
LIB     = ../lib
GTK     = pkg-config --cflags --libs gtk+-2.0

TARGET_SERVICE = lws-server
TARGET_PROC    = user-proc

all: $(TARGET_SERVICE) $(TARGET_PROC)

$(TARGET_SERVICE): $(TARGET_SERVICE).o
	$(CC) -L$(LIB) $(TARGET_SERVICE).o -o $(TARGET_SERVICE) $(LWS)

$(TARGET_SERVICE).o: $(TARGET_SERVICE).c
	$(CC) -c $(CFLAGS) $(TARGET_SERVICE).c

$(TARGET_PROC): $(TARGET_PROC).o
	$(CC) $(TARGET_PROC).o -o $(TARGET_PROC) `$(GTK)`

$(TARGET_PROC).o: $(TARGET_PROC).c
	$(CC) -c $(CFLAGS) $(TARGET_PROC).c `$(GTK)`

clean:
	rm -rf *.o $(TARGET_SERVICE) $(TARGET_PROC) *.txt~  *.tmp
