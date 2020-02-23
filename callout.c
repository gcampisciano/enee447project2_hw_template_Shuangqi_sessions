//
// callout table - timeout queue
//

#include "time.h"
#include "utils.h"
#include "log.h"
#include "os.h"

#include "callout.h"



#define MAX_EVENTS	256
struct event queue[ MAX_EVENTS ];

// list anchors -- important, but ignore them; they are never used directly
llobject_t TQ;
llobject_t FL;

struct event *timeoutq;
struct event *freelist;

uint64_t then_usec;

//
// sets up concept of local time
// initializes the timeoutq and freelist
// fills the freelist with entries
// timeoutq is empty
//
void
init_timeoutq()
{
	int i;

	timeoutq = (struct event *)&TQ;
	LL_INIT(timeoutq);
	freelist = (struct event *)&FL;
	LL_INIT(freelist);

	for (i=0; i<MAX_EVENTS; i++) {
		struct event *ep = &queue[i];
		LL_PUSH(freelist, ep);
	}

	then_usec = get_time();

	return;
}


//
// account for however much time has elapsed since last update
// return timeout or MAX
//
// note: then_usec matches the most recent update to the TQ head entry
//
int
bring_timeoutq_current()
{
	// get current time and return difference from then_usec 
	uint64_t now_time = get_time();
	return now_time - then_usec;
}



//
// get a new event structure from the freelist,
// initialize it, and insert it into the timeoutq
// 
void
create_timeoutq_event(int timeout, int repeat, pfv_t function, namenum_t data)
{
	struct event *ep = LL_POP(freelist);
	// initialize ep
	ep->timeout = timeout;
	ep->repeat = repeat;
	ep->pfv_t = function;
	ep->data = data;
	// add event to end of the list
	LL_APPEND(timeoutq, ep);
}



//
// bring the time current
// check the list to see if anything expired
// if so, do it - and either return it to the freelist 
// or put it back into the timeoutq
//
// return whether or not you handled anything
//
int
handle_timeoutq_event( )
{
	//get current time
	uint64 time = bring_timeoutq_current();
	
	//iterate through the list and check if any events have expired
	struct event *ep;
	LL_EACH(timeoutq, ep, struct event) {
		// if expired, remove from list, execute and put in freelist
		if(time > ep->timeout) { 
			struct event *tmp = LL_DETACH(timeoutq, ep);
			tmp->pfv_t(tmp->data);
			LL_APPEND(freelist, tmp);
		}
	}
}

// for debugging
pfv_t
tq_gofunc()
{
    struct event *ep;
    ep = (struct event *)LL_TOP(timeoutq);
    if (!ep) {
        panic(ERRNO_ASSERT, "tq_gofunc returning NULL pointer? ...");
    }
    return ep->go;
}


