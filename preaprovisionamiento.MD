Los proveedores pueden pre-aprovisionar dispositivos.

También se pre-aprovisionan dispositivos al crear delegados.


# Formato de pre-aprovisionamiento
Para preaprovisionar dispositivos se requiere un archivo CSV que use como separador el signo ; (punto y coma) y contenga, al menos, los siguientes campos:

* name: Nombre del dispositivo precedido del carácter P; por ejemplo: P0000000100000001
* type: Tipo de dispositivo; por ejemplo V02_001
* nombreOriginal: Nombre original del dispositivo para recuperarlo si el usuario lo resetease; coincidirá con el name (P0000000100000001)
* tipoOriginal: Tipo original del dispositivo para recuperarlo si el usuario lo resetease; coincidirá con el type (V02_001)
* claimingData: Objeto en formato JSON con la clave de reclamación del dispositivo; por ejemplo: {"secretKey": "ABCDEFGHIJKLMNOP", "expirationTime": "1640995200000"}
* claimingDataOriginal: Copia del parámetro anterior porque ThingsBoard elimina el parámetro claimingData cuando el usuario reclama el dispositivo, y necesitamos un modo de recuperar esa información si el usuario borrase el dispositivo.
* devEUI: Si es un dispositivo LoRaWAN, podemos indicar su devEUI de modo que la integración pueda realizarse a través del dispositivo control ~~de cualquier cliente~~ del cliente proveedor (el objetivo es facilitar que un integrador pueda dar de alta los dispositivos en el Network Server, y entregar los datos de aprovisionamiento al cliente para evitarle este trabajo que puede resultar complicado para usuarios poco experimentados, y a la vez reducir la tarea para el proveedor en sistemas como The Things Network (en los que las integraciones son a nivel de aplicación) permitiéndole dar de alta todos los dispositivos en la misma aplicación y con una sola integración a su dispositivo de control en myIoT). 

Opcionalmente, un proveedor puede decidir incluir un numero de créditos con el dispositivo; en este caso se añadirá el campo **credito** (el sistema lo replicará en un atributo llamado **creditoOriginal** antes de ponerlo a cero, para que si se reclama nuevamente no se sume de nuevo el crédito y quede constancia del crédito.

