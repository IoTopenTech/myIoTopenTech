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




