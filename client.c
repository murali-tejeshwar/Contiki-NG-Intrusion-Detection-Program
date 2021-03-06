#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contiki.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/adxl345.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"

/* Declare our "main" process, the client process*/
PROCESS(client_process, "Clicker client");
/* The client process should be started automatically when
 * the node has booted. */
AUTOSTART_PROCESSES(&client_process);

/* Callback function for received packets.
 *
 * Whenever this node receives a packet for its broadcast handle,
 * this function will be called.
 *
 * As the client does not need to receive, the function does not do anything
 */
static void recv(const void *data, uint16_t len, const linkaddr_t * src, const linkaddr_t * dest)
{

}

/* Our main process. */
PROCESS_THREAD(client_process, ev, data)
{
	static char payload[] = "alarm";
	static struct etimer et;
	static int8_t error = 10;
	static int16_t x_prev = 0, y_prev = 0, z_prev = 0;
	static int16_t x_curr, y_curr, z_curr;

	PROCESS_BEGIN();

	accm_init();
	
	x_prev = accm_read_axis(X_AXIS);
        y_prev = accm_read_axis(Y_AXIS);
        z_prev = accm_read_axis(Z_AXIS);

	/* Activate the button sensor */
	SENSORS_ACTIVATE(button_sensor);

	/* Initialize NullNet */
	nullnet_buf = (uint8_t *) & payload;
	nullnet_len = sizeof(payload);
	nullnet_set_input_callback(recv);
		
	/* Loop forever */
	while (1) {
		etimer_set(&et, CLOCK_SECOND / 100);
		/* Wait until an event occurs. If the event has
		 * occured, ev will hold the type of event, and
		 * data will have additional information for the
		 * event. In the case of a sensors_event, data will
		 * point to the sensor that caused the event.
		 * Here we wait until the button was pressed. */
		PROCESS_WAIT_EVENT_UNTIL((ev == sensors_event && data == &button_sensor) || etimer_expired(&et));

		x_curr = accm_read_axis(X_AXIS);
                y_curr = accm_read_axis(Y_AXIS);
                z_curr = accm_read_axis(Z_AXIS);

		if ((abs(x_curr - x_prev) > error) || (abs(y_curr - y_prev) > error) || (abs(z_curr - z_prev) > error)) {
			leds_toggle(LEDS_RED);

			/* Copy the string "alarm" into the packet buffer. */
			memcpy(nullnet_buf, &payload, sizeof(payload));
			nullnet_len = sizeof(payload);

			/* Send the content of the packet buffer using the
			 * broadcast handle. */
			NETSTACK_NETWORK.output(NULL);

			x_prev = x_curr;
			y_prev = y_curr;
			z_prev = z_curr;
		}
	}

	PROCESS_END();
}
