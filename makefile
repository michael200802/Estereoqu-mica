compiler=x86_64-w64-mingw32-gcc
compiler=gcc

clean:
	rm *.o
	rm -r Chemistry/constants/get_uma/*.o
	rm -r Chemistry/constants/get_oxistates/*.o
	rm -r Chemistry/constants/get_atomicnum/*.o
	rm -r Chemistry/constants/get_elem_symbol/*.o
	rm -r Chemistry/types/elem/*.o
	rm -r Chemistry/types/substance/*.o
	rm -r Chemistry/types/reaction/*.o
	rm GUI/*.o

#constants--------------------------------------------------

Chemistry/constants/%.o: Chemistry/constants/%.c Chemistry/constants/%.h Chemistry/constants/%.py
#	python $(@:.o=.py)
	$(compiler) -c $(@:.o=.c) -o $@

constants_objf:=Chemistry/constants/get_elem_symbol/get_elem_symbol.o Chemistry/constants/get_atomicnum/get_atomicnum.o Chemistry/constants/get_oxistates/get_oxistates.o Chemistry/constants/get_uma/get_uma.o

#constants--------------------------------------------------

#types------------------------------------------------------
Chemistry/types/elem/elem.o: Chemistry/types/elem/elem.h Chemistry/types/elem/elem.c
	$(compiler) -c Chemistry/types/elem/elem.c -o $@

Chemistry/types/substance/substance.o: Chemistry/types/substance/substance.c Chemistry/types/substance/substance.h Chemistry/types/elem/elem.o
	$(compiler) -c Chemistry/types/substance/substance.c -o $@

Chemistry/types/reaction/reaction.o: Chemistry/types/reaction/reaction.c Chemistry/types/reaction/reaction.h Chemistry/types/substance/substance.o
	$(compiler) -c Chemistry/types/reaction/reaction.c -o $@

types_objf:=Chemistry/types/reaction/reaction.o Chemistry/types/substance/substance.o Chemistry/types/elem/elem.o

test_elem: test_elem.c Chemistry/types/elem/elem.o $(constants_objf)
	$(compiler) $? -o $@

test_substance: test_substance.c Chemistry/types/substance/substance.o Chemistry/types/elem/elem.o $(constants_objf)
	$(compiler) $? -o $@

test_reaction: test_reaction.c $(types_objf) $(constants_objf)
	$(compiler) $? -o $@

#types------------------------------------------------------

#GUI--------------------------------------------------------
GUI/%.o: GUI/%.c GUI/%.h
	$(compiler) -c $(@:.o=.c) -o $@
#GUI--------------------------------------------------------

wmain.exe: main.o GUI/WIN.o GUI/input.o GUI/output.o $(types_objf) $(constants_objf)
	$(compiler) -mwindows -pthread --static $? -o $@
