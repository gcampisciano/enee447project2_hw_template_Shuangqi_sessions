//
// callout table - timeout queue
//

#include "time.h"
#include "utils.h"
#include "log.h"
#include "os.h"

#include "callout.h"



#define MAX_EVENTS	256
#define MAX_WAIT	5 * ONE_SEC
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
	// get current time and return difference subtracted from head of timeoutq 
	uint64_t time = get_time() - then_usec;
	struct event *ep = LL_FIRST(timeoutq);
	uint64_t diff = ep->timeout - time;
	
	if(diff > MAX_WAIT) {
		return MAX_WAIT;
	} else {
		return diff;
	}
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
	ep->repeat_interval = repeat;
	ep->go = function;
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
	// get time from head of list
	uint64_t time = bring_timeoutq_current();
	
	// if time has expired
	if(time < 0) {
		//Remove head from list
		struct event *tmp = LL_POP(timeoutq) 
			
		// execute event
		tmp->go(tmp->data);
			
		// If repeat add to timeoutq else add to freelist
		if(tmp->repeat_interval > 0) {
			tmp->timeout = tmp->repeat_interval;
			LL_APPEND(timeoutq, tmp);
		} else {
			LL_APPEND(freelist, tmp);
		}
		return 1;
	} else {
		return 0;
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


