#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

struct challenge {
    char category[16];
    char name[16];
    char* description;
    unsigned long points;
};

int menu(){
    puts("1. Make Challenge");
    puts("2. Patch Challenge");
    puts("3. Deploy Challenge");
    puts("4. Take Down Challenge");
    puts("5. Do nothing");
    printf("> ");
    char buf[10];
    read(0, buf, 10);
    return atoi(buf);
}
int ctftime_rating = 25;
struct challenge* challenges[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
int main(){
    setvbuf(stdin, NULL,_IONBF, 0);
    setvbuf(stdout, NULL,_IONBF, 0);
    setvbuf(stderr, NULL,_IONBF, 0);
    puts("Welcome to the RaRCTF 2021 Admin Panel!");
    puts("This totally has effect on the actual challenges!");
    unsigned int index;
    unsigned int length;
    while (1) {
        switch (menu()){
            case 1:
                printf("Challenge number: ");
                scanf("%d", &index);
                if(index >= 10 || challenges[index] != NULL){
                    puts("Error: invalid index");
                    break;
                }
                challenges[index] = malloc(sizeof(struct challenge));
                printf("Challenge category: ");
                read(0, challenges[index]->category, 16);
                printf("Challenge name: ");
                read(0, challenges[index]->name, 16);
                printf("Challenge description length: ");
                scanf("%d", &length);
                challenges[index]->description = malloc(length);
                printf("Challenge description: ");
                read(0, challenges[index]->description, length);
                printf("Points: ");
                scanf("%d", &challenges[index]->points);
                puts("Created challenge!");
                break;
            case 2:
                printf("Challenge number: ");
                scanf("%d", &index);
                if(index >= 10 || challenges[index] == NULL){
                    puts("Error: invalid index");
                    break;
                }
                if (strncmp("web", challenges[index]->category, 3) != 0){
                    puts("Challenge does not need patching, no unintended.");
                    break;
                }
                printf("New challenge description: ");
                read(0, challenges[index]->description, strlen(challenges[index]->description));
                puts("Patched challenge!");
                ctftime_rating -= 5;
                break;
            case 3:
                printf("Challenge number: ");
                scanf("%d", &index);
                if(index >= 10 || challenges[index] == NULL){
                    puts("Error: invalid index");
                    break;
                }
                puts("Deploying...");
                printf("Category: %s\n", challenges[index]->category);
                printf("Name: %s\n", challenges[index]->name);
                printf("Description: %s\n", challenges[index]->description);
                break;
            case 4:
                printf("Challenge number: ");
                scanf("%d", &index);
                if(index >= 10 || challenges[index] == NULL){
                    puts("Error: invalid index");
                    break;
                }
                free(challenges[index]->description);
                free(challenges[index]);
                challenges[index] = NULL;
                ctftime_rating -= 3;
                break;
            default:
                puts("I guess we're done here.");
                exit(0);
        }
        if (ctftime_rating <= 0){
            puts("Well... great.");
            exit(0);
        }
    }
}