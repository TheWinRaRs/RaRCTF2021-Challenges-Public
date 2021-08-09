#pragma once

#define RED "[0;31m"
#define GREEN "[0;32m"
#define BLUE "[0;34m"
#define WHITE "[0;37m"

#define UNDERLINE "\033[4m"
#define CLOSEUNDERLINE "\033[0m"

#define nchan 9
#define nuser 23
#define nrole 3

typedef struct Role {
   int id;
   char col[7];
   char name[40];
   void (*update_role)(struct Role *);
} role;


typedef struct Channel {
    int   id;
    int   fd;
    int  allowedRole;
    char  title[50];
} channel;

typedef struct User {
   int id;
   int role;
   void (*set_role)(struct User *, int);
   char username[32];
   void (*update_username)(struct User *);
} user;

void setup_channels();

void setup_users();

void setup_roles();

void set_role(user *usr, int role);

void update_role(role *rol);

void promote_user(user *usr);

void init();

void print_role(role therole);

void print_user(user *usr);

void print_user_info(user *usr);

void print_channel(channel *chan);

void update_username(user *usr);

void clear (void);

