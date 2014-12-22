#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <iostream>
#include <signal.h>

#define MAX_SIZE 50

int main(){
    socklen_t sock_descriptor;
    int conn_desc;
    struct sockaddr_in serv_addr, client_addr;
    char buff[MAX_SIZE];

    sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if(sock_descriptor < 0)
        std::cout << "Failed creating socket" << std::endl;

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(1234);

    if(bind(sock_descriptor, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Failed to bind" << std::endl;
    }

    listen(sock_descriptor, 5);

    std::cout << "Waiting for connection...\n" << std::endl;

    socklen_t size = sizeof(client_addr);

    bool status = true;
    int connection = 1;
    do{
        conn_desc = accept(sock_descriptor, (struct sockaddr *)&client_addr, &size);

        if(conn_desc == -1) {
            std::cout << "Failed accepting connection" << std::endl;
            status = false;
            break;
        } else {
            std::cout << "Connected" << std::endl;
        }

        if(read(conn_desc, buff, sizeof(buff)-1) > 0) {
            ++connection;
            std::cout << connection << " Received %s" << buff << ", forking" << std::endl;

            pid_t pid = fork();

            if (pid < 0) {
                std::cout << connection << "ERROR on fork" << std::endl;
                status = false;
            } else {
                std::cout << connection << "PID: " << pid << std::endl;
            }

            write(conn_desc, buff, sizeof(buff)-1);

            kill(pid, SIGKILL);
        } else {
            std::cout << connection << "Failed receiving" << std::endl;
        }

        close(conn_desc);
    } while (status == true);

    close(sock_descriptor);

    return 0;
}
