/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

// hex2ioio
// This utility translate a hex file into a IOIO application firmware image.
// Usage: hex2ioio <infile> <outfile>
//
// The input format is a standard hex, as produced by the C30 compiler. This
// program does some sanity checking on it, but not full validation.
//
// The output format is very simple and intended to be:
//   1. Light-weight
//   2. Simple to parse by the IOIO
//   3. Forward compatible
//
// The file starts with 4B "IOIO".
// Then followed by a 4B (little endian) format version code.
// Currently version must be 1.
//
// The rest of the file is a sequnence of blocks, each 196B long, containing:
//   1. 4B address (little endian)
//   2. 192B contents (equal to one Flash row in a PIC24), which are actually
//      64 triplets of little-endian, 24-bit instructions.

#include <iostream>
#include <fstream>
#include <map>
#include <stdint.h>
#include <cstring>
#include <cstdlib>

using namespace std;

// a block the size of one flash row = 64 instructions = 256 bytes
// high (last) byte in each dword must be 0.
class block_t {
 public:
  block_t() { memset(b_, 0xFF, sizeof b_); }

  uint8_t& operator[] (size_t pos) { return b_[pos]; }

  void serialize(ostream &out) const {
    for (int i = 0; i < 256; ++i)
      if ((i & 0x03) != 3)
        out.put(b_[i]);
  }

 private:
  uint8_t b_[256];
};

// a map from address (128-word aligned - 7LSB are 0) to block
typedef map<uint32_t, block_t> memory_map_t;

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

  bool got_eof_rec = false;
  uint16_t address_hi = 0;
  while (!infile.eof() && !got_eof_rec) {
    ++line_num;
    char line[256];
    infile.getline(line, sizeof line);
    if (line[0] != ':') continue;
    const char * p = line + 1;
    uint8_t count = read8(p);
	  uint16_t address_lo = read16(p);
	  uint8_t type = read8(p);
	  switch (type) {
      case 0:  // data
        {
          uint32_t address = static_cast<uint32_t>(address_hi) << 16 | address_lo;
		      for (int i = 0; i < count; ++i) {
            block_t &block = memory_map[(address & 0xFFFFFF00) >> 1];
            uint8_t byte = read8(p);
            block[address & 0xFF] = byte;
            if ((address & 0x03) == 3 && byte != 0) error("high byte of each dword must be 0");
            ++address;
          }
        }
        break;

      case 1:  // eof
        got_eof_rec = true;
        break;

      case 4:
        address_hi = read16(p);
        break;
    }
	  read8(p);  // for checksum
    if (checksum) error("Failed checksum");
  }
  infile.close();

  cerr << "Read " << memory_map.size() << " blocks" << endl;

  // write memory map to output file
  ofstream outfile(argv[2], ios::out | ios::binary);
  outfile.write("IOIO", 4);
  outfile.write("\1\0\0\0", 4);
  for (memory_map_t::const_iterator iter = memory_map.begin(); iter != memory_map.end(); ++iter) {
    outfile.write(reinterpret_cast<const char*>(&iter->first), 4);
    iter->second.serialize(outfile);
  }
  outfile.close();

  cerr << "Success!" << endl;
  return 0;
}
