/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 */

#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include <stdio.h> /* For printf() */


int a = 3;
static struct ctimer timer_ctimer;
static process_event_t e1;
static process_event_t e2;

PROCESS(p1, "Proc1");
//PROCESS(p2, "Proc2");
PROCESS(p3, "Proc3");
AUTOSTART_PROCESSES(&p1, &p3);
/*---------------------------------------------------------*/
/*---------------------------------------------------------*/
void
do_callback() //timer p3 expired
{
  ctimer_reset(&timer_ctimer);
  process_post(&p1,e1,NULL); //async to p1
  printf("(p3) (callback)\n");
}

PROCESS_THREAD(p1, ev, data)
{
  static struct etimer timer_etimer;
  static int b = 0;
  PROCESS_BEGIN();
  e1 = process_alloc_event();
  etimer_set(&timer_etimer, 4 * CLOCK_SECOND); //timer 4 seconds deadline
  while(1) {
      PROCESS_WAIT_EVENT(); //waits 4 events
      if (ev == PROCESS_EVENT_TIMER){ //timer expired
          etimer_reset(&timer_etimer);
          printf("(p1)timer expirad\n");
          process_post_synch(&p3,e2,NULL); //sync t0 p3
      }
      else if (ev == e1){
          b+=1;
          printf("(p1)async call from p3 cont: %d\n", b);
      }
      else
          printf("(p1)event:%d \n", ev);
  }
  PROCESS_END();
}

//PROCESS_THREAD(p2, ev, data)
//{
//  static struct etimer timer_etimer2;
//  PROCESS_BEGIN();
//  etimer_set(&timer_etimer2, 1 * CLOCK_SECOND);
//  PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
//   for (int i = 0; i<5;i++) {
//      printf("P2 Envio evento e1 sincrono\n");
//      process_post_synch(&p1,e1,NULL);
//      //printf("P2 Envio evento e1 asincrono\n");
//      //process_post(&p3,e1,NULL);
//      printf("P2 enviado\n");
//  }
//  PROCESS_END();
//}

PROCESS_THREAD(p3, ev, data)
{
  PROCESS_BEGIN();
  e2 = process_alloc_event();
  ctimer_set(&timer_ctimer, 5 * CLOCK_SECOND, do_callback, NULL); //callback timer 5 seconds deadline
  static int b = 0;
  while(1){
      PROCESS_WAIT_EVENT(); //wait 4 events
      if (ev == e2){
          b+=1;
          printf("(p3) sync call from p1 cont: %d\n", b);
      }
      else
          printf("(p3)event:%d \n", ev);
  }

  PROCESS_END();
}



















/*---------------------------------------------------------------------------*/
