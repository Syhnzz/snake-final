all:
	gcc -Iinclude -Iinclude/SDL -Iinclude/headers -Llib -o Main src/*.c -lmingw32 -lSDLmain -lSDL
