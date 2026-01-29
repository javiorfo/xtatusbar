CC = gcc
CFLAGS = -Wall -O2
TARGET = xtatusbar
SRC = src/*.c
DESTDIR = /usr/local/bin
LIBS = -lasound -lX11

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

install: $(TARGET)
	install -m 0755 $(TARGET) $(DESTDIR)

uninstall:
	rm -f $(DESTDIR)/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install uninstall clean
