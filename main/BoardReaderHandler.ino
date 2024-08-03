#include <stdio.h>
#include <stdbool.h>

/*
Author: Nicklas Jeppesen (nicje) and laurtiz Bonde (?)

This code is used to read two brick for tic tac toe
Circle bricks: has a 680K resistor inside, and output a small value around 10-11 ish when reading by arduino. 
Cross bricks: has a 10K resistor inside, and output a high value around 500 ish when reading by arduino 

The board labels and connections. 
there is two holes, to the "left" of the board, whichis represent by "O" in the graph below. 
each value represent that channel, on the  board. The channels shall be translated into columns and rows. 
_________
|O|8|5|2|
|O|7|4|1|
| |6|3|0| 
--------- 

*/

void readBoardValue(int grid[3][3], int* _row, int* _column, int* _boardValue);
void readBoard(int grid[3][3], int* _row, int* _column, int* _boardValue);
void getGridPosition(int channel, int* row, int* col);
int getChannelGridPosition(int row, int col);
float readMux(int channel);

int grid[3][3] = {
  {0,0,0},
  {0,0,0},
  {0,0,0},
};


int ChanelsPositions[3][3] = {
  {8,5,2},
  {7,4,1},
  {6,3,0},
  };

   int gameBoardChannelsPosistions[9][2] = {
  {2,2}, // Channel 0
  {1,2}, // Channel 1
  {0,2}, // Channel 2
  {2,1}, // Channel 3
  {1,1}, // Channel 4
  {0,1}, // Channel 5
  {2,0}, // Channel 6
  {1,0}, // Channel 7
  {0,0}, // Channel 8
  };

// Set up

  //Setups: 
  //Mux control pins
  int s0 = 8;
  int s1 = 9;
  int s2 = 10;
  int s3 = 11;

  //Mux in "SIG" pin
  int SIG_pin = A0;
  
  void setupBoardReader(){
    pinMode(s0, OUTPUT); 
    pinMode(s1, OUTPUT); 
    pinMode(s2, OUTPUT); 
    pinMode(s3, OUTPUT); 

    digitalWrite(s0, LOW);
    digitalWrite(s1, LOW);
    digitalWrite(s2, LOW);
    digitalWrite(s3, LOW);

  }



 // return the updated row and column of a board
void readBoardValue(int grid[3][3], int* _row, int* _column, int* _boardValue)
{

  int row, column, boardValue;
  
  readBoard(grid, &row, &column, &boardValue);
  *_row = row; 
  *_column = column; 
  *_boardValue = boardValue; 

}

// assume each player make a move, each turn. 
void readBoard(int grid[3][3], int* _row, int* _column, int* _boardValue) 
{ 
  for(int row = 0; row < 4; row++) { // rows
    for (int column = 0; column < 4; column++) { // columns
      
      int gridValue = grid[row][column]; 
      int channel = getChannelGridPosition(row, column);
      int chanelValue = (int)readMux(channel);
      bool brickplacedHere = gridValue != chanelValue;

      if(brickplacedHere) {
        *_row = row; 
        *_column = column; 
        *_boardValue = chanelValue; 
        return;
      }
      
    }
  }
  *_boardValue = 0; 
}

void getGridPosition(int channel, int* row, int* col) {
  
  int channel_row = grid[channel][0]; 
  int channel_col = grid[channel][1]; 
  *row = channel_row; 
  *col = channel_col;   
}

int getChannelGridPosition(int row, int col) {
  return ChanelsPositions[row][col];
}

// read the board 
float readMux(int channel){
 
  //read the value at the SIG pin
  int val = 20;
  return val;
}

// read the board 
float readMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[9][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
  };

  //loop through the 4 sig
  for(int i = 0; i < 9; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(SIG_pin);
  return val;
}
