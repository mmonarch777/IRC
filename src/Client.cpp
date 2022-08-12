#include "../include/Client.hpp"

Client::Client() : _status(false) {}

Client::Client(int fd, const std::string& address, const std::string& password):
		_fd(fd), _status(false), _address(address) , _password(password) {}

Client::~Client() {}

Client& Client::operator=(const Client& source) {
	if (this != &source) {
		_fd = source._fd;
		_address = source._address;
		_status = source._status;
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

bool    Client::isCheckRegistration() {
    return _status && !_username.empty() && !_nickname.empty();
}

void    Client::execMessage(Server &server) {
	Message msg;

	parceBuffer(msg);
    if (!msg.isCheckCom()) {
        std::string mes = msg.getCommand() + ": Command don't exist!!! Try again..\r\n";
        send(this->_fd, mes.c_str(), mes.length() + 1, 0);
        return;
    }
    if (msg.getCommand() == "PASS")
        msg.cmdPass(*this, msg);
    else if (msg.getCommand() == "USER")
        msg.cmdUser(*this, msg);
    else if (msg.getCommand() == "NICK")
        msg.cmdNick(*this, msg, server);
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
//    if (isCheckRegistration()) {
//		msg.sendReply(*this, RPL_MOTDSTART);
//		msg.sendReply(*this, RPL_MOTD);
//		msg.sendReply(*this, RPL_ENDOFMOTD);
//	}
}

void	Client::parceBuffer(Message &msg)
{
    std::string com = _buffer;
    std::vector<std::string> param;

    size_t pos = 0;
    while ((pos = com.find(' ')) != std::string::npos) {
        param.push_back(com.substr(0, pos));
        com.erase(0, pos + 1);
    }
    pos = 0;
    while ((pos = com.find('\n')) != std::string::npos) {
        param.push_back(com.substr(0, pos));
        com.erase(0, pos + 1);
    }
    msg.setCommand(param);
    param.erase(param.begin());
	msg.setParams(param);
}

