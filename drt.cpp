#include <iostream>
#include <fstream>
using namespace std;


void readData()
{
	ifstream infile("user_info.txt");
	string a,b;
	while(infile>>a>>b)
	{
	cout<<a<<"  "<<b<<endl;
	}

	infile.close();

}

int verifyCredentials(string user_id,string password)
{
   
    ifstream infile("user_info.txt");
	string a,b;
	while(infile>>a>>b)
	{
       if(user_id.compare(a)==0 && password.compare(b)==0) return 1;
 	}
 	infile.close();
 	return 0;
}

void addUser(string user_id,string password)
{
	ofstream oft;
	oft.open ("user_info.txt",ios::app); // Append mode
    oft<< user_id<<" "<<password<<"\n"; // Writing data to file
    oft.close(); // Closing the file
}

int groupExistence(int group_id)
{
  ifstream infile("All_Groups.txt");
  int gid;
  while(infile>>gid)
  {
    if(gid==group_id) 
    { infile.close();
      return 1;
    }
  }
  infile.close();
  return 0;

}

int main()
{
    cout<<groupExistence(7645)<<endl;

	return 0;
}