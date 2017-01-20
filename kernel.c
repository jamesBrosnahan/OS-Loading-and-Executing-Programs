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

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define WHITE 7
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

void printString(char*);
void readString(char*);
void clearScreen(int, int);
void writeInt(int);
void readInt(int*);
void handleInterrupt21(int, int, int, int);
void readInt_test();
void part_1_main();

void main()
{
   char line[80];
   int x;
   int backgroundColor = BLACK;
   int foregroundColor = WHITE;
   
    makeInterrupt21();
    /* BlackDOS header with interrupts */
    interrupt(33, 12, backgroundColor, foregroundColor, 0);
    interrupt(33, 0, "Hello world\r\n\0", 0, 0);
    interrupt(33, 0, "\r\n\0", 0, 0);
    interrupt(33, 0, "Enter a line: \0", 0, 0);
    interrupt(33, 1, line, 0, 0);
    interrupt(33, 0, "You wrote: \0", 0, 0);
    interrupt(33, 0,line,0,0);
    interrupt(33, 0,"\r\n\0",0,0);
    interrupt(33, 0, "Enter a number: \0", 0, 0);
    interrupt(33, 14, &x, 0, 0);
    interrupt(33, 0, "You entered \0", 0, 0);
    interrupt(33, 13, x, 0, 0);
    interrupt(33, 0, "\r\n\0", 0, 0);

    while(1);
}

void readInt_test(){
    char line[80];
    int x;
   
    /* BlackDOS header */
    printString("Enter a line: \0");
    readString(line);
    printString("\r\nYou typed: \0");
    printString(line);
    printString("\r\n\0");
    printString("Enter a number: \0");
    readInt(&x);
    printString("You entered \0");
    writeInt(x);
    printString("\r\n\0");
    while (1);
}

void part_1_main(){
   char line[80];
   int x;
   
   /* makeInterrupt21(); */
   clearScreen(CYAN, LIGHT_RED);

   printString("___.   .__                 __       .___           \r\n\0");
   printString("\\_ |__ |  | _____    ____ |  | __ __| _/___  ______\r\n\0");
   printString(" | __ \\|  | \\__  \\ _/ ___\\|  |/ // __ |/ _ \\/  ___/\r\n\0");
   printString(" | \\_\\ \\  |__/ /\\ \\\\  \\___|    </ /_/ ( <_> )___ \\ \r\n\0");
   printString(" |___  /____(____  /\\___  >__|_ \\____ |\\___/____  >\r\n\0");
   printString("     \\/          \\/     \\/     \\/    \\/         \\/ \r\n\0");
   printString(" V. 1.03, C. 2017. Based on a project by M. Black. \r\n\0");
   printString(" Author(s): your name(s) here.\r\n\r\n\0");
   
   printString("Hola mondo.\r\n\0");
   printString("Enter a line: \0");
   readString(line);
   printString("\r\nYou typed: \0");
   printString(line);
   printString("\r\n\0");
   /* x = 5; */
   /* printString("Your number is \0"); */
   /* writeInt(x); */
   /* printString("\r\n\0"); */
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

void handleInterrupt21(int ax, int bx, int cx, int dx){
    switch(ax){
        case 0: /* ax=0, call printString(bx) */
            printString(bx);
            break;
        case 1: /* ax=1, call readString(bx) */
            readString(bx);
            break;
        case 12: /* ax = 12, call clearScreen(bx, cx) */
            clearScreen(bx, cx);
            break;
        case 13: /* ax = 13, call writeInt(bx) */
            writeInt(bx);
            break;
        case 14: /* ax = 14, call readInt(bx) */
            readInt(bx);
            break;
    }
}









