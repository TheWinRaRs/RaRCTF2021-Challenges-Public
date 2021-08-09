//
// Created by Jamie on 12/02/2021.
//

#ifndef CONSOLEGAME_ENETHOST_H
#define CONSOLEGAME_ENETHOST_H
#include <vector>
#include <string>
#include <enet/enet.h>

class ENetHostWrapper {
public:
    int poll();
    std::vector<ENetEvent*> events;
    ~ENetHostWrapper();
protected:
    ENetHost* host;
};

class ENetServer: public ENetHostWrapper {
public:
    ENetServer(const char* addr, int port);
    void SendPacket(std::string data, ENetPeer* host);
    void SendPacket(char* data, int size, ENetPeer* peer);
};

class ENetClient: public ENetHostWrapper {
public:
    ENetClient(const char* addr, int port);
    ~ENetClient();
    void SendPacket(std::string data);
    void SendPacket(char* buf, int size);
private:
    ENetPeer* server;
};

#endif //CONSOLEGAME_ENETHOST_H
