#pragma warning(push)
#pragma warning(disable : 4005)
#include <enet/enet.h>
#include <curses.h>
#pragma warning(pop)
#include <cstdlib>

#include "ENetHost.h"
#include "game.pb.h"

#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h> // for STDOUT_FILENO

using namespace consolegame;

void handle_events(ENetClient* client);

enum tiles {EMPTY=' ', WATER='~', PLAYER='*', DOOR='X', WALL='W'};
WINDOW * mainwin;
Level level;
typedef struct player {
    int y;
    int x;
    int level;
} player;

player current_player;


bool move_okay(int y, int x) {
    int test_char = mvinch(y, x);
    return ((test_char == tiles::EMPTY) || (test_char == tiles::DOOR));
}

void draw_level(const char* data, int size) {
    clear();
    move(1, 1);
    int x, y;
    for (int i = 0; i < size; i++) {
        if (data[i] == '\n') {
            getyx(stdscr, y, x);
            move(y+1, 1);
        } else {
            addch(data[i]);
        }
    }

    move(current_player.y, current_player.x);
    addch(tiles::PLAYER);
    border(0,0,0,0,0,0,0,0);
    refresh();
}

void request_level(ENetClient* client, bool start) {
    Request req;
    auto* lreq = new LevelRequest;
    lreq->set_id(current_player.level);
    lreq->set_is_start(start);
    req.set_allocated_lreq(lreq);
    int size = req.ByteSizeLong();
    void *p = malloc(size);
    req.SerializeToArray(p, size);
    client->SendPacket((char*)p, size);

}

void client_loop(ENetClient* client) {
    while (true) {
        client->poll();
        handle_events(client);
        Level_Door door;
        int chr;
        //if (mvinch(current_player.y, current_player.x) == tiles::DOOR) {
        for (auto door: level.doors()) {
            if (door.x()+1 == current_player.x && door.y() == current_player.y) {
                current_player.level = door.id();
                request_level(client, true);
                goto loop_end;
            }
        }
        //}
        request_level(client, false);

        chr = getch();
        switch (chr) {
            case KEY_UP:
                if (move_okay(current_player.y-1, current_player.x)) {
                    current_player.y--;
                }
                break;
            case KEY_DOWN:
                if (move_okay(current_player.y+1, current_player.x)) {
                    current_player.y++;
                }
                break;
            case KEY_LEFT:
                if (move_okay(current_player.y, current_player.x-1)) {
                    current_player.x--;
                }
                break;
            case KEY_RIGHT:
                if (move_okay(current_player.y, current_player.x+1)) {
                    current_player.x++;
                }
                break;
            case 27:
                exit(-1);
        }

        loop_end:;
    }

}

void handle_packet(ENetEvent* event) {
    ENetPacket* packet = event->packet;
    Response resp;
    if (!resp.ParseFromArray((char*)packet->data, packet->dataLength)) {
        fprintf(stderr, "Malformed packet!");
        return;
    }

    if (resp.has_level()) {
        level = resp.level();
        std::string data(level.leveldata());
        if (level.has_start_pos()) {
            current_player.x = level.start_pos().x() + 1;
            current_player.y = level.start_pos().y() + 1;
        }
        draw_level(data.c_str(), data.size());
    }
}

void handle_events(ENetClient* client) {
    std::vector<ENetEvent*>::iterator it;
    for (it = client->events.begin(); it != client->events.end();) {
        switch ((*it)->type) {
            case ENET_EVENT_TYPE_CONNECT:
                puts("Got a connection");
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                handle_packet((*it));
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                puts("Got a disconnect");
                (*it)->peer->data = nullptr;
                break;
            default:
                break;
        }
        it = client->events.erase(it);
    }
}

int main(int argc, char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(-1);
    }
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    int port = std::stoi(argv[2]);
    if ( (mainwin = initscr()) == nullptr ) {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(EXIT_FAILURE);
    }

    keypad(stdscr, TRUE);
    cbreak();
    noecho();

    resize_term(size.ws_row - 2, size.ws_col - 5);

    curs_set(0);
    refresh();

    if (enet_initialize() != 0) {
        fprintf(stderr, "Couldn't start networking\n");
        return 0;
    }
    atexit(enet_deinitialize);

    ENetClient* client;
    client = new ENetClient(argv[1], port);
    current_player.level = 1;
    request_level(client, true);
    client_loop(client);

    delete client;
    delwin(mainwin);
    endwin();
    refresh();

    return EXIT_SUCCESS;
}
