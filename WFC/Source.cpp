#include <iostream>
#include <vector>
using namespace std;


struct FTest
{
	FTest(){}
	~FTest(){cout << "test" << endl; }
};

int main()
{
	vector<FTest*>* listval = new vector<FTest*>();
	listval->push_back(new FTest());

	listval->clear();

	return 0;
}
