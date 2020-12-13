#include <string>
#include "stdlib.h"

#include "../includes/waiter.h"
#include "../includes/externs.h"
#include "../includes/PRINT.h"

using namespace std;

Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){
}

Waiter::~Waiter()
{
}

//gets next Order from file_IO
//if return == SUCCESS then anOrder
//contains new order
//otherwise return contains fileIO error
int Waiter::getNext(ORDER &anOrder){
	return myIO.getNext(anOrder);
}


//contains a loop that will get orders from filename one at a time
//then puts them in order_in_Q then signals baker(s) using cv_order_inQ
//so they can be consumed by baker(s)
//when finished exits loop and signals baker(s) using cv_order_inQ that
//it is done using b_WaiterIsFinished
void Waiter::beWaiter() {
	PRINT1("entered waiter");
	bool notify = true;
	while(true){
		ORDER currentOrder;

		if(this->getNext(currentOrder) != SUCCESS){
			break;
		}
		PRINT1("Order successfully entered to the waiter");


		unique_lock<mutex> in_uLock(mutex_order_inQ);
		order_in_Q.push(currentOrder);
//		PRINT1("Order pushed to the queue in waiter");
		in_uLock.unlock();
		if(notify){
			PRINT1("Waiter unlocked in-queue mutex");
			PRINT1("Waiter notifies all");
			cv_order_inQ.notify_all();
			PRINT1("Waiter has notified all");
			notify = false;
		}

	}
	PRINT1("Waiter is done");
	b_WaiterIsFinished = true;
//	cv_order_inQ.notify_all();
}






