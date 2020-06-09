# To-Do
- [x] Permitir indicar en los downlinks cómo es el schedule (replace, first o last). Parece que ChirpStack no admite first.
- [ ] Trasladar la rama "Tomar posesión" de la cadena raíz a la cadena de reglas de cada tipo de nodo. Debería estar en el panel Configuración, pero como es muy dependiente del tipo de dispositivo no podría manejarse en la cadena configurarEntidad directamente, sino que habría que pasarla a la cadena del nodo en particular. raiz --> configurarEntidad --> V02_001 --> raiz nuevamente (para enviar los downlinks). También convendría agrupar todo el tema de donwlinks (actualmente en la cadena raíz, pero probablemente requiera su propia cadena a medida que vayamos creando integraciones con más plataformas).
- [ ] Permitir que un dispositivo pueda delegar en otro de un cliente diferente. Al delegar se preaprovisiona un nuevo dispositivo delegado vinculado al original mediante una relación de tipo haDelegadoEn. El propietario podrá indicar qué telemetrías, parámetros de configuración configurables por downlink y canales de downlink quiere delegar.
- [ ] Integración con Everynet
- [ ] Skill de Alexa
