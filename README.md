# c-http-web-server
HTTP/1.1 web server using C language

## C concurent programming

1. Multi-Processing (using `fork()`)
2. Multi-Threading (using `pthreads`)
3. I/O Multiplexing (using `select()`, `poll()`, or `epoll()`)

**Common Concepts:**
1. Do you want me to also show you how to avoid zombie processes (by reaping the child with waitpid in the parent), so your server doesn’t leak PIDs after many clients disconnect?
