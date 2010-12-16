#include <iostream>
#include <fstream>
using namespace std;

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

int main(int argc, const char* argv[]) {
  if (argc != 3) usage();

  ifstream infile(argv[1], ios::in);
  ofstream outfile(argv[2], ios::out | ios::binary);

  while (!infile.eof()) {
    ++line_num;
    char line[256];
    infile.getline(line, sizeof line);
    if (line[0] != ':') continue;
    const char * p = line + 1;
    uint8_t count = read8(p);
    outfile.put(count);
    for (int i = 0; i < count + 4; ++i) outfile.put(read8(p));
    if (checksum) error("Failed checksum");
  }
  cerr << "Success!" << endl;
  return 0;
}
