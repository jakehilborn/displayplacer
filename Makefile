displayplacer: displayplacer.c header.h
	$(CC) -o $@ $< -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations
