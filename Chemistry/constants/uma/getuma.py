from mendeleev import element
import os

path = os.path.realpath(__file__).split("/getuma.py").pop(0)

file = open(path + "/table.h","w")

header  = "#include \"../../types/types.h\"\n"
header += "\n"
header += "struct\n"
header += "{\n"
header += "    num_t mass;\n"
header += "}\n"
header += "uma_table[NUMBER_OF_ELEMENTS] = \n"
header += "{\n"

file.write(header)

for i in range(1,119):

	elem = element(i)

	file.write("\t{ .mass = " + str(elem.mass) + " }")
	if i != 118:
		file.write(",\n")
	else:
		file.write("\n")

file.write("};")