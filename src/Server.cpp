#include "../include/Server.hpp"

bool	g_status;

void	sighandler(int signum)
{
	if (signum == SIGINT)
	{
		g_status = false;
		std::cout << '\n';
	}
}

Server::Server(void) : _port(6667), _password("pass"), _serverFD(-1), _connections(0) {}

Server::Server(int port, std::string password) : _port(port), _password(password), _serverFD(-1), _connections(0) {}

Server::Server(const Server& source) { *this = source; }

Server::~Server() { removeServer(); }

Server& Server::operator=(const Server& src) {
	if (this != &src) {
		_port = src._port;
		_password = src._password;
		_serverFD = src._serverFD;
		_connections = src._connections;
	}
	return *this;
}

int Server::getServerFd() {
    return _fds[0].fd;
}

void	Server::start() {
	g_status = true;
	signal(SIGINT, &sighandler);
	preInit();
	initServer();
	mainLoop();
}

void	Server::stop() { g_status = false; }

void	Server::preInit(void) {
	_connections = 1;
	for (size_t i = 0; i < (size_t)MAX_EVENTS; i++) //создвем массив nfds структур
		_fds[i].fd = -1;
	_fds[0].events = POLLIN;
	_fds[0].revents = 0;
}

void	Server::initServer() {
	if ((_serverFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) // создаем конечную точку соединения (сокет)
		error("Error: socket!");

	int			optval = 1;
	if (setsockopt(_serverFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) // устанавливаем флаг в сокете
		error("Error: setsockopt!");
	_hints.sin_family = AF_INET;
	_hints.sin_port = htons(_port);
	_hints.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "127.0.0.1", &_hints.sin_addr);
	if (bind(_serverFD, (sockaddr *)&_hints, sizeof(_hints)) < 0) // присваиваем сокету локальный адрес
		error("Error: bind!");

	if (listen(_serverFD, SOMAXCONN) < 0) // слушаем соединение на сокете
		error("Error: listen!");
	_fds[0].fd = _serverFD;
	fcntl(_fds[0].fd, F_SETFL, O_NONBLOCK);
}

void	Server::mainLoop(void) {
	std::cout << "Server started!\n";
	while(g_status) {
		if (poll(_fds, _connections, -1) < 0) {
			if (!g_status) {
				std::cout << "Exit\n";
				break ;
			}
			error("Error: poll!");
		}
		for (size_t i = 0; i < (size_t )_connections; i++) {
			if (_fds[i].fd > 0 && (_fds[i].revents & POLLIN) == POLLIN) {
				g_status = false;
				if (i == 0)
					setNewConnection(i);
				else
					continueConnection(i);
			}
		}
	}
}

void	Server::setNewConnection(size_t &i) {
	char str[INET_ADDRSTRLEN];
    g_status = true;

	_fds[_connections].fd = accept(_fds[i].fd, NULL, NULL); // принимаем соединение на сокете
	inet_ntop(AF_INET, &(_hints.sin_addr), str, INET_ADDRSTRLEN);
    Client *client = new Client(_fds[_connections].fd, str, _password);
    _clients.push_back(*client);
    delete client;
	std::cout << "New client #" << _fds[_connections].fd << " from " << str << ":" << _port << '\n';
	_fds[_connections].events = POLLIN;
	_fds[_connections].revents = 0;
	_connections += 1;
}


void	Server::continueConnection(size_t &i) {
	char buf[BUFFER_SIZE];
	Client&	client = _clients[i - 1];
    g_status = true;

	memset(buf, 0, BUFFER_SIZE);
	int readed = recv(_fds[i].fd, buf, BUFFER_SIZE, MSG_DONTWAIT);
	_fds[i].revents = 0;
	if (!readed) {
		std::cout << "Client #" << _fds[i].fd << " Disconnected!" << std::endl; //работатет некорректно
	    _clients.erase(_clients.begin() + (int)i - 1);
		_fds[i].fd = -1;
		_connections -= 1;
        return;
	}
	buf[readed] = 0;
    client.setBuffer(buf);
	client.execMessage(*this);
	_fds[i].revents = 0;
}

void	Server::removeServer(void) {
	std::vector<Client>::iterator it;

	for(it = _clients.begin(); it != _clients.end(); ++it)
		close(_fds[(*it).getFd()].fd);
	close(_serverFD);
}

void	Server::error(const char* error) {
	removeServer();
	std::cout << error << std::endl;
	exit(EXIT_FAILURE);
}

std::vector<Client> &Server::getVectorCl() {
    return _clients;
}

std::vector<Channel> &Server::getVectorCh() {
    return _channel;
}

void Server::addChannel(Channel *channel) {
    _channel.push_back(*channel);
}

int Server::findClient(const std::string &nickName) {
    std::vector<Client> tmp = getVectorCl();
    std::vector<Client>::iterator it = tmp.begin();
    std::vector<Client>::iterator ite = tmp.end();
    for (; it != ite; it++) {
        if ((*it).getNickname() == nickName) {
            return (*it).getFd();
        }
    }
    return 0;
}

void Server::incrementConnection(int nb) {
    _connections += nb;
}
