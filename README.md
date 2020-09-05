# my IoT open Tech
myIoT (https://my.iotopentech.io) es un sitio web para gestionar y configurar dispositivos IoT.
Algunas de las funciones que podrá realizar con myIoT son:
* Configurar automáticamente sus dispositivos. Con sólo indicar el tipo de dispositivo, myIoT organizará todo lo necesario para que pueda empezar a utilizarlo de forma inmediata.
* Organizar sus dispositivos de forma jerárquica. Por ejemplo, los dispositivos de un hotel podrían organizarse por plantas y habitaciones, o los de una explotación agraria por parcelas.
* Establecer reglas que le envíen notificaciones (por email, Telegram, IFTTT…), por ejemplo, cuando la lectura de un sensor supere cierto umbral, o cuando su nivel de batería sea bajo.
* Conocer el estado de sus dispositivos y comunicarse con ellos a través de atractivos paneles de control. Por ejemplo, podría conocer el estado de un huerto y controlar el riego remotamente.
* Pre-aprovisionar dispositivos para que puedan ser fácilmente reclamados por otros usuarios, que simplemente tendrán que indicar el nombre del dispositivo y su clave para empezar a utilizarlos.
* Delegar sus dispositivos en otros usuarios para que puedan utilizarlos con las limitaciones que decida. Por ejemplo, la dirección de un huerto urbano podría delegar en sus clientes la funcionalidad de monitorización/ actuación sobre sus parcelas (temperatura, humedad del suelo, control del riego…), pero reservarse la información referente al mantenimiento del propio sistema de monitorización/actuación (nivel de las baterías, calidad de la conexión…).

Las instrucciones siguientes están dirigidas a los usuarios que quieran desarrollar nuevos tipos de activos y dispositivos en my IoT open Tech. Si es su caso, solicite en contacto@iotopentech.io una cuenta de nivel tenant.

Si sólo desea utilizar my IoT open Tech como usuario final, siga las instrucciones de este vídeo:

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
        <md-dialog-content> <div class="md-dialog-content" flex> <md-expansion-panel-group> <md-expansion-panel md-component-id="panelGeneral" id="panelGeneral"> <md-expansion-panel-collapsed> <div class="md-title" translate>Configuraci&oacute;n general</div> <div class="md-summary">Configurar atributos de la entidad</div> <md-expansion-panel-icon></md-expansion-panel-icon> </md-expansion-panel-collapsed> <md-expansion-panel-expanded> <md-expansion-panel-header ng-click="$panel.collapse()"> <div class="md-title" translate>Configuraci&oacute;n general</div> <div class="md-summary">Configurar atributos de la entidad</div> <md-expansion-panel-icon></md-expansion-panel-icon> </md-expansion-panel-header> <md-expansion-panel-content> <form name="form.configuracionGeneral" class="configure-entity-form" ng-submit="vm.configurar()"> <sustituir-coordenadas class="ng-scope"></sustituir-coordenadas> <sustituir-chirpstack class="ng-scope"></sustituir-chirpstack> <div class="md-body-1" style="padding-bottom: 10px; color: rgba(0, 0, 0, 0.57);">Alarmas</div> <div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de cambio de estado</label> <md-checkbox ng-model="vm.configuracion.__alarmas.cambioDeEstado.enable" style="margin-bottom: 10px;"> {{(vm.configuracion.__alarmas.cambioDeEstado.enable ? "value.true" : "value.false") | translate}} </md-checkbox> </div> <div class="row" layout="row"> <md-input-container class="md-block" style="min-width: 100px;"> <label>Disparar al </label> <md-select ng-disabled="!vm.configuracion.__alarmas.cambioDeEstado.enable" ng-required="vm.configuracion.__alarmas.cambioDeEstado.enable" name="cambioDeEstadoTrigger" ng-model="vm.configuracion.__alarmas.cambioDeEstado.trigger" > <md-option value="abrir"> abrir </md-option> <md-option value="cerrar"> cerrar </md-option> </md-select> <div ng-messages="editEntityForm.cambioDeEstadoTrigger.$error"><div ng-message="required">Este dato es obligatorio.</div></div> </md-input-container> <sustituir-notificaciones class="ng-scope">cambioDeEstado</sustituir-notificaciones> </div> </div> </div> </div> <div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de nivel bajo de batería</label> <md-checkbox ng-model="vm.configuracion.__alarmas.nivelDeBateria.enable" style="margin-bottom: 10px;"> {{(vm.configuracion.__alarmas.nivelDeBateria.enable ? "value.true" : "value.false") | translate}} </md-checkbox> </div> <div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Umbral (V)</label> <input type="decimal" size="10" ng-disabled="!vm.configuracion.__alarmas.nivelDeBateria.enable " ng-model="vm.configuracion.__alarmas.nivelDeBateria.umbralBateria" ng-required="vm.configuracion.__alarmas.nivelDeBateria.enable" /> </md-input-container> <sustituir-notificaciones class="ng-scope">nivelDeBateria</sustituir-notificaciones> </div> </div> </div> </div> <div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de inactividad</label> <md-checkbox ng-model="vm.configuracion.__alarmas.inactividad.enable" style="margin-bottom: 10px;"> {{(vm.configuracion.__alarmas.inactividad.enable ? "value.true" : "value.false") | translate}} </md-checkbox> </div> <div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Umbral en segundos</label> <input type="number" size="10" ng-disabled="!vm.configuracion.__alarmas.inactividad.enable " ng-model="vm.configuracion.__alarmas.inactividad.umbralInactividad" ng-required="vm.configuracion.__alarmas.inactividad.enable" /> </md-input-container> <sustituir-notificaciones class="ng-scope">inactividad</sustituir-notificaciones> </div> </div> </div> </div> <md-button type="submit" ng-disabled="form.configuracionGeneral.$invalid || !form.configuracionGeneral.$dirty" class="md-raised md-primary"> Configurar </md-button> </form> </md-expansion-panel-content> </md-expansion-panel-expanded> </md-expansion-panel> <md-expansion-panel md-component-id="panelHeartbeat" id="panelHeartbeat"> <md-expansion-panel-collapsed> <div class="md-title" translate>Hearbeat</div> <div class="md-summary">Configurar periodo de envío de heartbeat</div> <md-expansion-panel-icon></md-expansion-panel-icon> </md-expansion-panel-collapsed> <md-expansion-panel-expanded> <md-expansion-panel-header ng-click="$panel.collapse()"> <div class="md-title" translate>Hearbeat</div> <div class="md-summary">Configurar periodo de envío de heartbeat</div> <md-expansion-panel-icon></md-expansion-panel-icon> </md-expansion-panel-header> <md-expansion-panel-content> <form name="form.configuracionDownlink" class="configure-entity-form" ng-submit="vm.configurar()"> <div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Número de minutos entre heartbeats</label> <input type="number" size="10" step="1" min="0" max="60" ng-model="vm.configuracion.___heartbeat" /> <md-button type="submit" ng-disabled="form.configuracionDownlink.$invalid || !form.configuracionDownlink.$dirty" class="md-raised md-primary" ng-click="vm.configuracion.___ultimoDownlink='heartbeat'"> Configurar </md-button> </md-input-container> </div> </form> </md-expansion-panel-content> </md-expansion-panel-expanded> </md-expansion-panel> <md-expansion-panel md-component-id="panelTomarPosesion" id="panelTomarPosesion" ng-if="vm.entityType.toLowerCase()=='device' && vm.attributes.hasOwnProperty('apropiable') && vm.attributes.apropiable==true"> <md-expansion-panel-collapsed> <div class="md-title" translate>Credenciales LoRaWAN</div> <div class="md-summary">Configurar las credenciales LoRaWAN del dispositivo</div> <md-expansion-panel-icon></md-expansion-panel-icon> </md-expansion-panel-collapsed> <md-expansion-panel-expanded> <md-expansion-panel-header ng-click="$panel.collapse()"> <div class="md-title" translate>Credenciales LoRaWAN</div> <div class="md-summary">Configurar las credenciales LoRaWAN del dispositivo</div> <md-expansion-panel-icon></md-expansion-panel-icon> </md-expansion-panel-header> <md-expansion-panel-content> <form name="form.configuracionEspecifica" class="configure-entity-form" ng-submit="vm.configurar()"> <div class="md-dialog-content"> <div layout="row"> <md-input-container flex class="md-block" style="min-width: 100px; width: 150px;"> <label>Método de activación</label> <md-select ng-model="vm.configuracion.___tomarPosesionMetodoActivacion" required> <md-option ng-if="vm.attributes.hasOwnProperty('admiteABP') && vm.attributes.admiteABP==true" value="A" ng-selected=""> ABP </md-option> <md-option value="O" ng-selected="true"> OTAA </md-option> </md-select> </md-input-container> </div> <div layout="row"> <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='A' " class="md-block" style="min-width: 100px; width: 150px;"> <label>Device Address (msb)</label> <input type="text" pattern="[0-9a-fA-F]{8}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam1" required /> </md-input-container> </div> <div layout="row"> <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='A'" class="md-block" style="min-width: 100px; width: 150px;"> <label>Network Session Key (msb)</label> <input type="text" pattern="[0-9a-fA-F]{32}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam2" required /> </md-input-container> </div> <div layout="row"> <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='A' " class="md-block" style="min-width: 100px; width: 150px;"> <label>Application Session Key (msb)</label> <input type="text" pattern="[0-9a-fA-F]{32}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam3" required /> </md-input-container> </div> <div layout="row"> <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='O' " class="md-block" style="min-width: 100px; width: 150px;"> <label>Device EUI (msb)</label> <input type="text" pattern="[0-9a-fA-F]{16}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam1" name="vm.configuracion.___tomarPosesionParam1" required /> </md-input-container> </div> <div layout="row"> <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='O'" class="md-block" style="min-width: 100px; width: 150px;"> <label>Application EUI (msb)</label> <input type="text" pattern="[0-9a-fA-F]{16}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam2" required /> </md-input-container> </div> <div layout="row"> <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='O'" class="md-block" style="min-width: 100px; width: 150px;"> <label>Application Key (msb)</label> <input type="text" pattern="[0-9a-fA-F]{32}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam3" required /> </md-input-container> </div> <md-button type="submit" ng-disabled="form.configuracionEspecifica.$invalid || !form.configuracionEspecifica.$dirty" class="md-raised md-primary" ng-click="vm.configuracion.___ultimoDownlink='tomarPosesion'"> Configurar </md-button> </div> </form> </md-expansion-panel-content> </md-expansion-panel-expanded> </md-expansion-panel> </md-expansion-panel-group> </div></md-dialog-content><md-dialog-actions> <md-button ng-click="vm.cancel()" class="md-primary">Cancelar </md-button></md-dialog-actions>
        ```
   * IMAGE01_config

        ```html
        <div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div class="row" layout="row"> <md-input-container flex class="md-block"> <label>URL imagen fondo</label> <input type="string" size="50" ng-model="vm.configuracion.__urlImagenFondo" ng-required="true"> </md-input-container> </div> </div> </div> </div>
        ```
    
3. Crear un customer llamado cliente_patron. Utilizaremos este customer para almacenar los atributos de configuración de los distintos tipos de activos y dispositivos (como V02_001_config e IMAGE01_config), de modo que cuando un usuario cree por primera vez una entidad, se obtenga su atributo de configuración copiándolo del cliente_patron. Crear una relación desde el cliente_patron al cliente 00001 cuyo tipo coincida con el id del customer 00001. Actualmente también usamos el cliente_patron para almacenar un atributo llamado accounting en el que se guarda el número de dispositivos activos y créditos de cada cliente a modo de copia de seguridad (en cada customer existen dos atributos llamados dispositivosActivos y credito con esta misma información), o facilidad para acceder a toda la información de contabilidad de un modo más rápido; ~~no obstante, es posible que en versiones posteriores se revise esta el uso de la propiedad accounting y se elimine si no es necesaria~~ adicionalmente, el accounting del cliente_patron también se utiliza en la reglas actualizarCustomers_for y actualizarCustomers_next para obtener un listado de todos los clientes y poder así actualizar atributos y asignar dashboards en todo el sistema. Además de en atributos del customer, los valores de dispositivosActivos y credito se almacenan como telemetría del propio customer para que en el dashboard de configuración pueda mostrarse un gráfico histórico.
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

  1. enviarDownlink
  2. notificaciones
  3. actualizarConfigDashboardSingular
  4. V02_001
  5. borrarEntidad
  6. editarEntidad
  7. crearEntidad
  8. configurarEntidad
  9. reclamarDispositivo
  10. Root Rule Chain (convertir esta regla en la nueva regla raíz, y borrar la Root Rule Chain original).
 
 Concretamente la regla Root Rule Chain deberá quedar vinculada a las demás reglas del siguiente modo (es posible que tenga que borrar algún nodo si ha decidido no importar todas las reglas/dashboards, sino sólo los indicados como imprescindibles en este documento):
 
 ![](.//media/readme_01.png)
 
 7. Asignar al customer (00001) el activo ROOT, el dispositivo CONTROL y todos los dashboards.

Si desea crear un tipo de dispositivo puede consultar el siguiente documento:

https://github.com/IoTopenTech/MyIoTopenTech/blob/master/crearTipoDispositivo.md
