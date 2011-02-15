#!/usr/bin/python

import random
import socket
import string
import time
import threading

PORT = 7149
ACK_BYTE = 'A'

def test_upload_throughput(sock, packet_size, packets_per_test):
  sock.send(ACK_BYTE)
  print "Acked for testing upload throughput"
  start = time.time()
  for x in xrange(packets_per_test):
    r = sock.recv(packet_size)
    if not r:
      print "ERROR: couldn't read from socket"
      return
    if len(r) != packet_size:
      print "WARNING: wrong packet length %s != %s" % (len(r), packet_size)
  sock.send(ACK_BYTE)
  print "Recieved all packets for upload throughput test in %s seconds" % (time.time() - start)

def test_download_throughput(sock, packet_size, packets_per_test):
  sock.send(ACK_BYTE)
  print "Acked for testing download throughput"
  s = "".join(random.choice(string.printable) for x in xrange(packet_size))
  start = time.time()
  for x in xrange(packets_per_test):
    sock.send(s)
  sock.send(ACK_BYTE)
  print "Sent all packets for download throughput test in %s seconds" % (time.time() - start);

def test_both_throughput(sock, packet_size, packets_per_test):
  sock.send(ACK_BYTE)
  print "Acked for testing both throughput"
  s = "".join(random.choice(string.printable) for x in xrange(packet_size))

  class Reciever(threading.Thread):
    def run(self):
      for x in xrange(packets_per_test):
        r = sock.recv(packet_size)
        if not r:
          print "ERROR: couldn't read from socket for both test"
          return
        if len(r) != packet_size:
          print "WARNING: wrong packet length %s != %s" % (len(r), packet_size)
  reciever = Reciever()

  class Sender(threading.Thread):
    def run(self):
      for x in xrange(packets_per_test):
        sock.send(s)
  sender = Sender()

  start = time.time()
  reciever.start()
  sender.start()
  reciever.join()
  sender.join()
  sock.send(ACK_BYTE)
  print "Echoed all packets for both throughput test in %s seconds" % (time.time() - start)

def test_latency_avg(sock, packet_size, packets_per_test):
  sock.send(ACK_BYTE)
  print "Acked for testing latency average"
  start = time.time()
  for x in xrange(packets_per_test):
    r = sock.recv(1)
    if not r:
      print "ERROR: couldn't read from socket"
      return
    sock.send(r)
  print "Echoed all packets for latency test in %s seconds" % (time.time() - start)

def main():
  print "Creating connection to Android...",
  sock = socket.create_connection(("localhost", PORT))
  print "Connected!"
  sizes = sock.recv(4)
  while len(sizes) < 4:
    sizes += sock.recv(4 - len(sizes))
  packet_size = (ord(sizes[0]) << 8) | ord(sizes[1])
  packets_per_test = (ord(sizes[2]) << 8) | ord(sizes[3])
  sock.send(ACK_BYTE)
  while True:
    byte = sock.recv(1)
    if not byte:
      print "Disconnected!"
      break
    print "recieved: " + byte
    if byte == 'U':
      test_upload_throughput(sock, packet_size, packets_per_test)
    elif byte == 'D':
      test_download_throughput(sock, packet_size, packets_per_test)
    elif byte == 'B':
      test_both_throughput(sock, packet_size, packets_per_test)
    elif byte == 'L':
      test_latency_avg(sock, packet_size, packets_per_test)

if __name__ == "__main__":
  main()
