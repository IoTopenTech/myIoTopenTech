# Tipo de dispositivo V02_001
## Descripción general
El tipo de dispositivo V02_001 tiene una finalidad eminentemente didáctica para mostrar las posibilidades de la plataforma My IoT open Tech.
Se trata de un dispositivo LoRaWAN con las siguientes características:
* Envía un mensaje con el estado de un sensor de efecto hall (sensor de puerta abierta) cada vez que éste cambia de estado.
* Envía periódicamente una señal heartbeat indicando el estado de la puerta, el nivel de batería y el estado del LED.
* Sólo admite OTAA.
* Puede ser pre-aprovisionado (un proveedor se encarga de dar de alta el dispositivo en su cuenta de The Things Network antes de vendérselo al cliente), reclamado (el cliente sólo tiene que introducir el identificador del dispositivo para empezar a utilizarlo en My IoT open Tech, pero en The Things Network el dispositivo sigue siendo propiedad del proveedor), y tomado en posesión (una vez reclamado, el usuario puede tomar la posesión completa del dispositivo vinculándolo a su propia cuenta de The Things Network).
* El identificador del dispositivo puede leerse como código Morse a través del LED.
* Puede resetearse para volver al estado pre-reclamado, por si el usuario quisiera revincularlo a otra aplicación de The Things Network, o pasar la propiedad a un tercero.
## Hardware
El tipo de dispositivo V02_001 está construído a partir de la plataforma del nodo V2 de la comunidad The Things Network Madrid
