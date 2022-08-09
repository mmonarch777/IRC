#pragma once

# include "Client.hpp"

class Client;

class Channel {
    private:
        std::string             _name;
        int                     _adminFd;
        std::vector<Client*>    _clients;

    public:
        Channel();
        Channel(const std::string& name);
        Channel(const Channel& src);
        Channel& operator=(const Channel& src);
        ~Channel();

        std::string&            getName();
        std::vector<Client*>&   getClients();
        void                    setName(const std::string name);
        void                    addClient(Client& client);

};