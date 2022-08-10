#pragma once

# include "Client.hpp"

class Client;

class Channel {
    private:
        std::string             _name;
        int                     _adminFd;
        std::string             _adminNick;
        std::vector<int>        _clientsFd;

    public:
        Channel();
        Channel(std::string& name, int fd, std::string adminNick);
        Channel(const Channel& src);
        Channel& operator=(const Channel& src);
        ~Channel();

        std::string&            getName();
        void                    setName(const std::string& name);
        void                    setClientsFd(int fd);
        void                    delClientsFd(int fd);

        std::vector<int>&       getClientsFd();
        std::string             getAdminNick();
        bool                    isCheckCurFd(int fd);


};