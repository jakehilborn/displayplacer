prefix ?= /usr/local
exec_prefix ?= $(prefix)
bindir ?= $(exec_prefix)/bin
datarootdir ?= $(prefix)/share
docdir ?= $(datarootdir)/doc/displayplacer

INSTALL ?= install
INSTALL_PROGRAM ?= $(INSTALL)
INSTALL_DATA ?= $(INSTALL) -m 644

.PHONY: all
all: displayplacer

displayplacer: displayplacer.c header.h
	$(CC) -I. $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations

.PHONY: install
install: displayplacer
	mkdir -p $(DESTDIR)$(bindir) $(DESTDIR)$(docdir)
	$(INSTALL_PROGRAM) $< $(DESTDIR)$(bindir)
	$(INSTALL_DATA) LICENSE $(DESTDIR)$(docdir)

.PHONY: clean
clean:
	rm -f displayplacer
