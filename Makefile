displayplacer: displayplacer.c header.h
	gcc -o displayplacer displayplacer.c -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations