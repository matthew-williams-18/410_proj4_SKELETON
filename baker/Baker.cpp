#include <mutex>

#include "../includes/baker.h"
#include "../includes/externs.h"
#include "../includes/PRINT.h"
using namespace std;

Baker::Baker(int id):id(id)
{
}

Baker::~Baker()
{
}

//bake, box and append to anOrder.boxes vector
//if order has 13 donuts there should be 2 boxes
//1 with 12 donuts, 1 with 1 donut
void Baker::bake_and_box(ORDER &anOrder) {
	PRINT2("Entered bake_and_box function in baker ",id);
	int numBoxes = anOrder.number_donuts / DOZEN + (anOrder.number_donuts % DOZEN == 0 ? 0 : 1); //figure out how many boxes we need
	int donutsMade = 0; //track the gross number of donuts made across all boxes

	for( int i = 0; i < numBoxes; i++){ //will run (number of full boxes) times, plus once more if any more donuts remain to made in the order (if anOrder.number_donuts % DOZEN != 0)
		Box box;
		DONUT donut; //I suppose we're making donuts that are copies of one another at the quantum level if we do it this way. I guess they'll taste equally good
		while(box.addDonut(donut)){ //if box not full, make and add a donut; if full, exit while loop
			if(++donutsMade == anOrder.number_donuts){ break; } //increment donutsMade counter, and then check if we've fulfilled the order yet;
																//if so, exit while loop, at which point the for loop should be out of iterations and exit
		}
		anOrder.boxes.push_back(box);
	}
	PRINT2("End of bake_and_box function reached in baker",id);
}


//as long as there are orders in order_in_Q then
//for each order:
//	create box(es) filled with number of donuts in the order
//  then place finished order on order_outvector
//  if waiter is finished (b_WaiterIsFinished) then
//  finish up remaining orders in order_in_Q and exit
//
//You will use cv_order_inQ to be notified by waiter
//when either order_in_Q.size() > 0 or b_WaiterIsFinished == true
//hint: wait for something to be in order_in_Q or b_WaiterIsFinished == true
void Baker::beBaker() {
	unique_lock<mutex> lk(mutex_order_inQ, defer_lock);
	PRINT2("In baker ",this->id);
	cv_order_inQ.wait(lk);
	PRINT2("After wait in baker ",this->id);

	while(!b_WaiterIsFinished || !order_in_Q.empty()){
		PRINT2("Top of while loop in baker ", id);
		ORDER *currentOrder = NULL;
		std::unique_lock<std::mutex> in_uLock(mutex_order_inQ);
//		in_uLock.lock();
		PRINT2("After unique lock acquired in baker ",id);
		if(!order_in_Q.empty()){
			PRINT2("Inside first conditional in baker",id);
			currentOrder = &order_in_Q.front();
			order_in_Q.pop();
			PRINT2("End of first conditional in baker",id);
		}
		PRINT2("Gonna unlock u_lock in baker",id);
		in_uLock.unlock();
		PRINT2("Unlocked u_lock in baker",id);

		if(currentOrder != NULL){ //only do if not null
			this->bake_and_box(*currentOrder);

			std::unique_lock<std::mutex> out_uLock(mutex_order_outQ);
			order_out_Vector.push_back(*currentOrder);
			out_uLock.unlock();
		}
	}
}






