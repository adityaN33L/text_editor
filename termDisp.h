//ADITYA NEEL
//20172135

#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#define gotoxy(x,y) printf("\033[%d;%dH",x,y)

struct winsize w;

void drawTildes(){

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    for (int x = 0; x< w.ws_row; x++) {
        write(STDOUT_FILENO,"~\r\n",3);
    }

    write(STDOUT_FILENO,"\x1b[H",3);
}

void clearScreen(){
  	const char *clrscrn ="\e[1;1H\e[2J";
  	write(STDOUT_FILENO,clrscrn,12);
  	drawTildes();
}
