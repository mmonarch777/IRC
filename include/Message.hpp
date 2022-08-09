#pragma once

# include <sys/socket.h>
# include <map>
# include "Client.hpp"
# include "Server.hpp"
# include "Channel.hpp"
# include <sstream>

# define WELCOME                1

# define ERR_NORECIPIENT        411
# define ERR_ERRONEUSNICKNAME	432
# define ERR_NICKNAMEINUSE		433
# define ERR_NOTREGISTERED		451
# define ERR_NEEDMOREPARAMS		461
# define ERR_ALREADYREGISTRED	462
# define ERR_PASSWDMISMATCH     464

#define RPL_ENDOFMOTD			376
#define RPL_MOTDSTART			375
#define RPL_MOTD				372

class Server;
class Client;
class Channel;

class Message {
    private:
        std::string                 _command;
        std::vector<std::string>    _params;

    public:
        Message();
        Message(std::string command, std::vector<std::string> params);
        Message(const Message& src);
        Message& operator=(const Message& src);
        ~Message();

        std::string getCommand();
        std::vector<std::string> getParams();

        void    setCommand(const std::vector<std::string> &param);
        void    setParams(const std::vector<std::string> &param);
        void    setAllComands();
        bool    isCheckCom(Server &server);

		void	sendReply(Client& client, int flag);
		void	sendError(Client& client, Message& msg, int error);
        void	cmdPass(Client& client, Message& msg);
		void	cmdNick(Client& client, Message& msg, Server &server);
		void	cmdUser(Client& client, Message& msg);

        int     checkNick(const std::string &nick);
        bool    checkDuplicate(Server &server);

        void    joinToChannel(Client &client, Server &server);
        bool    checkChannel(Server &server, std::string channelName);
        void    creatNewChannel(Server &server, Client &client);

        void    privMsg(Client &client, Server &server);
};