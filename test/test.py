import socket

# Server info
HOST = '127.0.0.1'  # Server IP (localhost)
PORT = 8080         # Server port

# Create a TCP socket
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    # Connect to server
    s.connect((HOST, PORT))
    print(f"Connected to {HOST}:{PORT}")

    # Send a message
    message = "Hello from Python client"
    s.sendall(message.encode())

    # Receive a reply (up to 1024 bytes)
    data = s.recv(1024)
    print("Received:", data.decode())
