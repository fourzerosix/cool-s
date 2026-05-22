# cool-s Makefile
#
# Targets:
#   make          — build the binary
#   make install  — install to PREFIX (default /usr/local)
#   make uninstall— remove installed files
#   make clean    — remove build artifacts
#   make dist     — create a release tarball
#   make check    — quick smoke-test

CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra -std=gnu99 -D_POSIX_C_SOURCE=200809L
LDFLAGS ?= -lm

PREFIX  ?= /usr/local
BINDIR  := $(PREFIX)/bin
MANDIR  := $(PREFIX)/share/man/man6

BINARY  := cool-s
SRC     := src/cool-s.c
MAN     := man/cool-s.6
VERSION := $(shell cat VERSION 2>/dev/null || echo "1.0.0")
TARBALL := cool-s-$(VERSION).tar.gz

.PHONY: all install uninstall clean dist check

all: $(BINARY)

$(BINARY): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(BINARY)
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(BINARY) $(DESTDIR)$(BINDIR)/$(BINARY)
	@if [ -f $(MAN) ]; then \
	    install -d $(DESTDIR)$(MANDIR); \
	    install -m 644 $(MAN) $(DESTDIR)$(MANDIR)/$(notdir $(MAN)); \
	    gzip -f $(DESTDIR)$(MANDIR)/$(notdir $(MAN)) || true; \
	fi
	@echo "Installed cool-s to $(DESTDIR)$(BINDIR)"

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(BINARY)
	rm -f $(DESTDIR)$(MANDIR)/$(notdir $(MAN)).gz
	rm -f $(DESTDIR)$(MANDIR)/$(notdir $(MAN))

clean:
	rm -f $(BINARY) $(TARBALL)

dist: clean
	mkdir -p /tmp/cool-s-$(VERSION)
	cp -r . /tmp/cool-s-$(VERSION)
	rm -rf /tmp/cool-s-$(VERSION)/.git
	tar -czf $(TARBALL) -C /tmp cool-s-$(VERSION)
	rm -rf /tmp/cool-s-$(VERSION)
	@echo "Created $(TARBALL)"

check: $(BINARY)
	@echo "==> Fast mode test"
	./$(BINARY) -f --plain -s 2 > /dev/null && echo "PASS: fast+plain"
	@echo "==> Help test"
	./$(BINARY) --help > /dev/null && echo "PASS: --help"
	@echo "All checks passed."
