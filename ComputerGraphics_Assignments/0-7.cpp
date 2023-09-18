#include <iostream>
#include <Windows.h>
#include <queue>

constexpr int SIZEOF_BOARD = 50;
constexpr int MAX_LENGTH = 5;

int board[SIZEOF_BOARD][SIZEOF_BOARD];
int direction[4][2] = { 1, 0, 0, -1, -1, 0, 0, 1 };

bool create_path(int fx, int fy, int tx, int ty, int ld)
{
	if (fx == tx && fy == ty)
		return true;

	std::queue<int> ndq;
	ndq.push((ld + 1 + rand() * 2) % 4);
	ndq.push((ndq.front() + 2) % 4);

	while (!ndq.empty())
	{
		int nd = ndq.front();
		ndq.pop();

		int l = 1;
		for (; l < MAX_LENGTH; ++l)
		{
			int nx = fx + direction[nd][0] * l;
			int ny = fy + direction[nd][1] * l;

			if (nx < 0 || nx >= SIZEOF_BOARD || ny < 0 || ny >= SIZEOF_BOARD)
				break;
			if (board[nx][ny] > 0)
				break;

			bool flag = true;
			for (int npx = nx - 1; npx <= nx + 1; ++npx)
			{
				for (int npy = ny - 1; npy <= ny + 1; ++npy)
				{
					if (npx < 0 || npx >= SIZEOF_BOARD || npy < 0 || npy >= SIZEOF_BOARD)
						continue;
					if (npx == nx - direction[nd][0] || npy == ny - direction[nd][1])
						continue;
					if (board[npx][npy])
						flag = false;
				}
			}
			if (!flag)
				break;

			if (nx == tx && ny == ty)
			{
				l += 1;
				break;
			}
		}

		if (l < 2)
			continue;
		for (int i = 0; i < l; ++i)
			board[fx + direction[nd][0] * i][fy + direction[nd][1] * i] = 1;
		if (create_path(fx + direction[nd][0] * (l - 1), fy + direction[nd][1] * (l - 1), tx, ty, nd))
			return true;
		else
		{
			for (int i = 0; i < l; ++i)
				board[fx + direction[nd][0] * i][fy + direction[nd][1] * i] = 0;
		}
	}
	return false;
}

int cpos[2] = { -1, -1 };
int lpos[2] = { -1, -1 };

void print_path()
{
	for (int i = 0; i < SIZEOF_BOARD; ++i)
	{
		for (int j = 0; j < SIZEOF_BOARD; ++j)
		{
			if (i == cpos[0] && j == cpos[1])
			{

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
				std::cout << '*';
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
			else
				std::cout << (board[i][j] ? "." : "1");
		}
		std::cout << std::endl;
	}
}

void forward_player()
{
	for (int d = 0; d < 4; ++d)
	{
		int xp = cpos[0] + direction[d][0];
		int yp = cpos[1] + direction[d][1];

		if (xp == lpos[0] && yp == lpos[1])
			continue;
		if (xp < 0 || xp >= SIZEOF_BOARD || yp < 0 || yp >= SIZEOF_BOARD)
			continue;
		if (board[xp][yp])
		{
			lpos[0] = cpos[0];
			lpos[1] = cpos[1];
			cpos[0] = xp;
			cpos[1] = yp;
			return;
		}
	}
}

bool backward_player()
{
	if (lpos[0] >= 0 && lpos[1] >= 0)
	{
		std::swap(lpos[0], cpos[0]);
		std::swap(lpos[1], cpos[1]);
		return true;
	}
	return false;
}

int main()
{
	bool forward = true;
	int n = 0;

	memset(board, 0, sizeof(board));
	create_path(0, 0, SIZEOF_BOARD - 1, SIZEOF_BOARD - 1, 0);

	char c;
	do
	{
		print_path();
		std::cout << "> ";
		std::cin >> c;
		c = toupper(c);

		switch (c)
		{
		case 'N':
			if (++n > 3)
				break;
			memset(board, 0, sizeof(board));
			create_path(0, 0, SIZEOF_BOARD - 1, SIZEOF_BOARD - 1, 0);
			break;
		case 'R':
			cpos[0] = 0;
			cpos[1] = 0;
			forward = true;
			break;
		case '+':
			if (!forward)
			{
				if (backward_player())
					forward = true;
			}
			else
				forward_player();
			break;
		case '-':
			if (forward)
			{
				if (backward_player())
					forward = false;
			}
			else
				forward_player();
			break;
		}

	} while (c != 'Q');
}