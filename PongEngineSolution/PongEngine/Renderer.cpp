static void ClearScreen(u32 color)
{
	u32* pixel = (u32*)renderState.memory;
	for (int y = 0; y < renderState.height; y++)
	{
		for (int x = 0; x < renderState.width; x++)
		{
			*pixel++ = color;
		}
	}
}

static void DrawRectInPixels(int x0, int y0, int x1, int y1, u32 color)
{
	x0 = Clamp(x0, 0, renderState.width);
	x1 = Clamp(x1, 0, renderState.width);
	y0 = Clamp(y0, 0, renderState.height);
	y1 = Clamp(y1, 0, renderState.height);

	for (int y = y0; y < y1; y++)
	{
		u32* pixel = (u32*)renderState.memory + x0 + y * renderState.width;
		for (int x = x0; x < x1; x++)
		{
			*pixel++ = color;
		}
	}
}

GLOBAL float renderScale = 0.01f;

static void DrawArenaBorders(float arena_x, float arena_y, u32 color) {
	arena_x *= renderState.height * renderScale;
	arena_y *= renderState.height * renderScale;

	int x0 = (int)((float)renderState.width * .5f - arena_x);
	int x1 = (int)((float)renderState.width * .5f + arena_x);
	int y0 = (int)((float)renderState.height * .5f - arena_y);
	int y1 = (int)((float)renderState.height * .5f + arena_y);

	DrawRectInPixels(0, 0, renderState.width, y0, color);
	DrawRectInPixels(0, y1, x1, renderState.height, color);
	DrawRectInPixels(0, y0, x0, y1, color);
	DrawRectInPixels(x1, y0, renderState.width, renderState.height, color);
}

static void DrawRect(float x, float y, float halfSizeX, float halfSizeY, u32 color)
{
	x *= renderState.height * renderScale;
	y *= renderState.height * renderScale;
	halfSizeX *= renderState.height * renderScale;
	halfSizeY *= renderState.height * renderScale;

	x += renderState.width * 0.5f;
	y += renderState.height * 0.5f;

	//Change to pixels
	int x0 = x - halfSizeX;
	int x1 = x + halfSizeX;
	int y0 = y - halfSizeY;
	int y1 = y + halfSizeY;

	DrawRectInPixels(x0, y0, x1, y1, color);
}

const char* letters[][7] = {
	" 00",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"000",
	"0  0",
	"0  0",
	"000",
	"0  0",
	"0  0",
	"000",

	" 000",
	"0",
	"0",
	"0",
	"0",
	"0",
	" 000",

	"000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"000",

	"0000",
	"0",
	"0",
	"000",
	"0",
	"0",
	"0000",

	"0000",
	"0",
	"0",
	"000",
	"0",
	"0",
	"0",

	" 000",
	"0",
	"0",
	"0 00",
	"0  0",
	"0  0",
	" 000",

	"0  0",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"000",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",
	"000",

	" 000",
	"   0",
	"   0",
	"   0",
	"0  0",
	"0  0",
	" 000",

	"0  0",
	"0  0",
	"0 0",
	"00",
	"0 0",
	"0  0",
	"0  0",

	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0000",

	"00 00",
	"0 0 0",
	"0 0 0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",

	"00  0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0  00",

	"0000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0000",

	" 000",
	"0  0",
	"0  0",
	"000",
	"0",
	"0",
	"0",

	" 000 ",
	"0   0",
	"0   0",
	"0   0",
	"0 0 0",
	"0  0 ",
	" 00 0",

	"000",
	"0  0",
	"0  0",
	"000",
	"0  0",
	"0  0",
	"0  0",

	" 000",
	"0",
	"0 ",
	" 00",
	"   0",
	"   0",
	"000 ",

	"000",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",

	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	" 00",

	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	" 0 0",
	"  0",

	"0   0 ",
	"0   0",
	"0   0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	" 0 0 ",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	" 0 0",
	"0   0",
	"0   0",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	"  0",
	"  0",
	"  0",

	"0000",
	"   0",
	"  0",
	" 0",
	"0",
	"0",
	"0000",

	"",
	"",
	"",
	"",
	"",
	"",
	"0",

	"   0",
	"  0",
	"  0",
	" 0",
	" 0",
	"0",
	"0",
};

static void DrawAText(const char* text, float x, float y, float size, u32 color) {
	float half_size = size * .5f;
	float original_y = y;

	while (*text) {
		if (*text != 32) {
			const char** letter;
			if (*text == 47) letter = letters[27];
			else if (*text == 46) letter = letters[26];
			else letter = letters[*text - 'A'];
			float original_x = x;

			for (int i = 0; i < 7; i++) {
				const char* row = letter[i];
				while (*row) {
					if (*row == '0') {
						DrawRect(x, y, half_size, half_size, color);
					}
					x += size;
					row++;
				}
				y -= size;
				x = original_x;
			}
		}
		text++;
		x += size * 6.f;
		y = original_y;
	}
}

static void DrawNumbers(int number, float xPos, float yPos, float size, u32 color)
{
	float halfSize = size * 0.5f;

	bool drewNumber = false;
	while (!drewNumber || number)
	{
		drewNumber = true;
		int digit = number % 10;
		number = number / 10;

		switch (digit)
		{
			case 0:
				DrawRect(xPos - size, yPos, halfSize, 2.5f * size, color);
				DrawRect(xPos + size, yPos, halfSize, 2.5f * size, color);
				DrawRect(xPos, yPos + size * 2.0f, halfSize, halfSize, color);
				DrawRect(xPos, yPos - size * 2.0f, halfSize, halfSize, color);
				xPos -= size * 4.0f;
			break;
			
			case 1:
			{
				DrawRect(xPos + size, yPos, halfSize,2.5f * size,color);
				xPos -= size * 2.0f;
			}
			break;
			case 2:
			{
				DrawRect(xPos, yPos + size * 2.0f, 1.5f * size, halfSize, color);
				DrawRect(xPos, yPos, 1.5f * size, halfSize, color);
				DrawRect(xPos, yPos - size * 2.0f, 1.5f * size, halfSize, color);
				DrawRect(xPos + size, yPos + size, halfSize, halfSize, color);
				DrawRect(xPos - size, yPos - size, halfSize, halfSize, color);
				xPos -= size * 4.0f;
			}
			break;
			case 3:
			{
				DrawRect(xPos - halfSize, yPos + size * 2.0f, size, halfSize, color);
				DrawRect(xPos - halfSize, yPos, size, halfSize, color);
				DrawRect(xPos - halfSize, yPos - size * 2.0f, size, halfSize, color);
				DrawRect(xPos + size, yPos, halfSize, 2.5f * size, color);
				xPos -= size * 4.0f;
			}
			break;
			case 4:
			{
				DrawRect(xPos + size, yPos, halfSize, 2.5f * size, color);
				DrawRect(xPos - size, yPos + size, halfSize, 1.5f * size, color);
				DrawRect(xPos, yPos, halfSize, halfSize, color);
				xPos -= size * 4.0f;
			}
			break;
			case 5:
			{
				DrawRect(xPos, yPos + size * 2.0f, 1.5f * size, halfSize, color);
				DrawRect(xPos, yPos, 1.5f * size, halfSize, color);
				DrawRect(xPos, yPos - size * 2.0f, 1.5f * size, halfSize, color);
				DrawRect(xPos - size, yPos + size, halfSize, halfSize, color);
				DrawRect(xPos + size, yPos - size, halfSize, halfSize, color);
				xPos -= size * 4.0f;
			}
			break;
			case 6:
			{
				DrawRect(xPos + halfSize, yPos + size * 2.0f, size, halfSize, color);
				DrawRect(xPos + halfSize, yPos, size, halfSize, color);
				DrawRect(xPos + halfSize, yPos - size * 2.0f, size, halfSize, color);
				DrawRect(xPos - size, yPos, halfSize, 2.5f * size, color);
				DrawRect(xPos + size, yPos - size, halfSize, halfSize, color);
				xPos -= size * 4.0f;
			}
			break;
			case 7:
			{
				DrawRect(xPos + size, yPos, halfSize, 2.5f * size, color);
				DrawRect(xPos - halfSize, yPos + size * 2.0f, size, halfSize, color);
				xPos -= size * 4.0f;
			}
			break;
			case 8:
			{
				DrawRect(xPos - size, yPos, halfSize, 2.5f * size, color);
				DrawRect(xPos + size, yPos, halfSize, 2.5f * size, color);
				DrawRect(xPos, yPos + size * 2.0f, halfSize, halfSize, color);
				DrawRect(xPos, yPos - size * 2.0f, halfSize, halfSize, color);
				DrawRect(xPos, yPos, halfSize, halfSize, color);
				xPos -= size * 4.0f;
			}
			break;
			case 9:
			{
				DrawRect(xPos - halfSize, yPos + size * 2.0f, size, halfSize, color);
				DrawRect(xPos - halfSize, yPos, size, halfSize, color);
				DrawRect(xPos - halfSize, yPos - size * 2.0f, size, halfSize, color);
				DrawRect(xPos + size, yPos, halfSize, 2.5f * size, color);
				DrawRect(xPos - size, yPos + size, halfSize, halfSize, color);
				xPos -= size * 4.0f;
			}
			break;
			default:
			break;
		}
	}
}