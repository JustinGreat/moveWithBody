#!/usr/bin/python

import socket
import threading 
import time
from  multiprocessing import Process,Queue
ch='r'
q=Queue(maxsize=1)
def Start():
    p_input=Process(target=lr_input,args=(q,))
    p_net=Process(target=net_work,args=(q,))
    p_input.start()
    p_net.start()
    p_input.join()
    p_net.join()

def net_work(q):
    s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.bind(('0.0.0.0',6666))
    while True:
        data,addr=s.recvfrom(1)
        try:
            cmd=q.get(True)
        except:
            pass
        if cmd=='r':
            s.sendto('r'+chr(80),addr)
            print 'right'
        if cmd=='l':
            s.sendto('l'+chr(80),addr)
            print 'left'

def lr_input(q):
    while True:
        try:
            if ch=='r':
                q.put('r')
            elif ch=='l':
                q.put('l')
            else:
                pass
        except:
            q.get(True)
            q.put(ch)

if __name__ == "__main__":
    Start()
