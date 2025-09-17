# Makefile for rt
CC ?= gcc
CFLAGS ?= -O2 -pipe -Wall -Wextra -std=gnu11
BINARY = rt
SRC = rt.c

PREFIX ?= /usr/local
SBINDIR ?= $(PREFIX)/sbin
DESTDIR ?=

.PHONY: all clean install

all: $(BINARY)

$(BINARY): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ 

install: all
	@echo "Installing $(BINARY) -> $(DESTDIR)$(SBINDIR)/$(BINARY)"
	install -d "$(DESTDIR)$(SBINDIR)"
	install -m 4750 "$(BINARY)" "$(DESTDIR)$(SBINDIR)/$(BINARY)"
	# chown may fail for non-root builds; that's fine. fakeroot used in packaging will record it.
	chown root:wheel "$(DESTDIR)$(SBINDIR)/$(BINARY)" || true

clean:
	rm -f $(BINARY)
