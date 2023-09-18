#include <iostream>
#include <Windows.h>

constexpr int SIZEOF_BOARD = 30;

void translate(int& x1, int& y1, int& x2, int& y2, int dx, int dy)
{
	x1 = (x1 + SIZEOF_BOARD + dx) % SIZEOF_BOARD;
	y1 = (y1 + SIZEOF_BOARD + dy) % SIZEOF_BOARD;
	x2 = (x2 + SIZEOF_BOARD + dx) % SIZEOF_BOARD;
	y2 = (y2 + SIZEOF_BOARD + dy) % SIZEOF_BOARD;
}

void scale(int& x1, int& y1, int& x2, int& y2, int dx, int dy)
{
	if ((dx > 0 || x1 != x2) && (dx < 0 || x2 < SIZEOF_BOARD - 1))
		x2 += dx;
	if ((dy > 0 || y1 != y2) && (dy < 0 || y2 < SIZEOF_BOARD - 1))
		y2 += dy;
}

void print_board(int x1, int y1, int x2, int y2)
{
	for (int y = 0; y < SIZEOF_BOARD; ++y)
	{
		for (int x = 0; x < SIZEOF_BOARD; ++x)
		{
			bool flag = true;
			if (x1 <= x2)
				flag &= x >= x1 && x <= x2;
			else
				flag &= x >= x1 || x <= x2;
			if (y1 <= y2)
				flag &= y >= y1 && y <= y2;
			else
				flag &= y >= y1 || y <= y2;
			std::cout << (flag ? "@" : ".");
		}
		std::cout << std::endl;
	}
}

void input_board(int& x1, int& y1, int& x2, int& y2)
{
	std::cout << "Input Coord Value : ";
	std::cin >> x1 >> y1 >> x2 >> y2;
}

int main()
{
	int x1, y1, x2, y2;
	input_board(x1, y1, x2, y2);

	char c;
	do
	{
		print_board(x1, y1, x2, y2);

		std::cout << "> ";
		std::cin >> c;
		c = toupper(c);

		switch (c)
		{
		case 'W':
			translate(x1, y1, x2, y2, 0, -1);
			break;
		case 'A':
			translate(x1, y1, x2, y2, -1, 0);
			break;
		case 'S':
			translate(x1, y1, x2, y2, 0, 1);
			break;
		case 'D':
			translate(x1, y1, x2, y2, 1, 0);
			break;
		case '+':
			scale(x1, y1, x2, y2, 1, 1);
			break;
		case '-':
			scale(x1, y1, x2, y2, -1, -1);
			break;
		case 'I':
			scale(x1, y1, x2, y2, 1, 0);
			break;
		case 'J':
			scale(x1, y1, x2, y2, -1, 0);
			break;
		case 'K':
			scale(x1, y1, x2, y2, 0, 1);
			break;
		case 'L':
			scale(x1, y1, x2, y2, -1, 0);
			break;
		case 'R':
			input_board(x1, y1, x2, y2);
			break;
		}

	} while (c != 'Q');
}