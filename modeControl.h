//ADITYA NEEL
//20172135
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/wait.h>

struct termios oldtio, newtio;

//disabling the non-canonical mode 
void disableNonCanon(){
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtio);
}

//Initializing the termios object with non canonical attributes and flags
void enableNonCanon(){
    tcgetattr(STDIN_FILENO, &oldtio);
    atexit(disableNonCanon);
    newtio=oldtio;
    newtio.c_cc[VMIN]=1;
    newtio.c_cc[VTIME]=1;
    newtio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newtio);
}