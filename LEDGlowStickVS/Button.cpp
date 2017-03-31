#include "Button.h"

Button::Button(byte pin)
{
	PIN = pin;

	pinMode(PIN, INPUT_PULLUP);

	HeldTime = 0;
	State = BTN_Up;
}

void Button::Update(int deltaTime)
{
	bool btnPressed = !digitalRead(PIN);
	if (btnPressed)
	{
		if (HeldTime == 0)
		{
			State = BTN_OnPressed;
		}
		else
		{
			State = BTN_Down;
		}

		HeldTime += deltaTime;
	}
	else
	{
		if ((HeldTime > 0) && (State != BTN_OnReleased))
		{
			State = BTN_OnReleased;
		}
		else
		{
			State = BTN_Up;
			HeldTime = 0;
		}
	}
}
