struct ButtonState 
{
	bool isDown;
	bool changed;
};

enum
{
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_W,
	BUTTON_S,

	BUTTON_COUNT,
};

struct Input
{
	ButtonState buttonsState[BUTTON_COUNT];
};