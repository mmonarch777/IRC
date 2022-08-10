#include "../include/Message.hpp"

Message::Message() {
}

Message::Message(std::string command, std::vector<std::string> params):
        _command(command), _params(params) {
}

Message::Message(const Message& src) { *this = src; }

Message& Message::operator=(const Message& src) {
    if (this != &src) {
        // _prefix = src._prefix;
        _command = src._command;
        _params = src._params;
    }
    return *this;
}

Message::~Message() {}

std::string Message::getCommand() { return _command; }

std::vector<std::string> Message::getParams() { return _params; }

// void    Message::setPrefix(const std::string prefix) { _prefix = prefix; }

void    Message::setCommand(const std::vector<std::string> &param) {
    _command = param.front();
}
void    Message::setParams(const std::vector<std::string> &param) {
    _params = param;
}

bool    Message::isCheckCom() {
    std::string	commands[] = {"PASS", "USER", "NICK", "QUIT", "PRIVMSG", "NOTICE", "JOIN", "PART", "KICK", "BOT"};
    if (std::find(std::begin(commands), std::end(commands), _command) != std::end(commands)){
        return true;
    }

    return false;
}

void	Message::sendReply(Client& client, int flag) {
    std::stringstream	ss;
    ss << flag;
    std::string replyMsg = ":IRC "+ ss.str() + " " + client.getNickname() + " ";

    switch (flag)
    {
    case WELCOME:
        replyMsg += ":Welcom IRC " + client.getNickname() + "!" + client.getUsername() + "@"
                + client.getAddress() + "\r\n";
        break;
    case RPL_MOTDSTART:
        replyMsg += ":- IRC Message of the day - \n";
        break;
    case RPL_MOTD:
        replyMsg += ":- xxx\n";
        break;
    case RPL_ENDOFMOTD:
        replyMsg += ":End of /MOTD command\n";
        break;
    default:
        replyMsg += "Empty reply\n";
        break;
    }
    send(client.getFd(), replyMsg.c_str(), replyMsg.size(), MSG_DONTWAIT);
}

void	Message::sendError(Client& client, Message& msg, int error) {
    std::string errMsg;

    switch (error)
    {
        case ERR_NEEDMOREPARAMS:
            errMsg += msg._command + "  :Not enough parameters\r\n";
            break;
        case ERR_ALREADYREGISTRED:
            errMsg += ":You may not reregister\r\n";
            break;
        case ERR_NOTREGISTERED:
            errMsg += ":You have not reregister\r\n";
            break;
        case ERR_ERRONEUSNICKNAME:
            errMsg += msg._params.front() + " :Erroneus nickname\r\n";
            break;
        case ERR_NICKNAMEINUSE:
            errMsg += msg._params.front() + " :Nickname is already in use\r\n";
            break;
        case ERR_PASSWDMISMATCH:
            errMsg += ":Password incorrect\r\n";
            break;
        case ERR_NORECIPIENT:
            errMsg += ":No recipient given " + this->_params[0] + "\r\n";
            break;
        case ERR_BADCHANNELKEY:
            errMsg += ":Bad channel key " + this->_params[0] + "\r\n";
            break;
        case ERR_NOSUCHCHANNEL:
            errMsg += ":No such channel " + this->_params[0] + "\r\n";
            break;
        case ERR_NOTONCHANNEL:
            errMsg += ":You're not on that channel " + this->_params[0] + "\r\n";
            break;
        case ERR_NOTEXTTOSEND:
            errMsg += ":No text to send " + this->_params[0] + "\r\n";
            break;
    }
    send(client.getFd(), errMsg.c_str(), errMsg.size(), MSG_DONTWAIT);
}

void	Message::cmdPass(Client& client, Message& msg) {
    if (!msg._params.size())
        sendError(client, msg, ERR_NEEDMOREPARAMS);
    else if (client.getStatus() && !client.getNickname().empty())
        sendError(client, msg, ERR_ALREADYREGISTRED);
    else if (!client.getStatus() && msg._params[0] != client.getPassword())
        sendError(client, msg, ERR_PASSWDMISMATCH);
    else
        client.setStatus(true);
}

void	Message::cmdNick(Client& client, Message& msg, Server &server) {
    std::string nick;

    if (msg._params.size() == 0)
        sendError(client, msg, ERR_NEEDMOREPARAMS);
    else if (msg.checkNick(msg._params[0])) //проверить допустимый ли никнейм
        sendError(client, msg, ERR_ERRONEUSNICKNAME);
    else if (msg.checkDuplicate(server)) //проверить, есть ли ник в списке зарегестрированных
        sendError(client, msg, ERR_NICKNAMEINUSE);
    else {
        nick = ":" + client.getNickname() + "!" + client.getUsername() + "@"
                + client.getAddress() + " " + msg._command + " :" + msg._params.front() + "\r\n";
        client.setNickname(msg._params.front());
    }
}

bool    Message::checkDuplicate(Server &server) {
    std::vector<Client> tmp = server.getVectorCl();
    for (std::vector<Client>::iterator it = tmp.begin(); it != tmp.end(); it++) {
        if ((*it).getNickname() == getParams().front())
            return true;
    }
    return false;
}

void	Message::cmdUser(Client& client, Message& msg) {
	if (!msg._params.size())
		sendError(client, msg, ERR_NEEDMOREPARAMS);
	else if (!client.getUsername().empty())
		sendError(client, msg, ERR_ALREADYREGISTRED);
	else
	{
		if (client.getStatus() && !client.getNickname().empty()) {
            client.setUsername(msg._params[0]);
            sendReply(client, WELCOME);
            std::string string = "NEW CLIENT: USER " + client.getUsername() + ", NICK " + client.getNickname();
            std::cout << string << std::endl;
        } else {
            sendError(client, msg, ERR_NOTREGISTERED);
        }
	}
}

int    Message::checkNick(const std::string &nick){
    if (nick.size() > 9)
        return 1;
    for (size_t i = 0; i < nick.size(); ++i)
    {
        if (('A' <= nick[i] && nick[i] <= 'Z') \
			|| ('a' <= nick[i] && nick[i] <= 'z') \
			|| ('0' <= nick[i] && nick[i] <= '9') \
			|| strchr("-[]\\`^{}|", nick[i]))
            continue;
        return 1;
    }
    return 0;
}

void Message::privMsg(Client &client, Server &server) {
    std::string msg;

    if (!client.isCheckRegistration()) {
        sendError(client, *this, ERR_NOTREGISTERED);
        return;
    } else if (this->_params.size() == 1){
        sendError(client, *this, ERR_NOTEXTTOSEND);
        return ;
    } else if (this->_params.size() < 2){
        sendError(client, *this, ERR_NEEDMOREPARAMS);
        return ;
    }
    if (this->_params[0][0] == '#') {
        return;
    } else {
        std::vector<Client> tmp = server.getVectorCl();
        std::vector<Client>::iterator it = tmp.begin();
        std::vector<Client>::iterator ite = tmp.end();
        for (; it != ite; it++) {
            if ((*it).getNickname() == this->_params.front()) {
                msg = ":" + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1 " + getStrParams(1) + "\r\n";
                send((*it).getFd(), msg.c_str(), msg.length() + 1, 0);
                return;
            }
        }
    }
    sendError(client, *this, ERR_NORECIPIENT);
}

std::string Message::getStrParams(int nb) {
    std::string str;
    int size = (int)this->_params.size();
    for (int i = nb; i < size; i++) {
        str += this->_params[i];
        if (i + 1 < size) {
            str += " ";
        }
    }
    return str;
}

void    Message::joinToChannel(Client &client, Server &server) {
    if (!client.isCheckRegistration()) {
        sendError(client, *this, ERR_NOTREGISTERED);
        return;
    }
    if (this->_params[0][0] != '#') {
        sendError(client, *this, ERR_BADCHANNELKEY);
        return;
    } else if (this->_params.empty()) {
        sendError(client, *this, ERR_NEEDMOREPARAMS);
        return;
    }

    if (!checkChannel(server, this->_params.front())) {
        creatNewChannel(server, client);
    } else {

        std::vector<Channel> &tmp = server.getVectorCh();
        std::vector<Channel>::iterator it = tmp.begin();
        std::vector<Channel>::iterator ite = tmp.end();
        for (; it != ite; it++) {
            if ((*it).getName() == this->_params.front()) {
                if (!(*it).isCheckCurFd(client.getFd())) {
                    (*it).setClientsFd(client.getFd());
                    std::string str = ": You JOIN to channel " + this->_params.front() + ". Channel admin is " + (*it).getAdminNick() + " \r\n";
                    send(client.getFd(), str.c_str(), str.length() + 1, 0);
                    str = ": " + client.getNickname() + " JOIN to channel " + this->_params.front() + "\r\n";
                    sendAllToChannel(client, (*it).getClientsFd(), str);
                } else {
                    std::string string = ": You are already in the channel " + this->_params.front();
                    send(client.getFd(), string.c_str(), string.length() + 1, 0);
                }
                return;
            }
        }
    }
}

bool    Message::checkChannel(Server &server, std::string channelName) {
    std::vector<Channel> tmp = server.getVectorCh();
    std::vector<Channel>::iterator it = tmp.begin();
    std::vector<Channel>::iterator ite = tmp.end();

    for(;it != ite; it++) {
        if ((*it).getName() == channelName) {
            return true;
        }
    }
    return false;
}

void Message::creatNewChannel(Server &server, Client &client) {
    Channel *channel = new Channel(this->_params.front(), client.getFd(), client.getNickname());
    server.addChannel(channel);
    delete channel;

    std::cout << ": Created new channel " << this->_params.front() << std::endl;
    std::string tmp = ": You JOIN to channel " + this->_params.front() + ". Channel admin is " + client.getNickname() + " \r\n";
    send(client.getFd(), tmp.c_str(), tmp.length() + 1, 0);
}

void Message::sendAllToChannel(Client &client, std::vector<int> &fds, std::string str) {
    std::vector<int>::iterator it = fds.begin();
    std::vector<int>::iterator ite = fds.end();

    for (; it != ite; it++) {
        if (*it != client.getFd()) {
            send(*it, str.c_str(), str.length() + 1, 0);
        }
    }
}

void Message::outFromChannel(Client &client, Server &server) {
    if (!client.isCheckRegistration()) {
        sendError(client, *this, ERR_NOTREGISTERED);
        return;
    }else if (this->_params[0][0] != '#') {
        sendError(client, *this, ERR_BADCHANNELKEY);
        return;
    } else if (this->_params.empty()) {
        sendError(client, *this, ERR_NEEDMOREPARAMS);
        return;
    }

    if (!checkChannel(server, this->_params.front())) {
        sendError(client, *this, ERR_NOSUCHCHANNEL);
    } else {

        std::vector<Channel> &tmp = server.getVectorCh();
        std::vector<Channel>::iterator it = tmp.begin();
        std::vector<Channel>::iterator ite = tmp.end();
        for (; it != ite; it++) {
            if ((*it).getName() == this->_params.front()) {
                if ((*it).isCheckCurFd(client.getFd())) {
                    (*it).delClientsFd(client.getFd());
                    std::string str = ": You left the channel " + this->_params.front() + " \r\n";
                    send(client.getFd(), str.c_str(), str.length() + 1, 0);
                    str = ": " + client.getNickname() + " LEFT the channel " + this->_params.front() + "\r\n";
                    sendAllToChannel(client, (*it).getClientsFd(), str);
                } else {
                    sendError(client, *this, ERR_NOTONCHANNEL);
                }
                return;
            }
        }
    }
}

