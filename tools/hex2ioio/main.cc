#include <iostream>
#include <fstream>
#include <map>
using namespace std;

// a block the size of one flash row = 64 instructions = 192 bytes
typedef uint8_t block[192];

// a map from address (128-word aligned - 7LSB are 0) to block
typedef map<uint32_t, block> memory_map_t;

void usage() {
cerr << "Usage: hex2ioio <in> <out>" << endl;
  exit(1);
}

int line_num = 0;
uint8_t checksum = 0;

void error(const char* err) {
  cerr << "Error in line " << line_num << ": " << err << endl;
  exit(1);
}

uint8_t read4(const char *& p) {
  char c = *p++;
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c + 10 - 'a';
  if (c >= 'A' && c <= 'F') return c + 10 - 'A';
  error("Invalid character");
  return 0;
}

uint8_t read8(const char *& p) {
  uint8_t result = read4(p) << 4 | read4(p);
  checksum += result;
  return result;
}

// big-endian!
uint16_t read16(const char *& p) {
  return read8(p) << 8 | read8(p);
}

int main(int argc, const char* argv[]) {
  if (argc != 3) usage();

  memory_map_t memory_map;

  // process input hex file into memory map
  ifstream infile(argv[1], ios::in);
  while (!infile.eof()) {
    ++line_num;
    char line[256];
    infile.getline(line, sizeof line);
    if (line[0] != ':') continue;
    const char * p = line + 1;
    uint8_t count = read8(p);
	uint16_t address = read16(p);
	uint8_t type = read8(p);
	cerr << "Address 0x" << hex << address << dec << " type: " << int(type) << endl;
    for (int i = 0; i < count; ++i) cerr << hex << int(read8(p)) << " ";
	cerr << endl;
	read8(p);  // for checksum
    if (checksum) error("Failed checksum");
  }

  // write memory map to output file
  ofstream outfile(argv[2], ios::out | ios::binary);

  cerr << "Success!" << endl;
  return 0;
}
