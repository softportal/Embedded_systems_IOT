# Practica 2 
## Aniot

### Alumnos


#### Cuestiones

Trata de responder a las siguientes preguntas:

* ¿Qué función de tmp-007-sensor.c se ejecuta cuando, desde el proceso, se
invoca SENSORS_ACTIVATE(tmp_007_sensor)? ¿Con qué argumentos de
entrada?

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

* Tras la activación del nodo, ¿cuánto tiempo transcurrirá hasta que se envíe un evento al proceso para que proceda a la lectura?

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

Una vez llega el evento, el proceso ejecuta la función get_tmp_reading().
* ¿Por qué se hace una primera llamada a value() con el argumento
TMP_007_SENSOR_TYPE_ALL para luego hacer sendas llamadas con
TYPE_AMBIENT y TYPE_OBJECT?

* ¿Cuándo se producirá la siguiente lectura del sensor?

Estudia el código de la función convert() en tmp-007-sensor.c y lee la
sección 7.3.7 del datasheet. 

* ¿Se está respetando lo que se indica en el datasheet?

Considera ahora el sensor MPU. Ejecuta el código de ejemplo y observa los valores leídos del sensor MPU mientras mueves el sensor. 

*   ¿A qué direcciones del espacio se
corresponden los ejes X / Y / Z del acelerómetro?
