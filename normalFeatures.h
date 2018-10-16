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

char** arr;
int rtemp,ctemp,lc;
#define placeCursor(x,y) printf("\033[%d;%dH",x,y)

//*****replace a character at a given cursor position in a file*****

void replaceChar(char ch,char *argv){

    int y=rtemp,x=ctemp;
    arr[y-1][x-1]=ch;

    FILE *fptr;
    fptr=fopen(argv,"w");
    for(int i=0;i<lc;i++){
        fprintf(fptr,"%s\n",arr[i]);
    }
    fclose(fptr);
}

//*****forward deletion implementation*****

void fwdDel(char *argv){
    int y=rtemp-1,x=ctemp-1;
    while(arr[y][x]!='\0'){
        arr[y][x]=arr[y][x+1];
        x++;
        arr[y][strlen(arr[y])-1]=' ';
    }
    placeCursor(y+1,1);
    printf("%s",arr[y]);
    placeCursor(y+1,ctemp);

    FILE *fptr;
    fptr=fopen(argv,"w");
    for(int i=0;i<lc;i++){
        fprintf(fptr,"%s\n",arr[i]);
    }
    fclose(fptr);
}


//*****backward deletion implementation*****

void backDel(char *argv){
    int y=rtemp-1,x=ctemp-1;

    if(x>0){
        while(arr[y][x]!='\0'){
            arr[y][x-1]=arr[y][x];
            x++;
            arr[y][strlen(arr[y])-1]=' ';
        }
        ctemp--;
        placeCursor(y+1,1);
        printf("%s",arr[y]);
        placeCursor(y+1,ctemp);

        FILE *fptr;
        fptr=fopen(argv,"w");
        for(int i=0;i<lc;i++){
            fprintf(fptr,"%s\n",arr[i]);
        }
        fclose(fptr);
    }
}


//*****tabSpace Rendering*****

void tabRender(char *argv){
    int shift=4-ctemp%4,k;
    int y=rtemp-1,x=ctemp-1;
    int len=strlen(arr[y]);
    for(k=len-1;k>=x;k++){
        arr[y][k+shift]=arr[y][k];
    }
    arr[y][k]='\0';
    for(k=1;k<=shift;k++){
        arr[y][ctemp-1]=' ';
        ctemp++;
    }
    printf("%s",arr[y]);
}


//*****writing contents of data structure into the file and priting data on the terminal*****
void writeFile(char *argv){
    int i;
    FILE *fptr;
    fptr=fopen(argv,"w");
    for(int i=0;i<lc;i++){
        fprintf(fptr,"%s\n",arr[i]);
    }
    fclose(fptr);            

     //if lines in the file exceed the terminal rows
    if(lc>w.ws_row){
        for (i=0;i<w.ws_row-1;i++){
            printf("%s\n",arr[i]);
        }    
    }

    //if lines in the file are lesser than the terminal rows
    else{
        for(i=0;i<lc;i++){
            printf("%s\n",arr[i]);
        }
    }   
}