#ifndef __FUNCS_H__
#define __FUNCS_H__
#include "hgevector.h"
#include <stdint.h>
#include <math.h>

typedef hgeVector point2d_t;
typedef hgeVector vector2d_t;

// работа на плоскости
namespace math2d
{
	// расстояние между двумя точками
	float point_distance(const point2d_t &p1, const point2d_t &p2);

	// уравнение прямой в параметрическом виде
	class line2d
	{
	public:
		// начальная точка
		point2d_t origin;

		// вектор направления прямой
		vector2d_t direction;

		// получаем вектор нормали
		vector2d_t get_normal_vector() const
		{
			vector2d_t ret;
			ret.x = -direction.y;
			ret.y = direction.x;

			return ret;
		}

		// определение точки пересечения 2х прямых на плоскости
		// TODO: сделать нормальным способом
		bool line_intersect(const line2d &l1, point2d_t &res) const
		{
			// проверим коллинеарность векторов

			float x1 = origin.x, x2 = x1 + direction.x, x3 = l1.origin.x, x4 = x3 + l1.direction.x;
			float y1 = origin.y, y2 = y1 + direction.y, y3 = l1.origin.y, y4 = y3 + l1.direction.y;

			float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
			
			if (d == 0) 
			{
				return false;
			}

			float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
			float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
			float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

			res.x = x;
			res.y = y;

			return true;
		}
	};

	class triangle
	{
	public:
		triangle(const point2d_t t1, const point2d_t t2, const point2d_t t3)
			: a(t1),
			b(t2),
			c(t3)
		{
		}

		// площадь треугольника 
		float square() const 
		{
			return abs(b.x * c.y - c.x * b.y - a.x * c.y 
				+ c.x * a.y + a.x * b.y - b.x * a.y) / 2;
		}

		// проверка принадлежности точки треугольнику
		// метод площадей (есть другие более быстрые по производительности
		// решения, но этот метод реализуется быстрее всех)
		bool test_point(const point2d_t p)
		{
			// имеем треугольник и точку на плоскости
			// проводим триангуляцию на 3 треугольника
			
			triangle abp(a, b, p);
			triangle bcp(b, c, p);
			triangle acp(a, c, p);

			// суммируем площади получившихся треугольников
			float sum = abp.square() + bcp.square() + acp.square();
			
			// сравниваем получившуюся сумму площадей с площадью оригинального
			// треугольника, если они равны - наша точка в треугольнике.
			// необходимо использовать константу eps - т.к. имеем машинную погрешность
			static const float eps = 0.1f;
			if (abs(square() - sum) > eps )
			{
				return false;
			}

			return true;
		}

		/*
		// проверка принадлежности точки p треугольнику
		// метод трассировки
		bool test_point(const pointxd_t p)
		{
			// пустим луч из точки p в (0,..,0)
			line <pointxd_t> l;
			l.direction = p - pointxd_t();
			l.origin = p;
			
			// теперь найдем точку пересечения луча с каждой из сторон
			// если число таких точек - четно, значит мы вне треугольника
			// нечетно - в нем
			// их нет - векторы луча и стороны треугольника коллинеарны
			
			point2d_t res;
			int intersect_count = 0;

			// сторона AB
			line <pointxd_t> ab;
			ab.origin = __a;
			ab.direction = __b - __a;

			if (line_intersect(l, ab, res))
			{
				if( res.x >= max(min(__a.x, __b.x), min(point2d_t().x, l.origin.x)) &&
					res.x <= min(max(__a.x, __b.x), max(point2d_t().x, l.origin.x)))
					++intersect_count;
			}

			// сторона BC
			line <pointxd_t> bc;
			bc.origin = __b;
			bc.direction = __c - __b;

			if (line_intersect(l, bc, res))
			{
				if( res.x >= max(min(__c.x, __b.x), min(point2d_t().x, l.origin.x)) &&
					res.x <= min(max(__c.x, __b.x), max(point2d_t().x, l.origin.x)))
					++intersect_count;
			}

			// линия не может одновременно пересечь все стороны треугольника
			// поэтому проверяем AC, только если это необходимо
			if (intersect_count < 2)
			{
				// сторона AC
				line <pointxd_t> ac;
				ac.origin = __a;
				ac.direction = __c - __a;

				if (line_intersect(l, ac, res))
				{
					if( res.x >= max(min(__a.x, __c.x), min(point2d_t().x, l.origin.x)) &&
						res.x <= min(max(__a.x, __c.x), max(point2d_t().x, l.origin.x)))
						++intersect_count;
				}
			}

			if ((intersect_count % 2) == 1)
			{
				return true;
			}
			
			return false;
		}
		}
		
		*/

		point2d_t a;
		point2d_t b;
		point2d_t c;
	};
}

#endif