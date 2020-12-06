#include <pthread.h> 

#define NUM_CLIENT 1
class TcpComm
{
private:
    /* data */
    int connFd[NUM_CLIENT];
    pthread_rwlock_t connFdLOCK[NUM_CLIENT];
    pthread_t listenServer;
    pthread_t messReciever[NUM_CLIENT];
    pthread_rwlock_t messRecieverLOCK[NUM_CLIENT];
    int numConnected;
    pthread_rwlock_t numConnectedLOCK;
    bool sendReady;
    pthread_rwlock_t sendReadyLOCK;

    
public:
    TcpComm(/* args */);
    ~TcpComm();
    int getNumConnected();
    int addNumConnected(int i);
    bool ifSendReady();
    int sendMsg(void* msg,int len,int connFdsIdx);
    pthread_t* getMessReciever(int i);
    int recvFunction(void* msg,int len);
    int addConnFd(int i, int connFd);
    int getConnFd(int i);

    int sockFd;
};
