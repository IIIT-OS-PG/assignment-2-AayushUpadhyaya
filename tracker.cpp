#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "fidemo.cpp"
#include <utility>
#include <vector>
using namespace std;
#define BUFF_SIZE 1024
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define NO_USER -9999

char *SERVER_IP[2];
int SERVER_PORT[2];
static int global_uid=NO_USER;
unordered_map<string,int> login_map;

void login(string,string);
int verifyCredentials(string,string);
void addUser(string,string);
void logout(string,string);
int groupExistence(int group_id);
void createGroup(int group_id,string);
void listGroups();
int getGroupOwner(int);
int isUserInGroup(int group_id,int user_id);
void joinGroup(int group_id);
int isRequestSent(int group_id,int user_id);
void listPendingGroupReq(int group_id);
void acceptGroupRequest(int group_id,int user_id);
void leaveGroup(int group_id);
void uploadFile(string file_path,int group_id,int user_id);
void listFilesInGroup(int);



void listFilesInGroup(int group_id)
{
  string filename=to_string(group_id)+"_files.txt";
  ifstream file1(filename);
  string input_line;
  while(file1)
  {
    if(getline(file1,input_line))
    {
      string temp=input_line.substr(0,input_line.find(' '));
      cout<<temp.substr(temp.rfind("/")+1,temp.size())<<endl;
    }
  }
  file1.close();
}

vector<string> listFilesInGroup2(int group_id)
{
  string filename=to_string(group_id)+"_files.txt";
  ifstream file1(filename);
  string input_line;
  vector<string> opt;
  while(file1)
  {
    if(getline(file1,input_line))
    {
      string temp=input_line.substr(0,input_line.find(' '));
      opt.push_back(temp.substr(temp.rfind("/")+1,temp.size()));
    }
  }
  file1.close();
}



void uploadFile(string file_path,int group_id,int user_id,string CLIENT_INFO)
{
   if(!isUserInGroup(group_id,user_id))
   {
    cout<<"User : "<<user_id<<" not a part of Group : "<<group_id<<" hence upload aborted!"<<endl;
    return;
   }
   else
   {
      string filename=to_string(group_id)+"_files.txt";
      ofstream oft;
      oft.open (filename,ios::app); // Append mode
      oft<<file_path<<" "<<user_id<<" "<<CLIENT_INFO<<"\n"; // Writing data to file
      oft.close();
      prepareForUpload(file_path,user_id);
      cout<<"File : "<<file_path<<" Uploaded to Group : "<<group_id<<" successfully!"<<endl;


   }
} //end of functino uploadFile

void leaveGroup(int group_id,string CLIENT_PID)
{
  int user_id=login_map[CLIENT_PID];
  if(isUserInGroup(group_id,user_id))
  {

    string filename1=to_string(group_id)+"_users.txt";
    string filename2=to_string(group_id)+"_temp.txt";
    fstream file1(filename1);
    ofstream file2;
    file2.open(filename2);
    int a;
    while(file1>>a)
    {
      if(a!=user_id)
        file2<<a<<"\n";
    }
    file2.close();
    file1.close();
    remove(filename1.c_str());
    rename(filename2.c_str(), filename1.c_str());
    cout<<"User : "<<user_id<<" removed from Group : "<<group_id<<endl;

  }
  else cout<<"User : "<<user_id<<" not a part of Group : "<<group_id<<endl;
}

void acceptGroupRequest(int group_id,int user_id,string CLIENT_PID)
{
  if(isUserInGroup(group_id,user_id)) cout<<"User : "<<user_id<<" already in Group : "<<group_id<<endl;
 
  //cout<<"getGroupOwner("<<group_id<<"): "<<getGroupOwner(group_id)<<endl;
  //cout<<"login_map[CLIENT_PID]: "<<login_map[CLIENT_PID]<<endl;
  //cout<<"isRequestSent("<<group_id<<","<<user_id<<") : "<<isRequestSent(group_id,user_id)<<endl;

   if (getGroupOwner(group_id)==login_map[CLIENT_PID] && isRequestSent(group_id,user_id))
  {
    /*If the logged in Owner is the owner of the group_id and if there is pending request for user_id in
    group_id, then
    (1)Remove request from Pending_Users.txt
    (2)Add user_id in Group_users.txt
    */
    string filename1=to_string(group_id)+"_pending_users.txt";
    string filename2="temp.txt";
    string filename3=to_string(group_id)+"_users.txt";
    fstream file1(filename1);
    ofstream file2;
    file2.open(filename2);
    int a;
    while(file1>>a)
    {
      if(a!=user_id)
        file2<<a<<"\n";
    }
    file2.close();
    file1.close();
    remove(filename1.c_str());
    rename("temp.txt", filename1.c_str());

    ofstream oft;
    oft.open (filename3,ios::app); // Append mode
    oft<<user_id<<"\n"; // Writing data to file
    oft.close();
    cout<<user_id<<"is now a part of Group : "<<group_id<<endl;

  }

  else cout<<"Group Request Accept Failed!"<<endl;

} //end of function acceptGroupRequest

void listPendingGroupReq(int group_id)
{
   string filename=to_string(group_id)+"_pending_users.txt";
  fstream file1(filename);
  int a;
  cout<<"All pending group request for group : "<<group_id<<endl;
  while(file1 >>a)
  {
    cout<<a<<endl;
  }
  file1.close();

} //end of function listPendingGroupReq


int isRequestSent(int group_id,int user_id)
{
  string filename=to_string(group_id)+"_pending_users.txt";
   ifstream file1(filename);
  int a;
  while(file1 >>a)
  {
    if(a==user_id){
     file1.close();
    return 1;
    }
    file1.close();
  return 0;
  } //end of while loop.
} //end of function isRequestSent

void joinGroup(int group_id,string CLIENT_PID)
{
  int user_id=login_map[CLIENT_PID];
  if(isRequestSent(group_id,user_id)) cout<<"Request to join this group already sent!"<<endl;
  else if(!isUserInGroup(group_id,user_id)) //user is not the member of the group.
  {
    string filename=to_string(group_id)+"_pending_users.txt";
    ofstream oft;
    oft.open (filename,ios::app); // Append mode
    oft<<user_id<<"\n"; // Writing data to file
    oft.close();

  }
  else cout<<"User : "<<user_id<<" already a part of Group: "<<group_id<<endl;
}

int isUserInGroup(int group_id,int user_id)
{
   string filename=to_string(group_id)+"_users.txt";
   ifstream file1(filename);
  int a;
  while(file1 >>a)
  {
    if(a==user_id) return 1;
  }
  return 0;
}

void listGroups()
{
   ifstream file1("All_Groups.txt");
  int a,b;
  cout<<"All groups in this network"<<endl;
  while(file1 >>a >>b)
  {
    cout<<"Group Id: "<<a<<" , Owner: "<<b<<endl;
  }
  file1.close();

}

int getGroupOwner(int group_id)
{
  ifstream file1("All_Groups.txt");
  int a,b;
  while(file1>>a>>b)
  {
    if(a==group_id) return b;
  }

  return -1;

}


void createGroup(int group_id,string CLIENT_PID)
{
  if(!groupExistence(group_id) && login_map.find(CLIENT_PID)!=login_map.end())
  {
      cout<<"User active is : "<<login_map[CLIENT_PID]<<endl;

    string filename1=to_string(group_id)+"_users.txt";
    string filename2=to_string(group_id)+"_pending_users.txt";
    string filename3=to_string(group_id)+"_files.txt";
    ofstream file1,file2,file3;
    file1.open(filename1);
    file1<<login_map[CLIENT_PID]<<"\n";
    file1.close();
    file2.open(filename2);
    file2.close();
    file3.open(filename3);
    file3.close();

    //add group info to All_Groups.txt
    ofstream oft;
    oft.open ("All_Groups.txt",ios::app); // Append mode
    oft<< group_id<<" "<<login_map[CLIENT_PID]<<"\n"; // Writing data to file
    oft.close();
    cout<<"Group : "<<group_id<<" created successfully!"<<endl;
  }
  else cout<<group_id<<": Group already Exists!"<<endl; 
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

void logout(string CLIENT_PID,string user_id)
{
  if(login_map.find(CLIENT_PID)!=login_map.end())
  {
    login_map.erase(CLIENT_PID);
    cout<<"User : "<<user_id<<" logged out!"<<endl;
  }
  else
  cout<<"Logout failed!"<<endl;

}


void login(string user_id,string password,string CLIENT_PID)
{
  if(verifyCredentials(user_id,password))
  {
     //global_uid=atoi(user_id.c_str());
     if(login_map.find(CLIENT_PID)==login_map.end())
     {
        login_map[CLIENT_PID]=stoi(user_id);
        cout<<"User : "<<user_id<<" logged in."<<endl;
     }
     else
     {
      cout<<"User : "<<user_id<<" already logged in!"<<endl;
      return;
     }
    
  }

  else cout<<"Login failed.Try again!"<<endl;

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




char ** commandParser(char input[1024])
{
	char *token;
	char **tokens=(char **)malloc(sizeof(char *)*10);
	int index=1;
	token=strtok(input," ");
        //printf("IP : ");
        tokens[0]=token;

        while(token!=NULL)
        {
        	
        	token=strtok(NULL," ");
        	if(token!=NULL)
        	{ 
        		tokens[index++]=token;

             }
        }

        return tokens;

}

void commandHandler(char input[1024])
{
	int length=strlen(input);
	char **command_tokens;

	command_tokens=commandParser(input);

	//printf("\n Command is : %s\n",command_tokens[0]);

	if(strcmp(command_tokens[0],"create_user")==0)
	{
		addUser(command_tokens[1],command_tokens[2]);
    cout<<command_tokens[1]<<": USER created!."<<endl;
	}

 
  if(strcmp(command_tokens[0],"login")==0)
  {
    login(command_tokens[1],command_tokens[2],command_tokens[3]);
  }

  if(strcmp(command_tokens[0],"logout")==0)
  {
    logout(command_tokens[1],to_string(login_map[command_tokens[1]]));
  }

  if(strcmp(command_tokens[0],"create_group")==0)
  {
    int new_gid=stoi(command_tokens[1]);
    createGroup(new_gid,command_tokens[2]);
  }

  if(strcmp(command_tokens[0],"list_groups")==0)
  {
    listGroups();
  }

  if(strcmp(command_tokens[0],"get_group_owner")==0)
  {
     int owner=0;
     if((owner=getGroupOwner(stoi(command_tokens[1])))>0)
       cout<<owner<<endl;
  }

  if(strcmp(command_tokens[0],"check_user")==0)
  {
    cout<<isUserInGroup(stoi(command_tokens[1]),stoi(command_tokens[2]))<<endl;
  }

  if(strcmp(command_tokens[0],"join_group")==0)
  {
    if(login_map.find(command_tokens[2])!=login_map.end())
    {
      joinGroup(stoi(command_tokens[1]),command_tokens[2]);
    }
    else cout<<"Login first!"<<endl;
  }

  if(strcmp(command_tokens[0],"list_requests")==0)
  {
    listPendingGroupReq(stoi(command_tokens[1]));
  }

  if(strcmp(command_tokens[0],"accept_request")==0)
  {
    acceptGroupRequest(stoi(command_tokens[1]),stoi(command_tokens[2]),command_tokens[3]);
  }

  if(strcmp(command_tokens[0],"leave_group")==0)
  {
     if(login_map.find(command_tokens[2])!=login_map.end())
    {
      leaveGroup(stoi(command_tokens[1]),command_tokens[2]);
    }
    else cout<<"Login first!"<<endl;
  }

  if(strcmp(command_tokens[0],"upload_file")==0)
  {
    if(login_map.find(command_tokens[3])!=login_map.end())
    {
       int user_id=login_map[command_tokens[3]];
       uploadFile(command_tokens[1],stoi(command_tokens[2]),user_id,command_tokens[4]);
     }
    else cout<<"Login first!"<<endl;
  }

  if(strcmp(command_tokens[0],"list_files")==0)
  {
     listFilesInGroup(stoi(command_tokens[1]));
  }

}

void handleClient(int sockfd,char buffer[1024])
{
         int val_read;
        val_read=read(sockfd,buffer,1024);
        if(val_read){ printf("Command read from Client is : %s \n",buffer);
        commandHandler(buffer);
          }
          bzero(buffer,1024);

}

int main(int argc,char *argv[])
{ 
	 //printf("%s\n",argv[1]);
	char *file_name=argv[1];
	FILE *fp;
	char *line=NULL;
	size_t len=0;
	ssize_t read1;

	int tracker_id=atoi(argv[2]);
	char buffer[1024]={0};
	fp=fopen(file_name,"r");
	if(fp==NULL)
		exit(EXIT_FAILURE);

	char *token;

	
	int index=0;

   while ((read1 = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        token=strtok(line," ");
        //printf("IP : ");
        SERVER_IP[index]=token;

        while(token!=NULL)
        {
        	//printf("%s\n",token);
        	token=strtok(NULL," ");
        	if(token!=NULL)
        	{ //printf("PORT : ");
              SERVER_PORT[index++]=atoi(token);
             }
        }
       // printf("%s\n", line);
    }

    for(int i=0;i<2;i++)
    {
    	printf("Server Ip : %s\n",SERVER_IP[i]);
    	printf("Server Port : %d \n",SERVER_PORT[i]);
    }


    if(tracker_id==1)
    {
    	//start tracker 1.
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
   struct sockaddr_in addr;
   addr.sin_family=AF_INET;
   addr.sin_port=htons(SERVER_PORT[0]);
   addr.sin_addr.s_addr=inet_addr(SERVER_IP[0]);
   int addrlen=sizeof(addr);

   bind(server_fd,(struct sockaddr *)&addr,sizeof(addr));
   //listen(server_fd,20);

     
     size_t buf_sz=50;
     int sockfd;
       listen(server_fd,20);
              
       printf("\nTracker 1 listening at %s:%d\n",SERVER_IP[0],SERVER_PORT[0]);
       //sockfd=accept(server_fd,(struct sockaddr *)&addr,(socklen_t *)&addrlen);

     while(1)
     {
              
        sockfd=accept(server_fd,(struct sockaddr *)&addr,(socklen_t *)&addrlen);
        thread th1(handleClient,sockfd,buffer);
      
     
       th1.join();
      close(sockfd);

       buffer[0]={'\0'};
     } //keep listening forever.
       close(sockfd);
    } //end of if tracker_id==1




    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);



	return 0;
}