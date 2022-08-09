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
	// if (client.getNickname().empty())
	// 	sendError(client, msg, ERR_NOTREGISTERED);
	if (!msg._params.size())
		sendError(client, msg, ERR_NEEDMOREPARAMS);
	else if (!client.getUsername().empty())
		sendError(client, msg, ERR_ALREADYREGISTRED);
	else
	{
		client.setUsername(msg._params[0]);
		// sendReply(client, WELCOME);
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

void    Message::joinToChannel(Client &client, Server &server) {
    if (this->_params[0][0] != '#') {
        std::cout << "Error params" << std::endl;
    }

    if (!checkChannel(server, this->_params.front())) {
        creatNewChannel(server, client);
    } else {
        std::cout << "Hellow world!" << std::endl;
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
}

