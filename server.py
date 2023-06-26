import socket
import sys

# CREATE A SERVER, RECEIVE FILE FROM CLIENT AND WRITE IT IN A FILE
def server_init(server_address, port_address):
    print("server_init")

# EXECUTE PROGRAM
if __name__ == '__main__':
    # GET THE IP  AND PORT FROM TERMINAL
    server_address = sys.argv[1] 
    port_address = int(sys.argv[2])

    server_init(server_address, port_address)
