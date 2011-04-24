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

// wrapioio
// This utility wraps a ioio image file with metadata required for the IOIOManager application
// in order to properly stage this image for installation.
//
// Usage: wrapioio <platform> <infile> <outfile>
//
// The platform is a unique identifier of the hardware and bootloader ABI this image has been
// built for. It is a 8-character ASCII string, where the first 4 characters designate the
// authority who defined the hardware/ABI specification and the last 4 characters are a unique
// designator of this particular specification.
//
// The input format is a ioio file, as produced by the hex2ioio tool. 
//
// The output format is currently only the platform ID followed by the ioio file, but this is
// likely to change in the future. 

#include <iostream>
#include <fstream>
#include <map>
#include <stdint.h>
#include <cstring>
#include <cstdlib>

using namespace std;

void usage() {
  cerr << "Usage: wrapioio <platform> <in> <out>" << endl;
  exit(1);
}

int main(int argc, const char* argv[]) {
  if (argc != 4 || strlen(argv[1]) != 8) usage();

  ofstream outfile(argv[3], ios::out | ios::binary);
  outfile.write(argv[1], 8);

  ifstream infile(argv[2], ios::in | ios::binary);

  while (true) {
    int c = infile.get();
    if (infile.eof()) break;
    outfile.put(c);
  }
  
  infile.close();
  outfile.close();

  cerr << "Success!" << endl;
  return 0;
}
