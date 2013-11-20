#include "objects.h"

HGE* scene_object::__hge = 0;

scene_object::scene_object(const float x, const float y)
	: __texture(0),
	__position(x, y)
{
	// счетчик дл€ объектов
	static int object_id_counter = 1;

	__object_id = object_id_counter++;

	__hge = hgeCreate(HGE_VERSION);
}

scene_object::~scene_object()
{
	__hge->Release();
}

int scene_object::get_object_id() const
{
	return __object_id;
}

void scene_object::set_position(const point2d_t &p)
{
	__position = p;
}

const point2d_t& scene_object::get_position() const
{
	return __position;
}

animation_object::animation_object(const char *file_name, const int frames,
	const int fps, const float xpos, const float ypos, const float w, const float h)
	: __fname(file_name),
	__frame_count(frames),
	__fps(fps),
	__xpos(xpos),
	__ypos(ypos),
	__width(w),
	__height(h)
{
	__texture	= __hge->Texture_Load(file_name);
	__animation = new hgeAnimation(__texture, frames, fps, xpos, ypos, w, h);
}

animation_object::~animation_object()
{
	__hge->Texture_Free(__texture);

	delete __animation;
}

image_object::image_object(const char *fname, const float x, const float y,
	const float w, const float h)
	: __fname(fname),
	__xpos(x),
	__ypos(y),
	__width(w),
	__height(h)
{
	__texture	= __hge->Texture_Load(fname);
	__sprite = new hgeSprite(__texture, x, y, w, h);
}

image_object::~image_object()
{
	__hge->Texture_Free(__texture);

	delete __sprite;
}

people::people(const char *file_name, const int frames, const int fps, const float xpos, const float ypos,
	const float w, const float h, const int stop_frame)
	: animation_object(file_name, frames,fps, xpos, ypos, w, h),
	__movement_speed(1.7f),
	__is_moving(false),
	__stop_frame(stop_frame)
{
	__animation->SetHotSpot(0, ypos * 0.85f);
}

people::~people()
{
}

void people::render()
{
	__animation->Render(__position.x - 20.0f, __position.y);
}

void people::update(const float d)
{
	// если персонаж движетс€
	if (__animation->IsPlaying())
	{
		static const float eps = 1.0f;

		// проверка остановки
		if (math2d::point_distance(__destination, __position) < eps)
		{
			// пришли куда надо - останавливаемс€
			stop();
		}
		else
		{
			// обновим текущую позицию
			__position = __future_position;
		}

		// позици€ следущего цикла (необходима чтобы не врезатьс€ в преп€тствие)
		// а в будущем - чтобы обходить снар€ды
		__future_position += __get_direction();
	}
	
	__animation->Update(d);
}

const point2d_t& people::get_future_position() const
{
	return __future_position;
}

vector2d_t people::__get_direction()
{
	return *(__movement_direction.Normalize()) * __movement_speed;
}

void people::go(const vector2d_t &direction, const point2d_t &dest)
{
	// указываем точку, в которую надо прийти
	__destination = dest;

	// вектор движени€
	__movement_direction = direction;

	__future_position = __position + __get_direction();

	if (__position.x <= dest.x)
	{
		__animation->SetFlip(false, false);
	}
	else
	{
		__animation->SetFlip(true, false);
	}

	// запускаем анимацию
	__animation->Play();

	__is_moving = true;
}

void people::stop()
{
	__destination = __position;

	__movement_direction = vector2d_t(0.0f, 0.0f);


	__animation->SetFrame(__stop_frame);

	// останавливаем анимацию
	__animation->Stop();

	

	__is_moving = false;
}

bool people::is_stopped() const
{
	return !__is_moving;
}

bullet::bullet(const char *file_name, const int frames, const int fps,
	const float xpos, const float ypos, const float w, const float h, 
	const vector2d_t &dir, const float angle)
	: animation_object(file_name, frames, fps, xpos, ypos, w, h),
	__shot_direction(dir),
	__angle(angle),
	__speed(5.5f)
{
	__animation->SetHotSpot(25, 25);

	__animation->Play();
}

void bullet::render()
{
	__animation->RenderEx(__position.x, __position.y, __angle);
}

void bullet::update(const float d)
{
	__position += *(__shot_direction.Normalize()) * __speed;
	__animation->Update(d);
}

cannon::cannon(const char *fname_bullet, const int frames, const int fps,
	const float xpos, const float ypos, const float w, const float h, 
	const vector2d_t &direction, const int spm, const float angle)
	: __fname_bullet(fname_bullet),
	__frame_count(frames),
	__fps(fps),
	__xpos(xpos),
	__ypos(ypos),
	__width(w),
	__height(h),
	__shot_speed(spm),
	__angle(angle),
	__shot_direction(direction)
{
}

cannon::~cannon()
{
	for (bullets_iterator_t iter = __bullets.begin(); iter != __bullets.end(); ++iter)
	{
		delete (*iter);
	}
}

void cannon::render()
{
	for (bullets_iterator_t iter = __bullets.begin(); iter != __bullets.end(); ++iter)
	{
		(*iter)->render();
	}
}

void cannon::update(const float d)
{
	for (bullets_iterator_t iter = __bullets.begin(); iter != __bullets.end(); ++iter)
	{
		const bullet* b = *(iter);
		(*iter)->update(d);
	}
}

int cannon::make_shot()
{
	// создаем снар€д
	bullet* b = new bullet(__fname_bullet, __frame_count, __fps, __xpos, __ypos,
		__width, __height, __shot_direction, __angle);
	
	// указываем позицию старта
	b->set_position(__position);

	// добавл€ем в контейнер
	__bullets.push_back(b);

	return b->get_object_id();
}

int cannon::get_shot_speed() const
{
	return __shot_speed;
}

cannon::bullets_t& cannon::get_bullets()
{
	return __bullets;
}

explosion::explosion(const char *file_name, const int frames, 
	const int fps, const float xpos, const float ypos, const float w, const float h)
	: animation_object(file_name, frames, fps, xpos, ypos, w, h)
{
	__animation->Play();
}

void explosion::render()
{
	__animation->Render(__position.x - 60, __position.y - 60);
}

void explosion::update(const float d)
{
	__animation->Update(d);

	// нам надо только один раз показать цикл картинок
	if (__animation->GetFrame() == __animation->GetFrames() - 1)
	{
		__animation->Stop();
	}
}

bool explosion::is_active() const
{
	return __animation->IsPlaying();
}

explosions::explosions(const char *file_name, const int frames,
	const int fps, const float xpos, const float ypos, const float w, const float h)
	: __fname_expl(file_name),
	__frame_count(frames),
	__fps(fps),
	__xpos(xpos),
	__ypos(ypos),
	__width(w),
	__height(h)
{
}

void explosions::render()
{
	for (explosions_iterator_t iter = __explosions.begin(); iter != __explosions.end();
		++iter)
	{
		(*iter)->render();
	}
}

void explosions::update(const float d)
{
	// обновим каждый объект взырва
	for (explosions_iterator_t iter = __explosions.begin(); iter != __explosions.end();)
	{
		explosion *expl = (*iter);
	
		// если взрыв уже произошел (все фреймы были показаны) - удалим объект
		if (!expl->is_active())
		{
			delete expl;

			iter = __explosions.erase(iter);
		}
		else
		{
			// иначе - обновим
			expl->update(d);

			++iter;
		}
	}
}

void explosions::make_explosion(const point2d_t &p)
{
	// создаем объект взрыва
	explosion* expl = new explosion(__fname_expl, __frame_count, __fps, __xpos,
		__ypos, __width, __height);

	// указываем позицию дл€ анимации
	expl->set_position(p);

	// добавл€ем в контейнер
	__explosions.push_back(expl);
}

death_ball::death_ball(const char *fname,const float xpos, const float ypos,
	const float w, const float h)
	: image_object(fname, xpos, ypos, w, h)
{
	__sprite->SetHotSpot(w / 2, h / 2);
}

void death_ball::update(const float d)
{
	__position += *(__direction.Normalize()) * __speed;
}

void death_ball::render()
{
	__sprite->Render(__position.x, __position.y);
}

void death_ball::go(const vector2d_t &direction)
{
	// вектор движени€
	__direction = direction;

	__speed = __hge->Random_Float(1.0f, 10.0f);
}

void death_ball::stop()
{
	__direction = vector2d_t(0.0f, 0.0f);
}

const vector2d_t& death_ball::get_direction() const
{
	return __direction;
}