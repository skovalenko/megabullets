#ifndef __FIELD_H__
#define __FIELD_H__
#include "objects.h"
#include <iostream>
#include "func.h"
#include <assert.h>

class cell : public scene_object
{
public:
	enum CELL_TYPE
	{
		BLOCK,
		START,
		FINISH
	};

	cell(const float x, const float y, const CELL_TYPE t)
		: scene_object(x, y)
	{
		// параметры загружаемого изображения
		static const char* block_name = "images/block.png";
		static const char* start_finish_name = "images/gs.png";

		static const float width = 40;
		static const float hight = 21;

		switch (t)
		{
		case BLOCK:
			__texture = __hge->Texture_Load(block_name);
			break;

		case START:
			__texture = __hge->Texture_Load(start_finish_name);
			break;

		case FINISH:
			__texture = __hge->Texture_Load(start_finish_name);
			break;

		default:
			assert(false);
			break;
		}
		

		__sprite = new hgeSprite(__texture, 0, 0, width, hight);
	}

	~cell()
	{
		delete __sprite;
	}

	virtual void render()
	{
		__sprite->Render(__position.x, __position.y);
	}

	virtual void update(const float delta)
	{
	}

private:
	hgeSprite			*__sprite;
};


// 480x22 - топовая точка
// 520x500 - нижняя точка
// 21х252 - левая точка
// 980х272 - правая точка
class game_field : public image_object
{
public:
	game_field(const char *f, const float w, const float h,
		const point2d_t &top, const point2d_t &left, const point2d_t &right, const point2d_t &bottom,
		const int cell_xcount, const int cell_ycount)
		: image_object(f, 0, 0, w, h),
		__top(top),
		__left(left),
		__right(right),
		__bottom(bottom),
		__x_cell_count(cell_xcount),
		__y_cell_count(cell_ycount),
		__cell_x_length((left - top).Length() / cell_xcount),
		__cell_y_length((right - top).Length() / cell_ycount)
	{
	}

	~game_field()
	{
	}

	virtual void render()
	{
		__sprite->Render(0, 0);

		// рендерим все препятствия
		for (cells_iterator_t iter = __cells.begin(); iter != __cells.end(); ++iter)
		{
			(*iter)->render();
		}
	}

	int add_block_cell(const point2d_t &pos)
	{
		cell *c = new cell(pos.x, pos.y, cell::BLOCK);

		__cells.push_back(c);

		return c->get_object_id();
	}

	// удаление препятствия по id
	void remove_cell(const int cellid)
	{
		for (cells_iterator_t iter = __cells.begin(); iter != __cells.end(); ++iter)
		{
			cell *c = *iter;

			if (c->get_object_id() == cellid)
			{
				delete c;

				__cells.erase(iter);

				break;
			}
		}
	}

	int add_start_cell(const point2d_t &pos)
	{
		cell *c = new cell(pos.x, pos.y, cell::START);

		__cells.push_back(c);

		return c->get_object_id();
	}


	int add_finish_cell(const point2d_t &pos)
	{
		cell *c = new cell(pos.x, pos.y, cell::FINISH);

		__cells.push_back(c);

		c->get_object_id();

		return c->get_object_id();
	}

	// проверка принадлежности точки игровому полю
	bool check_field_border(const point2d_t pt) const
	{
		// площадка представляет собой 4угольник
		// триангулируем его - получим два треугольника
		math2d::triangle tr1(__top, __left, __bottom);

		math2d::triangle tr2(__top, __right, __bottom);

		// теперь проверим - попал ли курсор в нашу область
		// если он попал хотя бы в один треугольник - success
		return (tr1.test_point(pt) || tr2.test_point(pt));
	}

	virtual void update(const float delta)
	{
		
	}

	const int& get_x_cell_count()		const { return __x_cell_count; }
	const int& get_y_cell_count()		const { return __y_cell_count; }
	const float& get_cell_x_length()	const { return __cell_x_length; }
	const float& get_cell_y_length()	const { return __cell_y_length; }
	const point2d_t& get_top_point()	const { return __top; }
	const point2d_t& get_left_point()	const { return __left; }
	const point2d_t& get_right_point()	const { return __right; }
	const point2d_t& get_bottom_point() const { return __bottom; }

private:
	typedef std::vector <cell*> cells_t;
	typedef cells_t::iterator cells_iterator_t;

	cells_t		__cells;

	// границы игрового поля
	const point2d_t __top;
	const point2d_t __left;
	const point2d_t __right;
	const point2d_t __bottom;

	// количество ячеек
	const int __x_cell_count;
	const int __y_cell_count;

	// параметры ячейки
	const float __cell_x_length;
	const float __cell_y_length;
};


#endif