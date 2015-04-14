#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <assert.h>

#define IO_MAN 0
#define IO_NET 1
#define PANEL_CENTER 20000
#define PANEL_HALF_RANGE 19800
#define PANEL_RIGHT PANEL_CENTER+PANEL_HALF_RANGE
#define PANEL_LEFT  PANEL_CENTER-PANEL_HALF_RANGE
#define SVR_PORT 6666
#define SVR_IP  "192.168.1.108"

struct sockaddr_in svr_addr;
int client_socket_fd;
int connect_to_server(void);
int main(int argc,char*argv[])
{
    int fd=open("/dev/io_control",O_RDWR);
    if(fd<0)
    {
        printf("Open file error.\n");
        return 0;
    }
    int work_mode=IO_MAN;
    int link_status=connect_to_server();
    if(0==link_status)
    {
        work_mode=IO_NET;
    }
    fd_set r_fds;
    fd_set w_fds;
    struct timeval timeout;
    timeout.tv_sec=0;
    timeout.tv_usec=100000;
    int panel=20000;
    while(1)
    {
        FD_ZERO(&r_fds);
        FD_ZERO(&w_fds);
        FD_SET(fd,&r_fds);
        FD_SET(fd,&w_fds);
        switch(select(fd+1,&r_fds,&w_fds,NULL,0))
        {
            case -1:exit(-1);break;
            case 0:break;
            default:
                if(FD_ISSET(fd,&r_fds))
                    work_mode=IO_MAN;
                else
                    work_mode=IO_NET;
                break;
        }
        if(work_mode==IO_NET && link_status!=0)
        {
            link_status-connect_to_server();
            if(0==link_status)
            {
                work_mode=IO_NET;
            }
        }

        char r_buf[2];
        char w_buf=0;
        char val=80;
        if(work_mode==IO_MAN)
        {
            read(fd,&r_buf,sizeof(r_buf));
        }
        else
        {   
            int len=sizeof(svr_addr);
            char trig[1];
            trig[0]='w';
            sendto(client_socket_fd,trig,1,0,(struct sockaddr*)&svr_addr,len); 
            FD_ZERO(&r_fds);
            FD_SET(client_socket_fd,&r_fds);
            if(select(client_socket_fd+1,&r_fds,NULL,NULL,&timeout)>0)
            {
                recvfrom(client_socket_fd,r_buf,sizeof(r_buf),0,(struct sockaddr*)&svr_addr,&len);
                val=r_buf[1];
            }
        }
        if(r_buf[0]=='r')
        {
            if(panel<PANEL_RIGHT)  
            {
                panel+=val;
                w_buf=val;
            }
        }
        else if(r_buf[0]=='l')
        {
            if(panel>PANEL_LEFT)  
            {
                panel-=val;
                w_buf=128+val;
            }
        }
        write(fd,&w_buf,1);
    }
}
int connect_to_server(void)
{
    bzero(&svr_addr,sizeof(svr_addr));
    svr_addr.sin_family=AF_INET;
    svr_addr.sin_addr.s_addr=inet_addr(SVR_IP);
    svr_addr.sin_port=htons(SVR_PORT);
    
    client_socket_fd=socket(AF_INET,SOCK_DGRAM,0);
    if(client_socket_fd < 0)
    {
        printf("Create Socket Failed!\n");
        return -1;
    }
    
    return 0;
}
