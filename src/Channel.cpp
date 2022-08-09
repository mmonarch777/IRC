# include "../include/Channel.hpp"

Channel::Channel() {}

Channel::Channel(const std::string& name) : _name(name) {}

Channel::Channel(const Channel& src) { *this = src; }

Channel& Channel::operator=(const Channel& src) {
    if (this != &src) {
        _name = src._name;
        _clients = src._clients;
    }
    return *this;
}

Channel::~Channel() {}

std::string& Channel::getName() { return _name; }

std::vector<Client*>& Channel::getClients() { return _clients; }

void    Channel::setName(const std::string name) { _name = name; }

void    Channel::addClient(Client& client) {
    _clients.push_back(&client);
}
