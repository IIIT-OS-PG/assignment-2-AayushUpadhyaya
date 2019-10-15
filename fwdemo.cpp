#include <iostream>
#include <fstream>
using namespace std;
int main(){
	ofstream file1;
	file1.open("new_file.txt");
	file1<<"Writing to new_file\n";
	file1.close();
	return 0;
}