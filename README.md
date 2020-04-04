# ThingsBoard TTN Edition

Procedimiento (todo esto se realizará automáticamente en breve gracias a la colaboración de todos, pero de momento hay que hacerlo a mano):

1. Crear un customer cuyo nombre sea un número de 5 dígitos; por ejemplo 00001.
2. Asignar a este customer los siguientes atributos del lado del servidor:
   * tiposDeActivos

        ```
        MAP01,IMAGE01
        ```
    
   * tiposDeDispositivos

        ```
        V02_001,V02_002
        ```
    
   * V02_001_config

        ```
        <div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);"> Alarmas</div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de cambio de estado</label> <md-checkbox ng-model="vm.attributes.alarmas.cambioDeEstado.enable" style="margin-bottom: 10px;">{{(vm.attributes.alarmas.cambioDeEstado.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div><div class="row" layout="row"> <md-input-container class="md-block" style="min-width: 100px;"> <label>Disparar al </label> <md-select ng-disabled="!vm.attributes.alarmas.cambioDeEstado.enable" ng-required="vm.attributes.alarmas.cambioDeEstado.enable" name="cambioDeEstadoTrigger" ng-model="vm.attributes.alarmas.cambioDeEstado.trigger"> <md-option value="abrir"> abrir </md-option> <md-option value="cerrar"> cerrar </md-option> </md-select> <div ng-messages="editEntityForm.cambioDeEstadoTrigger.$error"> <div ng-message="required">Este dato es obligatorio. </div></div></md-input-container> <sustituir-notificaciones class="ng-scope">cambioDeEstado</sustituir-notificaciones> </div></div></div></div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de nivel bajo de batería</label> <md-checkbox ng-model="vm.attributes.alarmas.nivelDeBateria.enable" style="margin-bottom: 10px;">{{(vm.attributes.alarmas.nivelDeBateria.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div><div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Umbral (V)</label> <input type="decimal" size="10" ng-disabled="!vm.attributes.alarmas.nivelDeBateria.enable " ng-model="vm.attributes.alarmas.nivelDeBateria.umbralBateria" ng-required="vm.attributes.alarmas.nivelDeBateria.enable" > </md-input-container> <sustituir-notificaciones class="ng-scope">nivelDeBateria</sustituir-notificaciones> </div></div></div></div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de inactividad</label> <md-checkbox ng-model="vm.attributes.alarmas.inactividad.enable" style="margin-bottom: 10px;">{{(vm.attributes.alarmas.inactividad.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div><div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Umbral en segundos</label> <input type="number" size="10" ng-disabled="!vm.attributes.alarmas.inactividad.enable " ng-model="vm.attributes.alarmas.inactividad.umbralInactividad" ng-required="vm.attributes.alarmas.inactividad.enable" > </md-input-container> <sustituir-notificaciones class="ng-scope">inactividad</sustituir-notificaciones> </div></div></div></div>
        ```
    
3. Crear un activo que comience con el nombre del customer seguido de \_ROOT; por ejemplo: 00001\_ROOT. Este activo nos permitirá seleccionar mediante relaciones de tipo CONTAINS todos los demás activos y dispositivos que cree el customer a través del dashboard Configuración.
  * Indicar como tipo del activo: ROOT
  * Crear en el activo los 2 atributos del lado del servidor siguientes:    
    * nombreEntidad
      
      ```
      ROOT
      ```       
    
    * tipoEntidad
    
      ```
      ROOT
      ```
    
  * Copiar el Asset ID del activo creado (será del tipo ffeabc0-6ffa-121a-b4b5-4bbff7e83283) y crear en el activo una relación de tipo TO hacia el Customer y que utilice este ID como Relation type.
      
4. Crear un dispositivo que comience con el nombre del customer seguido de \_CONTROL; por ejemplo: 00001\_CONTROL. Mediante telemetrías que enviaremos a este dispositivo desde los widgets de los dashboard podremos realizar operaciones de gestión, como crear dispositivos o configurarlos (ver la regla Root Rule Chain).
    * Indicar como tipo del dispositivo SYSTEM.
5. Importar las reglas y los dashboards de este repositorio (no es necesario importar los widgets porque ya están disponibles a nivel de sistema). Al importar las reglas, hacerlo en el siguiente orden:
    1. notificaciones
    2. V02_001
    3. borrarEntidad
    4. editarEntidad
    5. crearEntidad
    6. configurarEntidad
    7. Root Rule Chain (convertir esta regla en la nueva regla raíz, y borrar la Root Rule Chain original).

