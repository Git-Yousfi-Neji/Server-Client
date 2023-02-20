import socket
import threading

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8888        # The port used by the server

def receive_message(s):
    while True:
        data = s.recv(1024).decode()
        print(data)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print('Connected to server')
    username = input('Enter your username: ')
    s.sendall(username.encode())
    receive_thread = threading.Thread(target=receive_message, args=(s,))
    receive_thread.start()
    while True:
        message = input('')
        if message == 'exit':
            break
        s.sendall(message.encode())
print('Disconnected from server')
