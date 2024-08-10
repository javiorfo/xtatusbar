CC = gcc
CFLAGS = -Wall
TARGET = xtatusbar
SRC = xtatusbar.c
DESTDIR = /usr/local/bin
LIBS = -lasound -lcurl

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
