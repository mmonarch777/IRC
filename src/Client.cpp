#include "../include/Client.hpp"

Client::Client() : _status(false), _registration(false) {}

Client::Client(int fd, const std::string& address, const std::string& password):
		_fd(fd), _status(false), _registration(false), _address(address) , _password(password) {}

Client::~Client() {}

Client& Client::operator=(const Client& source) {
	if (this != &source) {
		_fd = source._fd;
		_address = source._address;
		_status = source._status;
        _registration = source._registration;
		_buffer = source._buffer;
		_password = source._password;
		_nickname = source._nickname;
		_username = source._username;
	}
	return *this;
}

int		Client::getFd() { return _fd; }

bool	Client::getStatus() { return _status; }

std::string&	Client::getAddress() { return _address; }

std::string&	Client::getBuffer() { return _buffer; }

std::string&	Client::getNickname() { return _nickname; }

std::string&	Client::getPassword() { return _password; }

std::string& Client::getUsername() { return _username; }

void	Client::setStatus(bool status) { _status = status; }

void	Client::setAddress(const std::string& address) { _address = address; }

void	Client::setBuffer(const std::string& buffer) { _buffer = buffer; }

void	Client::setNickname(const std::string& nickname) { _nickname = nickname; }

void	Client::setPassword(const std::string& password) { _password = password; }

void	Client::setUsername(const std::string& username) { _username = username; }

void    Client::setRegistration(bool reg) {
    _registration = reg;
}

bool    Client::getRegistration() {
    return _registration;
}

bool    Client::isCheckRegistration() {
    return _status && !_username.empty() && !_nickname.empty();
}

void    Client::execMessage(Server &server) {
	Message msg;

    server.checkChannel();
	parceBuffer(msg);
    std::vector<std::string> &tmp = msg.getVector();
    std::vector<std::string>::iterator begin = tmp.begin();
    std::vector<std::string>::iterator end = tmp.end();
    while (begin != end) {
        std::vector<std::string> param;
        size_t pos = 0;
        while ((pos = (*begin).find(' ')) != std::string::npos) {
            param.push_back((*begin).substr(0, pos));
            (*begin).erase(0, pos + 1);
        }
        pos = 0;
        while ((pos = (*begin).find('\n')) != std::string::npos) {
            param.push_back((*begin).substr(0, pos - 1));
            (*begin).erase(0, pos + 1);
        }
        msg.clearCommand();
        msg.setCommand(param);
        param.erase(param.begin());
        msg.setParams(param);

        if (!msg.isCheckCom()) {
            std::string mes = msg.getCommand() + ": Command don't exist!!! Try again..\r\n";
            send(this->_fd, mes.c_str(), mes.length() + 1, 0);
            return;
        }
        if (msg.getCommand() == "PASS")
            msg.cmdPass(*this);
        else if (msg.getCommand() == "USER")
            msg.cmdUser(*this);
        else if (msg.getCommand() == "NICK")
            msg.cmdNick(*this, server);
        else if (msg.getCommand() == "JOIN")
            msg.joinToChannel(*this, server);
        else if (msg.getCommand() == "PRIVMSG")
            msg.privMsg(*this, server);
        else if (msg.getCommand() == "PART")
            msg.outFromChannel(*this, server);
        else if (msg.getCommand() == "NOTICE")
            msg.msgToChannel(*this, server);
        else if (msg.getCommand() == "KICK")
            msg.kickFromChannel(*this, server);
        else if (msg.getCommand() == "QUIT")
            msg.quiteFromServer(*this, server);
    if (!getRegistration() && isCheckRegistration()) {
		msg.sendReply(*this, RPL_MOTDSTART);
		msg.sendReply(*this, RPL_MOTD);
		msg.sendReply(*this, RPL_ENDOFMOTD);
        setRegistration(true);
	}
        begin++;
    }

}

//void	Client::parceBuffer(Message &msg)
//{
//    std::vector<std::string> param;
//
//    size_t pos = 0;
//    while ((pos = this->_buffer.find(' ')) != std::string::npos) {
//        param.push_back(this->_buffer.substr(0, pos));
//        this->_buffer.erase(0, pos + 1);
//    }
//    pos = 0;
//    while ((pos = this->_buffer.find('\n')) != std::string::npos) {
//        param.push_back(this->_buffer.substr(0, pos));
//        this->_buffer.erase(0, pos + 1);
//    }
//    msg.setCommand(param);
//    param.erase(param.begin());
//	msg.setParams(param);
//    this->_buffer.clear();
//}

void	Client::parceBuffer(Message &msg)
{
    std::vector<std::string> param;

    size_t pos = 0;
    while ((pos = this->_buffer.find('\n')) != std::string::npos) {
        param.push_back(this->_buffer.substr(0, pos + 1));
        this->_buffer.erase(0, pos + 1);
    }
    msg.setAllCommand(param);
}


