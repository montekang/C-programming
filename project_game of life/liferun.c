/*
 * The Game of Life
 *
 * Any live cell with fewer than two live neighbours dies, as if caused by under-population.
 * Any live cell with two or three live neighbours lives on to the next generation.
 * Any live cell with more than three live neighbours dies, as if by over-population.
 * Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 *
 * The game has what we call state – the life as it exists until next generation is calculated.
 * The state shall be implemented as two-dimensional array of short. 
 * The program shall use bit 15 of the state array to save future generation as it is being calculated.
 * Bit 14 shall be used for the current state of the game as it is displayed.
 * Bits 13 down to 0 shall save 14 previous generations of the game, bit 0 being the oldest.
 *
 * Option --random means the seed should be randomly generated by reading /dev/urandom. 
 * option [filename].txt, the program will read the seed from the text file, one cell per line, row coordinate is first
*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> // compare the strings  

#define CELL_SIZE 1 // set the size of cell as 1 x 1
#define MAX_ROWS 50 
#define MAX_COLS 200 
#define MIN_ROWS 24 
#define MIN_COLS 80  

// use the external functions from the lifedraw_xxx.c file
extern int draw_init(int size_rows, int size_cols);
extern void draw_life(int rows, int cols,  short * state); 
extern void draw_finish(void);

//hard coded part (glider gun)
char default_seed[MIN_ROWS][MIN_COLS] = {
"................................................................................",
"................................................................................",
"................................................................................",                                                                 
"................................................................................",                                                                 
"................................................................................",                                                                 
"................................................................................",                                                                 
"........................................#.......................................",                                                                 
"......................................#.#.......................................",                                                                
"............................##......##............##............................",                              
"...........................#...#....##............##............................",                                                                 
"................##........#.....#...##..........................................",                                                                 
"................##........#...#.##....#.#.......................................",                                                               
"..........................#.....#.......#.......................................",                                                               
"...........................#...#................................................",                                                               
"............................##..................................................",                                                              
"................................................................................",                                                                 
"................................................................................",                                                                 
"................................................................................",                                                                 
"................................................................................",                                                                 
"................................................................................",                                                                 
"................................................................................",                                                                 
"................................................................................",                                                                 
"................................................................................",                                                                
};    

int rows = MAX_ROWS;
int cols = MAX_COLS;
unsigned short state[MAX_ROWS][MAX_COLS]={0};

void initial_default(void)
{
  int i, j;
  for(i=0; i<MIN_ROWS; i++)
    {
      for(j=0; j<MIN_COLS; j++)
	{
	  if(default_seed[i][j]=='#')
	    {
	      state[i][j] = (1<<14);
	    }
	  else
	    {
	      state[i][j] = (0<<14);
	    }
	}
    }
}

// FILE  This is an object type suitable for storing information for a file stream.
// FILE *fopen(const char *filename, const char *mode) opens the filename pointed to, by filename using the given mode,r-read,w-write.
// int fclose(FILE *stream)  Closes the stream. All buffers are flushed.
// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
// Reads data from the given stream into the array pointed to by ptr.
void initial_devrandom()
{
  int i ,j;
  unsigned int randval; // the random value from dev/urandom
  FILE *f; // define the file pointer
  f = fopen("/dev/urandom", "r"); // read from dev/urandom      
  for(i=0; i<rows; i++)
    {
      for(j=0; j<cols; j++)
	{
	  fread(&randval, sizeof(randval), 1, f); // read one value everytime
	  state[i][j]=randval%2;  //generate a random number (0-16900),0x4000 is 16384 (14th bit =1)
	  
	  if (state[i][j]==1) 
	    {
	      state[i][j] = (1<<14);
	    }
	  else
	    {
	      state[i][j] = (0<<14);
	    }
	}
    }
   fclose(f);
}

// read data from txt file.
// each line contains two numbers for the coordinates(row col)
// deliminate by space
// char *strtok_r(char *str, const char *delim, char **saveptr);
void initial_fileseed(char *filename) // pass file name as a parameter (ex. test.txt)
{
  FILE *f; 
  char *s; 
  char str[10];  
  int i,j;
  char *ptoken=NULL; 
  char *psave=NULL;
  f = fopen(filename, "r"); // open the file for reading the value
  if (f==NULL)
    {
      perror("Error opening file");
      exit(-1);
    }
  while (1)
    {
      s=fgets(str, 10, f); // read one line to string str, from txt file
      if (s == NULL)
	break;
      else
	{
	  ptoken = strtok_r(str," ",&psave); // ptoken point to the first number in the line, psave point to the rest of the line.
	  i=atoi(ptoken)-5; // get the value of row
	  j=atoi(psave); // get the value of col
	}  
      state[i][j] = (1<<14);
    }
  fclose(f);
}    

int count_neighbors(int row, int col)
{
  int i,j;
  int count=0;

  // Suppose that we have 3x3 grid to count the neighbors
  // we calculate how many neighbors the state[i][j] has 
  // | i-1, j-1 | i-1, j | i-1, j+1 |
  // |  i , j-1 |  i , j |  i , j+1 | 
  // | i+1, j-1 | i+1, j | i+1, j+1 | 
  for (i=-1;i<2;i++)
    {
      for(j=-1;j<2;j++)
	{  
	  if (i==0 && j==0) // We don't need to count the state[i][j]
	    continue;
	  
	  // check the 14th bit(current generation) is alive or not
          // use '%rows' for a edge of the array (explain with white board) 
	  if(state [(row+i+rows)%rows][(col+j+cols)%cols] & (1<<14)) 
	  // if i =-1 or j=-1, on the edge, array coordinates will be set to -1.
	  // the array coordinates can not be -1. so we set the row=-1 & col=-1 as row=last value & col=last value
	  // ex.( row=0 1 2 3 4),when row =-1, we set the row as 4
	  // row=0,i=-1,rows=5; 0-1+5%5=4;
	  if(state [(row+i+rows)%rows][(col+j+cols)%cols] & (1<<14)) // check the 14th bit(current generation) is alive or not
	    count++;
	}
    }
  return count;
}

// change the 15th bit for the next generation
void next_generation( void)
{
  int row,col;
  int count=0;
  int val=0;
  for(row=0;row<rows;row++)
    {
      for (col=0;col<cols;col++)
	{	  
	  count=count_neighbors(row, col);
	  if(state[row][col] & (1<<14) )//check if the cell is live.
	    {
	      // any live cell with two or three live neighbours lives on to the next generation.
	      if (count==2 || count==3) 
		state[row][col] |= (1<<15); // change the 15th bit to 1.
	      // any live cell with fewer than two or more than three live neighbours dies.
	      else
		state[row][col] &= ~(1<<15); // change the 15th bit to 0. 
	    }
	  else // the dead cell
	    {
	      if (count==3) 
		state[row][col] = (state[row][col]) | (1<<15); // change the 15th bit to 1.
	      else
		state[row][col] = (state[row][col]) & ~(1<<15); // change the 15th bit to 0.
	    }
	}
    }
  // change the values for 15th bit to 14th bit
  for(row=0;row<rows;row++)
    {
      for (col=0;col<cols;col++)
	{
	  state[row][col] = state[row][col] >> 1;
	}
    }
}

int stringcmp(char *str1,char *str2, int length)
{
  int i=0; 
  for(i; i<length; i++)
    {    
      if (*str1++!=*str2++)
	{
	  return 0;
   	  break;
	} 
    }
  return 1;
}

/*
void print_storage()
{
  int i;
  int row,col;
  for(row=0;row<rows;row++)
    {
      for(col=0;col<cols;col++)
	{
	  printf ("storage[%d][%d]=  %6d  ",row,col,storage[row][col]);
	  for(i=0;i<16;i++)
	    printf("%3d", ((state[row][col] & (1<<i))));
	  puts("");
	}
    } 
}
*/
 
int main(int argc, char *argv[])
{ 
  short i,j;
  char *str;
  char key; 
  char *filename;
  if (argc<2) // no parameter
      initial_default();  
  else
    {
      str=argv[1];
      if (str!=NULL)
	{
	  if (stringcmp(str,"--random",8)==1) // if the command line has the same string with "--random"
	    {
	      initial_devrandom();
	    }
	  else 
	    {
	      filename=str;
	      initial_fileseed(filename);
	    }
	    
	}  
    }
  
  draw_init(MIN_ROWS, MIN_COLS); //initial a screen

  if(draw_init(MIN_ROWS, MIN_COLS) == -1) // the screen is too small
    exit;
  else
    {
      while(1)
	{
	  draw_life(rows,cols,*state); // display the cells on the screen
	  next_generation(); // calculate the next generation
	}
    }
} 

		