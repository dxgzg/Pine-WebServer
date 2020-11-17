#include <unordered_map>
#include <list>
#include <memory>
using std::shared_ptr;
using std::list;
using std::unordered_map;
constexpr int TIME = 5;//测试的时候改小一点即可

struct initial_timer{
    initial_timer() = default;
	//两个必备的材料
	int connfd = 0;
	time_t expire = 0;//到期时间
};
//设置成单例模式里的饿汉模式
class Timer{
private:
    static Timer* t_out;
private:
	using sp_init_tm = shared_ptr<initial_timer>;//智能指针重命名
private:
	list<sp_init_tm> l;//封装成智能指针
	unordered_map<int,list<sp_init_tm>::iterator> um;//通过sock来删除他所在的位置
	int epollfd = 0;
private:
	void settime(sp_init_tm ptr){
        
		time_t cur = time(nullptr);
		ptr->expire = cur + TIME;//TIMEs后超时
	}
private:
   
    //移动和拷贝构造函数都定义成
  //  Timer(const Timer&) = delete;
  //  Timer(Timer&& ) = delete;
  	Timer() = default;
public:
	
    static Timer* get_Timer();
    void del_Timer();
	void add_time_obj(int connfd);
    void adjust_obj(int connfd);
    //超时、正常关闭都需要调用这个函数
    void del_time_obj(int event_base,int connfd);
    void tick(int event_base);
};
