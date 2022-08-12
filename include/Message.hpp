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

# define ERR_BADCHANNELKEY      475
# define ERR_NOSUCHCHANNEL      403
# define ERR_NOTONCHANNEL       442
# define ERR_NOTEXTTOSEND       412

# define ERR_CHANOPRIVSNEEDED   482
# define ERR_NOTFOUNDCLIENT     488

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
        bool    isCheckCom();

		void	sendReply(Client& client, int flag);
		void	sendError(Client& client, Message& msg, int error);
        void	cmdPass(Client& client);
		void	cmdNick(Client& client, Server &server);
		void	cmdUser(Client& client);

        int     checkNick(const std::string &nick);
        bool    checkDuplicate(Server &server);
        std::string getStrParams(int nb);

        void    joinToChannel(Client &client, Server &server);
        bool    checkChannel(Server &server, std::string channelName);
        void    creatNewChannel(Server &server, Client &client);
        void    sendAllToChannel(Client &client, std::vector<int> &fds, std::string str);

        void    privMsg(Client &client, Server &server);
        void    msgToChannel(Client &client, Server &server);
        void    outFromChannel(Client &client, Server &server);
        void    kickFromChannel(Client &client, Server &server);
        void    quiteFromServer(Client &client, Server &server);
};