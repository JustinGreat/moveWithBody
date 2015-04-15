#!/usr/bin/python
import socket
import time
def Start1():
    s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    while True:
        cmd=raw_input('Input the char you want to send:')
        if cmd=='r':
            s.sendto('r'+chr(80),('localhost',7777))
            print 'right'
        if cmd=='l':
            s.sendto('l'+chr(80),('localhost',7777))
            print 'left'

def Start():
    s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.setblocking(0)
   # while True:
    for i in range(1,10):
        s.sendto('w',('localhost',7777))
        try:
            data,addr=s.recvfrom(2)
            print "%s:%s"%(data,addr)
        except:
            pass
        time.sleep(0.1)

if __name__ == "__main__":
    Start()
