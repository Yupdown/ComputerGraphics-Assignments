#include <iostream>
#include <algorithm>

using namespace std;

constexpr int SIZEOF_LIST = 10;

struct vector3d
{
	int x, y, z;

	vector3d(int x, int y, int z) : x(x), y(y), z(z)
	{

	}

	vector3d() : vector3d(0, 0, 0)
	{

	}

	friend ostream& operator<<(ostream& s, const vector3d& v)
	{
		s << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
		return s;
	}

	int sqr_magnitude() const
	{
		return x * x + y * y + z * z;
	}
};

vector3d veclist[SIZEOF_LIST];
int length = 0;
int offset = 0;

void print_veclist()
{
	for (int i = SIZEOF_LIST - 1; i >= 0; --i)
	{
		cout << "[" << i << "] = ";
		if (i >= offset && i < offset + length)
			cout << veclist[i - offset] << endl;
		else
			cout << "null" << endl;
	}
}

bool compare_less(vector3d& lhs, vector3d& rhs)
{
	return lhs.sqr_magnitude() < rhs.sqr_magnitude();
}

bool compare_greater(vector3d& lhs, vector3d& rhs)
{
	return lhs.sqr_magnitude() > rhs.sqr_magnitude();
}

int main()
{
	char c;
	int x, y, z;
	
	do
	{
		cout << "> ";
		cin >> c;
		c = toupper(c);

		switch (c)
		{
		case '+':
			cin >> x >> y >> z;
			if (length >= SIZEOF_LIST)
				cout << "List is out of space" << endl;
			else
			{
				if (offset + length >= SIZEOF_LIST)
					offset -= 1;
				veclist[length] = vector3d(x, y, z);
				length += 1;
				print_veclist();
			}
			break;
		case '-':
			if (length <= 0)
				cout << "List is empty" << endl;
			else
			{
				length -= 1;
				print_veclist();
			}
			break;
		case 'E':
			cin >> x >> y >> z;
			if (length >= SIZEOF_LIST)
				cout << "List is out of space" << endl;
			else
			{
				if (offset > 0)
					offset -= 1;
				for (int i = length; i > 0; --i)
					veclist[i] = veclist[i - 1];
				veclist[0] = vector3d(x, y, z);
				length += 1;
				print_veclist();
			}
			break;
		case 'D':
			if (length <= 0)
				cout << "List is empty" << endl;
			else
			{
				for (int i = 0; i < length - 1; ++i)
					veclist[i] = veclist[i + 1];
				offset += 1;
				length -= 1;
				print_veclist();
			}
			break;
		case 'I':
			cout << "Points Count : " << length << endl;
			break;
		case 'C':
			offset = 0;
			length = 0;
			print_veclist();
			break;
		case 'M':
			if (length <= 0)
				cout << "List is empty" << endl;
			else
			{
				int target = 0;
				for (int i = 0; i < length; ++i)
				{
					if (veclist[i].sqr_magnitude() > veclist[target].sqr_magnitude())
						target = i;
				}
				cout << "Farthest Point : " << veclist[target] << endl;
			}
			break;
		case 'N':
			if (length <= 0)
				cout << "List is empty" << endl;
			else
			{
				int target = 0;
				for (int i = 0; i < length; ++i)
				{
					if (veclist[i].sqr_magnitude() < veclist[target].sqr_magnitude())
						target = i;
				}
				cout << "Nearest Point : " << veclist[target] << endl;
			}
			break;
		case 'A':
			sort(veclist, veclist + length, compare_less);
			offset = 0;
			print_veclist();
			break;
		case 'S':
			sort(veclist, veclist + length, compare_greater);
			offset = 0;
			print_veclist();
			break;
		}
	} while (c != 'Q');
}