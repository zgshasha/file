#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/resource.h>
#include <netinet/in.h>

#define PORT 5552  //???�������һ��
#define buffer_size 1024
#define max_size 512

int main(int argc,char**argv)
{
    if(argc!=2)//???????????
    { printf("IP error\n");
      exit(1);
    }

    //��������internet����Э�飨TCP������socket,��client_socket����ͻ���socket
    int client_socket=socket(AF_INET,SOCK_STREAM,0);
    if(client_socket<0)
    {  printf("Create Socket Failed!\n");
       exit(1);
    }

    //����һ��socket��ַ�ṹserver_addr,�����������internet��ַ�Ͷ˿�
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));//????????bzero
    server_addr.sin_family=AF_INET;

    //��������IP��ַ���Գ���Ĳ���
    if(inet_aton(argv[1],&server_addr.sin_addr)==0)
   {  printf("Server IP Address Error!\n");
      exit(1);
    }
   server_addr.sin_port=htons(PORT);
   socklen_t server_addr_length=sizeof(server_addr);

    //����������������������ӳɹ���client_socket����ͻ��˺ͷ�������һ��socket����
    if(connect(client_socket,(struct sockaddr*)&server_addr,server_addr_length)<0)
    {  printf("Can Not Connect To %s!\n",argv[1]);
       exit(1);
     }

     char file_name[max_size+1];  //Ҫ������ļ���
     memset(file_name,0,sizeof(file_name));
     printf("Please Input The File Name On Server.\t");
     scanf("%s",file_name);

     char buffer[buffer_size];
     memset(buffer,0,sizeof(buffer));
     strncpy(buffer,file_name,strlen(file_name)>buffer_size?buffer_size:strlen(file_name));//?????????��ڶ��汾��һ��

     //�����������buffer�е����ݣ���ʱbuffer�д�ŵ��ǿͻ�����Ҫ���յ��ļ���
      send(client_socket,buffer,buffer_size,0);

     //���ļ����в���
     FILE *fp=fopen(file_name,"w");
     if(fp==NULL)
     {  printf("File:\t%s Can Not Open To Write!\n",file_name);
        exit(1);
      }

      //�ӷ������˽������ݵ�buffer��
      memset(buffer,0,buffer_size);
      int length=0;
      while(length=recv(client_socket,buffer,buffer_size,0))
      {  if(length<0)
         {  printf("Recieve Data From Server %s Failed!\n",argv[1]);
            break;
          }
          int write_length=fwrite(buffer,sizeof(char),length,fp);//��ָ��ָ����ļ�д��������
          if(write_length<length)
          {  printf("File:\t%s Write Failed!\n",file_name);
             break;
           }
           memset(buffer,0,buffer_size);//��󽫻������
       }
       printf("Recieve File:\t %s From Server[%s] Finished!\n",file_name,argv[1]);

       //������ϣ��ر�socket
       fclose(fp);
       close(client_socket);
       return (0);
}
