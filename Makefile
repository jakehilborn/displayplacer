displayplacer: displayplacer.c header.h
	gcc -o displayplacer displayplacer.c -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations

debug: displayplacer.c header.h
	gcc -g -o displayplacer displayplacer.c -framework IOKit -framework ApplicationServices -Wno-deprecated-declarations
