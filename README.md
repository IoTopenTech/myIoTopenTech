# To-Do
- [ ] Permitir indicar en los downlinks cómo es el schedulink (replace, first o last)
- [ ] Trasladar la rama Tomar posesión a la cadena de reglas de cada tipo de nodo
- [ ] Permitir que un dispositivo pueda delegar en otro de un cliente diferente. Al delegar se preaprovisiona un nuevo dispositivo delegado vinculado al original mediante una relación de tipo haDelegadoEn. El propietario podrá indicar qué telemetrías, parámetros de configuración configurables por downlink y canales de downlink quiere delegar.
- [ ] Integración con Everynet
- [ ] Skill de Alexa


# ThingsBoard TTN Edition

Las instrucciones siguientes están dirigidas a los usuarios que quieran desarrollar nuevos tipos de activos y dispositivos en My IoT open Tech. Si es su caso, solicite en contacto@iotopentech.io una cuenta de nivel tenant.

Si sólo desea utilizar My IoT open Tech como usuario final, siga las instrucciones de este vídeo:

[![Primeros pasos con My IoT open Tech](http://img.youtube.com/vi/PtA9cxz3UNI/0.jpg)](http://www.youtube.com/watch?v=PtA9cxz3UNI)

Procedimiento para crear el entorno de desarrollo en una cuenta tenant de tb IoT open Tech:

1. Crear un customer cuyo nombre sea un número de 5 dígitos; por ejemplo 00001.
2. Asignar a este customer los siguientes atributos del lado del servidor:
    * tiposDeActivos

        ```
        IMAGE01,MAP01
        ```
    
   * tiposDeDispositivos

        ```
        V02_001,V02_002
        ```
    
   * V02_001_config

        ```html
        <div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);">Geocercado</div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Pol&iacute;gono del geocercado en formato [[lat1,lon1],[lat2,lon2], ... ,[latN,lonN]]</label> <input type="text" size="30" pattern="^\[(\[([-+]?)([\d]{1,2})(((\.)(\d+)(,)))(\s*)(([-+]?)([\d]{1,3})((\.)(\d+))?)\],){2,}(\[([-+]?)([\d]{1,2})(((\.)(\d+)(,)))(\s*)(([-+]?)([\d]{1,3})((\.)(\d+))?)\]){1}\]$" maxlength="1500" ng-model="vm.configuracion.__geocercado"> </md-input-container> </div> </div> </div></div><div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);">Alarmas</div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de geocercado</label> <md-checkbox ng-model="vm.configuracion.__alarmas.geocercado.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.geocercado.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div> <div class="row" layout="row"> <md-input-container class="md-block" style="min-width: 100px;"> <label>Disparar al </label> <md-select ng-disabled="!vm.configuracion.__alarmas.geocercado.enable" ng-required="vm.configuracion.__alarmas.geocercado.enable" name="geocercadoTrigger" ng-model="vm.configuracion.__alarmas.geocercado.trigger"> <md-option value="entrar"> entrar </md-option> <md-option value="salir"> salir </md-option> </md-select> <div ng-messages="editEntityForm.cambioDeEstadoTrigger.$error"> <div ng-message="required">Este dato es obligatorio. </div> </div> </md-input-container> <sustituir-notificaciones class="ng-scope">geocercado</sustituir-notificaciones> </div> </div> </div></div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"><div flex layout="column"> <label class="checkbox-label">Activar alarma de nivel bajo de batería</label> <md-checkbox ng-model="vm.configuracion.__alarmas.nivelDeBateria.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.nivelDeBateria.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div> <div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Umbral (V)</label> <input type="decimal" size="10" ng-disabled="!vm.configuracion.__alarmas.nivelDeBateria.enable " ng-model="vm.configuracion.__alarmas.nivelDeBateria.umbralBateria" ng-required="vm.configuracion.__alarmas.nivelDeBateria.enable"> </md-input-container> <sustituir-notificaciones class="ng-scope">nivelDeBateria</sustituir-notificaciones> </div> </div> </div></div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de inactividad</label> <md-checkbox ng-model="vm.configuracion.__alarmas.inactividad.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.inactividad.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div> <div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Umbral en segundos</label> <input type="number" size="10" ng-disabled="!vm.configuracion.__alarmas.inactividad.enable " ng-model="vm.configuracion.__alarmas.inactividad.umbralInactividad" ng-required="vm.configuracion.__alarmas.inactividad.enable"> </md-input-container> <sustituir-notificaciones class="ng-scope">inactividad</sustituir-notificaciones> </div> </div> </div></div>
        ```
   * IMAGE01_config

        ```html
        <div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div class="row" layout="row"> <md-input-container flex class="md-block"> <label>URL imagen fondo</label> <input type="string" size="50" ng-model="vm.configuracion.__urlImagenFondo" ng-required="true"> </md-input-container> </div> </div> </div> </div>
        ```
    
3. Crear un customer llamado cliente_patron. Utilizaremos este customer para almacenar los atributos de configuración de los distintos tipos de activos y dispositivos (como V02_001_config e IMAGE01_config), de modo que cuando un usuario cree por primera vez una entidad, se obtenga su atributo de configuración copiándolo del cliente_patron. Crear una relación desde el cliente_patron al cliente 00001 cuyo tipo coincida con el id del customer 00001. Actualmente también usamos el cliente_patron para almacenar un atributo llamado accounting en el que se guarda el número de dispositivos activos y créditos de cada cliente a modo de copia de seguridad (en cada customer existen dos atributos llamados dispositivosActivos y credito con esta misma información), o facilidad para acceder a toda la información de contabilidad de un modo más rápido; no obstante, es posible que en versiones posteriores se revise esta el uso de la propiedad accounting y se elimine si no es necesaria. Además de en atributos del customer, los valores de dispositivosActivos y credito se almacenan como telemetría del propio customer para que en el dashboard de configuración pueda mostrarse un gráfico histórico.
4. Crear un activo que comience con el nombre del customer seguido de \_ROOT; por ejemplo: 00001\_ROOT. Este activo nos permitirá seleccionar (para mostrarlos en los widgets de los dashboards) mediante relaciones de tipo CONTAINS todos los demás activos y dispositivos que cree el customer a través del dashboard Configuración.
    * Indicar como tipo del activo: ROOT
    * Crear en el activo los 2 atributos del lado del servidor siguientes:    
        * nombreEntidad
      
            ```
            _ROOT
            ```       
    
        * tipoEntidad
    
            ```
            ROOT
            ```
    
    * Copiar el Asset ID del activo creado (será del tipo ffeabc0-6ffa-121a-b4b5-4bbff7e83283) y crear en el activo una relación de tipo TO hacia el Customer y que utilice este ID como Relation type.
      
5. Crear un dispositivo que comience con el nombre del customer seguido de \_CONTROL; por ejemplo: 00001\_CONTROL. Mediante telemetrías que enviaremos a este dispositivo desde los widgets de los dashboard podremos realizar operaciones de gestión, como crear dispositivos o configurarlos (ver la regla Root Rule Chain).
    * Indicar como tipo del dispositivo SYSTEM.
6. Importar las reglas y los dashboards de este repositorio (no es necesario importar los widgets porque ya están disponibles a nivel de sistema). No es necesario importar todos los dashboards; bastaría con los siguientes (el orden de importación es indiferente):
    * activo_image01
	* configuracion
	* dispositivo_v02_001
	* panel_de_control

Tampoco en necesario importar todas las reglas, bastaría con las siguientes, pero es importante hacerlo en el siguiente orden (durante la importación de la reglas, ThingsBoard pierde la vinculación que hay de unas reglas a otras; concretamente tendrá que revincular manualmente los vínculos de las reglas Root Rule Chain y V02_001; en la regla Root Rule Chain además deberá borrar los nodos que hagan referencia a reglas que no haya importado, como RAK7200):

  1. notificaciones
  2. actualizarconfigdashboardsingular
  3. V02_001
  4. borrarEntidad
  5. editarEntidad
  6. crearEntidad
  7. configurarEntidad
  8. reclamarDispositivo
  9. Root Rule Chain (convertir esta regla en la nueva regla raíz, y borrar la Root Rule Chain original).
 
 Concretamente la regla Root Rule Chain deberá quedar vinculada a las demás reglas del siguiente modo (es posible que tenga que borrar algún nodo si ha decidido no importar todas las reglas/dashboards, sino sólo los indicados como imprescindibles en este documento):
 
 ![](.//media/readme_01.png)
 
 7. Asignar al customer (00001) el activo ROOT, el dispositivo CONTROL y todos los dashboards.

Si desea crear un tipo de dispositivo puede consultar el siguiente documento:

https://github.com/IoTopenTech/MyIoTopenTech/blob/master/crearTipoDispositivo.md
