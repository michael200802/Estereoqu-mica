from mendeleev import element
import os

path = os.path.realpath(__file__).split("/get_elem_symbol.py").pop(0)

file = open(path + "/table.h","w")

header  = '#pragma once\n'
header += '#include "../../types/types.h"\n'
header += '\n'
header += 'elem_symbol_t table[NUMBER_OF_ELEMENTS] = \n'
header += '{\n'

file.write(header)

for i in range(1,119):
    elem = element(i)
    file.write('\t' + '{"' + str(elem.symbol) + '"}')
    if i != 118:
        file.write(',\n')
    else:
        file.write('\n')

file.write('};')
