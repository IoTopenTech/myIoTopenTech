El propietario de un dispositivo puede delegarlo en otros usuarios.

La delegación consiste en crear un dispositivo nuevo relacionado con el dispositivo original a través de una relación de tipo "id_del_delegado" desde el original delegador hacia el dispositivo delegado. Aviso a desarrolladores: Tenemos una relación del mismo tipo desde el customer a cada uno de sus dispositivos; en el caso de un delegado tendríamos 2 relacionea idénticas hacia él: una desde su delegador y otra desde su cliente cuando sea reclamado.

En el dispositivo original delegador se crea el atributo del lado del servidor delegados, con el siguiente formato:

[{"nombre":"D00000001_PuertaDelegada3","reclamado":false,"claimingData":{"secretKey":"7qg55y6qy98lq0as5i34hi2rpy","expirationTime":"1640995200000"},"id":"e0e9b240-c6df-11ea-b825-09d866a6d73c","delegaciones":{}}]

En el dispositivo delegado se crea el atributo del lado del servidor esDelegado con el valor true, y otro llamado delegación que contiene su delegación en particular (ver formato anterior).

El propietario del dispositivo podrá decidir qué telemetrías/funcionalidades permite en el dispositivo delegado.

Los dispositivos delegados no se pueden sub-delegar.

El dispositivo delegado se pre-aprovisiona, para que el segundo usuario pueda reclamarlo con las credenciales que le proporcionará el propietario.

El nombre del dispositivo delegado utilizará como prefijo D00000001_ (D + número de cliente del propietario + _). Al ser reclamado se renombrará con el esquema habitual (idUsuario_nombre), pero seguiremos teniendo el nombre original en el atributo nombreOriginal.

No está permitido cambiar el tipo de los dispositivos delegados ni delegador.

No está permitido borrar un dispositivo que tiene delegados.

Respecto al accounting: hay que encontrar la forma de que quién delega pueda hacerse cargo a perpetuidad de los créditos del dispositivo delegado (ej. una empresa que delega un sensor en su cliente, pero no desea que el cliente tenga que afrontar pagos a myIoT). Al crear el delegado el usuario podría decidir entre que vaya por cuenta suya, o que vaya por cuenta del delegado. Si va por cuenta suya se incrementaría su atributo/telemetría dispositivosAsumidos. No se empezará a cobrar el dispositivo hasta que no haya sido reclamado.

Cuando llega una telemetría al dispositivo Delegador, en la regla del propio tipo de dispositivo, se carga el atributo delegados (ss_delegados) que se mantendrá inalterado durante el envío de las telemetrías a todos los delegados, se duplica en el metadata.delegadosArray del que se irán extrayendo los delegados de uno en uno para enviarles la telemetría, se extrae el primero de los delegados de metadata.delegadosArray y se almacena el metadata.idDelegadoActual. Además se almacena en metadata.msgOriginal el stringifycado del msg original. 

¿Qué debería ocurrir al borrar una delegación? ¿Si la borra el delegado? ¿Si la borra el delegador? De momento, para simplificar, la borre quien la borre (delegador o delegado), se borrará el dispositivo delegado y se les comunicará a los customers por email. Si lo borra el subordinado, podríamos estudiar liberarlo (por si quisiera transferirlo a otro customer), pero si lo borra el principal siempre habría que borrarlo, porque si lo liberásemos el subordinado podría reclamarlo de nuevo (porque conoce las credenciales). Además hay que actualizar el accounting.

Se intentará utilizar la nomenclatura delegador y delegado para referirse a los customers, y principal y subordinado para referirse a los dispositivos.
