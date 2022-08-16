#pragma once

#include <iostream>
#include "Server.hpp"
#include "Client.hpp"

class Server;
class Client;

class Bot {
    typedef std::vector<Client>::iterator iterClient;
private:
    Bot(Bot const &other);
    Bot &operator=(Bot const &other);

public:
    Bot();
    ~Bot();

    void getInfoClient(Client &client, Server &server);

    std::string getLine(std::string str);
    std::string getStrFd(int fd);
    void getInfoBot(Client &client, Server &server);
};