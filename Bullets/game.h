#ifndef _GAME_H_
#define _GAME_H_
#include "objects.h"
#include "scene.h"
#include "field.h"
#include "game_controller.h"

class game
{
public:
	game()
	{
		__scene	= new scene;
	}

	~game()
	{
		delete __scene;
	}


	// инициализаци€ объектов сцены
	bool init()
	{
		if (__scene)
		{
			bool ret = __scene->init();

			game_controller *ctrl = new game_controller;

			//=============================================================
			// игровое поле
			// жестко задаем границы пол€ и параметры €чейки
			static const point2d_t top_point(480.0f, 22.0f);
			static const point2d_t left_point(21.0f, 252.0f);
			static const point2d_t right_point(980.0f, 272.0f);
			static const point2d_t bottom_point(520.0f, 500.0f);
			static const char *FIELD_FNAME	= "images/net3.png";
			static const int CELL_X_COUNT	= 23;
			static const int CELL_Y_COUNT	= 25;
			static const int TEX_WIDTH		= 1000;
			static const int TEX_HEIGHT		= 600;

			game_field *field = new game_field(FIELD_FNAME, TEX_WIDTH, TEX_HEIGHT, top_point,
				left_point, right_point, bottom_point, CELL_X_COUNT, CELL_Y_COUNT);

			__scene->insert_object(field);
			ctrl->add_field(field);

			//=============================================================
			// игрок
			static const char *PLAYER_NAME = "images/pl2.png";
			static const int   PLAYER_FRAME_COUNT	= 14;
			static const int   PLAYER_FPS			= 14;
			static const float PLAYER_XPOS			= 0.0f;
			static const float PLAYER_YPOS			= 40.5f;
			static const float PLAYER_WIDTH			= 30.0f;
			static const float PLAYER_HEIGHT		= 40.5f;
			static const int   PLAYER_STOP_FRAME	= 6;

			people *pl = new people(PLAYER_NAME, PLAYER_FRAME_COUNT, PLAYER_FPS,
				PLAYER_XPOS, PLAYER_YPOS, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_STOP_FRAME);
			__scene->insert_object(pl);
			ctrl->add_player(pl);

			//=============================================================
			// патруль
			static const char *SOLDIER_NAME = "images/pl3.png";

			people *pl2 = new people(SOLDIER_NAME, PLAYER_FRAME_COUNT, PLAYER_FPS,
				PLAYER_XPOS, PLAYER_YPOS, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_STOP_FRAME);
			__scene->insert_object(pl2);
			ctrl->add_soldier(pl2);

			
			//=============================================================
			// пушка на левой стене
			static const char *BULLET_NAME = "images/fireball1.png";
			static const int	SHOT_SPEED			= 60;
			static const int	CANNON_FRAME_COUNT	= 6;
			static const int	CANNON_FPS			= 6;
			static const float	CANNON_XPOS			= 0.0f;
			static const float	CANNON_YPOS			= 0.0f;
			static const float	CANNON_WIDTH		= 60.0f;
			static const float	CANNON_HEIGHT		= 60.0f;
			static const float	CANNON_LEFT_ANGLE	= 0.43f;
			static const float	CANNON_RIGHT_ANGLE	= 2.5f;
			static const point2d_t CANNON_LEFT_POSITION(279.0f, 130.f);
			static const point2d_t CANNON_RIGHT_POSITION(760.0f, 171.f);

			cannon *left_cannon = new cannon(BULLET_NAME,  CANNON_FRAME_COUNT, CANNON_FPS,
				CANNON_XPOS, CANNON_YPOS, CANNON_WIDTH, CANNON_HEIGHT,
				right_point - top_point, SHOT_SPEED, CANNON_LEFT_ANGLE);

			left_cannon->set_position(CANNON_LEFT_POSITION);

			__scene->insert_object(left_cannon);

			ctrl->add_cannon(left_cannon);

			//=============================================================
			// пушка на правой стене
			cannon *right_cannon = new cannon(BULLET_NAME,  CANNON_FRAME_COUNT, CANNON_FPS,
				CANNON_XPOS, CANNON_YPOS, CANNON_WIDTH, CANNON_HEIGHT,
				left_point - top_point, SHOT_SPEED + 50, CANNON_RIGHT_ANGLE);

			right_cannon->set_position(CANNON_RIGHT_POSITION);

			__scene->insert_object(right_cannon);

			ctrl->add_cannon(right_cannon);

			//=============================================================
			// взрывы
			static const char	*EXPL_NAME				= "images/expl.png";
			static const int	EXPLOSIONS_FRAME_COUNT	= 5;
			static const int	EXPLOSIONS_FPS			= 5;
			static const float	EXPLOSIONS_XPOS			= 0.0f;
			static const float	EXPLOSIONS_YPOS			= 0.0f;
			static const float	EXPLOSIONS_WIDTH		= 118.0f;
			static const float	EXPLOSIONS_HEIGHT		= 118.0f;

			explosions *expl = new explosions(EXPL_NAME, EXPLOSIONS_FRAME_COUNT, EXPLOSIONS_FPS,
				EXPLOSIONS_XPOS, EXPLOSIONS_YPOS, EXPLOSIONS_WIDTH, EXPLOSIONS_HEIGHT);

			__scene->insert_object(expl);

			ctrl->add_explosions(expl);

			//=============================================================
			// шар убица
			static const char	*BALL_NAME			= "images/ball.png";
			static const float	BALL_XPOS			= 0.0f;
			static const float	BALL_YPOS			= 0.0f;
			static const float	BALL_WIDTH			= 16.0f;
			static const float	BALL_HEIGHT			= 16.0f;

			death_ball *ball = new death_ball(BALL_NAME, BALL_XPOS, BALL_YPOS, BALL_WIDTH, BALL_HEIGHT);

			__scene->insert_object(ball);

			ctrl->add_deathball(ball);

			// устанавливаем обработчик дл€ сцены
			__scene->set_game_controller(ctrl);
		}

		return false;
	}

	bool start()
	{
		if (__scene)
		{
			return __scene->start();
		}

		return false;
	}

private:
	scene			*__scene;
};

#endif