//
// timeout queue example
//

#include "utils.h"
#include "time.h"
#include "led.h"
#include "uart.h"
#include "log.h"


#include "os.h"
#include "callout.h"
#include "events.h"




void
notmain()
{
	namenum_t data;
    int timeout;

    // init functions
	init_led();
    init_uart();
    init_time();
    init_errno();
    init_timeoutq();

	blink_led_stall(5);

    // wake up
    uart_puts("\n");
    uart_puts("\n");
	log("...", NOVAL);
    log("System is booting, cpuid = ", cpu_id());
    log("Please hit enter to continue.\n", NOVAL);

    uart_recv();

	void * ptr = GETPC();
	uart_put32x(sizeof(ptr)); uart_puts("\n");
	uart_put32x((unsigned long)ptr); uart_puts("\n");

    // create some timeout events
	data.num = 3;
	bring_timeoutq_current();
    create_timeoutq_event( 2 * ONE_SEC, 4 * ONE_SEC, do_hex, data );
	data.num = 20;
	bring_timeoutq_current();
    create_timeoutq_event( 3 * ONE_SEC, 4 * ONE_SEC, do_blink, data );
	data.num = 0xabcde123;
	bring_timeoutq_current();
    create_timeoutq_event( 4 * ONE_SEC, 4 * ONE_SEC, do_hex, data );
	data.name[0] = 'k';
	data.name[1] = 'e';
	data.name[2] = 'r';
	data.name[3] = 'n';
	bring_timeoutq_current();
    create_timeoutq_event( 5 * ONE_SEC, 4 * ONE_SEC, do_string, data);

    uart_puts("TQ items created; running loop.\n");
    uart_puts("\n");

    // run the list
	extern pfv_t tq_gofunc();
    while (1) {
		debug(DEBUG_LOW, "top of loop eventpc = ", (unsigned long)tq_gofunc());
		if (handle_timeoutq_event()) {
			debug(DEBUG_LOW, "handled event.", 0);
			continue;
		}
		timeout = bring_timeoutq_current();
		debug(DEBUG_LOW, "no event. about to wait for = ", (unsigned long)timeout);
		if (DEBUG_LEVEL >= DEBUG_LOW) {
			data.num = (unsigned long)get_time();	// since we're not using data anymore
		}
		wait(timeout);
		debug(DEBUG_LOW, "done waiting = ", get_time() - (uint64_t)data.num);
    }

	return;

}
