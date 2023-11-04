#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

using namespace std;

class Reactor
{
public:
    Reactor() {}
    ~Reactor() {}

    void RegisterSocket(int socket_fd, EventHandler *handler)
    {
        sockets_.push_back(socket_fd);
        handlers_.insert(make_pair(socket_fd, handler));
    }

    void UnregisterSocket(int socket_fd)
    {
        sockets_.erase(std::remove(sockets_.begin(), sockets_.end(), socket_fd));
        handlers_.erase(socket_fd);
    }

    void StartEventLoop()
    {
        while (true)
        {
            // Poll for incoming activity on all registered sockets
            pollfd *pollfds = new pollfd[sockets_.size()];
            for (int i = 0; i < sockets_.size(); i++)
            {
                pollfds[i].fd = sockets_[i];
                pollfds[i].events = POLLIN;
            }

            int poll_result = poll(pollfds, sockets_.size(), -1);
            if (poll_result < 0)
            {
                cerr << "Error polling sockets" << endl;
                exit(1);
            }

            // Handle events for all sockets that have activity
            for (int i = 0; i < sockets_.size(); i++)
            {
                if ((pollfds[i].revents & POLLIN) || (pollfds[i].revents & POLLOUT))
                {
                    EventHandler *handler = handlers_[sockets_[i]];
                    handler->HandleEvent();
                }
            }

            delete[] pollfds;
        }
    }

private:
    vector<int> sockets_;
    map<int, EventHandler *> handlers_;
};

class HttpEventHandler : public EventHandler
{
public:
    HttpEventHandler(int socket_fd) : socket_fd_(socket_fd) {}

    void HandleEvent() override
    {
        // Read the HTTP request from the socket
        char request[1024];
        int bytes_read = read(socket_fd_, request, sizeof(request));
        if (bytes_read < 0)
        {
            cerr << "Error reading request from client" << endl;
            exit(1);
        }

        // Parse the HTTP request
        string request_method;
        string request_path;
        stringstream(request) >> request_method >> request_path;

        // Serve the requested file
        string file_path = "htdocs/" + request_path;
        ifstream file(file_path);
        if (!file.is_open())
        {
            // TODO: Send a 404 Not Found response
            return;
        }

        // Send the HTTP response header
        string response_header = "HTTP/1.1 200 OK\nContent-Length: " + to_string(file.rdbuf()->in_avail()) + "\n\n";
        send(socket_fd_, response_header.c_str(), response_header.size(), 0);

        // Send the file contents
        char buffer[1024];
        while (file.read(buffer, sizeof(buffer)))
        {
            send(socket_fd_, buffer, file.gcount(), 0);
        }

        // Close the socket
        close(socket_fd_);
    }

private:
    int socket_fd_;
};



int main()
{
    Reactor reactor;

    // Create a socket and bind it to port 80
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        cerr << "Error creating socket" << endl;
        exit(1);
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(80);
    address.sin_addr.s_addr = INADDR_ANY;

    int bind_result = bind(socket_fd, (sockaddr *)&address, sizeof(address));
    if (bind_result < 0)
    {
        cerr << "Error binding socket" << endl;
    }
    reactor.StartEventLoop();
}