Para poder programar el Arduino (y para el funcionamiento normal del nodo) es necesario colocar el interruptor de deslizamiento en la posición más alejada a la antena.

Generalmente el programador no es capaz de aportar la corriente que el nodo necesita, por lo que es recomendable alimentarlo mediante las baterías también durante la programación.
- Para programar el ESP03 tengo que pulsar el RESET del Arduino, que estará unido al GPIO0 del ESP03 y, además usar el deslizador para poner EN_ESP en alto (es decir, colocar el interruptor de deslizamiento en la posición más próxima a la antena).
- Tener en cuenta que el pin RX del cabezal de programación corresponde al TX del ESP03 y al RX del Arduino; de similar forma, el pin TX del cabezal de programación corresponde al RX del ESP03 y al TX del Arduino. 
- El RESET del Arduino debe estar ya pulsado antes de alimentar el sistema

Resultado de la compilación con Arduino 1.8.10 y versión de las librerías utilizadas (la versión de Minicore es 2.0.3
![imagen](https://user-images.githubusercontent.com/52624907/134652531-3c782e80-79ed-4b29-9e56-d59d169ad6dc.png)
