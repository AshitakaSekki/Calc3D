#include <iostream>
#include <cmath>

using namespace std;

class Vector3;

float dot(Vector3 v1, Vector3 v2);
float cosAngle(Vector3 v1, Vector3 v2);
float * sort(float list[4], int size);
void vectorTest();
void sortTest();

//��ά����
class Vector3
{
public:
	Vector3(float x, float y, float z);
	Vector3(float beginPoint[3], float endPoint[3]);
	float X, Y, Z, Length; //���꼰ģ��
};

//�����깹������
Vector3::Vector3(float x, float y, float z) :X(x), Y(y), Z(z)
{
	Length = sqrt(X * X + Y * Y + Z * Z);
}
//����ʼ�㹹������
Vector3::Vector3(float beginPoint[3], float endPoint[3])
{
	X = endPoint[0] - beginPoint[0];
	Y = endPoint[1] - beginPoint[1];
	Z = endPoint[2] - beginPoint[2];
	Length = sqrt(X * X + Y * Y + Z * Z);
}

int main()
{
	//vectorTest();
	sortTest();

	system("pause");

	return 0;
}

//�����������
float dot(Vector3 v1, Vector3 v2)
{
	float dotProduct;
	dotProduct = v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z;
	return dotProduct;
}

//�����н����Һ���
float cosAngle(Vector3 v1, Vector3 v2)
{
	float cosine;
	cosine = dot(v1, v2) / (v1.Length*v2.Length);
	return cosine;
}

//������
float * sort(float list[4], int size)
{
	bool isInorder = false;
	int i = 0;
	float temp;
	while (!isInorder)
	{
		if (list[i] < list[i + 1])
		{
			temp = list[i];
			list[i] = list[i + 1];
			list[i + 1] = temp;
			i = 0;
		}
		else if (++i == size - 1) isInorder = true;

	}

	return list;
}

void vectorTest()
{
	float x, y, z;
	cout << "��һ�������� ";
	cin >> x >> y >> z;
	cout << endl;
	Vector3 v1(x, y, z);
	cout << "�ڶ��������� ";
	cin >> x >> y >> z;
	cout << endl;
	Vector3 v2(x, y, z);

	cout << dot(v1, v2) << endl;
	cout << cosAngle(v1, v2) << endl;
}

void sortTest()
{
	float list[4] = { 5,10,3,11 };
	float * l;
	l = sort(list, 4);
	for (int i = 0; i < 4; i++)
	{
		cout << *(l+i) << ' ';
	}
}