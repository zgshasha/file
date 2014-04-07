#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/resource.h>
#include <netinet/in.h>

#define PORT 5552  //宏定义



#define LISTEN 20

#define buffer_size 1024

#define max_size 512



int main(int argc,char**argv)

{
    //设置一个socket地址结构server_addr,代表服务器internet的地址和端口

    struct sockaddr_in server_addr;

    bzero(&server_addr,sizeof(server_addr));//清零

    server_addr.sin_family=AF_INET;

    //server_addr.sin_addr.s_addr=htons(INADDR_ANY);    //任意地址
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port=htons(PORT);



    //创建用于internet的流协议（TCP）socket,用server_socket代表服务器向客户端提供服务的接口

    int server_socket=socket(AF_INET,SOCK_STREAM,0);

    if(server_socket<0)

    {
        perror("Create Socket Failed!\n");//将错误信息输出到标准输出设备上

        exit(1);

    }



    //把协议地址赋予一个socket

    if(bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)

    {
        printf("Server Bind Port:%d Failed!\n",PORT);

        exit(1);

    }

    //server_socket用于监听

    if(listen(server_socket,LISTEN)<0)

    {
        perror("Server Listen Failed!\n");

        exit(1);

    }



    //服务器端一直运行用以持续为客户端服务

    while(1)

    {
        struct sockaddr_in client_addr;
        int sin_size = sizeof(client_addr);

        //accept返回一个新的socket,这个socket用来与此次连接到的server的client进行通信,它包含client的IP和PORT信息，同时继承server_socket中server的IP和PORT，而server_socket继续监听新的请求。

        int new_server_socket=accept(server_socket,(struct sockaddr*)&client_addr,&sin_size);

        if(new_server_socket<0)

        {
            perror("Server Accept Failed!\n");

            break;

        }


        char buffer[buffer_size];    //接收数据缓冲区

        memset(buffer,0,sizeof(buffer));   //将已开辟的内存空间设置为“0”

        int length=recv(new_server_socket,buffer,buffer_size,0);//接收从另一端发来的数据，返回copy的字节数

        if(length<0)

        {
            perror("Server Recieve Data Failed!\n");

            break;

        }

        char file_name[max_size+1];

        memset(file_name,0,sizeof(file_name));

        strncpy(file_name,buffer,strlen(buffer)>max_size?max_size:strlen(buffer));

        FILE*fp=fopen(file_name,"r");//只读

        if(fp==NULL)

        {
            printf("File:\t%s Not Found!\n",file_name);

        }

        else

        {
            memset(buffer,0,buffer_size);

            int file_block_length=0;

            while((file_block_length=fread(buffer,sizeof(char),buffer_size,fp))>0)//将fp指向的文件读到buffer中，返回元素个数

            {
                printf("file_block_length=%d\n",file_block_length);



//发送buffer中的字符串到new_server_socket,实际就是发送给客户端

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

        close(new_server_socket);  //关闭子程序

    }

    close(server_socket);

    return 0;

}
