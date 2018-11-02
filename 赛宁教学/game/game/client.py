import socket
import sys

host='xxx.xxx.xxx.xxx'
data_payload=2048

def echo_client(port):
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    # Connect the socket to the server
    server_address = (host,port)
    print("Connecting to %s port %s"%server_address)
    sock.connect(server_address)

    # Send data
    try:
        message=fout.read()
        sock.sendall(message.encode())
        # Look for the response

        data = sock.recv(data_payload).decode()
        fin.write(data)
        while len(data)>0:
            print("!Received: %s" %data)
            data = sock.recv(data_payload).decode()
            fin.write(data+"\n");
    except socket.errno as e:
        print("Socket error: %s" %str(e))
    except Exception as e:
        print("Other exception: %s" %str(e))
    finally:
        print("Closing connection to the server")
        sock.close()

if __name__=='__main__':
    fin=open("ranka.txt","w")
    fout=open("rankb.txt","r")
    echo_client(37777)

