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
#include "termDisp.h"
#include "normalFeatures.h"
#include "modeControl.h"
#include <signal.h>

#define placeCursor(x,y) printf("\033[%d;%dH",x,y)

int rt=1,rtemp=1,ctemp=1,lc=0,lastLineChars,fileExists,dirtyFlag=0;
struct winsize w;
char** arr=NULL;

void insertData(char *argv,char c){
    int y=rtemp-1,x=ctemp-1,k;
    int len=strlen(arr[y]);
    for(k=len-1;k>=x;k--){
        arr[y][k+1]=arr[y][k];
    }
    arr[y][k]=c;
    fflush(stdout);
    placeCursor(rtemp,1);
    printf("%s",arr[y]);
    placeCursor(rtemp,k+1);

    FILE *fptr;
    fptr=fopen(argv,"w");
    for(int i=0;i<lc;i++){
        fprintf(fptr,"%s\n",arr[i]);
    }
    fclose(fptr);
}

//*****displaying the contents of the file*****
void fileCall(char *argv){
    
    //***data structure implementation***//
    char line[200];
    int i;       

    FILE *fp; 
    fp = fopen(argv,"r+");
    while (fgets(line,sizeof(line),fp) != NULL)                                    
       lc++;
    
    rewind(fp);

    arr=malloc(lc*sizeof(char*));
    for (i=0;i<lc;i++){    
        fgets(line,sizeof(line),fp);
        line[strlen(line)-1]='\0';
        arr[i] = malloc(w.ws_col);

        if(i==lc-1)
            lastLineChars=strlen(line);

        strcpy(arr[i],line);
    }
    fclose(fp);

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

//*****loads a new file*****//
void newFile(char *argv){
    FILE *fp; 
    fp = fopen(argv,"w");
    char c[3];
    while(1){
        if(read(0,&c,3)>=1)
        {
            if(c[0]=='\x1B')
                break;
            else{
                fprintf(fp,"%c",c[0]);
            }
        }   
    }
    fclose(fp);
}


int main(int args, char *argv[]){

    if(args!=2){
        printf("Command line must contain a file name!\n");
        exit(1);
    }

    //*****clearing screen at the beginning of the editor and drawing rows*****    
    clearScreen();

    //*****opening a new file or loading an existing file and changing the flag's value accordingly***** 
    if(access(argv[1],F_OK)!=-1){
        fileCall(argv[1]);
    }
    else{
        newFile(argv[1]);
    }
    placeCursor(1,1);
    
    char c;     
    while(1){

        fflush(stdout);
        enableNonCanon();
        read(STDIN_FILENO,&c,1);

        switch(c){

            fflush(stdout);

            //move cursor to the first line of the file
            case 'g':{
                char c2;
                read(STDIN_FILENO,&c2,1);
                rtemp=1;ctemp=1;
                if(c2=='g')
                    placeCursor(1,1);
                break;
            }

            //move the cursor to the last line of the file
            case 'G':{
                placeCursor(lc-1,lastLineChars);
                break;
            }

            //cursor left movement
            case 68:
            case 'h':{
                if(ctemp>1)
                    placeCursor(rtemp,--ctemp);
                break;
            }

            //cursor down movement and refreshing the screen simultaneously
            case 66:
            case 'j':{
                if(rtemp<lc){

                    if(ctemp>strlen(arr[rtemp]) && strlen(arr[rtemp])<strlen(arr[rtemp-1]))
                        ctemp=strlen(arr[rtemp]);

                    placeCursor(++rtemp,ctemp);
                    if(rtemp<rt)
                        rt=rtemp;
                    if(rtemp>=rt+w.ws_row){
                        rt=rtemp-w.ws_row+1;
                        for(int i=rt;i<rt+w.ws_row-1;i++){
                            printf("%s\n",arr[i]);
                        }
                        placeCursor(rtemp,ctemp);
                    }
                }
                break;
            }
                
            //cursor up movement
            case 65:
            case 'k':{
                if(rtemp>1){
                    placeCursor(--rtemp,ctemp);
                }                
                break;
            }

            //cursor right movement
            case 67:
            case 'l':{
                if(ctemp<=strlen(arr[rtemp-1])-1)
                    placeCursor(rtemp,++ctemp);
                break;
            }

            //tab space implementation 
            case 9:{
                tabRender(argv[1]);
                break;
            }

                //*****backward deletion implementation*****
                case 127:{
                    backDel(argv[1]);
                    break;
                }

                //forward deletion implementation
                case '~':{
                    fwdDel(argv[1]);
                    break;
                }
            

            //replace character
            case 'r':{
                char ch;
                scanf("%c",&ch);
                printf("%c",ch);
                replaceChar(ch,argv[1]);
                break;
            }

            case ':':{
                disableNonCanon();
                printf("\x1b[%d;%dH:                      ",w.ws_row,1);
                placeCursor(w.ws_row,2);
                char str[20];
                int i=0;
                while(c!=10){
                    scanf("%c",&c);
                    if(c!=10){
                        str[i++]=c;
                        printf("%c",c);
                    }                   
                }
                str[i]='\0';
                if(str[0]=='!'){
                    char str1[20];
                    int i=1;
                    while(str[i]){
                        str1[i-1]=str[i];
                        i++;
                    }
                    str1[i]='\0';
                    pid_t pid=fork();
                    int status;
                    if(pid==0){
                        system("clear");
                        system(str1);
                        char ch;
                        printf("Press enter to quit.\n");
                        if(read(STDIN_FILENO,&ch,1)=='\n'){
                            kill(pid,SIGTERM);  
                        }
                    }
                    else{
                        wait(&status);
                    }
                }    
                if(strcmp(str,"q!")==0){
                    printf("\e[1;1H\e[2JExiting the text editor forcefully!!!\n");
                    exit(1);
                }

                //*****the dirtyFlag flag checks if the file is saved or not*****//
                else if(strcmp(str,"q")==0){
                    if(dirtyFlag==1){
                        printf("\x1b[%d;%dHthe file is not saved yet!\n",w.ws_row,1);
                        placeCursor(1,1);
                        break;
                    }
                    else
                        exit(1);
                }
                else if(strcmp(str,"w")==0){
                    printf("\x1b[%d;%dH%s saved!",w.ws_row,1,argv[1]);
                    dirtyFlag=0;
                    placeCursor(1,1);
                }
                else{
                    clearScreen();
                    placeCursor(1,1);
                    writeFile(argv[1]);
                    placeCursor(1,1);
                }
                break;
            }

            //*****insert mode entry*****//
            case 'i':{
                
                disableNonCanon();
                while(read(STDIN_FILENO,&c,1)){ 

                    //*****newline implementation*****
                    if(c=='\x0D'){
                        write(STDOUT_FILENO,"\r\n",3);
                        rtemp++;
                        ctemp=1;
                    }

                    //*****normal mode entry point*****
                    else if(c=='\x1B'){
                        break;
                    }

                    else{
                        write(STDOUT_FILENO,&c,1);
                        dirtyFlag=1;
                        ctemp++;
                        insertData(argv[1],c);
                        if(ctemp>w.ws_col){
                            rtemp++;
                            ctemp=1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

