#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flag(){
  system("/bin/cat flag.txt");
}

void get_license(){
  char license[25];
  gets(license); 
}

int main(){
  setvbuf(stdout, NULL, _IONBF, 0);
  printf("Hello, welcome to the WinRaRs!\n");
  printf("Please enter your WinRaR license key to get access: ");
  get_license();
  printf("Thanks for the license :)\n");
  return 0;
}
