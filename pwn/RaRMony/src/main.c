#include <stdio.h>
#include <stdlib.h>

 #include "harmony.h"
#include "logo.h"
/*
Users have a role integer. Lower roles are more privileged
If a users role is at or below a channels role number, they
may access it.
*/

extern channel *channels[];
extern user *users[];
extern role *roles[];
extern user *current_user;

channel* channel_menu();

void menu() {
   int option;
   puts(UNDERLINE "\nHarmony: Chat for CTFers" CLOSEUNDERLINE);
   puts("\n0. Read Channel\n1. View User Info\n2. Change role name\n3. Change username");
   printf("> ");
   scanf("%d", &option);
   clear();
   switch (option) {
      case 0:
         print_channel(channel_menu());
         break;
      case 1:
         print_user_info(current_user);
         break;
      case 2:
         roles[current_user->role]->update_role(roles[current_user->role]);
         break;
      case 3:
         current_user->update_username(current_user);
         break;
      default:
         puts("Unknown option");
   }

}

channel* channel_menu() {
   int option;
   puts(UNDERLINE "Choose channel to view" CLOSEUNDERLINE);
   for (int c = 0; c < nchan; c++) {
      printf("%d. %s\n", c, channels[c]->title);
   }
   printf("> ");
   scanf("%d", &option);
   clear();
   if (option >= nchan) {
      puts("Unknown channel");
      exit(-1);
   }
   return channels[option];
}

int main() {
   setvbuf(stdout, NULL, _IONBF, 0);
   puts(logo_txt);
   init();
   current_user = users[2];
   while (1==1) {
      menu();
   }
   return 0;
}
