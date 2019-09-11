Socket chat;

How to compile:
1. make
To delete obj files:
make clean
To delete obj and execution files
make fclean

Instruction:

1.Start server (./server_app);
2.Start Clients (./clients_app);
3.Log_in Or Register and go to the chatroom;

P.S. If u close server before clients, u may not be able to start the server for a while, becouse of closed bind, just w8 a minute and try again.


For comfort, two threads were created for login and registration.
For each new client the server will create new thread for it;
