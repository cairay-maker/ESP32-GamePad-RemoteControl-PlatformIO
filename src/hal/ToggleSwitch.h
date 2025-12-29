#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

class ToggleSwitch {
public:
  explicit ToggleSwitch(int pin);
  bool isOn();                   // Returns true when switch is ON (LOW with pull-up)

private:
  int pin;
  bool lastState = false;
};

#endif