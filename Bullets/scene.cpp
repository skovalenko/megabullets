#include "scene.h"

HGE* scene::__hge = 0;
scene::objects_t scene::__objects = objects_t();
game_controller * scene::__controller = 0;
scene::scene()
{
	// настраиваем фреймверк
	__hge = hgeCreate(HGE_VERSION);
	__hge->System_SetState(HGE_SCREENWIDTH, 1000);
	__hge->System_SetState(HGE_SCREENHEIGHT, 600);
	__hge->System_SetState(HGE_FRAMEFUNC, frame_func);
	__hge->System_SetState(HGE_RENDERFUNC, render_func);
	__hge->System_SetState(HGE_WINDOWED, true);
	__hge->System_SetState(HGE_USESOUND, false);
	__hge->System_SetState(HGE_FPS, HGEFPS_VSYNC);
	__hge->System_SetState(HGE_TITLE, "Mega BUllets");
	__hge->System_SetState(HGE_HIDEMOUSE, false);
}

bool scene::init()
{
	return __hge->System_Initiate();
}

bool scene::start()
{
	return __hge->System_Start();
}

scene::~scene()
{
	delete __controller;

	for (objects_iterator_t iter = __objects.begin(); iter != __objects.end(); ++iter)
	{
		delete *iter;
	}

	__hge->System_Shutdown();
	__hge->Release();
}

bool scene::frame_func()
{
	float delta = __hge->Timer_GetDelta();

	__controller->process();

	for (objects_iterator_t iter = __objects.begin(); iter != __objects.end(); ++iter)
	{
		(*iter)->update(delta);
	}

	return false;
}

bool scene::render_func()
{
	__hge->Gfx_BeginScene();
	__hge->Gfx_Clear(0);

	for (objects_iterator_t iter = __objects.begin(); iter != __objects.end(); ++iter)
	{
		(*iter)->render();
	}

	__hge->Gfx_EndScene();

	return false;
}

void scene::insert_object(scene_object *obj)
{
	if (obj)
	{
		__objects.push_back(obj);
	}
}

void scene::set_game_controller(game_controller *ctrl)
{
	__controller = ctrl;
}