displayplacer: displayplacer.c header.h
	$(CC) -o displayplacer displayplacer.c -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations
