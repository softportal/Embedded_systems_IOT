# Practica 2 
## Aniot

### Alumnos
Sergio Semedi Barranco

Lucas Segarra Fernández

#### Cuestiones

**Trata de responder a las siguientes preguntas:**

* **¿Qué función de tmp-007-sensor.c se ejecuta cuando, desde el proceso, se
invoca SENSORS_ACTIVATE(tmp_007_sensor)? ¿Con qué argumentos de
entrada?**

Para comprobar esto primero vamos a observar el valor de la macro SENSORS_ACTIVATE
nos colocamos en ~ y realizamos:

    user-iot@VM-IOT:~$ cat CreatorDev/contiki/core/lib/sensors.h | grep SENSORS_ACTIVATE
    #define SENSORS_ACTIVATE(sensor) (sensor).configure(SENSORS_ACTIVE, 1)

Recibira por parametro un sensor y llamará a el propio configure pasandole la macro SENSORS_ACTIVE y 1.
Comprobamos la implementación del sensor de emperatura:

    user-iot@VM-IOT:~/CreatorDev$ cat $(find . | grep tmp-007-sensor.h) | grep tmp_007
    sensors_changed(&tmp_007_sensor);
    SENSORS_SENSOR(tmp_007_sensor, "TMP007", value, configure, status);
    extern const struct sensors_sensor tmp_007_sensor;


tmp_007 es un struct del tipo sensors_sensor que es donde contiki guarda la información, vamos a observar la propia
implementación:

```c
static int configure(int type, int enable) {
  switch(type) {
    case SENSORS_ACTIVE:
    /* Must be initialised first */
    if(enabled == SENSOR_STATUS_DISABLED) {
      return SENSOR_STATUS_DISABLED;
    }
    if(enable) {
      enable_sensor(true);
      ctimer_set(&startup_timer, SENSOR_STARTUP_DELAY, notify_ready, NULL);
      enabled = SENSOR_STATUS_NOT_READY;
    } else {
      ctimer_stop(&startup_timer);
      enable_sensor(false);
      enabled = SENSOR_STATUS_INITIALISED;
    }
    break;
  }
  return enabled;
}

```

El configure nos lleva a una región de codigo en el que activamos el sensor de temperatura, esto no es mas que realizar la escritura correspondiente:

```c
static bool enable_sensor(bool enable) {
  bool success;
  SELECT();
  if(enable) {
    val = TMP007_VAL_CONFIG_ON;
  } else {
    val = TMP007_VAL_CONFIG_OFF;
  }
  val = SWAP(val);
  success = sensor_common_write_reg(TMP007_REG_ADDR_CONFIG, (uint8_t *)&val,
                                    REGISTER_LENGTH);
  return success;
}
```

* **Tras la activación del nodo, ¿cuánto tiempo transcurrirá hasta que se envíe un evento al proceso para que proceda a la lectura?**

En el fichero ~/CreatorDev/contiki/platform/srf06-cc26xx/sensortag/tmp-007-sensor.c encontramos la siguiente definición:

```c
#define SENSOR_STARTUP_DELAY 36
```
Establece a 275 ms dicho valor.

En el mismo fichero, en la función *configure(int type,int enable)*, la llamada:    

```c
ctimer_set(&startup_timer,SENSOR_STARTUP_DELAY,notify_ready,NULL);
```
Fija un *callback timer* para activar el sensor utilizando la constante *SENSOR_STARTUP_DELAY* previamente definida.    

**Una vez llega el evento, el proceso ejecuta la función get_tmp_reading().**   
* **¿Por qué se hace una primera llamada a value() con el argumento TMP_007_SENSOR_TYPE_ALL para luego hacer sendas llamadas con TYPE_AMBIENT y TYPE_OBJECT?**

value en una estructura de tipo sensor_sensors es un puntero a una functión que nos permi
te leer el valor del sensor en concreto.

Si nos vamos a platform/srf06.../tmp-007-sensor.c observamos lo siguiente:

```c
static int
value(int type)
{
  int rv;
  uint16_t raw_temp;
  uint16_t raw_obj_temp;
  float obj_temp;
  float amb_temp;

  if(enabled != SENSOR_STATUS_READY) {
    PRINTF("Sensor disabled or starting up (%d)\n", enabled);
    return CC26XX_SENSOR_READING_ERROR;
  }

  if((type & TMP_007_SENSOR_TYPE_ALL) == 0) {
    PRINTF("Invalid type\n");
    return CC26XX_SENSOR_READING_ERROR;
  }

  rv = CC26XX_SENSOR_READING_ERROR;

  if(type == TMP_007_SENSOR_TYPE_ALL) {
    rv = read_data(&raw_temp, &raw_obj_temp);

    if(rv == 0) {
      return CC26XX_SENSOR_READING_ERROR;
    }

    convert(raw_temp, raw_obj_temp, &obj_temp, &amb_temp);
    PRINTF("TMP: %04X %04X       o=%d a=%d\n", raw_temp, raw_obj_temp,
           (int)(obj_temp * 1000), (int)(amb_temp * 1000));

    obj_temp_latched = (int)(obj_temp * 1000);
    amb_temp_latched = (int)(amb_temp * 1000);
    rv = 1;
  } else if(type == TMP_007_SENSOR_TYPE_OBJECT) {
    rv = obj_temp_latched;
  } else if(type == TMP_007_SENSOR_TYPE_AMBIENT) {
    rv = amb_temp_latched;
  }

  return rv;
}
```

Observamos que para poder optener los demas y realizar una lectura del lector completo 
tenemos que pasarle primero la opción SENSOR_TYPE_ALL para leer los datos en "crudo y
almacenarlos en memoria estática, posteriormente y con los valores ya almacenados
podemos pasarle a value lo parametros OBJECT o AMBIENT para elegir la funcionalidad.

* **¿Cuándo se producirá la siguiente lectura del sensor?**

Despues de cada llamada al metodo concreto reding se procede a llamar a la macro
SENSORS_DEACTIVATE, esto no es mas que volver a llamar a configure para pedir que
quite el enable.

Justo después de desactivarlo se activa un ctimer:
```c
  clock_time_t next = SENSOR_READING_PERIOD +
    (random_rand() % SENSOR_READING_RANDOM);
  ctimer_set(&hdc_timer, next, init_tmp_reading, NULL);
```

que volvera a llamar a SENSORS_ACTIVATE con enable a 1 para que se repita el proceso anterior.
por lo tanto tenemos:

        t0 = clock_time_t next = SENSOR_READING_PERIOD + (random_rand() % SENSOR_READING_RANDOM);
        t1 = #define SENSOR_STARTUP_DELAY 36

dt = t0+t1


**Estudia el código de la función convert() en tmp-007-sensor.c y lee la sección 7.3.7 del datasheet.** 

* **¿Se está respetando lo que se indica en el datasheet?**

Totalmente, en el código vemos lo siguiente:

```c
static void
convert(uint16_t raw_temp, uint16_t raw_obj_temp, float *obj, float *amb)
{
  const float SCALE_LSB = 0.03125;
  float t;
  int it;

  it = (int)((raw_obj_temp) >> 2);
  t = ((float)(it)) * SCALE_LSB;
  *obj = t;

  it = (int)((raw_temp) >> 2);
  t = (float)it;
  *amb = t * SCALE_LSB;
}
```
Luego si nos vamos a http://www.ti.com/lit/ds/sbos685c/sbos685c.pdf y a Temperature Format (página 17). Leemos:


_The temperature register data format of the TMP007 is reported in a binary twos_
_complement signed integer format, as Table 3 shows, with 1 LSB = (1 / 32)°C = 0. 3125 °C._
_To convert the integer temperature result of the TMP007 to degrees Celsius, right-shift_
_the result by two bits.  Then perform a divide-by-32 of T DIE and T OBJ , the 14-bit_
_signed integers contained in the corresponding registers.  The sign of the temperature_
_is the same as the sign of the integer read from the TMP007.  In twos complement_
_notation, the MSB is the sign bit. If the MSB is 1, the integer is negative_
_and the absolute value can be obtained by inverting all bits and adding 1. An alternate_
_method of calculating the absolute value of negative integers is abs(i) = i xor FFFFh_
_+ 1._


Justo lo que observamos en el código.


**Considera ahora el sensor MPU. Ejecuta el código de ejemplo y observa los valores leídos del sensor MPU mientras mueves el sensor.** 

*   **¿A qué direcciones del espacio se corresponden los ejes X / Y / Z del acelerómetro?**
