El propietario de un dispositivo puede delegarlo en otros usuarios.

La delegación consiste en crear un dispositivo nuevo relacionado con el dispositivo original a través de una relación de tipo "DelegaEn" desde el original hacia el dispositivo delegado.

En el dispositivo original se crea el atributo del lado del servidor delegados, con el siguiente formato:

[{"nombre":"D00000001_PuertaDelegada", "estado": "sinReclamar", "delegaciones":[...lista de telemetrías/atributos delegados...]}]

En el dispositivo delegado se crea el atributo del lado del servidor esDelegado con el valor true;

El propietario del dispositivo podrá decidir qué telemetrías/atributos permite en el dispositivo delegado.

Los dispositivos delegados no se pueden sub-delegar.

El dispositivo delegado se pre-aprovisiona, para que el segundo usuario pueda reclamarlo con las credenciales que le proporcionará el propietario.

El nombre del dispositivo delegado utilizará como prefijo D00000001_ (D + número de cliente del propietario + _)

Si se cambia el tipo del dispositivo original habrá que cambiar también el de los delegados.

Respecto al accounting: hay que encontrar la forma de que quién delega pueda hacerse cargo a perpetuidad de los créditos del dispositivo delegado (ej. una empresa que delega un sensor en su cliente, pero no desea que el cliente tenga que afrontar pagos a myIoT). Al crear el delegado el usuario podría decidir entre cederle un crédito inicial, que vaya por cuenta suya, o que vaya por cuenta del delegado. Si va por cuenta suya se incrementaría su atributo/telemetría dispositivosConCargo.
