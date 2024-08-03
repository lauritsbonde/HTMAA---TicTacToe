#define statusMessagePrint 5000
#define updateGameInterval 30

GameState gameState = NotConnnected;

GamePiece iAmPlaying = Unknown;
GamePiece opponentPlaying = Unknown;

long randomNumberForStart = 0;

u_long lastStatusMessage = 0;
u_long lastGamePlayHandle = 0;


int grid[3][3] = {
  {0,0,0},
  {0,0,0},
  {0,0,0},
};

GamePiece_T analogReadToGamePiece(int readValue) {
  if (readValue < 10) return Unknown;
  if(readValue >= 10 && < 400) return Circle;
  if(readValue > 400) return Cross;
 
  return Unknown;
}

void setOppPiece(GamePiece piece){
  opponentPlaying = piece;
}

void decideWhoStarts(long oppRandomNum) {
  if(gameState != DecidingWhoStarts) return;
  // if they have the same number set the gamestate a step back to make them redo it;
  if(oppRandomNum == randomNumberForStart) {
    Serial.println("Same number");
    gameState = GamePiecesSelected;
  } else if (oppRandomNum > randomNumberForStart) {
    Serial.println("they start");
    gameState = TheirTurn;
  } else {
    Serial.println("i start");
    gameState = MyTurn;
  }
}

void printStatusMessage(String message){
  int time = millis();
  if(time - lastStatusMessage > statusMessagePrint){
    Serial.println(message);
    lastStatusMessage = time;
  }
}


void updateGameState(){
   if(gameState == NotConnnected) {
    Serial.println("Connected");
    gameState = Connected;
  }
  // this is on purpose not an else if - maybe we dont need the notConnected state
  if(gameState == Connected) {
    Serial.println("Select gamepiece");
    gameState = SelectGamePiece;
  }

  if(gameState == SelectGamePiece){
    bool iSelected = iAmPlaying != Unknown;
    bool oppSelected = opponentPlaying != Unknown;

    if(iSelected && !oppSelected){
      printStatusMessage("Waiting for opponent to select piece");
      return;
    }
    if(!iSelected && oppSelected){
      printStatusMessage("Waiting for you to select a piece");
      return;
    };
    if(iSelected && oppSelected){
      gameState = GamePiecesSelected;
      Serial.println("Play the game");
    }
  }

  if(gameState == GamePiecesSelected){
    // decideWhoStarts the play
    randomNumberForStart = random(1,1024);
    sendRandomNumber(randomNumberForStart); //mqttHandler.ino function
    gameState = DecidingWhoStarts;
  }
}

void handleGamePlay(){
  // Reading the analogpin too often somehow drops the wifi connection
  if(millis() - lastGamePlayHandle > updateGameInterval){
    if(gameState == SelectGamePiece){
      // read the middle field for a value and see if it has a piece
      GamePiece value = analogReadToGamePiece(analogRead(A0));
      if(value != Unknown && iAmPlaying == Unknown){
        iAmPlaying = value;
        sendSelectPieceMessage(value); //mqttHandler.ino - function;
      }
    } else if(gameState == MyTurn) {
      
       int row, coll, value;
      readBoardValue(grid, &row, &coll, &value);
      
      grid[row][col] = value;
      sendMove(row, col);
      gameState = TheirTurn;      
    
    }

    lastGamePlayHandle = millis();
  }
}

void gameLoop() {
  updateGameState();
  handleGamePlay();
}

