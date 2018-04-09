


#include <iostream>
#include <condition_variable>
#include <thread>
#include <chrono>

std::condition_variable cv;
std::mutex cv_m;
int i = 0;
bool done = false;

void waits()
{
	std::unique_lock<std::mutex> lk(cv_m);
	std::cout << "Waiting... \n";

	//调用wait，释放了互斥量cv_m，并等待通知
	//notify_one通知，获得互斥量cv_m，i!=1，重新等待，这时释放锁，signals线程才能得到互斥量cv_m
	cv.wait(lk, [] {return i == 1; });	

	std::cout << "...finished waiting. i == 1\n";
	done = true;
}

void signals()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "Notifying falsely...\n";
	cv.notify_one(); 

	std::unique_lock<std::mutex> lk(cv_m);
	i = 1;
	while (!done)
	{
		std::cout << "Notifying true change...\n";
		lk.unlock();
		cv.notify_one();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		lk.lock();
	}
}

int main()
{
	std::thread t1(waits), t2(signals);
	t1.join();
	t2.join();

	system("pause");
}