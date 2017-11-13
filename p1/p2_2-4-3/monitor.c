/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 */

#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include <stdio.h> /* For printf() */


static struct ctimer timer_ctimer;
static process_event_t e1;
static process_event_t e2;

PROCESS(p1, "Proc1");
PROCESS(p3, "Proc3");
AUTOSTART_PROCESSES(&p1, &p3);
/*---------------------------------------------------------*/
/*---------------------------------------------------------*/
void
do_callback() //timer p3 expired
{
  ctimer_reset(&timer_ctimer);
  process_post(&p1,e1,NULL); //async to p1
}

PROCESS_THREAD(p1, ev, data)
{
  static struct etimer timer_etimer;
  PROCESS_BEGIN();
  e1 = process_alloc_event();
  etimer_set(&timer_etimer, 4 * CLOCK_SECOND); //timer 4 seconds deadline
  while(1) {
      PROCESS_WAIT_EVENT(); //waits 4 events
      if (ev == PROCESS_EVENT_TIMER){ //timer expired
          etimer_reset(&timer_etimer);
          process_post_synch(&p3,e2,NULL); //sync t0 p3
      }
      else if (ev == e1){
          //code
      }
      else
  }
  PROCESS_END();
}

PROCESS_THREAD(p3, ev, data)
{
  PROCESS_BEGIN();
  e2 = process_alloc_event();
  ctimer_set(&timer_ctimer, 5 * CLOCK_SECOND, do_callback, NULL); //callback timer 5 seconds deadline
  while(1){
      PROCESS_WAIT_EVENT(); //wait 4 events
      if (ev == e2){
      }
      else
  }

  PROCESS_END();
}



















/*---------------------------------------------------------------------------*/
