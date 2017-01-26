/* ACADEMIC INTEGRITY PLEDGE                                              */
/*                                                                        */
/* - I have not used source code obtained from another student nor        */
/*   any other unauthorized source, either modified or unmodified.        */
/*                                                                        */
/* - All source code and documentation used in my program is either       */
/*   my original work or was derived by me from the source code           */
/*   published in the textbook for this course or presented in            */
/*   class.                                                               */
/*                                                                        */
/* - I have not discussed coding details about this project with          */
/*   anyone other than my instructor. I understand that I may discuss     */
/*   the concepts of this program with other students and that another    */
/*   student may help me debug my program so long as neither of us        */
/*   writes anything during the discussion or modifies any computer       */
/*   file during the discussion.                                          */
/*                                                                        */
/* - I have violated neither the spirit nor letter of these restrictions. */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* Signed:_____________________________________ Date:_____________        */
/*                                                                        */
/*                                                                        */
/* 3460:4/526 BlackDOS kernel, Version 1.03, Spring 2017.                 */

/*colors used with clearScreen function*/
/*Basic colors (foreground or background)*/
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define WHITE 7
/*Bright colors (foreground only)*/
#define GRAY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_MAGENTA 13
#define Yellow 14
#define BRIGHT_WHITE 15

#define AH 0xE


#define ENTER 0xD
#define BACKSPACE 0x8

/* macro functions for readSector*/
/* a is the absolute sector number*/
/*finds relative sector number*/
#define relSecNo(a) (mod(a, 18) + 1)
/*finds head number*/
#define headNo(a) mod(div(a, 18), 2)
/*finds track number*/
#define trackNo(a) div(a, 36)

/*constants for readSector*/
/*tells BIOS to read sector*/
#define READ_AH 2
/*number of sectors read*/
#define READ_AL 1
/*device number*/
#define READ_DL 0

/**/
#define MAP 0x200
#define DIRECTORY 0x400
#define ENTRIES 16
#define ENTRY_SIZE 32
#define NAME_LENGTH 6
#define SECTOR_NUMBERS 26

#define MAX_FILE_SIZE 13312

typedef int bool;
#define true 1
#define false 0



void printString(char*);
void readString(char*);
void clearScreen(int, int);
void writeInt(int);
void readInt(int*);
void readSector(char*, int);
void error(int);
void readFile(char*, char*, int*);
void stop();
void runProgram(char*, int);
void handleInterrupt21(int, int, int, int);

void main()
{
    char buffer[MAX_FILE_SIZE];
    int size;
    makeInterrupt21();
    /* BlackDOS header and clear screen*/
    interrupt(33, 3, "msg\0", buffer, &size);
    interrupt(33, 0, buffer, 0, 0);
    while(1);
}

/*printString function works correctly:*/
void printString(char* c)
{
   /* Fill this in. */
   int index = 0;
   /*Does not print the NULL character (loops ends when NULL character is reached)*/
   while(c[index] != '\0'){
       /*Correctly invokes BIOS interrupt 0x10*/
        interrupt(0x10, AH*256+c[index], 0, 0, 0);
        index++;
   }
   
   return;
}
/*readString function works correctly:*/
void readString(char* c)
{
   /* This too. */
   char typed_char;
   int index = 0;
   /*Reads characters into buffer until [ENTER] is pressed (loop ends when enter is pressed)*/
   while(typed_char != ENTER){
        /*Correctly invokes BIOS interrupt 0x16*/
        typed_char = interrupt(0x16, 0, 0, 0, 0);
        switch (typed_char){
            /*enter*/
            case ENTER:
                /*Appends NULL terminator to buffer*/
                c[index] = '\0';
                /*Does not place ENTER (0x0D) into buffer*/
                break;
            /*backspace*/
            /*Backspace key is handles correctly*/
            case BACKSPACE:
                /*Cursor moves back one space*/
                interrupt(16, AH*256+BACKSPACE, 0, 0, 0);
                /*Character is removed from buffer and overwritten on screen*/
                interrupt(16, AH*256+' ', 0, 0, 0);
                interrupt(16, AH*256+BACKSPACE, 0, 0, 0);
                if(index > 0){
                    index--;
                }
                break;
            default:
                c[index] = typed_char;
                index++;
                interrupt(16, AH*256+typed_char, 0, 0, 0);
                
        }
        
   }
   return;
}

void clearScreen(int bx, int cx)
{
	/* This too. */
	int color;
	printString("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	interrupt(16,512,0,0,0);
	if(bx > 0 && cx > 0){
	    if(cx < 17 && bx < 9){
	        bx--;
	        cx--;
	        color = 4096 * bx + 256 * cx;
	        interrupt(16, 1536, color, 0, 6223);
	    }
	}
	return;
}

int mod(int a, int b)
{
   int x = a;
   while (x >= b) x = x - b;
   return x;
}

int div(int a, int b)
{
   int q = 0;
   while (q * b <= a) q++;
   return (q - 1);
}

void writeInt(int x)
{
   char number[6], *d;
   int q = x, r;
   if (x < 1)
   {
      d = number; *d = 0x30; d++; *d = 0x0; d--;
   }
   else
   {
      d = number + 5;
      *d = 0x0; d--;
      while (q > 0)
      {
         r = mod(q,10); q = div(q,10);
         *d = r + 48; d--;
      }
      d++;
   }
   printString(d);
   return;
}

void readInt(int* number)
{
   /* Fill this in as well. */
   char line[80];
   int i;
   readString(line);
   (*number) = 0;
   for( i = 0; i < 6; i++){
        if(line[i] >= '0' && line[i] <= '9'){
            (*number) = 10 * (*number) + line[i] - '0';
        }else{
            break;
        }
   }
   
   return;
}

void readSector(char* buffer, int sector) {
    /*  
        read a disk sector into memory;
        buffer is a predefined char array of at least 512 bytes
        sector is an absolute sector number

        track, head, and relative sector numbers are calculated in macro functions
    */
    
    int ax, bx, cx, dx;
    bx = buffer;
    ax = READ_AH * 256 + READ_AL;
    cx = trackNo(sector) * 256 + relSecNo(sector);
    dx = headNo(sector) * 256 + READ_DL;
    
    /* Correctly invokes BIOS interrupt 0x13 */
    interrupt(0x13, ax, bx, cx, dx);
    return;
}

void error(int bx) {
    switch(bx){
        case 0:
            printString("File not found");
            break;
        default:
            printString("General error");
            
    
    }
    return;
}

void readFile(char* fname, char* buffer, int* size) {
    int i;
    int j;
	char directory[512];
	bool found = false;

	/*Load the directory into a 512-byte character array using readSector*/
	readSector(directory, DIRECTORY);
	printString(directory);
    /*Go through the directory trying to match the file name. 
    If you don't find it, return an error.*/
    /*iterate through 16 entries in the directory*/
	for (i = 0; i < ENTRIES; i++) {

		/*read first 6 letters of directoy entry*/
		for (j = 0; j < NAME_LENGTH; j++) {
			printString("I is equal to: ");
			writeInt(i);
			printString("\r\n");
			printString("J is equal to: ");
			writeInt(j);
			printString("\r\n");
			writeInt(j + (i * 32));
			printString("\r\n");
			printString("Reading file name\n\r");
			if (fname[j] != '\0') {
				if (fname[j] == directory[j + (i * 32)]) {
					printString("Chars do match\n\r");
				}else if(fname[j] != directory[j + (i * 32)]){
					printString("Chars do not match\n\r");
					break;
				}
			}else if (fname[j] == '\0') {
				printString("Entry found\r\n");
				found = true;
				break;
			}
			
			
		}
		if (found == true) {
			break;
		}
	}
	if(found == true){
		/*
		Using the sector numbers in the directory,
		load the file, sector by sector, into buffer. 
		You should add 512 to the buffer every time you call readSector */
		for(j = 0;j < SECTOR_NUMBERS && directory[j+6] != 0x00; j++){
			printString("Loading");
			writeInt(j);
            readSector(buffer, directory[j+6]*512);
            buffer += 512;
            size += 1;
            }
			return;
	}else if(found == false){
		error(0);
	}
    /*Write the sector count back and return*/

    return;
}

void stop(){
    return;
}

void runProgram(char* name, int segment){
    return;
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
    switch(ax){
        case 0: 
        /* 
            ax=0, call printString(bx) 
            
        */
            printString(bx);
            break;
        case 1: 
        /* 
            ax=1, call readString(bx) 
            
        */
            readString(bx);
            break;
        case 2: 
        /* 
            ax=2. call readSector(bx, cx) 
        
        
        */
            readSector(bx, cx);
            break;
        case 3: 
        /* 
            ax=3, call readFile(bx, cx, dx) 
            
        */
            readFile(bx, cx, dx);
            break;
        case 4: 
        /*  
            ax=4, call runProgram(bx, cx)
            launchs a program
            bx is a char * that represents the name of the program
            cx is the segment where the program is to ran, 2 to 9 inclusive
        */
            runProgram(bx, cx);
            break;
        case 5: 
        /* 
            ax=5, call stop() 
            
        */
            stop();
            break;
        case 12: 
        /* 
            ax = 12, call clearScreen(bx, cx)
            
        */
            clearScreen(bx, cx);
            break;
        case 13: 
        /* 
            ax = 13, call writeInt(bx) 
            
        */
            writeInt(bx);
            break;
        case 14: 
        /* 
            ax = 14, call readInt(bx) 
        
        
        */
            readInt(bx);
            break;
        case 15: 
        /* 
            writes an error message based upon the error number
            bx is an error number
        */
            error(bx);
            break;
    }
}









