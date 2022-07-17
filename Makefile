bindir ?= /usr/local/bin

# Compiler flags:
#  -Wall turns on most but not all compiler warnings
#  -Wno-deprecated-declarations is used because CGDisplayIOServicePort was
#    deprecated by Apple with no replacement
#    https://developer.apple.com/documentation/coregraphics/1543516-cgdisplayioserviceport
WARNINGS := -Wno-deprecated-declarations
WARNINGS += -Wall
WARNINGS += -Wempty-body
WARNINGS += -Wempty-init-stmt
WARNINGS += -Wignored-qualifiers
WARNINGS += -Winitializer-overrides
WARNINGS += -Wmissing-field-initializers
WARNINGS += -Wmissing-method-return-type
WARNINGS += -Wnull-pointer-arithmetic
WARNINGS += -Woverride-init
WARNINGS += -Wredundant-move
WARNINGS += -Wsemicolon-before-method-body
WARNINGS += -Wshift-negative-value
WARNINGS += -Wstring-compare
WARNINGS += -Wtype-limits
WARNINGS += -Wuninitialized
WARNINGS += -Wunused-parameter
WARNINGS += -Wbitfield-enum-conversion
WARNINGS += -Wbool-conversion
WARNINGS += -Wconstant-conversion
WARNINGS += -Wenum-conversion
WARNINGS += -Wimplicit-float-conversion
WARNINGS += -Wimplicit-int-conversion
WARNINGS += -Wint-conversion
WARNINGS += -Wliteral-conversion
WARNINGS += -Wnon-literal-null-conversion
WARNINGS += -Wnull-conversion
WARNINGS += -Wobjc-literal-conversion
WARNINGS += -Wshorten-64-to-32
WARNINGS += -Wstring-conversion
WARNINGS += -Wshadow-field
WARNINGS += -Wshadow-field-in-constructor
WARNINGS += -Wshadow-uncaptured-local
WARNINGS += -Wformat
WARNINGS += -Wlogical-not-parentheses
WARNINGS += -Wnull-dereference
WARNINGS += -Wextra #TODO enable after fixing these warnings

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
