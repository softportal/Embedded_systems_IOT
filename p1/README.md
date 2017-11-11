# Practica 2 
## Aniot

### Alumnos


#### Cuestiones

Trata de responder a las siguientes preguntas:

* ¿Qué función de tmp-007-sensor.c se ejecuta cuando, desde el proceso, se
invoca SENSORS_ACTIVATE(tmp_007_sensor)? ¿Con qué argumentos de
entrada?


* Tras la activación del nodo, ¿cuánto tiempo transcurrirá hasta que se envíe un
evento al proceso para que proceda a la lectura?
Una vez llega el evento, el proceso ejecuta la función get_tmp_reading().

* ¿Por qué se hace una primera llamada a value() con el argumento
TMP_007_SENSOR_TYPE_ALL para luego hacer sendas llamadas con
TYPE_AMBIENT y TYPE_OBJECT?

* ¿Cuándo se producirá la siguiente lectura del sensor?

Estudia el código de la función convert() en tmp-007-sensor.c y lee la
sección 7.3.7 del datasheet. 

* ¿Se está respetando lo que se indica en el datasheet?

