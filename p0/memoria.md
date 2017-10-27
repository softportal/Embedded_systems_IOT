# Práctica 1      
Lucas Segarra Fernández<br>
Sergio Semedi Barranco
## Cuestiones
### Cuestión 7
Lo mas relevante de esta cuestión es el valor de las macros:
* SWITCH_ALL_EVENTS: Funciona con los dos switchs.  
* SWITCH_0_PRESSED : Se activa al presionar el switch 0 
* SWITCH_1_PRESSED : Se activa al presionar el switch 1 

Cabe destacar la función *switch_add_callback* que permite pasar como parámetro tanto una macro de las que acabamos de definir anteriormente como una función que será invocada cada vez que la macro se cumpla.    

La función *switch_init* lanza un hilo que monitoriza los eventos lanzados por el switch.   


### Cuestión 7.2 

Para probar esta sección hemos lanzado lo siguiente en terminal:

    $ while true
    $ do
    $   cat /sys/class/leds/marduk\:red\:user4/brightness
    $   sleep 1
    $ done 
    
Parece ser que el valor de ese archivo cambia a 1 cuando el led 4 de la placa esta encedido, como es un barrido (que va manteniendo los leds encendidos) debemos esperar a que se enciendan los 4 primeros (user4) para ver el valor del fichero cambiado. El fichero sys/class/leds/marduk\:red\:user1/brightness estará siempre a 1 durante toda la ejecución del programa ya que ese led esta encendido siempre.


### Cuestión 7.3

Parece ser que: 
* /sys/class/gpio/gpio2[1 | 3]/direction contiene un string para describir la dirección.
* /sys/class/gpio/gpio2[1 | 3]/value puede tener 1 o 0 según el estado del sensor.

Cuando el sensor no detecta movimiento, el fichero /sys/class/gpio/gpio21/value tiene el valor 0 y al detectarlo el valor pasa a ser 1. 
El contenido de /sys/class/gpio/gpio21/direction parece ser "in", con independencia de lo que hagamos con el sensor. Análogamente el de /sys/class/gpio/gpio23/direction siempre es "out".

El valor del pin 23 parece no cambiar nunca, el proceso que hemos hecho para probarlo es:

    $ while true
    $ do
    $   cat /sys/class/gpio/gpio23/value
    $   sleep 1
    $ done 

Nunca ha dejado de poner 1 independientemente de lo que hicieramos con el sensor.

### Tarea 7

El código se adjunta en el fichero app_p1.c

al comprobar cat /sys/kernel/debuf/gpio | grep SWITCH : 

 gpio-54  (                    |SWITCH 1            ) in  hi IRQ
 gpio-46  (                    |SWITCH 2            ) in  hi IRQ
 gpio-54  (                    |SWITCH 1            ) in  lo IRQ
 gpio-46  (                    |SWITCH 2            ) in  lo IRQ
 
 Vemos como cambia el valor de in (hi->lo) al pulsar los switches.


