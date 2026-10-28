#include <memory>
#include <functional>

// Forward Declaration（前向声明）允许在函数声明中使用尚未完整定义的类，即使参数按值传递也可以
class TimeStamp;
class EventLoop;

class Channel {
public:
    using readEventCallback = std::function<void(TimeStamp)>;
    using eventCallback = std::function<void()>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= !kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= !kWriteEvent; update(); }

    void handleEvent(TimeStamp receiveTime);
    void handleEventWithGuard(TimeStamp receiveTime);

    void setReadCallback(readEventCallback cb) { readCallback_ = cb; }
    void setWriteCallback(eventCallback cb) { writeCallback_ = cb; }
    void setCloseCallback(eventCallback cb) { closeCallback_ = cb; }
    void setErrorCallback(eventCallback cb) { errorCallback_ = cb; }
    void remote();
private:
    void update();
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    int fd_;
    int events_;
    int revents_;
    
    bool tied_;
    std::shared_ptr<void> tie_;

    readEventCallback readCallback_;
    eventCallback writeCallback_;
    eventCallback errorCallback_;
    eventCallback closeCallback_;
};