
typedef unsigned long long uint64_t;

extern uint64_t get_time ( void );
extern unsigned long now_usec ( void );
extern unsigned long now_hrs ( void );
extern void wait( unsigned long usecs );
extern void init_time();

#define ONE_USEC	0x1
#define ONE_MSEC	(ONE_USEC * 1000)
#define ONE_SEC		(ONE_MSEC * 1000)

