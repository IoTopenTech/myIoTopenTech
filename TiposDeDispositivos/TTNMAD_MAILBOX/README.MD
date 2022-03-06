De los 3 puertos que incluye el RAK3172, el único capaz de entrar en modo de sueño (Deep sleep) es el I2C_3

Para que todos los puertos puedan entrar en Deep sleep tenemos que introducir una modificación en el archivo mbed-os/targets/TARGET_STM/i2c_api.c

https://github.com/ARMmbed/mbed-os/issues/15191

![imagen](https://user-images.githubusercontent.com/52624907/156921889-3a7ad87b-7542-4127-8afc-9b167932b1ab.png)
