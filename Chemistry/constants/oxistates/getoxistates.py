from mendeleev import element
from numpy import flexible

file = open("table.h","w")

header  = "\n"
header += "#include \"../../types/types.h\"\n"
header += "\n"
header += "struct\n"
header += "{\n"
header += "    oxistates_t states;\n"
header += "}oxistates_table[NUMBER_OF_ELEMENTS] = \n"

file.write(header)

file.write("{\n")
for i in range(1,119):
    file.write("\t{ ")
    elem = element(i)
    noxistates = len(elem.oxistates)
    file.write('.states.n = ' + str(noxistates) + ', ')
    file.write('.states.states = {')
    for j in range(noxistates):
        if elem.oxistates[j] != None:
            file.write(str(elem.oxistates[j]))
        if j != noxistates-1:
            file.write(', ')
    file.write("} },\n")
file.write('};\n')
