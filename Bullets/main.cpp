#include "hge.h"
#include "game.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	game g;
	
	g.init();
	
	g.start();

	return 0;
}
