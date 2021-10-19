Este nodo utiliza SF8 y, a pesar de las mejoras realizadas en la librería MCCI MLIC en lo referente al timing, he comprobado que sigue siendo necesario el uso de:

LMIC_setClockError(MAX_CLOCK_ERROR * 2 / 100);

Concretamente para SF8, parece que un 2% es un valor de error apropiado (algunos usuarios reportan que para SF7 se requiere al menos 5%).

Aparentemente, para que la librería MCCI LMIC acepte estos errores tan grandes (por defecto ignora errores superiores al 0.4%) es necesario incluir la siguiente directiva en el archivo lmic_project_config.json

![imagen](https://user-images.githubusercontent.com/52624907/137908161-caf523df-6cf1-4541-82a0-9887dff27d4f.png)
