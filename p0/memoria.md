# Práctica 1  
Lucas Segarra Fernández
Sergio Semedi Barranco
## Cuestiones
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
