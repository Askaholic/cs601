// simulator.cpp
// Rohan Weeden
// Creatd: Jan. 21, 2018

// Simulator for the you86 arcitecture
// Based on Dr. Orion Lawlor's UEMU architecture
// <https://www.cs.uaf.edu/2005/fall/cs301/uemu/>


#include <iostream>
#include <fstream>
#include <cstddef>


using std::cout;
using std::cin;
using std::endl;
using std::size_t;


class CPU {
public:
  int regs[16];    // General purpose registers
  int p_counter;  // Program counter
  int c_debug = 0; // Control register for enabling debug printout
  short prog_mem[256];
  int mem[256];

  int run();
};


int CPU::run() {
  while (true) {
    int instruction = prog_mem[p_counter];
    int opcode = (instruction >> 12) & 0xF;
    short i1 = (instruction >> 4) & 0xFF;
    short i2 = instruction & 0xF;
    switch (opcode) {
      case 0xF:
        if (c_debug) { cout << "Loading mem[" << i1 << "] into reg[" << i2 << "]\n"; }
        regs[i2] = mem[i1]; break; // LOAD
      case 0x1:
        if (c_debug) { cout << "Storing reg[" << i2 << "] into mem[" << i1 << "]\n"; }
        mem[i1] = regs[i2]; break; // STORE
      case 0x2:
        i1 = i1 & 0xF;
        if (c_debug) { cout << "Adding reg[" << i2 << "] to reg[" << i1 << "]\n"; }
        regs[i1] += regs[i2]; break; // ADD
      case 0x3:
        if (c_debug) { cout << "Adding " << i1 << " to reg[" << i2 << "]\n"; }
        regs[i2] += i1; break; // ADD IMMEDIATE
      case 0x4:
        if (c_debug) { cout << "Branching to" << p_counter + i1 << " if reg[" << i2 << "] is 0\n"; }
        if (regs[i2] == 0) { p_counter += i1; }
        break; // BRANCH IF EQUAL TO ZERO
      case 0x5:
        if (c_debug) { cout << "Branching to" << p_counter + i1 << " if reg[" << i2 << "] is not 0\n"; }
        if (regs[i2] != 0) { p_counter += i1; }
        break; // BRANCH IF NOT EQUAL TO ZERO
      case 0x6:
        if (c_debug) { cout << "Reading integer into reg[" << i2 << "]\n"; }
        cin >> regs[i2];
        break; // READ INT
      default:
        return -1;
    }
    p_counter++;
  }
}

void loadProgram(CPU& cpu, std::ifstream& program_file) {
  int p_loc = 0;
  short first = 0;
  bool has1 = false;
  while (!program_file.eof() && p_loc < 256) {
    auto byte = program_file.get();
    if (!has1) {
      first = byte & 0xFFFF;
      has1 = true;
      continue;
    }

    short second = (byte & 0xFFFF) << 8;
    cpu.prog_mem[p_loc] = first | second;

    first = 0;
    has1 = false;
    p_loc ++;
  }
}


int main(int argc, char const *argv[]) {
  if (argc < 2) {
    cout << "usage: ./simulator.out program_file.u86\n";
    return 0;
  }


  std::ifstream program_file(argv[1], std::ios_base::binary);
  if (!program_file.is_open()) {
    cout << "Could not open file '" << argv[1] << "'\n";
    return 0;
  }

  CPU cpu = CPU();
  cpu.c_debug = 1;
  loadProgram(cpu, program_file);

  if (program_file.is_open()) program_file.close();



  // cpu.c_debug = 1;
  //
  // cpu.mem[0] = 2;
  // cpu.mem[1] = 2;
  //
  // cpu.prog_mem[0] = 0x0000;
  // cpu.prog_mem[1] = 0x0011;
  // cpu.prog_mem[2] = 0x2010;
  // cpu.prog_mem[3] = 0x1020;
  // cpu.prog_mem[4] = 0xF000; // Illigal instruction should stop the program

  return cpu.run();
}
