#include <TcpServer.h>
#include <EventLoop.h>
#include <InetAddr.h>
#include <TimeStamp.h>
#include <TcpConnection.h>
#include <Logger.h>


class EchoServer {
public:
    EchoServer(EventLoop*loop, InetAddr addr):loop(loop), server(loop, addr){
        server.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        server.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    }
    void start()
    {
        server.start();
    }
    void setThreadNumber(int num)
    {
        server.setThreadNum(num);
    }
private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            LOG_INFO("connection UPUP %s", conn->peerAddr().toIpPort().c_str());
        } else {
            LOG_INFO("connection DOWN %s", conn->peerAddr().toIpPort().c_str());
        }
    }
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, TimeStamp)
    {
        std::string msg(buf->retrieveAllasString());
        conn->send(msg.c_str(), msg.size());
        conn->shutdown();
    }
    EventLoop* loop;
    TcpServer server;
};

int main()
{
    EventLoop loop;
    InetAddr addr(8000);
    EchoServer server(&loop, addr);
    server.setThreadNumber(4);
    server.start();
    loop.loop();

    return 0;
}
