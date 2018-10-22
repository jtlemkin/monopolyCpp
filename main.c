#include "userDefinedTypes.h"

int main(int argc, char** argv) {
  GameState gameState = newGameState(argv[1], argv[2], argv[3]);
  Move move;

  while(!isGameOver(&gameState)) {
    do {
      printGameState(&gameState);
      move = newMove(gameState.currentPlayerPt);
      makeMove(&move, &gameState);
    } while(!move.endsTurn);
    changeTurn(&gameState);
  }

  printGameState(&gameState);
  declareWinners(&gameState);
  deleteGameState(&gameState);
  return 0;
}