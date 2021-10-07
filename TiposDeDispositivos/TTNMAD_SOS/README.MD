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

# Procedimiento de montaje
1. Insertar los pines por el lado más largo en el PCB. Este paso nos permite aprovechar el PCB para alinear los pines correctamente, de modo que al soldar en ellos el Arduino pro mini (paso siguiente) no queden inclinados
![imagen](https://user-images.githubusercontent.com/52624907/136381722-f86443d5-bb9d-4a0f-935f-44fc098ebe00.png)

2. Insertar el Arduino pro mini en los pines y soldarlo. Retirar del PCB el Arduino pro mini con los pines ya soldados (el Arduino pro mini no debe soldarse aún al PCB; primero vamos a cambiar su firmware (paso siguiente)).
 ![imagen](https://user-images.githubusercontent.com/52624907/136381993-e3312ed7-9fc8-4c76-a0ed-5f283cf80715.png)
 
3. Conectar un programador al Arduino por mini para cargar en él el firmware de Minicore 2.0.3. Por ejemplo, en la siguiente imagen se realiza esta operación con un USBtinyISP. 
![imagen](https://user-images.githubusercontent.com/52624907/136382172-0406707d-94fd-4c2d-a208-cf05dab69323.png)

4. Configurar las opciones de Arduino IDE como se indica en la siguiente figura y ejecutar el comando Herramientas>Quemar bootloader. Comprobar que en la zona inferior de la interfaz de Arduino IDE se indica que la operación se ha realizado correctamente (este procedimiento es muy rápido; apenas dura 2 o 3 segundos).

![imagen](https://user-images.githubusercontent.com/52624907/136382695-d8d3115d-68d8-4fa2-abaa-5af52adfe9d3.png)

5. Soldar todos los componentes de la cara posterior, excepto el botón (no lo soldamos aún porque, como sobresale del PCB, podría resultar un poco complicado mantenerlo en posición mientras lo soldamos). En este caso se ha realizado con estaño en pasta y un soldador de aire caliente, pero también podría hacerse con una soldador de tipo lápiz. Obsérvese la orientación del los integrados MIC94092.
![imagen](https://user-images.githubusercontent.com/52624907/136383241-c6283e4d-9ea5-4427-bf46-61ff0d272be5.png)

6. Soldar el GPS y el LED por la otra cara del PCB. En la siguiente imagen el LED tiene el cátodo en el pin izquierdo.

![imagen](https://user-images.githubusercontent.com/52624907/136400894-252a9d4e-a1a6-43ef-b82f-40711d162d87.png)


7. Cortar los cables del portabaterías con una longitud de entre 7 y 8 centímetros (dejarlos más largos podría complicar la inserción posterior del nodo en de la carcasa). Insertar los cables por los orificios para evitar su torsión y soldarlos como se muestra en las siguientes imágenes.
![imagen](https://user-images.githubusercontent.com/52624907/136384583-f48538d1-5957-4806-82e8-d21a68269c8b.png)
![imagen](https://user-images.githubusercontent.com/52624907/136384656-3caea43a-cb6f-48d5-9954-11bd19b9b21c.png)

8. Ahora puede ser un buen momento para soldar el botón. Se recomienda hacerlo con un soldador de lápiz (usar uno de aire caliente podría derretir el plástico), y empezar soldando uno de los pines traseros, que son más sencillos y facilitarán que el botón se mantenga en posición mientras soldamos el resto de pines.
![imagen](https://user-images.githubusercontent.com/52624907/136384996-f5ec3b7e-dfc7-4e79-884d-f74788322a8c.png)

9. Insertar el Arduino pro mini en el el PCB como se indica en la siguiente imagen. Obsérvese que los cables del portabaterías pasan entre el Arduino pro mini y el PCB, y salen de este "emparedado" por el hueco que queda entre el GPS y el propio Arduino pro mini.
![imagen](https://user-images.githubusercontent.com/52624907/136385600-e43683e2-1c83-4641-a417-1c08916015c9.png)

10. Soldar la tira de pines, el interruptor deslizante y la antena helicoidal.
![imagen](https://user-images.githubusercontent.com/52624907/136385766-1e4ce9e3-0072-4485-a91a-67b475060252.png)

11. Terminar el proceso de soldadura eliminando con un soldador de aire caliente el LED y el regulador de tensión del Arduino pro mini. Esta operación reducirá el consumo del Arduino pro mini de un modo significativo.
![imagen](https://user-images.githubusercontent.com/52624907/136386001-4b4155ff-60f0-403e-9951-7ad4752895ef.png)

12. Cargar en Arduino IDE el programa TTNMAD_SOS de este mismo repositorio. Alimentar con baterías el nodo y conectarlo a un conversor USB-->Serial para programarlo. Obsérvese la siguiente imagen. Se recomienda utilizar baterías de alto rendimiento por los picos de corriente que puede requerir el ESP03. Programar el Atmega328p ejecutando el comando Programar>Subir de Arduino IDE (las opciones de placa deben ser las mismas que se mostraron en el paso 4 para cambiar el firmware).
![imagen](https://user-images.githubusercontent.com/52624907/136386989-9f2aa618-b79d-4977-97a4-f84acefbe22c.png)

13. Cargar en Arduino IDE el programa TTNMAD_SOS_ESP03 de este mismo repositorio y configurar las opciones de placa como se muestran en la siguiente imagen.
![imagen](https://user-images.githubusercontent.com/52624907/136388401-316ee367-78b5-498b-bf8f-be47bf8449cc.png)

14. Conectar el conversor USB-->Serial al nodo como se indica en la imagen. Colocar el interruptor deslizante en la posición más próxima a la antena. Retirar las baterías del nodo para interrumpir su alimentación. Pulsar y mantener pulsado el botón Reset del Arduino pro mini para activar el modo de programación del ESP03. Volver a colocar las baterías y, con el botón Reset aún pulsado, ejecutar el comando Programa>Subir de Arduino IDE para programar el ESP03.
![imagen](https://user-images.githubusercontent.com/52624907/136388846-561d18dc-0c55-4d75-99a1-50c49b7bbf30.png)

16. Soltar el botón de Reset y colocar el interruptor deslizante en la posición más alejada de la antena para salir del modo de programación del ESP03 y habilitar el modo de funcionamiento normal del nodo.














