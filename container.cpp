#include <iostream>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <string>


#define MAX_HOST_NAME 256

using namespace std;


int setHostName(int portNum, char *name, sockaddr_in *sa, hostent *hp)
{
    char hostName[MAX_HOST_NAME + 1];

    if ((hp = gethostbyname(hostName)) == nullptr)
    {
        std::cerr << "system error: could not set host name\n";
        return (-1);
    }

    memset(sa, 0, sizeof(*sa));
    sa->sin_family = hp->h_addrtype;
    sa->sin_port = htons((u_short) portNum);
    memcpy((char *) &sa->sin_addr, hp->h_addr, hp->h_length);

    strcpy(name, hp->h_name);
}


int establish(sockaddr_in *sa)
{
    int server_fd;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "system error: could not open socket/n";
        exit(EXIT_FAILURE);
    }
    if (bind(server_fd, reinterpret_cast<const sockaddr *>(&sa), sizeof(*sa)) < 0)
    {
        std::cerr << "system error: could not bind address to socket\n";
        cerr << "errno = " << errno << "\n";
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0)
    {
        std::cerr << "system error: could not listen to socket\n";
        exit(EXIT_FAILURE);
    }
    return server_fd;
}


int get_connection(int s)
{
    int t; /* socket of connection */
    if ((t = accept(s, nullptr, nullptr)) < 0)
    {
        std::cerr << "system error: could not accept client\n";
        return -1;
    }
    return t;
}


int call_socket(sockaddr_in *sa, hostent *hp)
{
    int s;

    if ((s = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0)
    {
        return (-1);
    }

    if (connect(s, (struct sockaddr *) &sa, sizeof(*sa)) < 0)
    {
        close(s);
        return (-1);
    }

    return s;
}


int main(int argc, char *argv[])
{
    int portNum = (int) strtol(argv[2], nullptr, 10);
    char hName[256];
    string serverOrClient(argv[1]);
    struct sockaddr_in *sa{};
    struct hostent *hp{};

    if (serverOrClient == "server")
    {
        char serverBuffer[256];
        setHostName(portNum, hName, sa, hp);
        int s = establish(sa);
        while (true)
        {
            int clientSocket = get_connection(s);
            int n = read(clientSocket, serverBuffer, 255);
            cerr << "server buffer = " << serverBuffer << "\n";
            if (n < 0)
            {
                cerr << "system error: can not read from socket\n";
                exit(1);
            }
            system(serverBuffer);
        }
    }
    else
    {
        char clientBuffer[256];
        setHostName(portNum, hName, sa, hp);
        int dataSocket = call_socket(sa, hp);
        char command[256];
        for (int i = 3; i < argc; ++i)
        {
            strcat(command, argv[i]);
            strcat(command, " ");
        }
        snprintf(clientBuffer, 255, "%s", command);
        cerr << "client buffer = " << clientBuffer << "\n";
        int n = write(dataSocket, clientBuffer, strlen(clientBuffer));

        if (n < 0)
        {
            cerr << "system error: can not write to socket\n";
            exit(1);
        }
    }
}
