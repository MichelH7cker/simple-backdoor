import socket
import time
from datetime import datetime

def send_file(filename, destination_address, destination_port):
    # CREATE SOCKET
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # CONNECT TO SERVER
        sock.connect((destination_address, destination_port))

    finally:
        # CLOSES SOCKET
        sock.close()


# EXECUTES THE CLIENT
if __name__ == '__main__':
    target_file = '/sys/kernel/debug/backdoor/keylogger'
    destination_address = '127.0.0.1'  
    destination_port = 8080  

    send_file(target_file, destination_address, destination_port)
