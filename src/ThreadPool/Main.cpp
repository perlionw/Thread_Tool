#include "threadpool.h"
#include <iostream>
#include <windows.h>
#include <string>


void fun1(int slp)
{
	printf("  hello, fun1 !  %d\n" ,std::this_thread::get_id());
	if (slp>0) {
		printf(" ======= fun1 sleep %d  =========  %d\n",slp, std::this_thread::get_id());
		std::this_thread::sleep_for(std::chrono::milliseconds(slp));
		//Sleep(slp );
	}
}

struct gfun {
	int operator()(int n) {
		printf("%d  hello, gfun !  %d\n" ,n, std::this_thread::get_id() );
		return 42;
	}
};

struct Test
{
	int num;
	std::string s;
};

class A {    //函数必须是 static 的才能使用线程池
public:
	static int Afun(int n = 0) {
		std::cout << n << "  hello, Afun !  " << std::this_thread::get_id() << std::endl;
		return n;
	}

	int CFun(int n = 0) {
		std::cout << n << "  hello, Afun !  " << std::this_thread::get_id() << std::endl;
		return n;
	}

	static std::string Bfun(int n, std::string str, char c) {
		std::cout << n << "  hello, Bfun !  "<< str.c_str() <<"  " << (int)c <<"  " << std::this_thread::get_id() << std::endl;
		return str;
	}
};

void get_result(std::future<Test*>& fut)
{
	while (fut.wait_for(std::chrono::milliseconds(20)) == std::future_status::timeout)
	{
		std::cout << ".";
	}

	std::cout << std::endl;
	std::cout << "the Afun result is " << fut.get()->s << fut.get()->num << std::endl;
}

std::string fetchDataFromDB(std::string recvData)
{
	std::cout << "fetchDataFromDbstart" << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return "DB_" + recvData;
}

void get_res(std::future<bool>& fut)
{
	fut.wait();
	bool flag = fut.get();
	while (flag)
	{
		std::cout << "fafs" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void unlockthread(std::promise<bool>& pr)
{
	std::this_thread::sleep_for(std::chrono::seconds(3));
	pr.set_value(true);
}

void test01()
{

	std::promise<bool> pr;

	auto f = pr.get_future();

	std::thread th1(get_res, std::ref(f));

	std::thread th2(unlockthread, std::ref(pr));
	th2.detach();
	th1.join();

	//Test* test = new Test;
	//test->num = 8;
	//test->s = "fas";
	//std::packaged_task<Test*(Test*)> t(A::Afun);
	//auto fut = t.get_future();
	//std::thread t1(std::move(t), test);

	//std::thread t2(get_result, std::ref(fut));
	//t1.join();
	//t2.join();

	auto fut = std::async(std::launch::async, fetchDataFromDB, "Data");

	std::thread t(get_res, std::ref(f));
	t.join();
	//std::future_status status;
	//fut.wait();
	//do 
	//{
	//	status = fut.wait_for(std::chrono::seconds(1));

	//	switch (status)
	//	{
	//		case std::future_status::ready:
	//			std::cout << "Ready..." << std::endl;
	//			//获取结果
	//			std::cout << fut.get() << std::endl;
	//			break;
	//		case std::future_status::timeout:
	//			std::cout << "timeout..." << std::endl;
	//			break;
	//		case std::future_status::deferred:
	//			std::cout << "deferred..." << std::endl;
	//			break;
	//		default:
	//			break;
	//	}

	//} while (status != std::future_status::ready);



}

int __stdcall consol_crtl_handler(unsigned long CtrlType)
{
	switch (CtrlType) {
		case CTRL_C_EVENT:

			break;
		default:
			break;
	}
	return 1;
}

int main()
	try {

	SetConsoleCtrlHandler(consol_crtl_handler, 1);
		std::threadpool executor{ 50 };
		A a;
		auto fff = std::bind(&A::CFun, &a, 4);
		std::future<int> f = executor.commit(fff);
		//std::future<void> ff = executor.commit(fun1, 0);
		//std::future<int> fg = executor.commit(gfun{}, 0);
		//std::future<int> gg = executor.commit(a.Afun, 9999); //IDE提示错误,但可以编译运行

		//std::future<std::string> gh = executor.commit(A::Bfun, 9998,"mult args", 123);
		//std::future<std::string> fh = executor.commit([]()->std::string { std::cout << "hello, fh !  " << std::this_thread::get_id() << std::endl; return "hello,fh ret !"; });

		//std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
		//std::this_thread::sleep_for(std::chrono::microseconds(900));

		//for (int i = 0; i < 50; i++) {
		//	executor.commit(fun1,i*100 );
		//}
		//std::cout << " =======  commit all ========= " << std::this_thread::get_id()<< " idlsize="<<executor.idlCount() << std::endl;

		//std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
		//std::this_thread::sleep_for(std::chrono::seconds(3));

		//ff.get(); //调用.get()获取返回值会等待线程执行完,获取返回值
		//std::cout << fg.get() << "  " << fh.get().c_str()<< "  " << std::this_thread::get_id() << std::endl;

		//std::cout << " =======  sleep ========= " << std::this_thread::get_id() << std::endl;
		//std::this_thread::sleep_for(std::chrono::seconds(3));

		//std::cout << " =======  fun1,55 ========= " << std::this_thread::get_id() << std::endl;
		//executor.commit(fun1,55).get();    //调用.get()获取返回值会等待线程执行完

		//std::cout << "end... " << std::this_thread::get_id() << std::endl;


		//std::threadpool pool(4);
		//std::vector< std::future<int> > results;

		//for (int i = 0; i < 8; ++i) {
		//	results.emplace_back(
		//		pool.commit([i] {
		//			std::cout << "hello " << i << std::endl;
		//			std::this_thread::sleep_for(std::chrono::seconds(1));
		//			std::cout << "world " << i << std::endl;
		//			return i*i;
		//		})
		//	);
		//}
		//std::cout << " =======  commit all2 ========= " << std::this_thread::get_id() << std::endl;

		//for (auto && result : results)
		//	std::cout << result.get() << ' ';
		//std::cout << std::endl;
		system("pause");
		return 0;
	}
catch (std::exception& e) {
	std::cout << "some unhappy happened...  " << std::this_thread::get_id() << e.what() << std::endl;
}
