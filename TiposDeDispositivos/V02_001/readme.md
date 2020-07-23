# Tipo de dispositivo V02_001
## Descripción general
El tipo de dispositivo V02_001 tiene una finalidad eminentemente didáctica para mostrar las posibilidades de la plataforma My IoT open Tech.

Se trata de un dispositivo LoRaWAN con las siguientes características:

* Envía un mensaje con el estado de un sensor de efecto hall (sensor de puerta abierta) cada vez que éste cambia de estado.
* Envía periódicamente una señal heartbeat indicando el estado de la puerta, el nivel de batería y el estado del LED.
* Sólo admite OTAA.
* Puede ser pre-aprovisionado (un proveedor se encarga de dar de alta el dispositivo en su cuenta de The Things Network antes de vendérselo al cliente), reclamado (el cliente sólo tiene que introducir el identificador del dispositivo para empezar a utilizarlo en My IoT open Tech, pero en The Things Network el dispositivo sigue siendo propiedad del proveedor), y tomado en posesión (una vez reclamado, el usuario puede tomar la posesión completa del dispositivo vinculándolo a su propia cuenta de The Things Network).
* El identificador del dispositivo puede leerse como código morse a través del LED.
* Puede resetearse para volver al estado pre-reclamado, por si el usuario quisiera revincularlo a otra aplicación de The Things Network, o pasar la propiedad a un tercero.

## Hardware
El tipo de dispositivo V02_001 está construído a partir de la plataforma del nodo V2.2 de la comunidad The Things Network Madrid:

https://github.com/IoTopenTech/Nodo_TTN_MAD_V2_2/wiki

Está basado en un Arduino Pro mini a 8MHz y 3.3V, y un RFM95W.

En el tipo de nodo V02_001 sólo se han incorporado las extensiones Hall y LED.

## Software
El código esta disponible en la carpeta software y para programarlo en el dispositivo se requiere un conversor USB / Serial con lógica de 3.3V.

Al arrancar, el dispositivo consulta si el botón está pulsado y, en caso afirmativo, empieza a hacer parpadear el LED cada segundo mientras espera a que el usuario suelte el botón.

Si el usuario suelta el botón antes de 5 segundos, empezará a emitir en código morse mediante el LED la clave para reclamar el dispositivo. Con otra pulsación del botón se saldrá del modo morse, y el dispositivo empezará a funcionar normalmente.

Si el usuario tarda más de 5 segundos en soltar el botón, el dispositivo se reseteará volviendo al estado pre-reclamado, es decir, regresará a las credenciales OTAA de The Things Network que tenía originalmente (antes de que el usuario final lo reclamase y/o tomase posesión de él).

Si el botón no está pulsado al arrancar, el dispositivo entra en el modo de funcionamiento normal, que consiste en estar dormido (para minimizar el consumo de energía), y sólo despertar cuando se produzca un cambio de estado del sensor de efecto hall o para enviar señales de tipo heartbeat periódicas (por defecto, aproximadamente cada minuto).

## Carga de pago para uplinks

El dispositivo utiliza el formato de carga de pago Cayenne LPP, con los siguientes canales:

Canal | Tipo | Valor
----- | ---- | -----
0x01 | Entrada analógica [0x02] | Tensión de la batería en cV (centivoltios)
0x02 | Entrada digital [0x00] | Estado del sensor hall: 0 cerrado y 1 abierto
0x03  |Entrada digital [0x00] | Estado del LED: 0 apagado y 1 encendido

## Carga de pago para downlinks

El dispositivo sólo atiende los downlinks que lleguen por el puerto 99, y usa un formato similar al de Cayenne LPP:

Canal | Tipo | Valor
----- | ---- | -----
0x06  |Salida digital [0x01] | Cambiar el estado del LED: 100 (0x64) encender; cualquier otro valor apagar
0x07|Salida digital [0x01] | Cambiar el periodo de envío de heartbeats (expresado en minutos)
0x46 | 0x46 (no estándar LPP) | Cambio de credenciales OTAA

El cambio de credenciales implica el envío de 3 downlinks, uno para cada credencial (DEV EUI, APP EUI y APP KEY). No pueden enviarse en un sólo downlink porque se superaría la longitud de carga de pago de LoRaWAN para los spread factors más lentos.

Cada downlink comenzará con 0x46 0x46 (caracteres "FF" ASCII expresados en hexadecimal).

A continuación se utilizará el siguiente par de bytes para indicar el tipo de credencial:

Byte 3 y 4 | Tipo de credencial
---------- | ------------------
0x30 0x31 ("01" ASCII2HEX) | DEV EUI
0x30 0x32 ("02" ASCII2HEX) | APP EUI
0x30 0x33 ("03" ASCII2HEX) | APP KEY

A continuación se utilizará un byte para indicar el tipo de activación (O de OTAA o A de ABP); en este caso sólo se admite OTAA, por lo que el 5º byte será 0x4F.

Por último se incluirá la credencial correspondiente en el mismo formato que la presenta la consola de The Things Network por defecto (sin cambiar el endianess).

# Parámetos de configuración
El tipo de dispositivo V02_001 ofrece los siguientes parámetros de configuración en la interfaz de My IoT open Tech:

* Número de minutos entre heartbeats.
* Coordenadas para mostrarlo sobre la imagen de un activo de tipo IMAGE01 (corresponden al porcentaje respecto a la esquina superior izquierda; sólo se admiten valores positivos).
* Alarmas: En todas las alarmas el usuario puede elegir por qué vía desea que le sean notificadas (email, IFTTT, Telegram...)
  * Cambio de estado: Se puede activar al abrir el sensor hall o al cerrarlo.
  * Nivel bajo de batería: Permite configurar un umbral mínimo.
  * Inactividad: Permite establecer el número de segundos que debe transcurrir sin actividad por parte del dispositivo para que se genere la alarma (obviamente, convendrá que sea un valor mayor que el del periodo de hearbeat).
  
## Notificaciones de alarmas
### Cambio de estado
Las notificaciones para la alarma de cambio de estado tienen la siguiente estructura:

* email y Telegram: El dispositivo [nombre del dispositivo] ha generado una alarma de tipo Puerta abierta/cerrada. La tensión actual de la batería es [tensión] V, y el umbral de alarma es [umbral de alarma de nivel bajo de batería] V.
* IFTTT: {"value1":"'[nombre del dispositivo]'","value2":"'[estado del sensor de efecto hall]'","value3":"'[tensión de la batería]'"}

### Nivel bajo de batería
Las notificaciones para la alarma de nivel bajo de batería tienen la siguiente estructura:

* email y Telegram: El dispositivo [nombre del dispositivo] ha generado una alarma de tipo Nivel bajo de batería. La tensión actual de la batería es [tensión] V, y el umbral de alarma es [umbral de alarma de nivel bajo de batería] V.
* IFTTT: {"value1":"'[nombre del dispositivo]'","value2":"'[estado del sensor de efecto hall]'","value3":"'[tensión de la batería]'"}

### Inactividad
Las notificaciones para la alarma de inactividad tienen la siguiente estructura:

* email y Telegram: El dispositivo [nombre del dispositivo] ha generado una alarma de inactividad.
* IFTTT: {"value1":"'[nombre del dispositivo]'","value2":"'[estado del sensor de efecto hall]'","value3":"'[tensión de la batería]'"}

# Formato de pre-aprovisionamiento
Para preaprovisionar dispositivos de tipo V02_001 se requiere un archivo CSV que use como separador el signo ; (punto y coma) y contenga los siguientes campos:
* name: Nombre del dispositivo precedido del carácter P; por ejemplo: P0000000100000001
* type: Tipo de dispositivo; por ejemplo V02_001
* nombreOriginal: Nombre original del dispositivo para recuperarlo si el usuario lo resetease; coincidirá con el name (P0000000100000001)
* tipoOriginal: Tipo original del dispositivo para recuperarlo si el usuario lo resetease; coincidirá con el type (V02_001)
* claimingData: Objeto en formato JSON con la clave de reclamación del dispositivo; por ejemplo: {"secretKey": "ABCDEFGHIJKLMNOP", "expirationTime": "1640995200000"}
* claimingDataOriginal: Copia del parámetro anterior porque el anterior será borrado si el usuario reclama el dispositivo, y necesitamos un modo de recuperar esa información si el usuario resetease el dispositivo.
* apropiable: Atributo en el que se indica si el usuario podría tomar posesión del dispositivo mediante downlinks; en este caso el valor será true.
* apropiado: Atributo en el que se indica si el usuario ha tomado posesión del dispositivo; contendrá el valor false
* __cs_url: Si el dispositivo está pre-aprovisionado en ChirpStack, aquí indicaremos el URL.
* __cs_token: Si el dispositivo está pre-aprovisionado en ChirpStack, aquí indicaremos el JWT autorizado en la API.
* admiteABP: Contendrá el valor false porque el tipo de nodo V02_001 no admite ABP
* __heartbeat: Este parámetro corresponde al periodo de envío de heartbeats y contendrá el valor 1 (que es el predeterminado para el tipo V02_001).
