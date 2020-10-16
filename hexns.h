#include <stdio.h>
#include <stdlib.h>

#define MAX_FILESIZE (1073741824L)	/* 1GB */
#define LINE 16
#define BYTES_ON_LINE 16

#ifdef _WIN32
	#include <windows.h>
	#include <conio.h>
	
	#define CLEAR() system("cls")
	#define RDN_FOR_ARROW_KEYS 224
	#define UP_ARROW_KEY 72
	#define DOWN_ARROW_KEY 80
	#define RIGHT_ARROW_KEY 77
	#define LEFT_ARROW_KEY 75
#elif __linux
	#include <term.h>
	#include <termios.h>
	#include <unistd.h>
	
	#define CLEAR() do { system("clear"); tigetstr( "clear" ); } while(0)
	#define RDN_FOR_ARROW_KEYS 27
	#define UP_ARROW_KEY 65
	#define DOWN_ARROW_KEY 66
	#define RIGHT_ARROW_KEY 67
	#define LEFT_ARROW_KEY 68
#else
	#error "TODO"
#endif

int hexns(void);
void gotoxy(short x, short y);
int getch();