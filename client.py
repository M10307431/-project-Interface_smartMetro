from socket import socket, AF_INET, SOCK_STREAM
import socket as sk
import msvcrt as m
import sys
from select import select
timeout = 3

def main():
    s=socket(AF_INET, SOCK_STREAM)
    s.connect((host, port))
    while True:
        s.settimeout(1)
        try:
            msg=s.recv(4096)
            #x = unicode(msg, 'utf-8').decode('big5')
            print 'info:',msg
        except:
            pass
        try:
            print '============================'
            start = raw_input('Start:')
            dest = raw_input('Dest.:')
            data = start+','+dest
            s.send(data)
        except:
            s.close()
            print 'shut down'
            break
if __name__=="__main__":
    host='140.118.206.163'
    port=12345
    main()
