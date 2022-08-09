# include "../include/Channel.hpp"

Channel::Channel() {}

Channel::Channel(std::string& name, int fd, std::string adminNick)
        : _name(name),
        _adminFd(fd),
        _adminNick(adminNick) {
    _clientsFd.push_back(fd);
}

Channel::Channel(const Channel& src) { *this = src; }

Channel& Channel::operator=(const Channel& src) {
    if (this != &src) {
        _name = src._name;
        _adminFd = src._adminFd;
        _adminNick = src._adminNick;
        _clientsFd.clear();
        _clientsFd = src._clientsFd;
    }
    return *this;
}

Channel::~Channel() {}

std::string& Channel::getName() { return _name; }

void    Channel::setName(const std::string& name) { _name = name; }

