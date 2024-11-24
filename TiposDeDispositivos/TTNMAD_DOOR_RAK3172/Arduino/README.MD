Este programa se ha compilado con la versión 4.1.1 del gestor de placas RUI3 para Arduino.

Para grabarlo en el nodo TTNMAD_DOOR seguir estos pasos:
* Asegurarse de que el nodo tiene instalado el bootloader de RUI3. 
* Compilar el programa con Arduino IDE
* Localizar el archivo BIN en la carpeta C:\Users\[miusuario]\AppData\Local\Temp\arduino\sketches\XXXX\ttnmad_door_rui3.ino.bin
* Utilizar STM32Programmer y un ST-LINK para grabar el programa BIN a partir de la dirección 0x08006000

![Conexiones ST-Link](TiposDeDispositivos/TTNMAD_DOOR_RAK3172/Arduino/imagenes/stlink.jpg)

El consumo en estado dormido es aproximadamente 7uA a 3V.

