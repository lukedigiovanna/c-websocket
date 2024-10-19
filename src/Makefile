CFLAGS=-g -O2 -pthread -Wall -Wmissing-prototypes

# include lib directory into runtime path to facilitate dynamic linking
LDFLAGS=-pthread

HEADERS=socket.h bufio.h http.h buffer.h base64.h sha1.h websocket.h
OBJ=main.o socket.o buffer.o bufio.o http.o base64.o sha1.o websocket.o

all:    server $(OTHERS)

$(OBJ) : $(HEADERS)

server: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS) 

clean:
	/bin/rm -f $(OBJ) $(OTHERS) server
