#!/usr/bin/python

#
# Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
#
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice, this list of
#       conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other materials
#       provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those of the
# authors and should not be interpreted as representing official policies, either expressed
# or implied.
#

import sys
import select
import io
import fcntl
import os

def main(pipe_name):
  pipe = io.open(pipe_name, 'r+b')
  fl = fcntl.fcntl(pipe.fileno(), fcntl.F_GETFL)
  fcntl.fcntl(pipe.fileno(), fcntl.F_SETFL, fl | os.O_NONBLOCK)
  while True:
    rlist, _, _ = select.select((pipe, sys.stdin), (), ())
    if pipe in rlist:
      got = pipe.read(1)
      while got is not None:
        if not got:
          print "[31mBridget disconnected[0m"
          return
        print "[32m%.2x[0m" % ord(got),
        sys.stdout.flush()
        got = pipe.read(1)
    if sys.stdin in rlist:
      line = sys.stdin.readline()
      if not line:
        print "[31mGot EOF[0m"
        return
      first = None
      for c in line:
        if c not in "0123456789abcdef":
          first = None
          continue
        if first is None:
          first = c
          continue
        i = int(first + c, 16)
        pipe.write(chr(i))
        pipe.flush()
        first = None

if __name__ == '__main__':
  main(sys.argv[1])
