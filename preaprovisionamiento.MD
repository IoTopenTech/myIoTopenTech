Los proveedores pueden pre-aprovisionar dispositivos.

También se pre-aprovisionan dispositivos al crear delegados.


# Formato de pre-aprovisionamiento
Para preaprovisionar dispositivos se requiere un archivo CSV que use como separador el signo ; (punto y coma) y contenga, al menos, los siguientes campos:

* name: Nombre del dispositivo precedido del carácter P; por ejemplo: P0000000100000001
* type: Tipo de dispositivo; por ejemplo V02_001
* nombreOriginal: Nombre original del dispositivo para recuperarlo si el usuario lo resetease; coincidirá con el name (P0000000100000001)
* tipoOriginal: Tipo original del dispositivo para recuperarlo si el usuario lo resetease; coincidirá con el type (V02_001)
* claimingData: Objeto en formato JSON con la clave de reclamación del dispositivo; por ejemplo: {"secretKey": "ABCDEFGHIJKLMNOP", "expirationTime": "1640995200000"}
* claimingDataOriginal: Copia del parámetro anterior porque el anterior será borrado si el usuario reclama el dispositivo, y necesitamos un modo de recuperar esa información si el usuario resetease el dispositivo.

Opcionalmente, un proveedor puede decidir incluir un numero de créditos con el dispositivo; en este caso se añadirá el campo **credito** (el sistema lo replicará en un atributo llamado **creditoOriginal** antes de ponerlo a cero, para que si se reclama nuevamente no se sume de nuevo el crédito y quede constancia del crédito.

