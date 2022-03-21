compiler=x86_64-w64-mingw32-gcc
#compiler=gcc

clean: *.o
	rm *.o
	rm GUI/*.o
	rm -r Chemistry/constants/uma/*.o
	rm -r Chemistry/constants/oxistates/*.o

#constants
Chemistry/constants/oxistates/get_oxistates.o: Chemistry/constants/oxistates/get_oxistates.c Chemistry/constants/oxistates/get_oxistates.h
	$(compiler) -c Chemistry/constants/oxistates/get_oxistates.c -o $@

Chemistry/constants/uma/get_uma.o: Chemistry/constants/uma/get_uma.c Chemistry/constants/uma/get_uma.h
	$(compiler) -c Chemistry/constants/uma/get_uma.c -o $@
#constants

GUI/WIN.o: GUI/WIN.c GUI/GUI.h
	$(compiler) -c GUI/WIN.c -o $@

main.o: main.c GUI/GUI.h
	$(compiler) -c main.c -o $@

wmain.exe: main.o GUI/WIN.o
	$(compiler) -mwindows -pthread --static $? -o $@
