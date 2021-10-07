Generalmente el programador no es capaz de aportar la corriente que el nodo necesita, por lo que es recomendable alimentarlo mediante las baterías también durante la programación.
- Para programar el Arduino (y para el funcionamiento normal del nodo) es necesario colocar el interruptor de deslizamiento en la posición más alejada a la antena.
- Para programar el ESP03 se tiene que pulsar el RESET del Arduino, que está unido al GPIO0 del ESP03 y, además usar el deslizador para poner EN_ESP en alto (es decir, colocar el interruptor de deslizamiento en la posición más próxima a la antena).
- Tener en cuenta que el pin RX del cabezal de programación corresponde al TX del ESP03 y al RX del Arduino; de similar forma, el pin TX del cabezal de programación corresponde al RX del ESP03 y al TX del Arduino. 
- El RESET del Arduino debe estar ya pulsado antes de alimentar el sistema

Resultado de la compilación del programa del ATmega328p con Arduino 1.8.10 y versión de las librerías utilizadas (la versión de Minicore es 2.0.3).
![imagen](https://user-images.githubusercontent.com/52624907/134652531-3c782e80-79ed-4b29-9e56-d59d169ad6dc.png)

Resultado de la compilación del programa del ESP03 con Arduino 1.8.10 y versión de las librerías utilizadas (la versión de las placas ESP8266 es 2.7.4).
![imagen](https://user-images.githubusercontent.com/52624907/136376825-1e3f0aeb-a2c3-45ee-8378-6d923f4e4e44.png)


Perfil de consumos
![imagen](https://user-images.githubusercontent.com/52624907/134661465-e4668836-8e76-4cf8-9a08-4740f311c621.png)

En estado dormido consume aproximadamente 10-20uA.

La salida por el monitor serie del ATmega328p a 9600bps debería ser similar a la siguiente.
![imagen](https://user-images.githubusercontent.com/52624907/136377468-1de53bfc-13ae-418d-95df-22dd412de35c.png)

Los caracteres extraños se deben a que el microcontrolador entra en estado de sueño (deep sleep) antes de terminar la impresión a través del monitor serie.

Procedimiento de montaje
1. Insertar los pines por el lado más largo en el PCB. Este paso nos permite aprovechar el PCB para alinear los pines correctamente, de modo que al soldar en ellos el Arduino pro mini (paso siguiente) no queden inclinados
![imagen](https://user-images.githubusercontent.com/52624907/136381722-f86443d5-bb9d-4a0f-935f-44fc098ebe00.png)

2. Insertar el Arduino pro mini en los pines y soldarlo. Retirar el Arduino pro mini con los pines ya soldados del PCB.
 ![imagen](https://user-images.githubusercontent.com/52624907/136381993-e3312ed7-9fc8-4c76-a0ed-5f283cf80715.png)
 
3. Conectar un programador al Arduino por mini para cargar en él el firmware de Minicore 2.0.3. Por ejemplo, en la siguiente imagen se realiza esta operación con un USBtinyISP. 
![imagen](https://user-images.githubusercontent.com/52624907/136382172-0406707d-94fd-4c2d-a208-cf05dab69323.png)

4. Configurar las opciones de Arduino IDE como se indica en la siguiente figura y ejecutar el comando Herramientas>Quemar bootloader. Comprobar que en la zona inferior de la interfaz de Arduino IDE se indica que la operación se ha realizado correctamente (este procedimiento es muy rápido; apenas dura 2-3 segundos).
![imagen](https://user-images.githubusercontent.com/52624907/136382695-d8d3115d-68d8-4fa2-abaa-5af52adfe9d3.png)





