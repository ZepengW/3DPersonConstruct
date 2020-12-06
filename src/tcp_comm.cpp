#include "tcp_comm.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <iostream>
#include <string.h>
#include <pthread.h>

#define SERV_PORT 9394
#define MAX_RECV_BUFFER 1000000



void* ServerListen(void* args);

void* RecvMessage(void* args);

TcpComm::TcpComm()
{
    this->sockFd=socket(AF_INET,SOCK_STREAM,0);
    if(this->sockFd<0)
    {
        std::cout<<"[ERROR] UDP Server Create Failed!"<<std::endl;
        return;
    }
    struct sockaddr_in addrServ;
    memset(&addrServ, 0, sizeof(struct sockaddr_in));
    addrServ.sin_family=AF_INET;
    addrServ.sin_port = htons(SERV_PORT);
    addrServ.sin_addr.s_addr = htonl(INADDR_ANY);
    int len = sizeof(addrServ);
    if(bind(sockFd, (struct sockaddr *)&addrServ, sizeof(addrServ)) < 0)
    {
        std::cout<<"[ERROR] UDP Server Bind Error!"<<std::endl;
        return;
    }
    listen(sockFd, 1);

    this->numConnected=0;
    pthread_rwlock_init(&this->numConnectedLOCK,NULL);
    this->sendReady=false;
    pthread_rwlock_init(&this->sendReadyLOCK,NULL);
    for(int i;i=0;i<NUM_CLIENT)
    {
        this->connFd[i]=-1;
        pthread_rwlock_init(&this->connFdLOCK[i],NULL);
        this->messReciever[i]=-1;
        pthread_rwlock_init(&this->messRecieverLOCK[i],NULL);
    }

    int ret = pthread_create(&this->listenServer,NULL,ServerListen,this);

}
TcpComm::~TcpComm()
{

}
int TcpComm::getNumConnected()
{
    pthread_rwlock_rdlock(&this->numConnectedLOCK);
    int i = this->numConnected;
    pthread_rwlock_unlock(&this->numConnectedLOCK);
    return i;
}
int TcpComm::addNumConnected(int i)
{
    pthread_rwlock_wrlock(&this->numConnectedLOCK);
    this->numConnected+=i;
    int ret = this->numConnected;
    pthread_rwlock_unlock(&this->numConnectedLOCK);
    return ret;
}
bool TcpComm::ifSendReady()
{
    pthread_rwlock_rdlock(&this->sendReadyLOCK);
    bool ret = this->sendReady;
    pthread_rwlock_unlock(&this->sendReadyLOCK);
    return ret;
}
int TcpComm::recvFunction(void* msg,int len)
{
    if(NULL==msg)
        return -1;
    if(((char*)msg)[0]=='R')
    {
        pthread_rwlock_wrlock(&this->sendReadyLOCK);
        this->sendReady=true;
        pthread_rwlock_unlock(&this->sendReadyLOCK);
    }
}
int TcpComm::sendMsg(void* msg,int len,int connFdsIdx)
{
    if(NULL==msg)
        return -1;
    pthread_rwlock_rdlock(&this->sendReadyLOCK);
    bool i = this->sendReady;
    pthread_rwlock_unlock(&this->sendReadyLOCK);
    if(i)
    {
        send(this->connFd[connFdsIdx],msg,len,0);
    }
    return -2;
    
}

int TcpComm::addConnFd(int i, int connFd)
{
    if(i<0||i>=NUM_CLIENT)
        return -1;
    pthread_rwlock_wrlock(&this->connFdLOCK[i]);
    this->connFd[i]=connFd;
    pthread_rwlock_unlock(&this->connFdLOCK[i]);
    return 0;
}
int TcpComm::getConnFd(int i)
{
    if(i<0||i>=NUM_CLIENT)
        return -1;
    pthread_rwlock_rdlock(&this->connFdLOCK[i]);
    int ret = this->connFd[i];
    pthread_rwlock_unlock(&this->connFdLOCK[i]);
    return ret;
}

pthread_t* TcpComm::getMessReciever(int i)
{
    if (i>=NUM_CLIENT)
        return NULL;
    return &this->messReciever[i];
}

void* ServerListen(void* args)
{
    TcpComm* tcpComm = (TcpComm*)args;
    int sockFd = tcpComm->sockFd;
    while(1)
    {
        struct sockaddr addrServ;
        socklen_t len = sizeof(struct sockaddr);
        int connFd = accept(sockFd,&addrServ,&len);
        if(connFd==-1)
        {
            std::cout<<"[ERROR] Client Connect Failed"<<std::endl;
            continue;
        }
        tcpComm->addNumConnected(1);

        pthread_t* recv = tcpComm->getMessReciever(0);
        tcpComm->addConnFd(0,connFd);
        int ret = pthread_create(recv,NULL,RecvMessage,tcpComm);

    }
}

void* RecvMessage(void* args)
{
    TcpComm* tcpComm=(TcpComm*)args;
    int s = tcpComm->getConnFd(0);
    while(1)
    {
        char recvData[MAX_RECV_BUFFER]="";
        int ret = recv(s,recvData,MAX_RECV_BUFFER,0);
        if (ret>0)
        {
            recvData[ret]=0x00;
            tcpComm->recvFunction(recvData,ret);
        }
    }
}

