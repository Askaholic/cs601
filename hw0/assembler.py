# assembler.py
# Rohan Weeden
# Created: Jan. 21, 2018

# Assembler for the you86 architecture

import sys
import re


codes = {
    "LOAD": 0x0,
    "STORE": 0x1,
    "ADD": 0x2,
    "ADDI": 0x3,
    "BEZ": 0x4,
    "BNZ": 0x5
}


def assemble(inname, outname):
    code_regex = ""
    for i, name in enumerate(codes):
        if i != 0:
            code_regex += "|"
        code_regex += name

    regex = r'{} (\d*) (\d*)'.format("({})".format(code_regex))
    with open(inname) as f:
        with open(outname, 'wb') as out:
            for line in f:
                if line[0] == ";" or line == "\n":
                    continue;

                match = re.match(regex, line)
                if not match:
                    print("ILLEGAL CODE: ")
                    print(line)
                    return;
                i = match.groups()
                byte1 = ((codes[i[0]] & 0xF) << 4) & ((int(i[1]) >> 4) & 0xF)
                byte2 = 0
                print(format(((codes[i[0]] & 0xF) << 4) & 0xFF, '02x'))
                out.write(bytes(byte2))
                out.write(bytes(byte1))




if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("usage: assembler.py filename")

    filename = sys.argv[1]

    assemble(filename, re.match(r'^(.*)\..*$', filename).group(1) + ".u86")
