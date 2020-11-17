#include<vector>
#include<thread>
#include<memory>
class Safe_queue;
class Thread_pool
{
private:
    std::shared_ptr<Safe_queue> sf_que;
private:
    int thread_num = std::thread::hardware_concurrency();
    std::vector<std::thread> thread_pool;
    bool thread_run_flag = true;
public:
    Thread_pool(std::shared_ptr<Safe_queue>);
    Thread_pool(const Thread_pool&) = delete;
    void worker();
   // void stop();
    ~Thread_pool() = default;
};

