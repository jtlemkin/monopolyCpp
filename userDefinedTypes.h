//
// Created by James Lemkin on 1/16/18.
//

#ifndef HOARDING_USERDEFINEDTYPES_H
#define HOARDING_USERDEFINEDTYPES_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef struct Player_struct Player;

typedef struct Tile_struct {
  char type[32];
  int setID;
  int intrasetID;
  char name[32];
  int propertyCost;
  int houseCost;
  int hotelCost;
  int rent;
  int rentWithHouse;
  int rentWithHotel;
  Player* ownerPt;
  int indexInBoard;
} Tile;

typedef struct Board_struct {
  int numberOfSpaces;
  int cashWhenPassingGo;
  Tile* tiles;
  int lengthOfLongestName;
  int numberOfSets;
} Board;

Board newBoard(char* boardFileName);
void deleteBoard(Board* board);

typedef struct Player_struct {
  int id;
  int numDollars;
  Tile** properties;
  Tile* locationPt;
  bool rolledDice;
  Player* nextPlayer;
  Player* previousPlayer;
} Player;

typedef struct Rules_struct {
  int startingCash;
  int turnLimit;
  int numberOfPlayersLeftToEndGame;
  int propertySetMultiplier;
  int numberOfHousesBeforeHotels;
  bool mustBuildHousesEvenly;
  bool putMoneyInFreeParking;
  bool auctionProperties;
  int salaryMultiplierForLandingOnGo;
} Rules;

bool convertStringToBool(char* str);

typedef struct Gamestate_struct {
  Rules rules;
  Board board;
  int numPlayers;
  Player* players;
  FILE* randomNumbersFPt;
  int currentTurn;
  Player* currentPlayerPt;
  Player* headPlayerPt;
  int origNumPlayers;
} GameState;

GameState newGameState(char* rulesFileName, char* boardFileName, char* randomNumbersFileName);
void deleteGameState(GameState* gameState);
bool isGameOver(GameState* gameState);
void printGameState(GameState* gameState);

typedef struct Move_struct {
  Player* playerPt;
  int type;
  bool endsTurn;
} Move;

Move newMove(Player* playerPt);
void makeMove(Move* move, GameState* gameState);
int getRandomNumber(GameState * gameState);
void movePlayer(GameState* gameState, Move* move, int playerID, int diceRoll);
void landOnTile(GameState* gameState, Move* move, int playerID, int position);
void purchaseProperty(GameState* gameState, int playerID, int tileID);
void payRent(GameState* gameState, Move* move, int tileIndex, int player1ID, int player2ID);
int rollDice(GameState* gameState);
bool isInCompletedSet(GameState* gameState, int tileIndex);
void makeTransaction(GameState* gameState, Move* move, int player1ID, int player2ID, int amountToPay);
void leaveGame(GameState* gameState, Move* move, int playerID);
void printPlayer(GameState* gameState, int playerID);
void inspectPlayer(GameState* gameState);
void endTurn(Move* move);
void changeTurn(GameState* gameState);
void declareWinners(GameState* gameState);
int lengthOfLongestName(Board* board);
void bankruptToPlayer(GameState* gameState, int player1ID, int player2ID);

#endif //HOARDING_USERDEFINEDTYPES_H
