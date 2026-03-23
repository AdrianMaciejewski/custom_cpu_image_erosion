#instruction set
isa = {'ADD': '0001',
        'SUB': '0010',
        'ADDI': '0011',
        'MULI': '0100',
        'SW': '0101',
        'LW': '0110',
        'BEQ': '1001',
        'BNE': '1010',
        'BGE': '1011',
        'END': '1111'}

#load assembly code
with open("A2_assembly.s", 'r') as file:
    content = file.readlines()

#remove empty lines and comments
content = [x for x in content if x.split(' ')[0].upper() in isa]

#remove comments on back of instructions
for i in range(len(content)):
    content[i] = content[i].split('#')[0].strip() + '\n'

#remove commas
for i in range(len(content)):
    content[i] = content[i].replace(',', '')

#asemble instructions
for instruction in content:
    opcode = instruction.strip().split(' ')[0].upper()
    operands = instruction.strip().split(' ')[1:]

    if opcode in ['ADD', 'SUB']: #R-type
        rd = format(int(operands[0][1:]), '04b')
        rs1 = format(int(operands[1][1:]), '04b')
        rs2 = format(int(operands[2][1:]), '04b')
        mc = isa[opcode] + rd + rs1 + rs2 + '0000000000000000'
        content[content.index(instruction)] = mc
    elif opcode in ['ADDI', 'MULI']: #I-type
        rd = format(int(operands[0][1:]), '04b')
        rs1 = format(int(operands[1][1:]), '04b')
        imm = format(int(operands[2]), '016b')
        mc = isa[opcode] + rd + rs1 + '0000' + imm
        content[content.index(instruction)] = mc
    elif opcode in ['SW', 'LW']: #D-type
        rs1 = format(int(operands[1][1:]), '04b')
        if opcode == 'SW':
            rs2 = format(int(operands[0][1:]), '04b')
            rd = '0000'
        else:
            rd = format(int(operands[0][1:]), '04b')
            rs2 = '0000'
        mc = isa[opcode] + rd + rs1 + rs2 + '0000000000000000'
        content[content.index(instruction)] = mc
    elif opcode in ['BEQ', 'BNE', 'BGE']: #B-type
        rs1 = format(int(operands[0][1:]), '04b')
        rs2 = format(int(operands[1][1:]), '04b')
        imm = format(int(operands[2]), '016b')
        mc = isa[opcode] + '0000' + rs1 + rs2 + imm
        content[content.index(instruction)] = mc
    else: #END
        mc = isa[opcode] + '0000000000000000000000000000'
        content[content.index(instruction)] = mc
#write file
with open("machine_code.txt", 'w') as mc_file:
    mc_file.write("object Programs{\n\tval program1 = Array(\n")
    for line in content[:-1]:
        mc_file.write("\t\t"+r'"h'+hex(int(line, 2))[2:]+r'".U(32.W),'+'\n')
    mc_file.write("\t\t"+r'"h'+hex(int(content[-1], 2))[2:]+r'".U(32.W),'+'\n\t)\n}')