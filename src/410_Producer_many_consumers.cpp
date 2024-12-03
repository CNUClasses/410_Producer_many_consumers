//============================================================================
// Name        : 410_Producer_many_consumers.cpp
// Author      : 
// Version     :
// Copyright   : Steal this code!
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <iostream>

using namespace std;

mutex m;				//mutual exclusion
condition_variable cv; 	//for signaling

bool bReady = false;	//used to make consumers wait until producer says go
bool bDone = false;		//used by producer to indicate its done and gone
int gCount = 0;

void producer(int numbcounts) {
	{
		unique_lock<mutex> lck(m);
		bReady = true;		//indicate we are open for business
		cv.notify_all();
	}

	for (int j = 0; j < numbcounts; j++) {
		unique_lock<mutex> lck(m);

		//produce something
		gCount++;
		cout << "Producer gCount=" << gCount << endl;
		lck.unlock();
		cv.notify_one();	//tell consumer to consume
	}

	//one last lock to tell everyone we are done
	{
		unique_lock<mutex> lck(m);
		bDone = true;
		cout << "Producer DONE!!" << endl;
	}
	cv.notify_all();
}

void consumer(int id) {

	//first lets wait until producer is ready to go
	{
		unique_lock<mutex> lck(m);
		cout << "Consumer:" << id << " waiting" << endl;

		while (!bReady)
			cv.wait(lck);
		cout << "Consumer:" << id << " done waiting" << endl;
	}

	while (true) {
		unique_lock<mutex> lck(m);

		//if we have consumed all the widgets and the 
		//producer is done then beat it
		if (gCount == 0 && bDone == true){
			cout << "               Consumer:" << id << " EXITING!" << endl;

			break;
		}

		while (gCount == 0 && bDone == false)
			cv.wait(lck);

		//consume something
		if (gCount > 0) {
			gCount--;
			cout << "               Consumer:" << id << " gCount=" << gCount<< endl;
		} else
			cout << "               Consumer:" << id << " gCount=" << gCount
					<< ", noticed that bDone is true!" << endl;
	}
}

int main() {
	cout << "The initial value of gCount is " << gCount << endl; //

	thread t_producer(producer, 1000);
	thread t_consumer1(consumer, 1);
	thread t_consumer2(consumer, 2);
	thread t_consumer3(consumer, 3);
	thread t_consumer4(consumer, 4);
	thread t_consumer5(consumer, 5);

	t_consumer1.join();
	t_consumer2.join();
	t_consumer3.join();
	t_consumer4.join();
	t_consumer5.join();
	t_producer.join();

	cout << endl << "The final value of gCount is " << gCount << endl; //

	return 0;
}
