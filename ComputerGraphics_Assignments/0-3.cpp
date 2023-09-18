#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

string reverse_line(const string& s)
{
	string output, temp = s;

	size_t p = 0;
	while ((p = temp.find('\n')) != string::npos)
	{
		string token = temp.substr(0, p);
		reverse(token.begin(), token.end());
		output.append(token);
		output.push_back(temp[p]);
		temp.erase(0, p + 1);
	}

	return output;
}

string reverse_word(const string& s)
{
	string output, temp = s;

	size_t p = 0;
	while ((p = min(temp.find('\n'), temp.find(' '))) != string::npos)
	{
		string token = temp.substr(0, p);
		if (!token.empty())
		{
			reverse(token.begin(), token.end());
			output.append(token);
		}
		output.push_back(temp[p]);
		temp.erase(0, p + 1);
	}

	return output;
}

string change_string(const string& s, char from, char to)
{
	string output = s;
	replace(output.begin(), output.end(), from, to);
	return output;
}

string change_value(const string& s, int offset)
{
	string output, temp = s;

	size_t p = 0;
	while ((p = min(temp.find('\n'), temp.find(' '))) != string::npos)
	{
		string token = temp.substr(0, p);
		if (!token.empty())
		{
			try
			{
				int n = stoi(token);
				n = max(n + offset, 0);
				output.append(to_string(n));
			}
			catch (invalid_argument e)
			{
				output.append(token);
			}
		}
		output.push_back(temp[p]);
		temp.erase(0, p + 1);
	}

	return output;
}

string insert_string(const string& s)
{
	string output;

	int cnt = 0;
	for (char c : s)
	{
		output.push_back(c);
		if (++cnt % 3 == 0)
			output.append("@@");
	}

	return output;
}

void count_palindrome(const string& s)
{
	string temp = s;

	size_t p = 0;
	while ((p = temp.find('\n')) != string::npos)
	{
		string token = temp.substr(0, p);
		if (!token.empty())
		{
			int mp = 0;
			while (mp < token.length() / 2)
			{
				if (token[mp] != token[token.length() - mp - 1])
					break;
				++mp;
			}
			cout << token << " : " << (mp > 0 ? token.substr(0, mp) : "None") << endl;
		}
		temp.erase(0, p + 1);
	}
}

int main()
{
	ifstream ifs("data.txt");

	string text;
	stringstream osstream;

	osstream << ifs.rdbuf();
	text = osstream.str();

	char c, p1, p2;

	char last_c = 0;
	string last_text;

	do
	{
		cout << "> ";
		cin >> c;
		c = toupper(c);

		string temp_text = last_text;
		last_text = text;

		switch (c)
		{
		case 'D':
			if (c != last_c)
				text = reverse_line(text);
			else
				text = temp_text;
			cout << text << endl;
			break;
		case 'E':
			if (c != last_c)
				text = insert_string(text);
			else
				text = temp_text;
			cout << text << endl;
			break;
		case 'F':
			if (c != last_c)
				text = reverse_word(text);
			else
				text = temp_text;
			cout << text << endl;
			break;
		case 'G':
			cin >> p1 >> p2;
			text = change_string(text, p1, p2);
			cout << text << endl;
			break;
		case 'H':
			count_palindrome(text);
			break;
		case '+':
			text = change_value(text, 1);
			cout << text << endl;
			break;
		case '-':
			text = change_value(text, -1);
			cout << text << endl;
			break;
		}

		last_c = c;
	} while (c != 'Q');
}