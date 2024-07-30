enum GameState {
  NotConnnected,
  Connected,
  SelectGamePiece,
  GamePiecesSelected,
  DecidingWhoStarts,
  MyTurn,
  TheirTurn,
  WaitForOpp,
  Finished,
};

typedef enum GamePiece {
  Cross,
  Circle,
  Unknown,
} GamePiece_T;

GamePiece_T analogReadToGamePiece(int readValue);