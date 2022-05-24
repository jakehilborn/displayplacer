displayplacer: displayplacer.c header.h
	$(CC) -I. $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations
