all:
	gcc -Wall -g -o y86dis y86dis.c
	gcc -Wall -g -o y86emul y86emul.c
y86dis:
	gcc -Wall -g -o y86dis y86dis.c
y86emul:
	gcc -Wall -g -o y86emul y86emul.c
clean:
	rm y86dis
	rm y86emul
