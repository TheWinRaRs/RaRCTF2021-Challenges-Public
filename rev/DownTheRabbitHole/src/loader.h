#define BASE (char *)0xabcdef000

#define LENGTH 0x8000
// #define NFRAMES 6

#define STANDARD 0
#define OBFUSCATED 1

#define RD 0
#define WR 1

typedef int func(void);
void xor(char *dest, char *str1, char *str2, int length, int mode);

struct sharedmem {
	// Flow 0: Hash
	int pwpipe[2];		// Carries password
	int saltpipe[2];	// Carries salt
	int hashpipe[2];	// Carries final hash
	int resultpipe[2];	// Carries AES result

	// Flow 1: Shell
	int cmdpipe[2];
	int cmdrespipe[2];
	int fnpipe[2];
	int filepipe[2];
};

#define SHMBASE 0xfedcba000
