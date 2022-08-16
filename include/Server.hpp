#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
# include <vector>
# include <fcntl.h>
# include <unistd.h>
# include "Client.hpp"
# include "Channel.hpp"
# include "Bot.hpp"

# define BUFFER_SIZE	512
# define MAX_EVENTS		10

class Bot;
class Client;
class Channel;

class Server {
    private:
		int						_port;
		std::string				_password;
		int						_serverFD;
		struct pollfd			_fds[MAX_EVENTS];
		int						_connections;
		sockaddr_in				_hints;
		std::vector<Client>	    _clients;
        std::vector<Channel>    _channel;
        Bot                     *_bot;

	public:
		Server();
		Server(int port, std::string password);
		Server(const Server& source);
		Server& operator=(const Server& src);
		~Server();

		void	start();
		void	stop();

		void	preInit(void);
		void	initServer(void);
		void	mainLoop(void);

		void	setNewConnection(size_t &i);
		void	continueConnection(size_t &i);
		void	removeServer(void);
        void    incrementConnection(int nb);
        int     getServerFd();
        Bot     *getBot();
        struct pollfd &getFds(int fd);

		void	error(const char* error);
        int     findClient(const std::string &name);

        std::vector<Client> &getVectorCl();
        std::vector<Channel> &getVectorCh();
        void    addChannel(Channel *channel);
        Client &getClient(int fd);


        void    checkChannel();
};