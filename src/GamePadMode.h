#ifndef GAMEPADMODE_H
#define GAMEPADMODE_H

#include "Mode.h"
#include "Hardware.h"
#include "PongGame.h"
#include "RacingGame.h"
#include "SpaceShooterGame.h"

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

  // Menu state
  enum GameState { MENU, PONG, FLAPPY, SHOOTER };
  GameState currentGame = MENU;

  int selectedGame = 0;
  const int numGames = 3;
  const char* gameNames[3] = {"PONG", "RACING", "SPACE SHOOTER"};

  void drawMenu();
  void startGame(int index);
};

#endif