// simulator.cpp
// Rohan Weeden
// Creatd: Jan. 21, 2018

// Simulator for the you86 arcitecture
// Based on Dr. Orion Lawlor's UEMU architecture
// <https://www.cs.uaf.edu/2005/fall/cs301/uemu/>


#include <iostream>
#include <fstream>
#include <cstddef>
#include <chrono>
#include <ctime>


using std::cout;
using std::cin;
using std::endl;
using std::size_t;


auto start = std::chrono::system_clock::now();


class CPU {
public:
  int regs[16];    // General purpose registers
  int p_counter;  // Program counter
  int c_debug = 0; // Control register for enabling debug printout
  short prog_mem[256];
  int mem[256];

  int run();

private:
  int _sign_ext(char);
};


int CPU::run() {
  while (true) {
    int instruction = prog_mem[p_counter];
    int opcode = (instruction >> 12) & 0xF;
    short i1 = _sign_ext((instruction >> 4) & 0xFF);
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
        if (c_debug) { cout << "Branching to " << p_counter + i1 << " if reg[" << i2 << "] is 0\n"; }
        if (regs[i2] == 0) { p_counter += i1; }
        break; // BRANCH IF EQUAL TO ZERO
      case 0x5:
        if (c_debug) { cout << "Branching to " << p_counter + i1 << " if reg[" << i2 << "] is not 0\n"; }
        if (regs[i2] < 0) { p_counter += i1; }
        break; // BRANCH IF LESS THAN ZERO
      case 0x6:
        if (c_debug) { cout << "Reading integer into reg[" << i2 << "]\n"; }
        cin >> regs[i2];
        // Start the timer after reading the input
        start = std::chrono::system_clock::now();
        break; // READ INT
      case 0x7:
        if (c_debug) { cout << "Printing integer in reg[" << i2 << "]\n"; }
        // Commented out for timing
        // cout << regs[i2] << "\n";
        break; // PRINT INT
      case 0x8:
        if (c_debug) { cout << "Jumping to " << i1 << "\n"; }
        p_counter = i1;
        break; // JUMP
      case 0x9:
        i1 = i1 & 0xF;
        if (c_debug) { cout << "Subtracting reg[" << i2 << "] from reg[" << i1 << "]\n"; }
        regs[i1] -= regs[i2]; break; // SUB
      case 0xA:
        i1 = i1 & 0xF;
        if (c_debug) { cout << "Storing reg[" << i2 << "] into mem[reg[" << i1 << "]]\n"; }
        mem[regs[i1]] = regs[i2]; break; // STORE from register
      case 0xB:
        i1 = i1 & 0xF;
        if (c_debug) { cout << "Loading mem[reg[" << i2 << "]] into reg[" << i1 << "]\n"; }
        regs[i2] = mem[regs[i1]]; break; // LOAD from register
      case 0xC:
        i1 = i1 & 0xF;
        if (c_debug) { cout << "XORing reg[" << i2 << "] to reg[" << i1 << "]\n"; }
        regs[i1] = regs[i1] ^ regs[i2]; break; // LOAD from register
      default:
        return -1;
    }
    p_counter++;
  }
}

int CPU::_sign_ext(char a) {
  if((a & 0x80) >> 7) {
    return (a & 0xFFFFFFFFFFFFFFFF);
  }
  else {
    return (a & 0x00000000000000FF);
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
  loadProgram(cpu, program_file);

  if (program_file.is_open()) program_file.close();

  // cpu.c_debug = 1;

  int ret = cpu.run();
  auto end = std::chrono::system_clock::now();
  std::cout << "Took " << ((std::chrono::nanoseconds)(end - start)).count() << "ns" << '\n';

  return ret;
}
