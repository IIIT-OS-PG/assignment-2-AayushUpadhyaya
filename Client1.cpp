#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <cstring>
#include <iostream>
#include <fstream>
#define PORT 8081
#define EXIT_FAILURE 1
using namespace std;

char * SERVER_IP[2];
int SERVER_PORT[2];

void readTrackerFile(char *file_name)
{
  
	FILE *fp;
	char *line=NULL;
	size_t len=0;
	ssize_t read1;

	
	char buffer[1024]={'\0'};
	fp=fopen(file_name,"r");
	if(fp==NULL)
		exit(EXIT_FAILURE);

	char *token;

	
	int index=0;

   while ((read1 = getline(&line, &len, fp)) != -1) {
        token=strtok(line," ");
        SERVER_IP[index]=token;

        while(token!=NULL)
        {
        	token=strtok(NULL," ");
        	if(token!=NULL)
        	{ //printf("PORT : ");
              SERVER_PORT[index++]=atoi(token);
             }
        }
    }

} //end of function readTrackerFile.


int main(int argc,char *argv[])
{   int sock=0,valread;
	pid_t CLIENT_PROCESS_ID=getpid();
	//cout<<"PID is : "<<CLIENT_PROCESS_ID<<endl;
	string user_input;
	struct sockaddr_in serv_addr;
	char buffer[1024]={0};
    readTrackerFile(argv[2]);
    string client_data(argv[1]);
    string CLIENT_IP=client_data.substr(0,client_data.find(':'));
    string CLIENT_PORT=client_data.substr(client_data.find(':')+1,client_data.size());
    cout<<"Client IP : "<<CLIENT_IP<<" and Client Port : "<<CLIENT_PORT<<endl;
    cout<<"Server 1 IP : "<<SERVER_IP[0]<<" and PORT : "<<SERVER_PORT[0]<<endl;
    cout<<"Server 2 IP : "<<SERVER_IP[1]<<" and PORT : "<<SERVER_PORT[1]<<endl;

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(PORT);
	serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

	
    char * writable;
	while(1)
  {		
     getline(cin,user_input);
     if(strcmp(user_input.c_str(),"exit")==0) exit(0);
     user_input+=" "+to_string(CLIENT_PROCESS_ID);
     user_input+=" ";
     user_input.append(argv[1]);
     

       if((sock=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("\n Socket creating error\n");
		return -1;
	}
 
       if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	{
		printf("\nConnection Failed \n"); 
        return -1; 
	}	
    

      writable = new char[user_input.size() + 1];
     std::copy(user_input.begin(), user_input.end(), writable);
     writable[user_input.size()] = '\0';
   

     
	write(sock,writable,strlen(writable));
	
     close(sock);
  }	
   delete [] writable;
	close(sock);

    

	return 0;
}