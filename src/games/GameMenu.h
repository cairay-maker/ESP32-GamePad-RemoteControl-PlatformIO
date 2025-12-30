#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "Activity.h"
#include "games/PongGame.h"
#include "games/RacingGame.h"
#include "games/SpaceShooterGame.h"
#include "games/StarshipGame.h"
#include "games/BalanceGame.h"

class GameMenu : public Activity {
public:
  explicit GameMenu(TFTHandler& tft, Hardware& hw);

  void init() override {} 
  void draw() override {} 
  void enter() override;
  void update() override;
  void exit() override;

private:
  unsigned long selectHoldStartTime = 0;
  bool isHoldingSelect = false;
  bool suppressNextRelease = false;
  bool lastWifiState = false; 

  // Game instances
  PongGame pongGame;
  RacingGame racingGame;
  SpaceShooterGame spaceShooterGame;
  StarshipGame starshipGame;
  BalanceGame balanceGame;

  enum GameState { MENU, BALANCE, SHOOTER, STARSHIP, RACING, PONG };
  GameState currentGame = MENU;

  int selectedGame = 0;
  const int numGames = 5;
  const char* gameNames[5] = {"BALANCE SIMULATOR", "SPACE SHOOTER", "STARSHIP RUN", "RACING", "PONG"};

  void drawMenu();
  void startGame(int index);
};

#endif