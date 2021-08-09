#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "harmony.h"

channel *channels[nchan];
user *users[nuser];
role *roles[nrole];

user *current_user;

static const char *channelNames[] = {"general", "pwn", "secret-admin-chat", "team-locator-inator", "crypto", "spam", "rev", "misc", "web"};
static const int channelRoles[nchan] = {3, 3, 0, 3, 3, 3, 3, 3, 3};
static const char *userNames[] = {"Tony", "UnbelievaBoat", "proleethaxor1337", "wiwam845",
                                  "Pig2", "Quintec", "diogoctf", "floralelements", "curdcompiler",
                                   "rak1507", "fieldracoon", "AlOnKali", "jammy", "mocksower",
                                  "Day", "Rag", "MovingDessert", "das", "Tango", "Strellic", "mop0",
                                  "JoJo", "PotatoK", "chop0"};
static const int userRoles[nuser] = {0, 1, 2, 0,
                                     0, 0, 2, 2, 2,
                                     0, 0, 2, 0, 2,
                                     0, 2, 2, 0, 2, 0,
                                     0, 0, 0};
static const char *roleNames[] = {"Organiser", "Bot", "Participant"};
static const char *roleCols[nrole] = {RED, BLUE, GREEN};


void setup_channels() {
   for (int i=0; i < nchan; i++) {
      channel *chan = malloc(sizeof(channel));
      strcpy(chan->title, channelNames[i]);
      chan->id = i;
      char fn[60] = "channels/";
      strcat(fn, channelNames[i]);
      chan->fd = open(fn, O_RDONLY);
      if (chan->fd == -1) {
         printf("Failed to open %s\n", fn);
         exit(-1);
      }
      chan->allowedRole = channelRoles[i];
      channels[i] = chan;
   }
}

void setup_users() {
   for (int i=0; i < nuser; i++) {
      user *usr = malloc(sizeof(user));
      strcpy(usr->username, userNames[i]);
      usr->id = i;
      usr->role = userRoles[i];
      usr->set_role = &set_role;
      usr->update_username = &update_username;
      users[i] = usr;
   }
}

void setup_roles() {
   for (int i=0; i < nrole; i++) {
      role *rol = malloc(sizeof(role));
      strcpy(rol->name, roleNames[i]);
      strncpy(rol->col, roleCols[i], 6);
      rol->id = i;
      rol->update_role = &update_role;
      roles[i] = rol;
   }
}

void clear (void) {
  while ( getchar() != '\n' );
}

void set_role(user *usr, int role) {
    usr->role = role;
}

void update_username(user *usr) {
    printf("Enter new username: ");
    char buf[40];
    fgets(buf, 40, stdin);
    buf[strlen(buf)-1] = 0;
    strncpy(usr->username, buf, 40);
}

void update_role(role *rol) {
    printf("Enter new role name: ");
    fgets(rol->name, 40, stdin);
}

void init() {
   setup_roles();
   setup_channels();
   setup_users();
}

void print_role(role therole) {
   printf("\033%s%s\n", therole.col, therole.name);
}

void print_user(user *usr) {
   printf("\033%s%s", roles[usr->role]->col, usr->username);
}

void print_user_info(user *usr) {
   printf("Username: %s\nUser ID and Role: %d - %s\n", usr->username, usr->role, roles[usr->role]->name);
}

void print_channel(channel *chan) {
    if (current_user->role > chan->allowedRole) {
        puts("Not allowed to see this channel!");
        return;
    }
   int userid;
   char msg[500];
   lseek(chan->fd, 0, SEEK_SET);
   FILE* fp = fdopen(chan->fd, "r");
   printf(UNDERLINE "%s\n" CLOSEUNDERLINE, chan->title);
   while (fscanf(fp, "%d:%[^\n]s", &userid, msg) > 0) {
      print_user(users[userid]);
      printf("\033" WHITE ": %s\n", msg);
   }
}
