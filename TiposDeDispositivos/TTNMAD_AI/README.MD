# Bootloader UF2

## Referencias
- https://www.hackster.io/wallarug/uf2-bootloader-creating-custom-boards-c9620c
- https://www.hackster.io/wallarug/arduino-ide-creating-custom-boards-89f7a6
- https://arduino.github.io/arduino-cli/latest/platform-specification/
- https://www.arduino.cc/en/Tutorial/SamdSercom

## Programación del bootloader con Jlink

![imagen](https://user-images.githubusercontent.com/52624907/143231056-833cdc39-498e-4930-8537-0909a6953bd6.png)


![imagen](https://user-images.githubusercontent.com/52624907/143231147-b0bff1fc-1e46-4993-9ad6-3793a03cbd7b.png)


## Programación con Arduino IDE
Para programarlo desde Arduino IDE, además de elegir la placa, hay que poner el TTNMAD_AI en modo bootloader pulsando 2 veces rápido el botón de RESET (el LED empezará a hacer dimming y aparecerán un puerto Serial y una unidad MSD).

He usado como referencia la placa Adafruit Metro M4, porque usa el mismo microcontrolador.
Usar estas placas en Arduino
http://iespf2014.villatic.org/package_ttnmad_index.json

![imagen](https://user-images.githubusercontent.com/52624907/143233815-d2ee316d-10e5-4979-ae74-6e6ce76775f5.png)

![imagen](https://user-images.githubusercontent.com/52624907/143233992-a4de0383-d8e4-4e69-aa30-5233a8d60b78.png)

![imagen](https://user-images.githubusercontent.com/52624907/143238809-55a3da08-a73f-4efe-a868-288005494866.png)
