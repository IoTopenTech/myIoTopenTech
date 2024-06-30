Este programa ha sido compilado con la versión 4.1.0 del gestor de placas RAK RUI3.

Si utilizas la versión 4.1.1 tendrás que sustituir en la línea 126 la expresión:

api.lorawan.nwm.set(1)

por:

api.lorawan.nwm.set()

Además este programa utiliza las librerías externas siguientes:

* LIS3dh de Sparkfun versión 1.0.3
* CRC de Rob Tillaart versión 1.0.3
