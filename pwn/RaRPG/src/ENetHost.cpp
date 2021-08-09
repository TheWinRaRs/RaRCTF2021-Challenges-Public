//
// Created by Jamie on 12/02/2021.
//
#include <stdio.h>

#include "ENetHost.h"


int ENetHostWrapper::poll() {
    int count = 0;
    auto* event = new ENetEvent;
    int res = enet_host_service(this->host, event, 40);
    while (res > 0) {
        count++;
        this->events.push_back(event);
        event = new ENetEvent;
        res = enet_host_service(this->host, event, 40);
    }
    if (res < 0) {
        puts("Error");
        printf("\n\n\n\n\n");
        exit(0);
    }
    return count;
}

ENetHostWrapper::~ENetHostWrapper() {
    enet_host_destroy(this->host);
}


ENetServer::ENetServer(const char *addr, int port) {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    this->host = enet_host_create(&address, // Bind address
                              32,       // Max clients
                              2,        // Max channels
                              0,        // Download limit
                              0);
    if (this->host == NULL) {
        fprintf(stderr, "Failed to bind server");
    }
}

void ENetServer::SendPacket(std::string data, ENetPeer* peer) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.size(), ENET_PACKET_FLAG_RELIABLE);
    if (enet_peer_send(peer, 0, packet) < 0) {
        puts("Failed to send");
    }
    enet_host_flush(this->host);
}

void ENetServer::SendPacket(char* data, int size, ENetPeer* peer) {
    ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
    if (enet_peer_send(peer, 0, packet) < 0) {
        puts("Failed to send");
    }
    enet_host_flush(this->host);
}

ENetClient::ENetClient(const char *addr, int port) {
    ENetAddress address;
    enet_address_set_host(&address, addr);
    address.port = port;
    this->host = enet_host_create(NULL, // Bind address
                                  1,       // Max clients
                                  2,        // Max channels
                                  0,        // Download limit
                                  0);
    if (this->host == NULL) {
        fprintf(stderr, "Failed to create client");
        return;
    }
    this->server = enet_host_connect(this->host, &address, 2, 0);
    if (this->server == nullptr ) {
        fprintf(stderr, "Couldn't connect to server\n");
        return;
    }
    ENetEvent event;
    if (enet_host_service(this->host, &event, 1000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT) {
        // printf("Connection succeeded");
    } else {
        enet_peer_reset(this->server);
        fprintf(stderr, "Connection failed");
        return;
    }
}

ENetClient::~ENetClient() {
    ENetEvent event;
    enet_peer_disconnect(this->server, 0);
    while (enet_host_service (this->host, &event, 3000) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy (event.packet);
                break;
            default:
                return;

        }
    }
}

void ENetClient::SendPacket(std::string data) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.size(), ENET_PACKET_FLAG_RELIABLE);
    if (enet_peer_send(this->server, 0, packet) < 0) {
        puts("Failed to send");
    }
    enet_host_flush(this->host);
}

void ENetClient::SendPacket(char* buf, int size) {
    ENetPacket* packet = enet_packet_create(buf, size, ENET_PACKET_FLAG_RELIABLE);
    if (enet_peer_send(this->server, 0, packet) < 0) {
        puts("Failed to send");
    }
    enet_host_flush(this->host);
}