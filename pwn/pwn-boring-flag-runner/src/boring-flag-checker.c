#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int file_size(FILE *f, size_t *size) {
    long off = ftell(f);
    if (off == -1) return -1;
    if (fseek(f, 0, SEEK_END) == -1) return -1;
    long len = ftell(f);
    if (len == -1) return -1;
    if (fseek(f, off, SEEK_SET) == -1) return -1;
    *size = (size_t) len;
    return 0;
}

int main(int argc, char *argv[]){
  setvbuf(stdout, NULL, _IONBF, 0);
  // Brainfuck stuff
  char cells[256] = {0};
  int pointer = 0; // Brainfuck pointer
  size_t len = 0;
  char *brainfuck = NULL;
  FILE *f;
  if (argc > 1){
      f = fopen(argv[1], "rb");
      if (f == NULL){
        f = fopen("prog.bin", "rb");
        if (f == NULL){
          puts("Couldn't open program.");
          exit(-1);
        }
      }
  }
  else {
    f = fopen("prog.bin", "rb");
    if (f == NULL) {
          puts("Couldn't open program.");
          exit(-1);
    }
  }
  if (file_size(f, &len) == -1 || (brainfuck = calloc(len, sizeof(char))) == NULL || fread(brainfuck, sizeof(char), len, f) != len) {
        puts("Couldn't read program.");
        exit(-1);
  }
    fclose(f);
  // Interpreter stuff
  int i = 0; // Instruction pointer
  int lists[256] = {0}; // List indexer thingly
  char listpointer = 0; // Most right free space in list
  int notlist = 0; // Are we skipping
  char flag[64] = {0};
  int charsread = 0;
  while (i < len){
    if (!notlist){
      switch (brainfuck[i] % 8){
        case 7: // Plus
          cells[pointer]++; 
          break;
        case 6: // Minus
          cells[pointer]--; 
          break;
        case 2: // Left
          pointer--; 
          //printf("%i\n", pointer);
          break;
        case 0: // Right
          pointer++; 
          //printf("%i\n", pointer);
          break;
        case 3: // Start loop
          if (cells[pointer]){
            lists[listpointer] = i;
            listpointer++;
          }
          else{
            notlist++;
          }
          break;
        case 1: // End loop
          if (cells[pointer] != 0){
            i = lists[listpointer-1];
          }
          else{
            lists[listpointer-1] = 0;
            listpointer--;
          }
          break;
        case 5: // Output 
          //printf("%i",cells[pointer]);
          putchar(cells[pointer]);
          break;
        case 4: // Input
          if (charsread == 0){
            read(0, flag, 55);
          }
          cells[pointer] = flag[charsread];
          charsread++;
          break;
        default:return 0;
      }
    } else if (brainfuck[i] % 8 == 3){ // Fix
      notlist++;
    } else if (brainfuck[i] % 8 == 1){
      notlist--;
    }
    i++;
  }
  free(brainfuck);
  return 0;
}
