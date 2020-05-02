# Tipo de dispositivo RAK7200
## Descripción general
Se trata de un tracker dotado de GPS, acelerómetro, giróscopo, magnetómetro y barómetro, alimentado por batería recargable de litio, y configurable mediante comandos AT.

La información detallada está disponible en la web del fabricante:

https://doc.rakwireless.com/quick-start/rak7200-lora-tracker/overview

## Hardware
Está basado en el módulo S76G, que incluye un microcontrolador (STM32L073x), módulo LoRa (sx1276) y módulo GPS (CXD5603GF) en el mismo integrado.

## Software
En la web del fabricante se explica cómo actualizar el firmware:

https://doc.rakwireless.com/rak7200-lora-tracker/upgrading-the-firmware

## Carga de pago para uplinks

El dispositivo utiliza el formato de carga de pago Cayenne LPP, excepto para el magnetómetro que lo codifica como valores analógicos, con los siguientes canales:

Canal | Tipo | Valor
----- | ---- | -----
0x01 | GPS [0x88] | Latitud, longitud y altitud
0x03 | Acelerómetro [0x71] | Aceleraciones (expresado en 0.001 g)
0x05 | Giróscopo [0x86] | Velocidades angulares (expresado en 0.01 °/s)
0x08 | Entrada analógica [0x02] | Tensión de la batería(expresado en 0.01 V = cV)
0x09 | Entrada analógica [0x02] | 	Proyección de la inducción magnética sobre el eje X (expresada en 0.01 uT)
0x10 | Entrada analógica [0x02] | 	Proyección de la inducción magnética sobre el eje Y (expresada en 0.01 uT)
0x11 | Entrada analógica [0x02] | 	Proyección de la inducción magnética sobre el eje Z (expresada en 0.01 uT)

> Nota: Aparentemente la unidad IMU está soldada por la cara de abajo porque la aceleración sobre el eje Z da una lectura positiva al situar el RAK7200 horizontal con el logotipo hacia arriba.

## Carga de pago para downlinks

No admite downlinks.

# Parámetos de configuración
El tipo de dispositivo V02_001 ofrece los siguientes parámetros de configuración en la interfaz de My IoT open Tech:

* Geocercado: Un polígono en formato [[lat1,lon1][lat2,lon2]...[latN,lonN]] sobre el que se podrán establecer alarmas de tipo "al entrar" o "al salir".
* Alarmas: En todas las alarmas el usuario puede elegir por qué vía desea que le sean notificadas (email, IFTTT, Telegram...)
  * Alarma de geocercado: Se puede activar al entrar al geocercado o al salir de él.
  * Nivel bajo de batería: Permite configurar un umbral mínimo.
  * Inactividad: Permite establecer el número de segundos que debe transcurrir sin actividad por parte del dispositivo para que se genere la alarma (obviamente, convendrá que sea un valor mayor que el del periodo de hearbeat que se puede establecer en el RAK7200 mediante comandos AT).
  
## Notificaciones de alarmas
### Geocercado
Las notificaciones para la alarma de cambio de estado tienen la siguiente estructura:

* email y Telegram: El dispositivo [nombre del dispositivo] ha generado una alarma de tipo [Entrada en geocercado/Salid de geocercado]. Sus coordenadas son [lat: [latitud], lon: [longitud]] .La tensión de la batería es [tensión de la batería] V.
* IFTTT:{"value1":"[nombre del dispositivo]","value2":"[latitud]","value3":"[longitud]"}

### Nivel bajo de batería
Las notificaciones para la alarma de nivel bajo de batería tienen la siguiente estructura:

* email y Telegram: El dispositivo [nombre del dispositivo] ha generado una alarma de tipo Nivel bajo de batería. La tensión actual de la batería es [tensión] V, y el umbral de alarma es [umbral de alarma de nivel bajo de batería] V.
* IFTTT: {"value1":"'[nombre del dispositivo]'","value2":"'[estado del sensor de efecto hall]'","value3":"'[tensión de la batería]'"}

### Inactividad
Las notificaciones para la alarma de inactividad tienen la siguiente estructura:

* email y Telegram: El dispositivo [nombre del dispositivo] ha generado una alarma de inactividad.
* IFTTT: {"value1":"'[nombre del dispositivo]'","value2":"'[estado del sensor de efecto hall]'","value3":"'[tensión de la batería]'"}

# Formato de pre-aprovisionamiento
Para preaprovisionar dispositivos de tipo RAK7200 se requiere un archivo CSV que use como separador el signo ; (punto y coma) y contenga los siguientes campos:
* name: Nombre del dispositivo precedido del carácter P; por ejemplo: P0000000100000001
* type: Tipo de dispositivo; en este caso RAK7200
* nombreOriginal: Nombre original del dispositivo para recuperarlo si el usuario lo resetease; coincidirá con el name (P0000000100000001)
* tipoOriginal: Tipo original del dispositivo para recuperarlo si el usuario lo resetease; coincidirá con el type (RAK7200)
* claimingData: Objeto en formato JSON con la clave de reclamación del dispositivo; por ejemplo: {"secretKey": "ABCDEFGHIJKLMNOP", "expirationTime": "1640995200000"}
* claimingDataOriginal: Copia del parámetro anterior porque el anterior será borrado si el usuario reclama el dispositivo, y necesitamos un modo de recuperar esa información si el usuario resetease el dispositivo.
* apropiable: Atributo en el que se indica si el usuario podría tomar posesión del dispositivo mediante downlinks; en este caso el valor será false.
