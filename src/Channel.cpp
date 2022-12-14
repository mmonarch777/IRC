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

void Channel::setClientsFd(int fd) {
    _clientsFd.push_back(fd);
}

void Channel::setAdminFd(int fd) {
    _adminFd = fd;
}
void Channel::setAdminNick(std::string name) {
    _adminNick = name;
}

std::string Channel::getAdminNick() {
    return _adminNick;
}

std::vector<int>& Channel::getClientsFd() {
    return _clientsFd;
}

int Channel::getAdminFd() {
    return _adminFd;
}

bool Channel::isCheckCurFd(int fd) {
    std::vector<int>::iterator it = _clientsFd.begin();
    std::vector<int>::iterator ite = _clientsFd.end();

    for (; it != ite; it++) {
        if (*it == fd) {
            return true;
        }
    }
    return false;
}

void Channel::delClientsFd(int fd) {
    std::vector<int>::iterator it = _clientsFd.begin();
    std::vector<int>::iterator ite = _clientsFd.end();

    for (;it != ite; it++) {
        if(*it == fd) {
            _clientsFd.erase(it);
        }
    }
}