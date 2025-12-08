# ![udscast](img/logo.png)

*udscast, UDScast*

Unix Domain Socket Broadcast

# How It Works

UDScast creates a Unix domain socket on launch. Clients may connect to this socket and send data to it.

![figure 1](img/udscast_fig1.svg)

UDScast sends this recieved data to all clients connected to the same socket.

![figure 2](img/udscast_fig1.svg)

