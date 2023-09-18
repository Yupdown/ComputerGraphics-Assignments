#include <iostream>

using namespace std;

struct matrix3x3
{
	int m00, m01, m02;
	int m10, m11, m12;
	int m20, m21, m22;

	matrix3x3() : matrix3x3(0, 0, 0, 0, 0, 0, 0, 0, 0)
	{

	}

	matrix3x3(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8)
	{
		m00 = arg0;
		m01 = arg1;
		m02 = arg2;
		m10 = arg3;
		m11 = arg4;
		m12 = arg5;
		m20 = arg6;
		m21 = arg7;
		m22 = arg8;
	}

	matrix3x3 operator+() const
	{
		return *this;
	}

	matrix3x3 operator-() const
	{
		return matrix3x3
		(
			-m00, -m01, -m02,
			-m10, -m11, -m12,
			-m20, -m21, -m22
		);
	}

	matrix3x3 operator+(const matrix3x3& m) const
	{
		return matrix3x3
		(
			m00 + m.m00, m01 + m.m01, m02 + m.m02,
			m10 + m.m10, m11 + m.m11, m12 + m.m12,
			m20 + m.m20, m21 + m.m21, m22 + m.m22
		);
	}

	matrix3x3 operator-(const matrix3x3& m) const
	{
		return *this + (-m);
	}

	matrix3x3 operator*(const matrix3x3& m) const
	{
		return matrix3x3
		(
			m00 * m.m00 + m01 * m.m10 + m02 * m.m20,
			m00 * m.m01 + m01 * m.m11 + m02 * m.m21,
			m00 * m.m02 + m01 * m.m12 + m02 * m.m22,
			m10 * m.m00 + m11 * m.m10 + m12 * m.m20,
			m10 * m.m01 + m11 * m.m11 + m12 * m.m21,
			m10 * m.m02 + m11 * m.m12 + m12 * m.m22,
			m20 * m.m00 + m21 * m.m10 + m22 * m.m20,
			m20 * m.m01 + m21 * m.m11 + m22 * m.m21,
			m20 * m.m02 + m21 * m.m12 + m22 * m.m22
		);
	}

	matrix3x3 operator*(const int s) const
	{
		return matrix3x3
		(
			m00 * s, m01 * s, m02 * s,
			m10 * s, m11 * s, m12 * s,
			m20 * s, m21 * s, m22 * s
		);
	}

	int determinant() const
	{
		return m00 * m11 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20 - m00 * m12 * m21 - m01 * m10 * m22;
	}

	matrix3x3 transposed() const
	{
		return matrix3x3
		(
			m00, m10, m20,
			m01, m11, m21,
			m02, m12, m22
		);
	}

	void print() const
	{
		printf("| %2d %2d %2d |\n", m00, m01, m02);
		printf("| %2d %2d %2d |\n", m10, m11, m12);
		printf("| %2d %2d %2d |\n", m20, m21, m22);
	}

	void print_as_4x4() const
	{
		printf("| %2d %2d %2d  0 |\n", m00, m01, m02);
		printf("| %2d %2d %2d  0 |\n", m10, m11, m12);
		printf("| %2d %2d %2d  0 |\n", m20, m21, m22);
		printf("|  0  0  0  1 |\n");
	}
};

void init_matrix(matrix3x3& dest)
{
	int v[9];
	for (int i = 0; i < 9; ++i)
		v[i] = rand() % 3;
	dest = matrix3x3(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);
}

int main()
{
	matrix3x3 ma, mb;
	init_matrix(ma);
	init_matrix(mb);

	char c;
	do
	{
		cout << "> ";
		cin >> c;
		c = toupper(c);

		if (isdigit(c))
		{
			int v = c - '0';
			cout << "Multiply Operation" << endl;
			cout << "Matrix A :" << endl;
			ma.print();
			cout << endl;
			cout << "Multiplied Matrix A :" << endl;
			(ma * v).print();
			cout << "Matrix B :" << endl;
			mb.print();
			cout << endl;
			cout << "Multiplied Matrix B :" << endl;
			(mb * v).print();
		}

		switch (c)
		{
		case 'M':
			cout << "Multiply Operation" << endl;
			cout << "Operand A :" << endl;
			ma.print();
			cout << endl;
			cout << "Operand B :" << endl;
			mb.print();
			cout << endl;
			cout << "Result : " << endl;
			(ma * mb).print();
			cout << endl;
			break;
		case 'A':
			cout << "Add Operation" << endl;
			cout << "Operand A :" << endl;
			ma.print();
			cout << endl;
			cout << "Operand B :" << endl;
			mb.print();
			cout << endl;
			cout << "Result : " << endl;
			(ma + mb).print();
			cout << endl;
			break;
		case 'D':
			cout << "Subtract Operation" << endl;
			cout << "Operand A :" << endl;
			ma.print();
			cout << endl;
			cout << "Operand B :" << endl;
			mb.print();
			cout << endl;
			cout << "Result : " << endl;
			(ma - mb).print();
			cout << endl;
			break;
		case 'R':
			cout << "Matrix A :" << endl;
			ma.print();
			cout << "Determinant Value of Matrix A : " << ma.determinant() << endl;
			cout << "Matrix B :" << endl;
			mb.print();
			cout << "Determinant Value of Matrix B : " << mb.determinant() << endl;
			break;
		case 'T':
			cout << "Matrix A :" << endl;
			ma.print();
			cout << endl;
			cout << "Transposed Matrix A : " << endl;
			ma.transposed().print();
			cout << endl;
			cout << "Matrix B :" << endl;
			mb.print();
			cout << endl;
			cout << "Transposed Matrix B : " << endl;
			mb.transposed().print();
			cout << endl;
			break;
		case 'H':
			cout << "Matrix A (4x4) :" << endl;
			ma.print_as_4x4();
			cout << "Determinant Value of Matrix A (4x4) : " << ma.determinant() << endl;
			cout << "Matrix B (4x4) :" << endl;
			mb.print_as_4x4();
			cout << "Determinant Value of Matrix B (4x4) : " << mb.determinant() << endl;
			break;
		case 'S':
			init_matrix(ma);
			init_matrix(mb);
			cout << "Matrix A :" << endl;
			ma.print();
			cout << endl;
			cout << "Matrix B :" << endl;
			mb.print();
			cout << endl;
			break;
		}

	} while (c != 'Q');
}