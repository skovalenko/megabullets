#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__
#include "objects.h"
#include "field.h"
#include <map>
#include <vector>

// этот класс хранит информацию о положении объектов в €чейках
// игрового пол€
class game_area
{
public:
	enum OBJECT_TYPE
	{
		PLAYER			= 0x01,
		BLOCKED_CELL	= 0x02,
		START_CELL		= 0x04,
		FINISH_CELL		= 0x08,
		BULLET			= 0x10,
		SOLDIER			= 0x20,
		DEATHBALL		= 0x40
	};

	// информаци€ об объекте сцены
	struct object_info
	{
		int object_id;
		int object_type;

		object_info(const int oid, const int ot)
			: object_id(oid),
			object_type(ot)
		{}
	};

	typedef std::vector <object_info> objects_t;
	typedef objects_t::iterator objects_iterator_t;
	typedef objects_t::const_iterator objects_const_iterator_t;

	struct cell_index
	{
		int x_num;
		int y_num;

		cell_index(const int x = 0, const int y = 0)
			: x_num(x),
			y_num(y)
		{
		}

		bool operator<(const cell_index &c1) const
		{
			if (x_num == c1.x_num) return y_num > c1.y_num;
			else return x_num > c1.x_num;
		}
	};

	struct cell_info
	{
		objects_t objects;
	};

	typedef std::map <cell_index, cell_info> cell_map_t;
	typedef cell_map_t::iterator cell_map_iterator_t;
	typedef cell_map_t::value_type cell_map_value_type_t;

	// задаем начальную точку, два вектора и параметры одной €чейки
	// считаем, что все €чейки имеют одинаковые параметры
	game_area(const point2d_t &p, const math2d::line2d &v1, const math2d::line2d &v2,
		const float alen, const float blen);

	// метод принимает точку на поле и тип объекта
	// возвращает true, если объект этого типа присутствует в €чейке с этой точкой
	bool check_object(const point2d_t &p, const int t);

	// добавл€ем информацию о том, что в €чейке объект типа t
	void add_object(const point2d_t &p, const object_info &info);

	// удал€ем объект из €чейки
	int delete_object(const point2d_t &p, const object_info &info);
	void delete_object_by_info(const object_info &info);

	// метод возвращает точку дл€ отображени€ текстуры
	// т.к. поле в изометрии, а текстуры нет.
	point2d_t get_cell_point_by_position(const point2d_t &p);

	// метод возвращает индекс €чейки
	point2d_t get_cell_point_by_index(const cell_index &idx);

private:
	cell_map_t __core;

	// определим индекс €чейки по точке, лежащей в ней
	cell_index __get_cell_index_by_point(const point2d_t &p) const;

	// параметры €чейки (длина по x и y)
	const float __cell_x_length;
	const float __cell_y_length;
	
	// точка начала отсчета
	const point2d_t		__top_point;

	// векторы направлени€
	math2d::line2d		__vector_left;
	math2d::line2d		__vector_right;
};

class game_controller
{
public:
	game_controller();
	~game_controller();

	void add_field(game_field *fld);
	void add_player(people *pl);
	void add_cannon(cannon *c);
	void add_explosions(explosions *expl);
	void add_soldier(people *s);
	void add_deathball(death_ball *ball);

	void process();

private:
	typedef std::vector <cannon*> cannons_t;
	typedef cannons_t::iterator cannons_iterator_t;

	void __process_user_action();

	void __process_mouse_rbtn_clicked();
	void __process_mouse_lbtn_clicked();
	
	void __process_cannon_shots();

	void __process_player_movement();
	void __process_soldier_movement();
	void __process_deathball_movement();

	void __process_collisions();

	game_field	*__field;
	HGE			*__hge;
	game_area   *__area;
	people		*__player;
	people		*__soldier;
	cannons_t	__cannons;
	explosions	*__explosions;
	death_ball	*__ball;

	const point2d_t	__start_position;
	const point2d_t	__soldier_start_position;
	const point2d_t	__soldier_fin_position;
	const point2d_t	__ball_start_position;
};


#endif