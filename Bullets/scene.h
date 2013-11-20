#ifndef SCENE_H_
#define SCENE_H_
#include "hge.h"
#include "hgesprite.h"
#include "hgevector.h"
#include <vector>
#include "scene.h"
#include "objects.h"
#include "game_controller.h"

class scene
{
public:
	scene();
	~scene();

	bool init();
	bool start();

	// этим методом сохраняем все объекты и дальше забываем про все вызовы
	// render и update
	void insert_object(scene_object *obj);

	void set_game_controller(game_controller *ctrl);

private:
	typedef std::vector <scene_object*> objects_t;
	typedef objects_t::iterator			objects_iterator_t;

	static bool frame_func();
	static bool render_func();

	// контейнер для объектов
	static objects_t		__objects;

	// контроллер объектов
	static game_controller	*__controller;
	static HGE				*__hge;
};

#endif