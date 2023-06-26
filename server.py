import socket
import sys

# CREATE A SERVER, RECEIVE FILE FROM CLIENT AND WRITE IT IN A FILE
def server_init(server_address, port_address):
    # CREATE SOCKET
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # ASSOCIATE SOCKET WITH AN ADRRESS AND PORT
    server.bind((server_address, port_address))

    # PUT SOCKET TO LISTEN
    server.listen()

    print(f"[+] Server started at {server_address}:{port_address}")

    while True:
        # WAIT TO CLIENT CONNECT
        print("...WAITING...")
        client, client_address = server.accept()
        print(f"[+] Client {client_address} connected")

        # CLOSE CONNECTION WITH CLIENT
        client.close()
        print(f"\n[+] Client {client_address} disconnected")

# EXECUTE PROGRAM
if __name__ == '__main__':
    # GET THE IP  AND PORT FROM TERMINAL
    server_address = sys.argv[1] 
    port_address = int(sys.argv[2])

    server_init(server_address, port_address)
