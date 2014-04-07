#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/resource.h>
#include <netinet/in.h>

#define PORT 5552  //�궨��



#define LISTEN 20

#define buffer_size 1024

#define max_size 512



int main(int argc,char**argv)

{
    //����һ��socket��ַ�ṹserver_addr,���������internet�ĵ�ַ�Ͷ˿�

    struct sockaddr_in server_addr;

    bzero(&server_addr,sizeof(server_addr));//����

    server_addr.sin_family=AF_INET;

    //server_addr.sin_addr.s_addr=htons(INADDR_ANY);    //�����ַ
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port=htons(PORT);



    //��������internet����Э�飨TCP��socket,��server_socket�����������ͻ����ṩ����Ľӿ�

    int server_socket=socket(AF_INET,SOCK_STREAM,0);

    if(server_socket<0)

    {
        perror("Create Socket Failed!\n");//��������Ϣ�������׼����豸��

        exit(1);

    }



    //��Э���ַ����һ��socket

    if(bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)

    {
        printf("Server Bind Port:%d Failed!\n",PORT);

        exit(1);

    }

    //server_socket���ڼ���

    if(listen(server_socket,LISTEN)<0)

    {
        perror("Server Listen Failed!\n");

        exit(1);

    }



    //��������һֱ�������Գ���Ϊ�ͻ��˷���

    while(1)

    {
        struct sockaddr_in client_addr;
        int sin_size = sizeof(client_addr);

        //accept����һ���µ�socket,���socket������˴����ӵ���server��client����ͨ��,������client��IP��PORT��Ϣ��ͬʱ�̳�server_socket��server��IP��PORT����server_socket���������µ�����

        int new_server_socket=accept(server_socket,(struct sockaddr*)&client_addr,&sin_size);

        if(new_server_socket<0)

        {
            perror("Server Accept Failed!\n");

            break;

        }


        char buffer[buffer_size];    //�������ݻ�����

        memset(buffer,0,sizeof(buffer));   //���ѿ��ٵ��ڴ�ռ�����Ϊ��0��

        int length=recv(new_server_socket,buffer,buffer_size,0);//���մ���һ�˷��������ݣ�����copy���ֽ���

        if(length<0)

        {
            perror("Server Recieve Data Failed!\n");

            break;

        }

        char file_name[max_size+1];

        memset(file_name,0,sizeof(file_name));

        strncpy(file_name,buffer,strlen(buffer)>max_size?max_size:strlen(buffer));

        FILE*fp=fopen(file_name,"r");//ֻ��

        if(fp==NULL)

        {
            printf("File:\t%s Not Found!\n",file_name);

        }

        else

        {
            memset(buffer,0,buffer_size);

            int file_block_length=0;

            while((file_block_length=fread(buffer,sizeof(char),buffer_size,fp))>0)//��fpָ����ļ�����buffer�У�����Ԫ�ظ���

            {
                printf("file_block_length=%d\n",file_block_length);



//����buffer�е��ַ�����new_server_socket,ʵ�ʾ��Ƿ��͸��ͻ���

                if(send(new_server_socket,buffer,file_block_length,0)<0)

                {
                    printf("Send File:\t%s Failed!\n",file_name);

                    break;

                }

                memset(buffer,0,sizeof(buffer));

            }

            fclose(fp);

            printf("File:\t%s Transfer Finished!\n",file_name);

        }

        close(new_server_socket);  //�ر��ӳ���

    }

    close(server_socket);

    return 0;

}
