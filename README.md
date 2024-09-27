## Understand the Basics

### OSI || TCP/IP model
- Layer 3 (Network Layer): `AF_INET`: Deals with IPv4 addressing and routing.
- Layer 4 (Transport Layer): `SOCK_STREAM`: Uses TCP for reliable, connection-oriented communication.
- Layer 7 (Application Layer): HTTP Protocol: The actual data being transmitted (e.g., HTTP requests and responses).

### Socket Programming
- Familiarize yourself with TCP/IP socket programming, as HTTP operates over TCP [Socket Programming tutorial](https://www.youtube.com/watch?v=bqj4dWG7v3c&list=PLhnN2F9NiVmAMn9iGB_Rtjs3aGef3GpSm)

### HTTP Protocol
-Study the HTTP/1.0 and HTTP/1.1 specifications to understand how requests and responses are formatted

### Concurrency Models
- Decide on how you'll handle multiple clients—either through multi-threading or using non-blocking I/O with `select()` or `poll()`
