// --------------------------------------------------------------------------------------
#include "Button.h"
#include "FastLED.h"

// --------------------------------------------------------------------------------------
#define NUM_LEDS 20

#define DATA_PIN 11
#define CLOCK_PIN 13

#define BTN1 6
#define BTN2 7

#define NUM_MODES 2

// --------------------------------------------------------------------------------------
// Program Variables
CRGB leds[NUM_LEDS];

long lastUpdateMillis = 0;
int timeSinceLastButtonPress = 0;
byte timeSinceButtonRepeat = 0;

byte mode = 0;
fract16 animPos = 0;

byte brightness = 64;
byte value = 128;

bool adjustingBrightness = false;
bool adjustingValue = false;
bool ignoreBtn1 = false;

Button btn1 = Button(BTN1);
Button btn2 = Button(BTN2);

// --------------------------------------------------------------------------------------
void setup()
{
	//Serial.begin(9600);
	lastUpdateMillis = millis();

	FastLED.addLeds<APA102, RGB>(leds, NUM_LEDS);
	FastLED.setBrightness(brightness);
}

// --------------------------------------------------------------------------------------
void loop()
{
	long newMillis = millis();
	int deltaTime = newMillis - lastUpdateMillis;

	// Update Buttons
	btn1.Update(deltaTime);
	btn2.Update(deltaTime);

	// Update Animation
	animPos += deltaTime * map8(255 - value, 1, 10);
	//Serial.println(animPos);

	UpdateState(deltaTime);
	Render(deltaTime);

	//Serial.println(mode);

	lastUpdateMillis = newMillis;
}

// --------------------------------------------------------------------------------------
void UpdateState(int deltaTime)
{
	if (adjustingBrightness || adjustingValue)
	{
		// Adjust timeout
		if ((btn1.State == BTN_Down) || (btn2.State == BTN_Down))
		{
			timeSinceLastButtonPress = 0;
		}
		else
		{
			timeSinceLastButtonPress += deltaTime;
			if (timeSinceLastButtonPress > 2000)
			{
				timeSinceLastButtonPress = 0;
				adjustingBrightness = false;
				adjustingValue = false;
				return;
			}
		}

		// TODO: Clean this copy-pasta up
		int change = 0;
		if (btn1.State == BTN_OnPressed)
		{
			change = 6;
			timeSinceButtonRepeat = 0;
		}
		else if ((btn1.State == BTN_Down) && (btn1.HeldTime > 500))
		{
			timeSinceButtonRepeat += deltaTime;
			if (timeSinceButtonRepeat > 10)
			{
				timeSinceButtonRepeat = 0;
				change = 1;
			}
		}
		else if (btn2.State == BTN_OnPressed)
		{
			change = -6;
			timeSinceButtonRepeat = 0;
		}
		else if ((btn2.State == BTN_Down) && (btn2.HeldTime > 500))
		{
			timeSinceButtonRepeat += deltaTime;
			if (timeSinceButtonRepeat > 10)
			{
				timeSinceButtonRepeat = 0;
				change = -1;
			}
		}

		if (adjustingBrightness)
		{
			brightness = max(min(change + brightness, 255), 0);;
			FastLED.setBrightness(dim8_video(max(brightness, 20)));
		}
		else
		{
			value += change;
			//Serial.println(value);
		}
	}
	else
	{
		// Power Up
		// TODO: Sleep here for power saver, use an ISR?
		if (mode == 0)
		{
			if ((btn1.State == BTN_OnReleased) || (btn2.State == BTN_OnReleased))
			{
				if (ignoreBtn1)
				{
					ignoreBtn1 = false;
					return;
				}
				mode = 1;
			}
		}
		else
		{
			// Power Off
			if (btn1.HeldTime > 2000)
			{
				mode = 0;
				ignoreBtn1 = true;
				fill_solid(leds, NUM_LEDS, CRGB::Black);
				FastLED.show();
			}
			// Cycle Modes
			else if (btn1.State == BTN_OnReleased)
			{
				++mode;
				if (mode > NUM_MODES)
				{
					mode = 1;
				}
			}
			// Change Brightness
			else if (btn2.HeldTime > 1000)
			{
				adjustingBrightness = true;
			}
			// Change Value
			else if (btn2.State == BTN_OnReleased)
			{
				adjustingValue = true;
			}
		}

		//if (btn1.State == BTN_OnReleased)
		//{
		//	// Mode 0 = off
		//	// Long press to turn off, otherwise cycle through modes
		//	if (btn1.HeldTime < 1000)
		//	{
		//		++mode;
		//		if (mode > NUM_MODES)
		//		{
		//			mode = 1;
		//		}
		//	}
		//	else
		//	{
		//		mode = 0;
		//	}
		//}
		//else if ((mode != 0) && (btn2.State == BTN_OnReleased))
		//{
		//	// Short press to adjust value
		//	// Long press to adjust brightness
		//	if (btn2.HeldTime < 1000)
		//	{
		//		adjustingValue = true;
		//	}
		//	else
		//	{
		//		adjustingBrightness = true;
		//	}
		//}
	}
}

// --------------------------------------------------------------------------------------
void Render(int deltaTime)
{
	// Off
	if (mode == 0)
	{
		// TODO: put into sleep mode and only check for wakeup once every few seconds
		fill_solid(leds, NUM_LEDS, CRGB::Black);
	}

	// Solid Color
	else if (mode == 1)
	{
		fill_solid(leds, NUM_LEDS, CHSV(value, 255, 255));
	}

	// Rainbow
	else if (mode == 2)
	{
		fill_solid(leds, NUM_LEDS, CHSV(scale8(255, animPos >> 8), 255, 255));
	}

	// Rainbow Chase
	else if (mode == 3)
	{

	}

	// Raindrops
	else if (mode == 4)
	{

	}

	// Draw Menu
	if (adjustingBrightness)
	{
		leds[map8(brightness, 0, NUM_LEDS - 1)] = CRGB::White;
	}
	else if (adjustingValue)
	{
		leds[map8(value, 0, NUM_LEDS - 1)] = CRGB::White;
	}

	// Show
	FastLED.show();
}
