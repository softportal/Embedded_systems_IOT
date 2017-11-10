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
do_callback()
{
  static int b = 13;
  b = b+1;
  ctimer_reset(&timer_ctimer);
  a = a+1;

  printf("P3 (callback) a = %d b = %d\n",a,b);
}

PROCESS_THREAD(p1, ev, data)
{
  static struct etimer timer_etimer;
  PROCESS_BEGIN();
  e1 = process_alloc_event();
  while(1) {
    timer_set(&timer_etimer, 4 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT();
    if (ev == PROCESS_EVENT_TIMER){
        printf("time expirado\n");
        process_post_synch(&p3,e2,NULL);
    }
    else if (ev == e1)
        printf("comunicacion de proceso p2\n");
    else
        printf("evento:%d \n", ev);
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
  ctimer_set(&timer_ctimer, 5 * CLOCK_SECOND, do_callback, NULL);
  static int b = 0;
  while(1){
      PROCESS_WAIT_EVENT();
      if (ev == e2)
          printf("comunicacion de proceso p2\n");
      else
          printf("evento:%d \n", ev);
  }

  PROCESS_END();
}



















/*---------------------------------------------------------------------------*/
