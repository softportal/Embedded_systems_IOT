/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 */

#include "contiki.h"

#include "sys/etimer.h"
#include "sys/ctimer.h"

#include "dev/leds.h"
#include "dev/watchdog.h"

#include "button-sensor.h"
#include "batmon-sensor.h"
#include "board-peripherals.h"

#include "rf-core/rf-ble.h"
#include "ti-lib.h"
#include "random.h"

#include <stdio.h> /* For printf() */
#include <stdint.h>



/*---------------------------------------------------------------------------*/
#define LOOP_INTERVAL       (CLOCK_SECOND * 20)
#define LEDS_PERIODIC       LEDS_YELLOW
#define LEDS_BUTTON         LEDS_RED
#define LEDS_REBOOT         LEDS_ALL
/*---------------------------------------------------------------------------*/
#define SENSOR_NONE         (void *)0xFFFFFFFF

#define SENSOR_1     &button_left_sensor
#define SENSOR_2     &button_right_sensor
#define SENSOR_5     &reed_relay_sensor

#define ACC_X 0
#define ACC_Y 1
#define ACC_Z 2

static struct etimer et;

PROCESS(p1, "Proc1");
PROCESS(p3, "Proc3");
AUTOSTART_PROCESSES(&p1, &p3);

#define SENSOR_READING_PERIOD (CLOCK_SECOND * 20)
#define SENSOR_READING_RANDOM (CLOCK_SECOND << 4)

/*---------------------------------------------------------*/
static process_event_t e1;
static process_event_t e2;

static struct ctimer mpu_timer;
static struct ctimer timer_ctimer;

static void init_mpu_reading(void *not_used);
/*---------------------------------------------------------*/
/*---------------------------------------------------------*/
static void print_mpu_reading(int reading)
{
  if(reading < 0) {
    printf("-");
    reading = -reading;
  }

  printf("%d.%02d", reading / 100, reading % 100);
}

static void get_mpu_reading()
{
  int value;
  static int data[3];

  //printf("MPU Gyro: X=");
  //value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_X);
  //print_mpu_reading(value);
  //printf(" deg/sec\n");



  //printf("MPU Gyro: Y=");
  //value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_Y);
  //print_mpu_reading(value);
  //printf(" deg/sec\n");

  //printf("MPU Gyro: Z=");
  //value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_GYRO_Z);
  //print_mpu_reading(value);
  //printf(" deg/sec\n");

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_X);
  data[ACC_X] = value;

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_Y);
  data[ACC_Y] = value;

  value = mpu_9250_sensor.value(MPU_9250_SENSOR_TYPE_ACC_Z);
  data[ACC_Z] = value;

  SENSORS_DEACTIVATE(mpu_9250_sensor);

  process_post_synch(&p3,e2,data);

  //ctimer_set(&mpu_timer, next, init_mpu_reading, NULL);
}

static void init_mpu_reading(void *not_used)
{
  mpu_9250_sensor.configure(SENSORS_ACTIVE, MPU_9250_SENSOR_TYPE_ALL);
}

void do_callback() //timer p3 expired
{
  ctimer_reset(&timer_ctimer);
  process_post(&p1,e1,NULL); //async to p1
}
/*---------------------------------------------------------*/
/*---------------------------------------------------------*/

PROCESS_THREAD(p1, ev, data)
{
  static struct etimer timer_etimer;

  PROCESS_BEGIN();
  //init_sensors
  SENSORS_ACTIVATE(reed_relay_sensor);
  SENSORS_ACTIVATE(batmon_sensor);

  /* Init the BLE advertisement daemon */
  rf_ble_beacond_config(0, BOARD_STRING);
  rf_ble_beacond_start();

  etimer_set(&et, LOOP_INTERVAL);


  //GARbAGE?
  e1 = process_alloc_event();
  etimer_set(&timer_etimer, 5 * CLOCK_SECOND); //timer 4 seconds deadline

  while(1) {
      //PROCESS_YIELD();
      PROCESS_WAIT_EVENT(); //waits 4 events

      if (ev == PROCESS_EVENT_TIMER){ //timer expired

          if(data == &et) {
            leds_toggle(LEDS_PERIODIC);
            etimer_set(&et,LOOP_INTERVAL);
          }

  	  //init mpu sensor
	  init_mpu_reading(NULL);

          //GARBAGE
          //etimer_reset(&timer_etimer);
          //process_post_synch(&p3,e2,NULL); //sync t0 p3
      }
      else if(ev == sensors_event) {
          if(data == SENSOR_1) {
              printf("Left: Pin %d, press duration %d clock ticks\n",
                     (SENSOR_1)->value(BUTTON_SENSOR_VALUE_STATE),
                     (SENSOR_1)->value(BUTTON_SENSOR_VALUE_DURATION));

              if((SENSOR_1)->value(BUTTON_SENSOR_VALUE_DURATION) >
                  CLOCK_SECOND) {
                  printf("Long button press!\n");
              }

            leds_toggle(LEDS_BUTTON);
          } else if(data == SENSOR_2) {
                leds_on(LEDS_REBOOT);
                watchdog_reboot();
          }  else if(data == SENSOR_5) {
                if(buzzer_state()) {
                    buzzer_stop();
                } else {
                    buzzer_start(1000);
                }
          } else if(ev == sensors_event && data == &mpu_9250_sensor) {
            get_mpu_reading();
          }
    }
      else if (ev == e1){
          //code
      }
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
	  printf("MPU Acc: X=");
	  print_mpu_reading(((int *)data)[ACC_X]);
	  printf(" G\n");

	  printf("MPU Acc: Y=");
	  print_mpu_reading(((int *)data)[ACC_Y]);
	  printf(" G\n");

	  printf("MPU Acc: Z=");
	  print_mpu_reading(((int *)data)[ACC_Z]);

	  printf(" G\n");
	  if (((int *)data) < 0)printf("HACIA ARRIBA\n");
	  else printf("HACIA HACIA ABAJO\n");
      }


  }

  PROCESS_END();
}
