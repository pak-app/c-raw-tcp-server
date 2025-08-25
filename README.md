# Raw TCP Server in C

A lightweight TCP server implementation in **C** with event-driven design.  
It allows you to register event listeners for socket communication, send messages, and manage connections gracefully or forcefully.

---

## ✨ Features
- Event listeners:
  - `on_data` → Triggered whenever data is received from the client.
  - `once` → Triggered only on the first message of the connection.
  - `on_close` → Triggered when a client disconnects or resources are released.
  - `on_end` → Triggered when the server initiates a graceful end.
- Send messages with:
  - `c_socket.emit(...)`
- Connection management:
  - `c_socket.end()` → Gracefully close the connection (sends **FIN**).
  - `c_socket.destroy()` → Immediately close the connection (like sending **RST**).

---

## 📂 Project Structure
```
.
├── src/
│   ├── main.c      # Example usage
│   └── tcp.c       # TCP server implementation
├── include/
│   └── tcp.h       # Header file
```

---

## 🚀 Build & Run
Compile the server:
```bash
gcc src/main.c src/tcp.c -I include -o tcp_server
```

Run the server:
```bash
./tcp_server
```

By default, the server listens on **localhost:8080** with a buffer size of **1024 bytes**.

---

## 📖 Usage Example (`main.c`)

Here’s how to define your event handlers and start the server:

```c
#include <stdio.h>
#include "tcp.h"

void on(Socket* c_socket, char *data, int bytes)
{
    data[bytes] = '\0';
    printf("[ON EVENT]: Received: %s\n", data);
    const char response[] = "Hello client!!!\n";
    c_socket->emit(response, 0);
}

void once(Socket* c_socket, char *data, int bytes)
{
    data[bytes] = '\0';
    printf("[ONCE EVENT]: Received: %s\n", data);
    const char response[] = "Hello client for the first connection!!!\n";
    c_socket->emit(response, 0);
}

void on_close(__attribute__((unused)) Socket* c_socket)
{
    printf("[CLOSE EVENT]: client disconnected\n");
}

void end(__attribute__((unused)) Socket* c_socket)
{
    printf("[END EVENT] end called.\n");
}

int main(void)
{
    Socket* my_socket = server(1024);
    my_socket->events.on_close = on_close;
    my_socket->events.on_data = on;
    my_socket->events.once = once;
    my_socket->events.on_end = end;
    
    start_server(my_socket, "localhost", 8080);

    return 0;
}
```

---

## 🛠 Events Summary
- **once**  
  Called on the very first message received after the connection is established. Useful for authentication, initialization, or caching.

- **on_data**  
  Called on every subsequent message received.

- **on_end**  
  Triggered when you call `c_socket.end()` → server sends FIN and gracefully shuts down.

- **on_close**  
  Triggered after connection termination (graceful or forced). This is where you can free resources.

---

## 📡 Connection Management
- **Send a message to client**
  ```c
  c_socket->emit("Hello world!", 0);
  ```

- **Graceful close (FIN)**
  ```c
  c_socket->end();
  ```

- **Force destroy (RST / immediate close)**
  ```c
  c_socket->destroy();
  ```

---

## 🧪 Test with `netcat`
On a second terminal, run:
```bash
nc localhost 8080
```
You should see the server respond according to your event handlers.

---

## 📌 Notes
- The server forks for each client (`fork()` in `communication_handler`).
- Child processes handle communication; the parent keeps listening.
- Zombie processes are cleaned up via `SIGCHLD` handler.

---

## 📜 License
MIT License – free to use, modify, and distribute.