
#include "time.h"
#include "utils.h"

#define CLOCK_BASE	0x3F003000

#define	CLOCK_CTL	CLOCK_BASE+0x00
#define	CLOCK_LO	CLOCK_BASE+0x04
#define	CLOCK_HI	CLOCK_BASE+0x08
#define	CLOCK_CMP0	CLOCK_BASE+0x0c
#define	CLOCK_CMP1	CLOCK_BASE+0x10
#define	CLOCK_CMP2	CLOCK_BASE+0x14
#define	CLOCK_CMP3	CLOCK_BASE+0x18

#define	TIMER_CTL	0x40000034
#define	TIMER_IRQ	0x40000038
#define	TIMER_ROUTE	0x40000024

#define	CORE_TIMER_INT0	0x40000040
#define	CORE_TIMER_INT1	0x40000044
#define	CORE_TIMER_INT2	0x40000048
#define	CORE_TIMER_INT3	0x4000004C
#define	CORE_TIMER_NOINT	0
#define	CORE_TIMER_IRQ0	(0x1)
#define	CORE_TIMER_IRQ1	(0x1 << 1)
#define	CORE_TIMER_IRQ2	(0x1 << 2)
#define	CORE_TIMER_IRQ3	(0x1 << 3)
#define	CORE_TIMER_FIQ0	(0x1 << 4)
#define	CORE_TIMER_FIQ1	(0x1 << 5)
#define	CORE_TIMER_FIQ2	(0x1 << 6)
#define	CORE_TIMER_FIQ3	(0x1 << 7)

#define TIMER_IRQ_TO_CORE0	0

#define IRQ_CLEAR		0x80000000
#define IRQ_RELOAD		0x40000000
#define IRQ_NORELOAD	0

#define	CTL_INTERRUPT	0x20000000
#define	CTL_NOINTERRUPT	0
#define	CTL_ENABLE		0x10000000
#define	CTL_DISABLE		0
#define	CTL_TIMEMASK	0x0FFFFFFF


uint64_t
get_time( )
{
	unsigned long hi, lo;
	uint64_t thi, tlo;

	hi = GET32(CLOCK_HI);
	lo = GET32(CLOCK_LO);
	if (hi != GET32(CLOCK_HI)) {
		hi = GET32(CLOCK_HI);
		lo = GET32(CLOCK_LO);
	}

	thi = (((uint64_t)hi) << 32) & 0xFFFFFFFF00000000;
	tlo = ((uint64_t)lo) & 0x00000000FFFFFFFF;

	return thi | tlo;
}

unsigned long
now_usec () 
{
	return GET32(CLOCK_LO);
}

unsigned long
now_hrs () 
{
	return GET32(CLOCK_HI);
}

void
clear_timer_interrupts()
{
	PUT32( TIMER_CTL, CTL_NOINTERRUPT | CTL_DISABLE | (CTL_TIMEMASK - 1) );	// reset timer to the max time possible
	PUT32( TIMER_IRQ, IRQ_CLEAR | IRQ_NORELOAD );
	return;
}

void
wait( unsigned long usecs )
{
	uint64_t until = (uint64_t)usecs + get_time();

	while (get_time() < until)
	;

	return;
}

void
int_wait ( unsigned int time )
{
	unsigned int ctl_value;

	time *= 38;	// adjust for difference between main clock and countdown timer

	ctl_value = CTL_INTERRUPT | CTL_ENABLE | (time & CTL_TIMEMASK);

	PUT32( TIMER_CTL, ctl_value );

	idle();		// calls a wfi instruction

	clear_timer_interrupts();

	return;
}

#define INT_IRQ	0x0F
#define INT_FIQ	0xF0
#define INT_NONE 0

void init_time()
{

	// for now, don't do anything
	return;

	// timers & interrupts

//	PUT32( CLOCK_CTL, CLOCK_BY_1 | CLOCK_CRYSTAL);
//	PUT32( TIMER_ROUTE, TIMER_IRQ_TO_CORE0);

//	PUT32(CORE_TIMER_INT0, CORE_TIMER_NOINT);
//	PUT32(CORE_TIMER_INT1, CORE_TIMER_NOINT);
//	PUT32(CORE_TIMER_INT2, CORE_TIMER_NOINT);
//	PUT32(CORE_TIMER_INT3, CORE_TIMER_NOINT);

}

