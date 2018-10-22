//
// Created by James Lemkin on 1/16/18.
//

#include "userDefinedTypes.h"

//BOARD
Board newBoard(char* boardFileName) {
  Board board;
  int highestSetNum = 0;

  FILE* fp = fopen(boardFileName, "r");

  if (fp == NULL) {
    printf("%s cannot be found\n", boardFileName);
    exit(0);
  }

  char buffer[250];

  fscanf(fp, "Number of Spaces,%d,,,,,,,,", &(board.numberOfSpaces));
  fgets(buffer, 250, fp);
  fgets(buffer, 250, fp);
  fgets(buffer, 250, fp);
  fgets(buffer, 250, fp);

  board.tiles = (Tile*)malloc(board.numberOfSpaces * sizeof(Tile));

  fgets(buffer, 250, fp);
  strcpy(board.tiles[0].type, strtok(buffer, ","));
  board.cashWhenPassingGo = atoi(strtok(NULL, ","));
  strcpy(board.tiles[0].name, strtok(NULL, ","));
  board.tiles[0].setID = -1;
  board.tiles[0].intrasetID = -1;
  board.tiles[0].ownerPt = NULL;
  board.tiles[0].indexInBoard = 0;

  for(int i = 1; i < board.numberOfSpaces; ++i) {
    fgets(buffer, 250, fp);
    strcpy(board.tiles[i].type, strtok(buffer, ","));
    board.tiles[i].setID = atoi(strtok(NULL, ","));

    if (board.tiles[i].setID > highestSetNum) {
      highestSetNum = board.tiles[i].setID;
    }

    board.tiles[i].intrasetID = atoi(strtok(NULL, ","));
    strcpy(board.tiles[i].name, strtok(NULL, ","));
    board.tiles[i].propertyCost = atoi(strtok(NULL, ","));
    board.tiles[i].houseCost = atoi(strtok(NULL, ","));
    board.tiles[i].hotelCost = atoi(strtok(NULL, ","));
    board.tiles[i].rent = atoi(strtok(NULL, ","));
    board.tiles[i].rentWithHouse = atoi(strtok(NULL, ","));
    board.tiles[i].rentWithHotel = atoi(strtok(NULL, ","));
    board.tiles[i].ownerPt = NULL;
    board.tiles[i].indexInBoard = i;
  }

  fclose(fp);

  board.lengthOfLongestName = lengthOfLongestName(&board);
  board.numberOfSets = highestSetNum + 1;

  return board;
}

void deleteBoard(Board* board) {
  free(board->tiles);
}

//RULES
Rules newRules(char* rulesFileName) {
  Rules rules;
  FILE* fp = fopen(rulesFileName, "r");

  if (fp == NULL) {
    printf("%s cannot be found\n", rulesFileName);
    exit(0);
  }

  char* booleanString = (char*)malloc(5 * sizeof(char));

  fscanf(fp, "Starting Cash: %d\n", &(rules.startingCash));
  fscanf(fp, "Turn Limit (-1 for no turn limit): %d\n", &(rules.turnLimit));
  fscanf(fp, "Number of Players Left To End Game: %d\n", &(rules.numberOfPlayersLeftToEndGame));
  fscanf(fp, "Property Set Multiplier: %d\n", &(rules.propertySetMultiplier));
  fscanf(fp, "Number of Houses Before Hotels: %d\n", &(rules.numberOfHousesBeforeHotels));
  fscanf(fp, "Must Build Houses Evenly: %s\n", booleanString);
  rules.mustBuildHousesEvenly = convertStringToBool(booleanString);
  fscanf(fp, "Put Money In Free Parking: %s\n", booleanString);
  rules.putMoneyInFreeParking = convertStringToBool(booleanString);
  fscanf(fp, "Auction Properties: %s\n", booleanString);
  rules.auctionProperties  = convertStringToBool(booleanString);
  fscanf(fp, "Salary Multiplier For Landing On Go: %d\n", &(rules.salaryMultiplierForLandingOnGo));

  free(booleanString);
  fclose(fp);

  return rules;
}

bool convertStringToBool(char* str) {
  if (strcmp(str, "Yes") == 0) {
    return true;
  } else if (strcmp(str, "No") == 0) {
    return false;
  } else {
    printf("FUCK\n");
    return false;
  }
}

//GAMESTATE
GameState newGameState(char* rulesFileName, char* boardFileName, char* randomNumbersFileName) {
  GameState gameState;

  gameState.rules = newRules(rulesFileName);
  gameState.board = newBoard(boardFileName);
  //gameState.numPlayersQuit = 0;
  gameState.randomNumbersFPt = fopen(randomNumbersFileName, "r");

  if (gameState.randomNumbersFPt == NULL) {
    printf("%s cannot be found\n", randomNumbersFileName);
    exit(0);
  }

  gameState.currentTurn = 0;
  //gameState.playerTurn = 0;

  printf("How many players will be playing: \n");
  scanf("%d", &gameState.numPlayers);
  gameState.origNumPlayers = gameState.numPlayers;

  gameState.players = (Player*)malloc(gameState.numPlayers * sizeof(Player));

  for(int i = 0; i < gameState.numPlayers; ++i) {
    gameState.players[i].id = i;
    gameState.players[i].numDollars = gameState.rules.startingCash;

    gameState.players[i].properties = (Tile**)malloc(gameState.board.numberOfSpaces * sizeof(Tile*));
    for(int j = 0; j < gameState.board.numberOfSpaces; ++j) {
      gameState.players[i].properties[j] = NULL;
    }

    gameState.players[i].locationPt = &(gameState.board.tiles[0]);
    gameState.players[i].rolledDice = false;
  }

  if (gameState.numPlayers >= 1) {
    for(int i = 0; i < gameState.numPlayers - 1; ++i) {
      gameState.players[i].nextPlayer = &(gameState.players[i + 1]);
      gameState.players[i].nextPlayer->previousPlayer = &(gameState.players[i]);
    }

    gameState.players[gameState.numPlayers - 1].nextPlayer = &(gameState.players[0]);
    gameState.players[0].previousPlayer = &(gameState.players[gameState.numPlayers - 1]);
  }

  gameState.currentPlayerPt = &(gameState.players[0]);
  gameState.headPlayerPt = &(gameState.players[0]);

  return gameState;
}

void deleteGameState(GameState* gameState) {
  fclose(gameState->randomNumbersFPt);

  for(int i = 0; i < gameState->numPlayers; ++i) {
    free(gameState->players[i].properties);
  }

  free(gameState->players);

  deleteBoard(&(gameState->board));
}

void printGameState(GameState* gameState) {
  for(int i = 0; i < gameState->board.numberOfSpaces; ++i) {
    printf("%d | %s ", i, gameState->board.tiles[i].name);

    for(int j = (int)strlen(gameState->board.tiles[i].name); j < gameState->board.lengthOfLongestName; ++j) {
      printf(" ");
    }

    printf("|     ");

    for(int j = 0; j < gameState->origNumPlayers; ++j) {
      if (gameState->players[j].locationPt == &(gameState->board.tiles[i])){
        printf("%d ", gameState->players[j].id);
      }
    }

    printf("\n");
  }
}

bool isGameOver(GameState* gameState) {
  if(gameState->numPlayers <= 1) {
    return true;
  }

  if(gameState->rules.turnLimit == -1 || gameState->currentTurn < gameState->rules.turnLimit) {
    return false;
  } else {
    return true;
  }
}

Move newMove(Player* playerPt) {
  Move move;
  move.playerPt = playerPt;
  move.endsTurn = false;

  printf("Player %d please choose an action\n", move.playerPt->id);

  if(!playerPt->rolledDice) {
    printf("1: Roll Dice\n");
  } else {
    printf("1: End Turn\n");
  }

  printf("2: Inspect Player\n");
  printf("3: Leave Game\n");
  printf("Your action: \n");

  scanf("%d", &(move.type));

  return move;
}

void makeMove(Move* move, GameState* gameState) {
  switch(move->type) {
    case 1 :
      if(!move->playerPt->rolledDice) {
        int diceRoll = rollDice(gameState);
        movePlayer(gameState, move, move->playerPt->id, diceRoll);
        move->playerPt->rolledDice = true;
      } else {
        move->playerPt->rolledDice = false;
        endTurn(move);
      }
      break;
    case 2:
      inspectPlayer(gameState);
      break;
    case 3:
      leaveGame(gameState, move, move->playerPt->id);
      break;
    default:
      printf("Invalid makeMove\n");
      exit(0);
  }
}

void inspectPlayer(GameState* gameState) {
  printf("Which player would you like to inspect?\n");

  Player* currentPlayerPt = gameState->headPlayerPt;
  for(int i = 0; i < gameState->numPlayers; ++i) {
    printf("%d for Player %d\n", currentPlayerPt->id, currentPlayerPt->id);

    currentPlayerPt = currentPlayerPt->nextPlayer;
  }

  printf("Your choice:\n");

  int choice;
  scanf(" %d", &choice);

  printPlayer(gameState, choice);
}

void printPlayer(GameState* gameState, int playerID) {
  printf(" Player %d\n", gameState->players[playerID].id);
  printf("  Cash: $%d\n", gameState->players[playerID].numDollars);
  printf("  Properties owned\n");

  int numPropertiesOwnedInSet = 0;

  for(int i = 0; i < gameState->board.numberOfSets; ++i) {
    numPropertiesOwnedInSet = 0;

    printf("    %d: ", i);
    for(int j = 0; j < gameState->board.numberOfSpaces; ++j) {
      if(gameState->board.tiles[j].setID == i && gameState->board.tiles[j].ownerPt != NULL && gameState->board.tiles[j].ownerPt->id == playerID) {
        if(numPropertiesOwnedInSet > 0) {
          printf(" | ");
        }

        printf("%s", gameState->board.tiles[j].name);

        numPropertiesOwnedInSet += 1;
      }
    }

    printf("\n");
  }


  /*for(int i = 1; i < gameState->board.numberOfSpaces; ++i){
    if(gameState->board.tiles[i].setID != idOfLastSet) {
      idOfLastSet = gameState->board.tiles[i].setID;
      numPropertiesOwnedInSet = 0;
      printf("\n    %d: ", idOfLastSet);
    }

    if(gameState->board.tiles[i].ownerPt != NULL && gameState->board.tiles[i].ownerPt->id == playerID) {
      if(numPropertiesOwnedInSet >= 1) {
        printf(" | ");
      }

      printf("%s", gameState->board.tiles[i].name);

      numPropertiesOwnedInSet += 1;
    }
  }
  printf("\n");*/
}

void endTurn(Move* move) {
  move->endsTurn = true;
}

void movePlayer(GameState* gameState, Move* move, int playerID, int diceRoll) {
  int currentIndexOfPlayer = gameState->players[playerID].locationPt->indexInBoard;
  int timesPassedGo = (currentIndexOfPlayer + diceRoll) / gameState->board.numberOfSpaces;
  int salary = gameState->board.cashWhenPassingGo;
  int position;

  if (currentIndexOfPlayer + diceRoll < gameState->board.numberOfSpaces - 1) {
    position = currentIndexOfPlayer + diceRoll;
    gameState->players[playerID].locationPt = &(gameState->board.tiles[position]);
  } else {
    position = (currentIndexOfPlayer + diceRoll) % gameState->board.numberOfSpaces;
    gameState->players[playerID].locationPt = &(gameState->board.tiles[position]);

    if(gameState->players[playerID].locationPt == &(gameState->board.tiles[0])) {
      for(int i = 0; i < timesPassedGo - 1; ++i) {
        gameState->players[playerID].numDollars += salary;
      }

      gameState->players[playerID].numDollars += gameState->rules.salaryMultiplierForLandingOnGo * salary;
    } else {
      gameState->players[playerID].numDollars += salary * timesPassedGo;
    }
  }

  landOnTile(gameState, move, playerID, position);
}

void landOnTile(GameState* gameState, Move* move, int playerID, int position) {
  if(strcmp(gameState->players[playerID].locationPt->type, "Property") == 0) {
    if (gameState->board.tiles[position].ownerPt == NULL) {
      purchaseProperty(gameState, playerID, position);
    } else if(gameState->board.tiles[position].ownerPt->id != playerID) {
      payRent(gameState, move, position, playerID, gameState->board.tiles[position].ownerPt->id);
    }
  }
}

void purchaseProperty(GameState* gameState, int playerID, int tileID) {
  if(gameState->players[playerID].numDollars >= gameState->board.tiles[tileID].propertyCost) {
    printf("Player %d would you like to buy %s for $%d?\n", gameState->players[playerID].id, gameState->board.tiles[tileID].name, gameState->board.tiles[tileID].propertyCost);
    printf("The rent on this property is $%d\n", gameState->board.tiles[tileID].rent);
    printf("You currently have $%d\n", gameState->players[playerID].numDollars);
    printf("Y for yes, N for no: \n");

    char response;

    scanf(" %c", &response);

    if(toupper(response) == 'Y') {
      gameState->players[playerID].numDollars -= gameState->board.tiles[tileID].propertyCost;
      gameState->players[playerID].properties[tileID] = &(gameState->board.tiles[tileID]);
      gameState->board.tiles[tileID].ownerPt = &(gameState->players[playerID]);
    } else if(toupper(response) == 'N') {
    } else {
      printf("That is not a valid response\n");
      exit(0);
    }
  } else {
    printf("Player %d you do not have enough money to purchase %s\n", playerID, gameState->board.tiles[tileID].name);
    printf("%s costs $%d, but you only have $%d\n", gameState->board.tiles[tileID].name, gameState->board.tiles[tileID].propertyCost, gameState->players[playerID].numDollars);
  }
}

void payRent(GameState* gameState, Move* move, int tileIndex, int player1ID, int player2ID) {
  int amountToPay;

  if(isInCompletedSet(gameState, tileIndex)) {
    amountToPay = gameState->rules.propertySetMultiplier * gameState->board.tiles[tileIndex].rent;
  } else {
    amountToPay = gameState->board.tiles[tileIndex].rent;
  }

  makeTransaction(gameState, move, player1ID, player2ID, amountToPay);

  if(amountToPay <= gameState->players[player1ID].numDollars) {
    printf("Player %d payed Player %d $%d for landing on %s\n", player1ID, player2ID, amountToPay, gameState->board.tiles[tileIndex].name);
  }
}

void makeTransaction(GameState* gameState, Move* move, int player1ID, int player2ID, int amountToPay) {
  bool isBankrupt = false;

  if(amountToPay >= gameState->players[player1ID].numDollars) {
    amountToPay = gameState->players[player1ID].numDollars;
    isBankrupt = true;
  }

  gameState->players[player1ID].numDollars -= amountToPay;

  gameState->players[player2ID].numDollars += amountToPay;

  if(isBankrupt) {
    printf("Player %d went bankrupt to Player %d\n", gameState->players[player1ID].id, gameState->players[player2ID].id);
    bankruptToPlayer(gameState, player1ID, player2ID);
    leaveGame(gameState, move, player1ID);
  }
}

void bankruptToPlayer(GameState* gameState, int player1ID, int player2ID) {
  for(int i = 0; i < gameState->board.numberOfSpaces; ++i) {
    if(gameState->players[player1ID].properties[i] != NULL) {
      gameState->players[player2ID].properties[i] = gameState->players[player1ID].properties[i];
      gameState->players[player1ID].properties[i]->ownerPt = &(gameState->players[player2ID]);
      gameState->players[player1ID].properties[i] = NULL;
    }
  }
}

void leaveGame(GameState* gameState, Move* move, int playerID) {
  gameState->players[playerID].nextPlayer->previousPlayer = gameState->players[playerID].previousPlayer;
  gameState->players[playerID].previousPlayer = gameState->players[playerID].nextPlayer;
  gameState->players[playerID].locationPt = NULL;

  if(gameState->players[playerID].id == gameState->headPlayerPt->id) {
    gameState->headPlayerPt = gameState->headPlayerPt->nextPlayer;
  }

  for(int i = 0; i < gameState->board.numberOfSpaces; ++i) {
    if(gameState->players[playerID].properties[i] != NULL) {
      gameState->players[playerID].properties[i]->ownerPt = NULL;
      gameState->players[playerID].properties[i] = NULL;
    }
  }

  gameState->numPlayers -= 1;
  move->endsTurn = true;

}

bool isInCompletedSet(GameState* gameState, int tileIndex) {
  int i = tileIndex;

  while(gameState->board.tiles[i].intrasetID > 0) {
    --i;
  }

  while(i < gameState->board.numberOfSpaces && gameState->board.tiles[i].setID == gameState->board.tiles[tileIndex].setID) {
    if(gameState->board.tiles[i].ownerPt == NULL) {
      return false;
    } else if (gameState->board.tiles[i].ownerPt->id != gameState->board.tiles[tileIndex].ownerPt->id) {
      return false;
    } else {
      //printf("tile: %d is in a completed set\n", tileIndex);
    }
    ++i;
  }

  return true;
}

int getRandomNumber(GameState * gameState) {
  int num;
  fscanf(gameState->randomNumbersFPt, "%d", &num);
  return num;
}

int rollDice(GameState* gameState) {
  int roll1 = getRandomNumber(gameState) % 6 + 1;
  int roll2 = getRandomNumber(gameState) % 6 + 1;
  int result = roll1 + roll2;

  printf("You rolled a %d!\n", result);

  return roll1 + roll2;
}

void changeTurn(GameState* gameState) {
  /*if(gameState->playerTurn == gameState->numPlayers - 1) {
    gameState->playerTurn = 0;
  } else {
    gameState->playerTurn += 1;
  }*/

  gameState->currentPlayerPt = gameState->currentPlayerPt->nextPlayer;
}

void declareWinners(GameState* gameState) {
  printf("The winners are\n");

  Player* currentPlayerPt = gameState->headPlayerPt;
  for(int i = 0; i < gameState->numPlayers; ++i) {
    printf("Player %d\n", currentPlayerPt->id);
    currentPlayerPt = currentPlayerPt->nextPlayer;
  }

  printf("\n");
}

int lengthOfLongestName(Board* board) {
  int longest = 0;
  for(int i = 0; i < board->numberOfSpaces; ++i) {
    if(strlen(board->tiles[i].name) > longest) {
      longest = (int)strlen(board->tiles[i].name);
    }
  }
  return longest;
}