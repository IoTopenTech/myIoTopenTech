Este nodo utiliza SF8 y, a pesar de las mejoras realizadas en la librería MCCI MLIC en lo referente al timing, he comprobado que sigue siendo necesario el uso de:

LMIC_setClockError(MAX_CLOCK_ERROR * 2 / 100);

Concretamente para SF8, parece que un 2% es un valor de error apropiado (algunos usuarios reportan que para SF7 se requiere al menos 5%).

Aparentemente, para que la librería MCCI LMIC acepte estos errores tan grandes (por defecto ignora errores superiores al 0.4%) es necesario incluir la siguiente directiva en el archivo lmic_project_config.json

![imagen](https://user-images.githubusercontent.com/52624907/137908161-caf523df-6cf1-4541-82a0-9887dff27d4f.png)

Resultado de la compilación y versiones de librerías:

![imagen](https://user-images.githubusercontent.com/52624907/137908388-b81c4265-d500-4883-b162-43e5c7798cfa.png)

Captura de consumos en un evento de apertura y cierre de puerta:
![imagen](https://user-images.githubusercontent.com/52624907/137925595-97a66655-37e0-4c18-99ee-e1ae04346eb5.png)


El consumo en estado "dormido" es aproximadamente 12uA.
