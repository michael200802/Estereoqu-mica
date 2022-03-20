compiler=x86_64-w64-mingw32-gcc

clean: *.o
	rm *.o

GUI/WIN.o: GUI/WIN.c GUI/GUI.h
	$(compiler) -c GUI/WIN.c -o $@

main.o: main.c GUI/GUI.h
	$(compiler) -c main.c -o $@

wmain.exe: main.o GUI/WIN.o
	$(compiler) -pthread --static $? -o $@
