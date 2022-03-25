from mendeleev import element

file = open("table.h","w")

header  = '\n'
header += '#include "../../types/types.h"\n'
header += '\n'
header += 'struct\n'
header += '{\n'
header += '    char symbol[3];\n'
header += '}table[NUMBER_OF_ELEMENTS] = \n'
header += '{\n'

file.write(header)

for i in range(1,119):
    elem = element(i)
    file.write('\t' + '"' + str(elem.symbol) + '"')
    if i != 118:
        file.write(',\n')
    else:
        file.write('\n')

file.write('};')

