#pragma once

//三维向量
class Vector3
{
public:
	float x, y, z; //xyz坐标
				   //默认构造函数
	Vector3() {}
	//拷贝构造函数
	Vector3(const Vector3 &a) :x(a.x), y(a.y), z(a.z) {}
	//用坐标构造向量
	Vector3(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {}
	//重载运算符，实现左值
	Vector3 &operator=(const Vector3 &a)
	{
		x = a.x; y = a.y; z = a.z;
		return *this;
	}
	//重载==
	bool operator==(const Vector3 &a) const
	{
		return x == a.x&&y == a.y&&z == a.z;
	}
	//重载!=
	bool operator!=(const Vector3 &a) const
	{
		return x != a.x || y != a.y || z != a.z;
	}
	//置为零向量
	void zero()
	{
		x = y = z = 0.0f;
	}
	//重载一元-
	Vector3 operator-()const
	{
		return Vector3(-x, -y, -z);
	}
	//重载二元+-
	Vector3 operator+(const Vector3 &a) const
	{
		return Vector3(x + a.x, y + a.y, z + a.z);
	}
	Vector3 operator-(const Vector3 &a) const
	{
		return Vector3(x - a.x, y - a.y, z - a.z);
	}
	//与标量的乘除法
	Vector3 operator*(float a) const
	{
		return Vector3(x*a, y*a, z*a);
	}
	Vector3 operator/(float a) const
	{
		float oneOverA = 1.0f / a;//不对除零进行检查
		return Vector3(x*oneOverA, y*oneOverA, z*oneOverA);
	}
	//重载自反运算符
	Vector3 &operator+=(const Vector3 &a)
	{
		x += a.x; y += a.y; z += a.z;
		return *this;
	}
	Vector3 &operator-=(const Vector3 &a)
	{
		x -= a.x; y -= a.y; z -= a.z;
		return *this;
	}
	Vector3 &operator*=(float a)
	{
		x *= a; y *= a; z *= a;
		return *this;
	}
	Vector3 &operator/=(float a)
	{
		float oneOverA = 1.0f / a;
		x *= oneOverA; y *= oneOverA; z *= oneOverA;
		return *this;
	}
	//向量标准化
	void normalize()
	{
		float magSq = x * x + y * y + z * z;
		if (magSq > 0.0f)
		{
			float oneOverMag = 0.0f / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}
	//向量点乘，重载*
	float operator*(const Vector3 &a)const
	{
		return x * a.x + y * a.y + z * a.z;
	}
};

//非成员函数
//向量求模
inline float vectorMag(const Vector3 &a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}
//向量叉乘
inline Vector3 crossProduct(const Vector3 &a, const Vector3 &b)
{
	return Vector3
	(
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
	);
}
//实现标量左乘
inline Vector3 operator*(float k, const Vector3 &v)
{
	return Vector3(k*v.x, k*v.y, k*v.z);
}
//计算两点间距离
inline float distance(const Vector3 &a, const Vector3 &b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrt(dx*dx + dy * dy + dz * dz);
}

//全局零向量
extern const Vector3 kZeroVector;