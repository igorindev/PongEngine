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