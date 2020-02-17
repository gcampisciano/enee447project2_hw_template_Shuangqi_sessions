/*
 * os.h
 */

//
// some useful types
//
typedef void (* pfv_t)();
typedef int (* pfi_t)();
typedef unsigned int (* pfu_t)();

typedef union namenum {
	char	name[4];
	short	half[2];
	long	num;
	void	*ptr;
	float	fnum;
} namenum_t;

#define NULL 0

//
// TIME values - caveats:
// a. this assumes you are using the 900MHz-driven clock-timer as "now"
// b. it is ridiculously inaccurate (only approximately == 1 usec, 1 msec, 1 sec)
// c. but, hey, it works for now - one can always trim it up to be more accurate later
//    (e.g., set these values to *precisely* the number of clock-timer cycles that
//    bring you to the desired time ... I just didn't have the *time* (haha get it?) 
//    to do all that work)
//
#define	MICROSECONDS(u)	(u * ONE_USEC)
#define	MILLISEC(m)		(m * ONE_MSEC)
#define	SECONDS(s)		(s * ONE_SEC)		// note we can only go up to 2000 seconds

//
// some system parameters
//
#define NUM_CORES	4
#define NUM_THREADS	16
#define MAX_SLEEP_INTERVAL	ONE_MSEC



