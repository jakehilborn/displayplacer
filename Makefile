TARGET := displayplacer
# Compiler flags:
#  -Wall turns on most but not all compiler warnings
#  -O3 optimize yet more. 
#  -Wno-deprecated-declarations is used because CGDisplayIOServicePort was
#    deprecated by Apple with no replacement
#    https://developer.apple.com/documentation/coregraphics/1543516-cgdisplayioserviceport
#  -Wextra enables some extra warning flags that are not enabled by -Wall
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

COMPILER := gcc
OPTIMIZATION := -O3

CFLAGS := -framework ApplicationServices -framework IOKit $(WARNINGS) $(OPTIMIZATION)

all: $(TARGET)

$(TARGET): displayplacer.c header.h
	$(COMPILER) $(CFLAGS) -o $(TARGET) displayplacer.c

check:
	exactly suite test/test.suite 

# -f needed to prevent error if file doesn't exist
clean:
	rm -f $(TARGET)
