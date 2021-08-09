#include <stdio.h>
#include <enet/enet.h>
#include <iostream>
#include <fstream>

#include "ENetHost.h"
#include "game.pb.h"

ENetServer* server;
using namespace consolegame;
bool setup_enet() {
    if (enet_initialize() != 0) {
        fprintf(stderr, "Couldn't start networking\n");
        return false;
    }
    atexit(enet_deinitialize);
    server = new ENetServer("0.0.0.0", 1337);
    return true;
}

Level* handle_level_request(LevelRequest req) {
    char* path = (char *)calloc(4096, 1);
    sprintf(path, "levels/%d", req.id());
    std::ifstream f(path);
    if (!f.good()) {
        printf("Bad file path %s\n", path);
        return NULL;
    }

    std::string* level_data = new std::string;
    std::string line;
    Level* level = new Level;
    PlayerPos* pos = new PlayerPos;
    std::vector<Level_Door*> doorsVec;
    int x, y, id;
    std::getline(f, line);
    sscanf(line.c_str(), "%d %d", &y, &x);
    if (req.is_start()) {
        pos->set_x(x);
        pos->set_y(y);
        level->set_allocated_start_pos(pos);
    }
    while (std::getline(f, line)) {
        if (sscanf(line.c_str(), "%d %d %d", &y, &x, &id) < 3) {
            break;
        }
        Level_Door* door = new Level_Door;
        door->set_y(y);
        door->set_x(x);
        door->set_id(id);
        doorsVec.push_back(door);

    }
    level_data->assign((std::istreambuf_iterator<char>(f)),
                       (std::istreambuf_iterator<char>()));
    level->set_allocated_leveldata(level_data);
    for (auto door: doorsVec) {
        Level_Door* doors = level->add_doors();
        doors->set_y(door->y());
        doors->set_x(door->x());
        doors->set_id(door->id());
    }
    return level;
}

void handle_packet(ENetEvent* event) {
    ENetPacket* packet = event->packet;
    Request req;
    Response resp;
    if (!req.ParseFromArray((char*)packet->data, packet->dataLength)) {
        fprintf(stderr, "Malformed packet!");
        return;
    }
    if (req.has_lreq()) {
        // printf("Level request: %d\n", req.lreq().id());
        resp.set_allocated_level(handle_level_request(req.lreq()));
        int size = resp.ByteSizeLong();
        void* p = malloc(size);
        resp.SerializeToArray(p, size);
        server->SendPacket((char *)p, size, event->peer);
    }
}

void handle_events() {
    std::vector<ENetEvent*>::iterator it;
    for (it = server->events.begin(); it != server->events.end();) {
        switch ((*it)->type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("Connect\n");
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                handle_packet((*it));
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Disconnect\n");
                (*it)->peer->data = NULL;
                break;
            default:
                break;
        }
        it =  server->events.erase(it);
    }
}



int main () {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (!setup_enet()) {
        return -1;
    }
    puts("Server starting");
    while (true) {
        server->poll();
        handle_events();
    }
}