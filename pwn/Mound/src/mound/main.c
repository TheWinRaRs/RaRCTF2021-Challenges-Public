#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include "mound.h"
#include <linux/seccomp.h>
#include <sys/prctl.h>

// Compile by running gcc *c -o mound

static void install_seccomp() {
  static unsigned char filter[] = {32,0,0,0,4,0,0,0,21,0,0,12,62,0,0,192,32,0,0,0,0,0,0,0,53,0,10,0,0,0,0,64,21,0,9,0,59,0,0,0,21,0,8,0,66,1,0,0,21,0,7,0,2,0,0,0,21,0,6,0,3,0,0,0,21,0,5,0,85,0,0,0,21,0,4,0,134,0,0,0,21,0,3,0,57,0,0,0,21,0,2,0,58,0,0,0,21,0,1,0,56,0,0,0,6,0,0,0,0,0,255,127,6,0,0,0,0,0,0,0};
  struct prog {
    unsigned short len;
    unsigned char *filter;
  } rule = {
    .len = sizeof(filter) >> 3,
    .filter = filter
  };
  if(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) { perror("prctl(PR_SET_NO_NEW_PRIVS)"); exit(2); }
  if(prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &rule) < 0) { perror("prctl(PR_SET_SECCOMP)"); exit(2); }
}
int menu(){
    int option;
    puts("1. Add sand");
    puts("2. Add dirt");
    puts("3. Replace dirt");
    puts("4. Remove dirt");
    puts("5. Go home");
    printf("> ");
    scanf("%d",&option);
    return option;
}
void getinput(char* prompt, char* buf, size_t size){
    printf(prompt);
    read(0, buf, size);
}

void win(){
    puts("Exploiting BOF is simple right? ;)");
    char buf[64];
    read(0, buf, 0x1000);
}
char* arr[16] = {NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL,NULL, NULL, NULL, NULL};
unsigned long sizes[16] = {0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0,0, 0, 0, 0};

int main(){
    setvbuf(stdin, NULL,_IONBF, 0);
    setvbuf(stdout, NULL,_IONBF, 0);
    setvbuf(stderr, NULL,_IONBF, 0);
    install_seccomp();
    puts("I am the witch mmalloc");
    puts("Force, Prime, Mind, Lore, Chaos, Orange, Einharjar, Poortho, Spirit, Red, Roman, Corrosion, Crust, Rust, all is known to me.");
    puts("It is, from all of my training, that I have seen the flaws in glibc heap.");
    puts("Welcome, fellow pwner, to The Mound");
    moundsetup();
    char string_buffer[0x100];
    memset(string_buffer, 0, 0x100);
    unsigned int index;
    unsigned int size;
    while (1) {
        switch (menu())
        {
            case 1:
                getinput("Pile: ", string_buffer, 0x100);
                printf("Pile index: ");
                scanf("%d", &index);
                if(index >= 16){
                    puts("No.");
                    continue;
                }
                arr[index] = strdup(string_buffer);
                sizes[index] = strlen(string_buffer);
                break;
            case 2:
                printf("Size of pile: ");
                scanf("%d",&size);
                if(size >= 0x1000){
                    puts("A bit too much dirt my friend.");
                    continue;
                }
                printf("Pile index: ");
                scanf("%d", &index);
                if(index >= 16){
                    puts("No.");
                    continue;
                }
                arr[index] = mmalloc(size);
                sizes[index] = 0;
                getinput("Pile: ", arr[index], size);
                break;
            case 3:
                printf("Pile index: ");
                scanf("%d", &index);
                if(index >= 16){
                    puts("No.");
                    continue;
                }
                if(arr[index] == NULL){
                    puts("No.");
                    continue;
                }
                if (sizes[index] == 0){
                    puts("No.");
                    continue;
                }
                getinput("New pile: ", arr[index], sizes[index]);
                break;
            case 4:
                printf("Pile index: ");
                scanf("%d", &index);
                if(index >= 16){
                    puts("No.");
                    continue;
                }
                if(arr[index] == NULL){
                    puts("No.");
                    continue;
                }
                mfree(arr[index]);
                sizes[index] = 0;
                break;
            default:
                puts("Cya later :p");
                exit(0);

        }
    }
}

