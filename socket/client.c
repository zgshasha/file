#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/resource.h>
#include <netinet/in.h>

#define PORT 5552  //???与服务器一样
#define buffer_size 1024
#define max_size 512

int main(int argc,char**argv)
{
    if(argc!=2)//???????????
    { printf("IP error\n");
      exit(1);
    }

    //创建用于internet的流协议（TCP）类型socket,用client_socket代表客户端socket
    int client_socket=socket(AF_INET,SOCK_STREAM,0);
    if(client_socket<0)
    {  printf("Create Socket Failed!\n");
       exit(1);
    }

    //设置一个socket地址结构server_addr,代表服务器的internet地址和端口
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));//????????bzero
    server_addr.sin_family=AF_INET;

    //服务器的IP地址来自程序的参数
    if(inet_aton(argv[1],&server_addr.sin_addr)==0)
   {  printf("Server IP Address Error!\n");
      exit(1);
    }
   server_addr.sin_port=htons(PORT);
   socklen_t server_addr_length=sizeof(server_addr);

    //向服务器发起连接请求，连接成功后client_socket代表客户端和服务器的一个socket连接
    if(connect(client_socket,(struct sockaddr*)&server_addr,server_addr_length)<0)
    {  printf("Can Not Connect To %s!\n",argv[1]);
       exit(1);
     }

     char file_name[max_size+1];  //要传输的文件名
     memset(file_name,0,sizeof(file_name));
     printf("Please Input The File Name On Server.\t");
     scanf("%s",file_name);

     char buffer[buffer_size];
     memset(buffer,0,sizeof(buffer));
     strncpy(buffer,file_name,strlen(file_name)>buffer_size?buffer_size:strlen(file_name));//?????????与第二版本不一样

     //向服务器发送buffer中的数据，此时buffer中存放的是客户端需要接收的文件名
      send(client_socket,buffer,buffer_size,0);

     //对文件进行操做
     FILE *fp=fopen(file_name,"w");
     if(fp==NULL)
     {  printf("File:\t%s Can Not Open To Write!\n",file_name);
        exit(1);
      }

      //从服务器端接收数据到buffer中
      memset(buffer,0,buffer_size);
      int length=0;
      while(length=recv(client_socket,buffer,buffer_size,0))
      {  if(length<0)
         {  printf("Recieve Data From Server %s Failed!\n",argv[1]);
            break;
          }
          int write_length=fwrite(buffer,sizeof(char),length,fp);//将指针指向的文件写道缓冲区
          if(write_length<length)
          {  printf("File:\t%s Write Failed!\n",file_name);
             break;
           }
           memset(buffer,0,buffer_size);//最后将缓存清空
       }
       printf("Recieve File:\t %s From Server[%s] Finished!\n",file_name,argv[1]);

       //传输完毕，关闭socket
       fclose(fp);
       close(client_socket);
       return (0);
}
