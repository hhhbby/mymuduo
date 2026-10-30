#include <functional>
#include <thread>
#include <string>
#include <memory>

class Thread {
public:
    using ThreadFunc = std::function<void()>;
    Thread(ThreadFunc, const std::string& = std::string());
    ~Thread();
    void start();
    void join();
    int tid() { return tid_; }
    static int numCreated() { return numCreated_; }
private:
    void setDefaultName();

    std::string name_;
    ThreadFunc func_;
    int tid_;
    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;

    static int numCreated_;
};