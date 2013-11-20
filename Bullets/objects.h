#ifndef OBJECTS_H_
#define OBJECTS_H_
#include "hge.h"
#include "hgevector.h"
#include "hgeanim.h"
#include <string>
#include <vector>
#include "func.h"

// ����������� ������� ����� ��� ���� �������� �����
class scene_object
{
public:
	scene_object(const float x = 0, const float y = 0);
	virtual ~scene_object();

	// ������������ ��� ���������� ������
	virtual void render() = 0;
	virtual void update(const float) = 0;

	// ����� ������� �������
	void set_position(const point2d_t &p);

	const point2d_t& get_position() const;
	int get_object_id() const;

protected:
	static HGE *__hge;
	
	// ����� ��� ��������
	HTEXTURE	__texture;
	
	// ������� ������� �� �����
	hgeVector	__position;
	
	// ���������� ������������� ������� �����
	int			__object_id;
};

// ����������� ������� ����� ��� ������������� �������� �����
class animation_object : public scene_object
{
public:
    animation_object(const char *file_name, const int frames, const int fps,
                     const float xpos, const float ypos, const float w, const float h);

    ~animation_object();

protected:
	// ���� ��������
	hgeAnimation	*__animation;
	
	// ��� ����� �� ��������
	const char		*__fname;
	
	// ���������� ������
	const int		__frame_count;
	
	// ���������� ������ � �������
	const int		__fps;
	
	// ��������� ������� �� X � Y
	const float		__xpos;
	const float		__ypos;
	
	// ������ � ������
	const float		__width;
	const float		__height;
};

// ����������� ������� ����� ��� ��������-�������� �����
class image_object : public scene_object
{
public:
    image_object(const char *fname, const float x, const float y, const float w, const float h);
    ~image_object();

protected:
	const char	*__fname;

	// �����������
    hgeSprite	*__sprite;

	// ��������� ������� �� X � Y
	const float	__xpos;
	const float	__ypos;

	// ������ � ������ �����������
	const float	__width;
	const float __height;
};

class people : public animation_object
{
public:
	people(const char *file_name, const int frames, const int fps,
		const float xpos, const float ypos, const float w, const float h,
		const int stop_frame);

	~people();

	virtual void render();
	virtual void update(const float);

	void go(const vector2d_t &direction, const point2d_t &p);
	void stop();

	bool is_stopped() const;

	const point2d_t& get_future_position() const;

private:
	vector2d_t __get_direction();

	// ����������� �������� ���������
	vector2d_t __movement_direction;

	// �������� �������� ���������
	float __movement_speed;

	// ��������� �����, ���� ������������ ��������
	point2d_t __future_position;

	// ���� ���������� ��������
	point2d_t __destination;

	// ����� � ������ ���������
	int		__stop_frame;

	bool		__is_moving;
};

class explosion : public animation_object
{
public:
	explosion(const char *file_name, const int frames, const int fps,
		const float xpos, const float ypos, const float w, const float h);

	virtual void render();
	virtual void update(const float);

	bool is_active() const;

private:

};

// ����� - ���������� ��� �������
// ��������� ��� �� scene_object, ����� �������������� ������ update � render
class explosions : public scene_object
{
public:
	explosions(const char *file_name, const int frames, const int fps,
		const float xpos, const float ypos, const float w, const float h);

	virtual void render();
	virtual void update(const float);

	void make_explosion(const point2d_t &p);

private:
	typedef std::vector <explosion*> explosions_t;
	typedef explosions_t::iterator explosions_iterator_t;

	const char		*__fname_expl;
	int				__frame_count;
	int				__fps;
	float			__xpos;
	float			__ypos;
	float			__width;
	float			__height;

	// ��������� ��� �������
	explosions_t	__explosions;
};

class bullet : public animation_object
{
public:
	bullet(const char *file_name, const int frames, const int fps,
		const float xpos, const float ypos, const float w, const float h,
		const vector2d_t &dir, const float angle);

	virtual void render();
	virtual void update(const float);

private:
	// �������� �������
	const float			__speed;

	// ����������� �������
	vector2d_t			__shot_direction;

	// ����, ��� ������� ���� ��������� ��������
	const float			__angle;
};


class cannon : public scene_object
{
public:
	typedef std::vector <bullet*> bullets_t;
	typedef bullets_t::iterator bullets_iterator_t;

	cannon(const char *fname_bullet, const int frames, const int fps,
		const float xpos, const float ypos, const float w, const float h,
		const vector2d_t &direction, const int spm, const float angle);

	~cannon();

	int make_shot();
	bullets_t& get_bullets();

	virtual void render();
	virtual void update(const float d);
	
	int get_shot_speed() const;

private:
	const char	*__fname_bullet;
	int			__frame_count;
	int			__fps;
	float		__xpos;
	float		__ypos;
	float		__width;
	float		__height;
	int			__shot_speed;
	float		__angle;
	vector2d_t	__shot_direction;
	bullets_t	__bullets;
};

class death_ball : public image_object
{
public:
	death_ball(const char *fname, const float xpos, const float ypos,
		const float w, const float h);

	void update(const float);
	void render();

	void go(const vector2d_t &direction);
	void stop();

	const vector2d_t& get_direction() const;

private:
	vector2d_t	__direction;
	float		__speed;
};

#endif
