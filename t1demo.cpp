#include <iostream>
#include <thread>
using namespace std;



void foo(int Z) 
{ 
    for (int i = 0; i < Z; i++) { 
        cout << "Thread using function"
               " pointer as callable\n"; 
    } 
} 

int main()
{
	thread t1(foo,3);
	t1.join();
	cout<<"\n Main Thread\n";
	return 0;
}