#compiler=x86_64-w64-mingw32-gcc
compiler=gcc

clean: *.o
	rm *.o
	rm -r Chemistry/constants/uma/*.o
	rm -r Chemistry/constants/oxistates/*.o
	rm -r Chemistry/constants/atomicnum/*.o
	rm -r Chemistry/types/*.o
	rm GUI/*.o

#constants--------------------------------------------------
Chemistry/constants/atomicnum/get_atomicnum.o: Chemistry/constants/atomicnum/get_atomicnum.h Chemistry/constants/atomicnum/get_atomicnum.c
	$(compiler) -c Chemistry/constants/atomicnum/get_atomicnum.c -o $@

Chemistry/constants/oxistates/get_oxistates.o: Chemistry/constants/oxistates/get_oxistates.c Chemistry/constants/oxistates/get_oxistates.h
	$(compiler) -c Chemistry/constants/oxistates/get_oxistates.c -o $@

Chemistry/constants/uma/get_uma.o: Chemistry/constants/uma/get_uma.c Chemistry/constants/uma/get_uma.h
	$(compiler) -c Chemistry/constants/uma/get_uma.c -o $@

constants_objf=Chemistry/constants/atomicnum/get_atomicnum.o Chemistry/constants/oxistates/get_oxistates.o Chemistry/constants/uma/get_uma.o

#constants--------------------------------------------------

#types------------------------------------------------------

Chemistry/types/constructors.o: Chemistry/types/constructors.h Chemistry/types/constructors.c
	$(compiler) -c Chemistry/types/constructors.c -o $@

test_elem: test_elem.c Chemistry/types/constructors.o $(constants_objf)
	$(compiler) $? -o $@

#types------------------------------------------------------

GUI/WIN.o: GUI/WIN.c GUI/GUI.h
	$(compiler) -c GUI/WIN.c -o $@

main.o: main.c GUI/GUI.h
	$(compiler) -c main.c -o $@

wmain.exe: main.o GUI/WIN.o
	$(compiler) -mwindows -pthread --static $? -o $@
