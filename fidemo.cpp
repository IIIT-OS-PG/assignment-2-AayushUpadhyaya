#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>
#include <sstream>
#include<string.h>
#define CHUNK_SIZE 524288
#include <openssl/sha.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

string sha256(const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

long findSize( const char *filename )
{
   struct stat statbuf;
   if ( stat( filename, &statbuf ) == 0 )
   {
      return statbuf.st_size;
   }
   else
   {
      return 0;
   }
}



void prepareForUpload(string filename,int user_id)
{
  
     string real_name;
	 streampos begin,end;
	 
	 
	 real_name=filename.substr(filename.rfind("/")+1,filename.size());
	 //cout<<"File name is : "<<real_name<<endl;
	 ifstream file1(filename,ios::binary);
	 //begin=file1.seekg(0,ios::end);
	 //end=file1.tellg();
	 long int file_size=findSize(filename.c_str());
	 int num_chunks=(int)floor(file_size/CHUNK_SIZE)+1;

     //cout<<"File size is : "<<file_size<<" bytes"<<endl;
     //cout<<"Number of chunks : "<<num_chunks<<endl;

   char *buffer = new char[CHUNK_SIZE];
  int chunk_Count=1;
  file1.seekg (0, ios::beg);
  string chunk_sha,file_sha;

  ofstream new_file1,new_file2;
  string nf_1,nf_2;
  nf_1="./File_data/"+real_name+"_metadata.txt";
  nf_2="./File_data/"+real_name+"_bv.txt";

  new_file1.open(nf_1);
  new_file2.open(nf_2,ios::app);

  new_file1<<num_chunks<<"\n";

while (file1)
{   //cout<<"Chunk No. : "<<chunk_Count<<"\n";
    // Try to read next chunk of data
    file1.read(buffer, CHUNK_SIZE);
    // Get the number of bytes actually read
    size_t count = file1.gcount();
    // If nothing has been read, break
    if (!count) 
        break;
    chunk_sha=sha256(buffer).substr(0,21);
    //cout<<"Chunk SHA : "<<chunk_sha<<endl;
    //cout<<"Chunk size : "<<count<<"  bytes\n\n"<<endl;
    bzero(buffer,CHUNK_SIZE);
    chunk_Count++;
    file_sha.append(chunk_sha);
    new_file1<<chunk_sha<<"\n";
    chunk_sha="";
    // Do whatever you need with first count bytes in the buffer
    // ...
    
    
}

delete[] buffer;

 file1.close();
  //cout<<"\n\nFull file SHA : "<<file_sha<<endl;
  //cout<<"Bit Vector : ";
  new_file2<<user_id<<" ";
  for(int z=1;z<=num_chunks;z++) {  new_file2<<"1";}
  	cout<<endl;
  new_file1<<file_sha<<"\n";
  new_file1.close();
  new_file2.close();

}


/*int main(){
	 int gid;
	 cin>>gid;
	 listFilesInGroup(gid);
	 return 0;

}  */