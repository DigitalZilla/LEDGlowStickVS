// Button.h

#ifndef _BUTTON_h
#define _BUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

enum BtnState
{
	BTN_OnPressed,
	BTN_OnReleased,
	BTN_Up,
	BTN_Down
};

class Button
{
 protected:
	 byte PIN;


 public:
	 int HeldTime;
	 enum BtnState State;

	 Button(byte pin);
	 void Update(int deltaTime);
};

#endif

