import socket
import threading 
ch=''
def Start():
    s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.bind(('0.0.0.0',6666))
    t=threading.Thread(target=lr_input)
    while True:
        data,addr=s.recvfrom(1)
        if cmd=='r':
            s.sendto('r'+chr(80),addr)
            print 'right'
        if cmd=='l':
            s.sendto('l'+chr(80),addr)
            print 'left'

def lrinput():
    while True:
        ch=raw_input('left(l) or right(r)')
        if ch=='l':
            cmd='l'
        if ch=='r':
            cmd='r'



if __name__ == "__main__":
    Start()
