#include <iostream>
#include <Windows.h>

char board[16];
bool visited[16];

void initialize_board()
{
	for (int i = 0; i < 16; ++i)
		board[i] = 'A' + i / 2;
	for (int i = 0; i < 16; ++i)
		std::swap(board[i], board[rand() % 16]);
}

void print_board()
{
	printf("%2c %2c %2c %2c %2c\n", ' ', 'a', 'b', 'c', 'd');
	for (int i = 0; i < 4; ++i)
	{
		printf("%2d", i + 1);
		for (int j = 0; j < 4; ++j)
		{
			if (visited[i * 4 + j])
			{
				char c = board[i * 4 + j];
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c - 'A' + 6);
				printf(" %2c", c);
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			}
			else
				printf(" %2c", '*');
		}
		printf("\n");
	}
}

inline int to_index(const char* input)
{
	int index = (input[1] - '1') * 4 + (toupper(input[0]) - 'A');
	return index >= 0 && index < 16 ? index : -1;
}

int main()
{
	int cnt = 0;
	char buffer[8];

	initialize_board();

	while (cnt < 16)
	{
		int ia, ib;

		print_board();
		std::cout << "\nInput Card Number 1\n";
		do
		{
			std::cout << "> ";
			std::cin >> buffer;
			ia = to_index(buffer);
		} while (ia < 0 || visited[ia]);
		visited[ia] = true;

		print_board();
		std::cout << "\nInput Card Number 2\n";
		do
		{
			std::cout << "> ";
			std::cin >> buffer;
			ib = to_index(buffer);
		} while (ib < 0 || visited[ib]);
		visited[ib] = true;

		system("cls");
		if (board[ia] == board[ib])
			cnt += 2;
		else
		{
			print_board();
			visited[ia] = false;
			visited[ib] = false;
		}
		std::cout << "\n";
	}
	print_board();
}