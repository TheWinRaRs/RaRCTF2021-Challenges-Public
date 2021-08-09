#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

extern int __libc_start_main(int *(main) (int, char * *, char * *), int argc, char * * ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end));

void init() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    srand(time(0));
}

int main() {
    init();
    // printf("%p\n%p\n", system-349200, (system-349200)+0xe6c7e);
    int score = 0;
    uint8_t nums[8];
    for (int i = 0; i < sizeof(nums); i++) {
        nums[i] = (uint8_t)(rand() % 0xff);
        // printf("%hhu ", nums[i]);
    }

    while (score < 8) {
        int index;
        uint8_t guess;
        printf("\nWhich number are you guessing (0-7)? ");
        scanf("%u", &index);
        printf("Enter your guess: ");
        scanf(" %hhu", &guess);
        // printf("Guess: %hhu, actual: %hhu\n", guess, nums[index]);
        if (guess < nums[index])
            puts("Ouch, too low!");
        else if (guess > nums[index])
            puts("Too high!");
        else {
            score++;
            puts("You got it!");
        }
    }
    getchar();
    char feedback[24];
    printf("So, what did you think of my game? ");
    read(0, feedback, 50);
    // printf("%p\n", ((unsigned long*)nums)[6]);
    asm("xor r12, r12");
}
