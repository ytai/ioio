#!/usr/bin/python
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
