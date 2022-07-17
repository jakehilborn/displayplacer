bindir ?= /usr/local/bin

INSTALL ?= install
INSTALL_PROGRAM ?= $(INSTALL)
INSTALL_DATA ?= $(INSTALL) -m 644

.PHONY: all
all: displayplacer

displayplacer: displayplacer.c header.h
	$(CC) -I. $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations

.PHONY: debug
debug: CFLAGS += -g
debug: displayplacer

.PHONY: install
install: displayplacer
	$(INSTALL_PROGRAM) $< $(DESTDIR)$(bindir)

.PHONY: clean
clean:
	rm -f displayplacer
