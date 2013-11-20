#include "game_controller.h"
#include <cassert>
#include <windows.h>

game_area::game_area(const point2d_t &p, const math2d::line2d &v1, const math2d::line2d &v2,
	const float alen, const float blen)
	: __top_point(p),
	__vector_left(v1),
	__vector_right(v2),
	__cell_x_length(alen),
	__cell_y_length(blen)
{
}

game_area::cell_index game_area::__get_cell_index_by_point(const point2d_t &p) const
{
	cell_index idx;

	// ������� ��� ���������� ������������� ����� p �� ������ ��
	// ��� ����� ��� ����������� ����� ������, ������������ A� �
	// ���������� ����� ����� p. ����� ������ ����� ����������� ���� ������.

	math2d::line2d xline;
	xline.origin = p;
	xline.direction = __vector_right.direction;
	
	point2d_t res;

	if (!xline.line_intersect(__vector_left, res))
	{
		assert(false);
	}
	
	idx.x_num = (int)((__top_point - res).Length() / __cell_x_length);


	// ����������� �������� ������ ��� ������ ������� ����
	xline.direction = __vector_left.direction;

	if (!xline.line_intersect(__vector_right, res))
	{
		assert(false);
	}

	idx.y_num = (int)((__top_point - res).Length() / __cell_y_length);

	return idx;
}

bool game_area::check_object(const point2d_t &p, const int t)
{
	// ���������� ����� ������
	cell_index idx = __get_cell_index_by_point(p);

	// �������, ��� ����� � ���� ������
	cell_map_iterator_t iter = __core.find(idx);

	if (iter != __core.end())
	{
		const objects_t &objs = iter->second.objects;
		
		// ���������� ���� ��������
		for (objects_const_iterator_t oiter = objs.begin(); oiter != objs.end(); ++oiter)
		{
			if ((t & (*oiter).object_type)) return true;
			//if (t == (*oiter).object_type) return true;
		}
	}

	return false;
}

void game_area::add_object(const point2d_t &p, const object_info &t)
{
	// ���������� ����� ������
	cell_index idx = __get_cell_index_by_point(p);

	// �������, ��� ����� � ���� ������
	cell_map_iterator_t iter = __core.find(idx);

	if (iter == __core.end())
	{
		cell_info info;
		info.objects.push_back(t);

		__core.insert(cell_map_value_type_t(idx, info));
	}
	else
	{
		iter->second.objects.push_back(t);
	}
}

int game_area::delete_object(const point2d_t &p, const object_info &t)
{
	// ���������� ����� ������
	cell_index idx = __get_cell_index_by_point(p);

	// �������, ��� ����� � ���� ������
	cell_map_iterator_t iter = __core.find(idx);

	if (iter != __core.end())
	{
		cell_info &info = iter->second;

		for (objects_const_iterator_t oit = info.objects.begin();
			oit != info.objects.end(); ++oit)
		{
			const object_info &i = *(oit);

			if (i.object_type == t.object_type)
			{
				info.objects.erase(oit);
				return i.object_id;
			}
		}
	}

	return 0;
}

void game_area::delete_object_by_info(const object_info &t)
{
	for (cell_map_iterator_t iter = __core.begin(); iter != __core.end(); ++iter)
	{
		cell_info& info = iter->second;

		for (objects_const_iterator_t oit = info.objects.begin(); oit != info.objects.end(); ++oit)
		{
			const object_info &i = *(oit);

			if (i.object_type == t.object_type &&
				i.object_id == t.object_id)
			{
				info.objects.erase(oit);
				return;
			}
		}
	}
}

point2d_t game_area::get_cell_point_by_position(const point2d_t &p)
{
	// ���������� ����� ������ �� ����� �� ���������
	cell_index idx = __get_cell_index_by_point(p);
	point2d_t ret;

	// ���������� ������� (22.f - ��� �����������, ������� � ���� ����������
	// �� �� ������ �������. ���� ��� ���������� ���������� ������
	ret = __top_point + *(__vector_left.direction.Normalize()) * 22.22f * (float)idx.x_num;
	ret += *(__vector_right.direction.Normalize()) * 22.23f * (float)idx.y_num;

	// ��������� �� ������ ������
	ret.x -= 20.0f;

	return ret;
}

point2d_t game_area::get_cell_point_by_index(const cell_index &idx)
{
	point2d_t ret;

	ret = __top_point + *(__vector_left.direction.Normalize()) * 22.22f * (float)idx.x_num;
	ret += *(__vector_right.direction.Normalize()) * 22.23f * (float)idx.y_num;

	ret.x -= 20.0f;

	return ret;
}

game_controller::game_controller()
	: __field(0),
	__area(0),
	__player(0),
	__soldier(0),
	__start_position(point2d_t(44.0f, 252.0f)),
	__soldier_start_position(point2d_t(480.0f, 33.0f)),
	__soldier_fin_position(point2d_t(520.0f, 490.0f)),
	__ball_start_position(point2d_t(379.0f, 82.0f))
{
	__hge = hgeCreate(HGE_VERSION);
}

game_controller::~game_controller()
{
	__hge->Release();
}

void game_controller::add_field(game_field *fld)
{
	__field = fld;
	
	// �������������� ���������� ��� ����
	const point2d_t &top = __field->get_top_point();
	const point2d_t &left = __field->get_left_point();
	const point2d_t &right = __field->get_right_point();

	math2d::line2d ac, ab;
	ac.origin = ab.origin = top;
	ab.direction = left - top;
	ac.direction = right - top;

	__area = new game_area(top, ab, ac, __field->get_cell_x_length(), __field->get_cell_y_length());

	// ������� ������ ������
	point2d_t pos = __area->get_cell_point_by_index(
		game_area::cell_index(0, __field->get_y_cell_count() - 1));

	int objid = __field->add_finish_cell(pos);

	__area->add_object(right, game_area::object_info(objid, game_area::FINISH_CELL));

	// ������� ������ ������
	pos = __area->get_cell_point_by_index(
		game_area::cell_index(__field->get_x_cell_count() - 1, 0));

	objid = __field->add_start_cell(pos);

	__area->add_object(pos, game_area::object_info(objid, game_area::START_CELL));
}

void game_controller::add_player(people *pl)
{
	__player = pl;

	__player->set_position(__start_position);
}

void game_controller::add_soldier(people *s)
{
	__soldier = s;

	__soldier->set_position(__soldier_start_position);

	__soldier->go(__soldier_fin_position - __soldier_start_position, __soldier_fin_position);
}

void game_controller::add_deathball(death_ball *ball)
{
	__ball = ball;

	__ball->set_position(__ball_start_position);

	point2d_t p(__ball_start_position.x, __hge->Random_Float(30.f, 500.f));

	__ball->go(p - __ball_start_position);
}

void game_controller::add_cannon(cannon *c)
{
	__cannons.push_back(c);
}

void game_controller::add_explosions(explosions *expl)
{
	__explosions = expl;
}

void game_controller::process()
{
	__process_user_action();

	__process_cannon_shots();

	__process_collisions();

	__process_player_movement();

	__process_soldier_movement();

	__process_deathball_movement();
}

void game_controller::__process_user_action()
{
	__process_mouse_rbtn_clicked();

	__process_mouse_lbtn_clicked();
}

void game_controller::__process_mouse_rbtn_clicked()
{
	// ���������� ������� ������ ������ �����
	if (__hge->Input_GetKeyState(HGEK_RBUTTON))
	{
		// �.�. ������ ����� ������ ��������� ������� (������� �������) -
		// ����� Input_GetKeyState ����� ���������� true ����� ���
		// (������� �� fps). � � ��� �� ���� ������� ������ ������ �����
		// ��� ��������: ������� ����������� � ���������� ���. ������� � �������
		// ������ ������� ����������� ����� ������� � ���������� ��������� ���.
		// ���� ����� �������� �������� ��������� ���, �� �� ��������� �������
		// ������� ���� ������� ������� (���� �� ������� ������������ ����� ��� �������!
		static SYSTEMTIME time;
		static DWORD last_time = 0;

		GetSystemTime(&time);

		if (abs((int)(time.wMilliseconds - last_time)) < 250) return;

		last_time = time.wMilliseconds;

		static point2d_t mouse_position;

		// ������� ���������� �������
		__hge->Input_GetMousePos(&mouse_position.x, &mouse_position.y);

		// �������� ������ �� �� � ����
		if (__field->check_field_border(mouse_position)
			&& !__area->check_object(mouse_position, game_area::PLAYER))
		{
			// �� ����������� ������� ��������� - ���� �� ��� �� ����
			// ����� �����������
			if (!__area->check_object(mouse_position, game_area::BLOCKED_CELL))
			{
				// ����� ��������� �����������
				point2d_t npos = __area->get_cell_point_by_position(mouse_position);
				int objid = __field->add_block_cell(npos);

				// �������, ��� � ���� ������ ����������� ��� ����
				__area->add_object(mouse_position, game_area::object_info(objid, game_area::BLOCKED_CELL));
			}
			else
			{
				// ���� ��� ���� - ������ ���
				int objid = __area->delete_object(mouse_position,
					game_area::object_info(0, game_area::BLOCKED_CELL));

				__field->remove_cell(objid);
			}
		}
	}
}

void game_controller::__process_mouse_lbtn_clicked()
{
	// ���������� ������� ����� ������ �����
	if (__hge->Input_GetKeyState(HGEK_LBUTTON))
	{
		static point2d_t mouse_position;

		// ������� ���������� �������
		__hge->Input_GetMousePos(&mouse_position.x, &mouse_position.y);

		// �������� ������ �� �� � ����
		if (__field->check_field_border(mouse_position))
		{
			__player->go(mouse_position - __player->get_position(), mouse_position);
		}
	}
}

void game_controller::__process_cannon_shots()
{
	// �������� �����
	static SYSTEMTIME time;
	static DWORD last_time = 0;

	GetSystemTime(&time);

	// ��������� �� ���� ������
	for (cannons_iterator_t iter = __cannons.begin(); iter != __cannons.end(); ++iter)
	{
		cannon* c = *(iter);

		cannon::bullets_t& bullets = c->get_bullets();

		// ������ �� ������ ������� ������� �����
		for (cannon::bullets_iterator_t bit = bullets.begin(); bit != bullets.end();)
		{
			bullet* b = *(bit);

			const point2d_t& p = b->get_position();

			// ���� ������ ����� �� ������� ���� - ������ ���
			if (!__field->check_field_border(p))
			{
				// ������� ������ �� �������
				__area->delete_object_by_info(game_area::object_info(b->get_object_id(), game_area::BULLET));

				delete b;

				bit = bullets.erase(bit);
			}
			else
			{
				// ���� ������� ��� � ����
				// ���������� �� ����� �������
				__area->delete_object_by_info(game_area::object_info(b->get_object_id(), game_area::BULLET));

				__area->add_object(p, game_area::object_info(b->get_object_id(), game_area::BULLET));

				++bit;
			}
		}

		if (last_time == time.wSecond)
		{
			continue;
		}

		// �������, �� ������� �� ����� ����� �������
		if ((time.wSecond % (60 / c->get_shot_speed() + 1)) == 0)
		{
			last_time = time.wSecond;
			int objid = c->make_shot();

			// ��������� ���������� � ������� � ������
			__area->add_object(c->get_position(), game_area::object_info(objid, game_area::BULLET));
		}
	}
}

void game_controller::__process_collisions()
{
	for (cannons_iterator_t iter = __cannons.begin(); iter != __cannons.end(); ++iter)
	{
		cannon *c = *iter;

		cannon::bullets_t &bullets = c->get_bullets();

		for (cannon::bullets_iterator_t bit = bullets.begin(); bit != bullets.end();)
		{
			bullet *b = *bit;
			const point2d_t &bullet_position = b->get_position();

			// ���� ������ ����� � ������
			if (__area->check_object(bullet_position, game_area::PLAYER))
			{
				// ������� ������
				__area->delete_object(bullet_position, game_area::object_info(0, game_area::BULLET));

				// ������� ����� �� ��� �����
				__explosions->make_explosion(bullet_position);

				delete b;

				bit = bullets.erase(bit);

				// ����� �����, ���������� ��� �� �������� �������
				__player->set_position(__start_position);

				// � ������������� ��� ��������
				__player->stop();

				continue;
			}

			// ���� ������ ����� � �����������
			if (__area->check_object(bullet_position, game_area::BLOCKED_CELL))
			{
				// ������ ������
				__area->delete_object(bullet_position, game_area::object_info(0, game_area::BULLET));

				// ������ �����
				int objid = __area->delete_object(bullet_position, 
					game_area::object_info(0, game_area::BLOCKED_CELL));

				__field->remove_cell(objid);

				// �������� ������
				__explosions->make_explosion(bullet_position);
				
				delete b;

				bit = bullets.erase(bit);

				continue;
			}

			++bit;
		}
	}
}

void game_controller::__process_player_movement()
{
	// ������� ������, �� ������� �� ������������ � ��������� ����� �����������
	const point2d_t& future_player_position = __player->get_future_position();

	// �������� - �� ������� �� �� �� ������ � ������������
	if (__area->check_object(future_player_position, game_area::BLOCKED_CELL))
	{
		// ���� ������� - ������, ���� ��� ���� ������
		// ��������� ������
		__player->stop();
	}

	const point2d_t &player_position = __player->get_position();

	// ���� ������� ������ - �����, ������ �� ��������.
	if (__area->check_object(player_position, game_area::FINISH_CELL | game_area::SOLDIER))
	{
		// ������ ������ �� �������� �������
		__player->set_position(__start_position);
		__player->stop();
	}
		
	game_area::object_info player_info(__player->get_object_id(), game_area::PLAYER);

	// ���������� �� ����� �������
	__area->delete_object_by_info(player_info);

	__area->add_object(player_position, player_info);
}

void game_controller::__process_soldier_movement()
{
	enum
	{
		DIRECTION_ONE,
		DIRECTION_TWO
	};

	const point2d_t &soldier_position = __soldier->get_position();

	// ��������� �����������
	if (__area->check_object(soldier_position, game_area::BLOCKED_CELL))
	{
		int objid = __area->delete_object(soldier_position, 
			game_area::object_info(0, game_area::BLOCKED_CELL));

		__field->remove_cell(objid);
	}

	game_area::object_info soldier_info(__soldier->get_object_id(), game_area::SOLDIER);

	// ���������� �� ����� �������
	__area->delete_object_by_info(soldier_info);

	__area->add_object(soldier_position, soldier_info);

	static int current_direction = DIRECTION_ONE;

	if (__soldier->is_stopped())
	{
		switch (current_direction)
		{
		case DIRECTION_ONE:
			current_direction = DIRECTION_TWO;
			__soldier->go(__soldier_start_position - __soldier_fin_position, __soldier_start_position);
			break;

		case DIRECTION_TWO:
			current_direction = DIRECTION_ONE;
			__soldier->go(__soldier_fin_position - __soldier_start_position, __soldier_fin_position);
			break;

		default:
			break;
		}
	}
}

void game_controller::__process_deathball_movement()
{
	const point2d_t &ball_position = __ball->get_position();

	game_area::object_info ball_info(__ball->get_object_id(), game_area::DEATHBALL);

	// ���������� �� ����� �������
	__area->delete_object_by_info(ball_info);

	__area->add_object(ball_position, ball_info);

	// ����� �������
	if (__area->check_object(ball_position, game_area::PLAYER))
	{
		// ������ ������ �� �������� �������
		__player->set_position(__start_position);
		__player->stop();

		// ������� ����� �� ��� �����
		__explosions->make_explosion(ball_position);

		game_area::object_info player_info(__player->get_object_id(), game_area::PLAYER);

		// ���������� �� ����� �������
		__area->delete_object_by_info(player_info);

		__area->add_object(__player->get_position(), player_info);
	}

	// ��������� � ����������� ��� � ����� ������� - ������ �����������
	if (!__field->check_field_border(ball_position)
		|| __area->check_object(ball_position, game_area::BLOCKED_CELL))
	{
		point2d_t np(__hge->Random_Float(40.f, 600.f), __hge->Random_Float(30.f, 300.f));
		
		__ball->go(np - ball_position);
	}
}