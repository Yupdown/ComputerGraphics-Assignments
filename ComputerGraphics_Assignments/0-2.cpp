#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

int main()
{
	ifstream ifs("data.txt");

	string buffer;
	stringstream isstream, osstream;
	int count = 0, countn = 0, countc = 0;

	vector<string> resultn, resultc;

	osstream << ifs.rdbuf();
	while (osstream >> buffer)
	{
		count += 1;
		if (all_of(buffer.begin(), buffer.end(), isdigit))
		{
			countn += 1;
			resultn.push_back(buffer);
		}
		if (any_of(buffer.begin(), buffer.end(), isupper))
		{
			countc += 1;
			resultc.push_back(buffer);
		}
	}

	cout << "Word Count: " << count << " words" << endl;
	cout << "Word with Numbers Count: " << countn << " words" << endl;
	for (const auto& s : resultn)
		cout << s << endl;
	cout << "Word with Capitals Count: " << countc << " words" << endl;
	for (const auto& s : resultc)
		cout << s << endl;
}