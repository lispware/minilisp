#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "../../lisp.h"
#include "../platform.h"

any plt_bind(Context *CONTEXT_PTR, word n)
{
    cell c1;


    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( n );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    Push(c1, mkNum(CONTEXT_PTR, server_fd));
    return Pop(c1);
}

any plt_listen(Context *CONTEXT_PTR, word n)
{
    int server_fd = (int)n;
    int new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                    (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    cell c1;
    Push(c1, mkNum(CONTEXT_PTR, new_socket));
    return Pop(c1);
}

any plt_socket(Context *CONTEXT_PTR, word n)
{
    char buffer[1024];
    int skt = (int)n;


     int r = read( skt , buffer, 1024);
     for(int i = 0;i<5; i++)
     {
         printf("%c\n", buffer[i]);
     }

    return Nil;
}
