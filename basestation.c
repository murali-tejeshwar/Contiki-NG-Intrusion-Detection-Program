#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"

/* LED process */
PROCESS(led_process, "Led process");

/* Declare our "main" process, the basestation_process */
PROCESS(basestation_process, "Clicker basestation");
/* The basestation process should be started automatically when
 * the node has booted. */
AUTOSTART_PROCESSES(&basestation_process, &led_process);

/* Holds the number of packets received. */
static int count = 0;
static struct etimer et;

/* Callback function for received packets.
 *
 * Whenever this node receives a packet for its broadcast handle,
 * this function will be called.
 *
 * As the client does not need to receive, the function does not do anything
 */
static void recv(const void *data, uint16_t len, const linkaddr_t * src, const linkaddr_t * dest)
{
	count++;

	/* 0bxxxxx allows us to write binary values */
	/* for example, 0b10 is 2 */
	leds_off(LEDS_ALL);
	leds_on(count & 0b1111);
	
	process_poll(&led_process);
}

PROCESS_THREAD(led_process, ev, data)
{
	PROCESS_BEGIN();

	while (1) {
		etimer_set(&et, CLOCK_SECOND * 10);

		/* wait until either the event poll occurs or the above timer expires */
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL || etimer_expired(&et));

		/* if the timer expired, then switch-off all the LEDs */
		if (etimer_expired(&et))
			leds_off(LEDS_ALL);
		else
			etimer_stop(&et);
	}

	PROCESS_END();
}

/* Our main process. */
PROCESS_THREAD(basestation_process, ev, data)
{
	PROCESS_BEGIN();

	/* Initialize NullNet */
	nullnet_set_input_callback(recv);

	PROCESS_END();
}
