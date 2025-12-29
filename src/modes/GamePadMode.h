#ifndef GAMEPADMODE_H
#define GAMEPADMODE_H

#include "modes/Mode.h"
#include "hal/Hardware.h"
#include "games/PongGame.h"
#include "games/RacingGame.h"
#include "games/SpaceShooterGame.h"
#include "games/StarshipGame.h"
#include "games/BalanceGame.h"

class GamePadMode : public Mode {
public:
  explicit GamePadMode(TFTHandler& tft, Hardware& hw);

  void enter() override;
  void update() override;
  void exit() override;

private:
  Hardware& hw;

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