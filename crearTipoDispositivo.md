> Se procurará aplicar la metodología KISS en el desarrollo de nuevos tipos de dispositivos. Téngase en cuenta que deberán ser revisados por la autoridad antes de integrarlos en el sistema, y ésta podría rechazarlos por excesiva complejidad.

Actualmente un tipo de dispositivo en myIoT está compuesto por los siguientes elementos (puede utilizarse como referencia el tipo de dispositivo V02_001):

* Una cadena de reglas para gestionar la funcionalidad del dispositivo, en la que generalmente se recibirán 4 tipos de mensajes:

  * Telemetrías, que deberán almacenarse y someterse a las condiciones de disparo de las alarmas configuradas por el usuario.
  
    * Para dispositivos LoRaWAN, se almacenan al menos los siguientes valores: app_id, dev_id, hardware_serial, counter, port, payload_raw y metadata. La información de downlink se gestiona por separado para cada servidor de red: TTN, CS, Everynet, Helium...
  
  * Alarmas de inactividad, que son directamente gestionadas por ThingsBoard.
  * Solicitudes para enviar un downlink que permita configurar un atributo del dispositivo (por ejemplo, borrar un contador, o cambiar la frecuencia de heartbeats).
  * Petición de guardar un atributo "disfrazada" de telemetría. Al configurar algunas funciones de los dispositivos, por ejemplo el periodo de heartbeat del tipo de dispositivo V02_001, se le envía un downlink con la orden, pero además se almacena el valor en un atributo. Si se trata de un dispositivo con subordinados, habrá que actualizar ese atributo en el delegador y todos los delegados que tengan permitida esa funcionalidad.
  
* Un dashboard que cumplirá 2 misiones:

  * Representar las telemetrías.
  * Ofrecer al usuario opciones de interacción con el dispositivo.

> En principio habrá un único dashboard por dispositivo, que servirá tanto para el usuario propietario como para los delegados. En el futuro podría estudiarse crear dashboards específicos para cada tipo de delegación.

* Un bloque de código de configuración que permita al usuario establecer los parámetros del dispositivo, como sus coordenadas sobre un activo de tipo IMAGE_01, su frecuencia de heartbeast, o sus alarmas.

* Un bloque de código de delegación que permita al propietario establecer qué funcionalidades de su dispositivo desea delegar en otro usuario.

El procedimiento general para desarrollar un tipo de dispositivo nuevo sería:

1.  Notificar que quieres integrar un tipo de dispositivo nuevo para que
    se te asigne un [identificador] de dispositivo, del tipo VXX\_XXX (por
    ejemplo, V02\_002).

2.  Crear un dashboard para tu tipo de dispositivo llamado
    Dispositivo\_V02\_002. Puedes usar como referencia el del nodo
    V02\_001 que hay en el repositorio
    ([[https://github.com/IoTopenTech/ThingsBoard-TTN-Edition/tree/master/dashboards]](https://github.com/IoTopenTech/ThingsBoard-TTN-Edition/tree/master/dashboards)).
    El sistema está preparado para que cuando el usuario haga clic en
    el dashboard \"Panel de control\" sobre un dispositivo de tipo
    [identificador] se abra automáticamente el dashboard
    \"Dispositivo\_[identificador]\", que es el que tú vas a crear.
    Asignar el dashboard al customer.
    

3.  Crear un bloque de HTML para la configuración del dispositivo. Por
    ejemplo, para configurar el periodo de tiempo de los heartbeat, las coordenadas, o los umbrales de las alarmas. Este bloque
    de código se almacenará en un atributo de cada customer con el
    nombre [identificador]\_config; por ejemplo, V02\_002\_config, y el sistema lo tomará de ahí para
    mostrarlo en el panel de configuración de este nodo. Cada parámetro será una propiedad del objeto vm.configuracion.
    Existen 3 tipos de parámetros configurables:
    
    * Los que simplemente se almacenan en un atributo del dispositivo, que deben empezar con doble guión bajo, como __xPos o __yPos que se utilizan para posicionar un dispositivo en un widget de tipo IMAGE.
    * Los de alarmas, que se agrupan dentro del atributo __alarmas, como __alarmas.cambioDeEstado, para distinguirlos de los demás y poder gestionarlos en las reglas para determinar si es necesario enviar algún tipo de notificación.
    * Los que requieren un tratamiento adicional específico para cada tipo de entidad, como enviar un downlink, que deben empezar con triple guión bajo, como ___fechaHora. Estos parámetros se gestionan individualmente, es decir, el usuario sólo puede cambiarlos de uno en uno (esto se enfatiza en el cuadro de diálogo de configuración con paneles desplegables para cada uno de ellos; cuando el usuario despliega uno se pliegan todos los demás). Su gestión depende de la existencia de un parámetro llamado ___ultimoDownlink, que indica qué atributo quiere cambiar el usuario. Cuando la cadena de reglas configurarEntidad encuentra el atributo ___ultimoDownlink, envía todos los parámetros con triple guión bajo como un mensaje de telemetría a la cadena de reglas raíz. En la cadena de reglas raíz se detecta la presencia del atributo ___ultimoDownlink y se redirige el mensaje a la cadena de reglas de la entidad específica (por ejemplo, a la cadena V02_001). En la cadena de reglas específica se gestiona la operación, por ejemplo, realizando los cambios de unidades o de codificacion (HEX, DEC, BIN...), enviando los downlinks correspondientes y guardando los atributos (por ejemplo ___heartbeat).
    
> Debemos tener en cuenta que si almacenamos en un atributo un valor que puede convertirse en un entero, ThingsBoard lo convertirá en un entero aunque no lo sea; por ejemplo, si queremos almacenar la cadena de texto "08", ThingsBoard la almacenará en el atributo como el entero 8.

![](.//media/image1.png)

A continuación, se muestra el bloque de código correspondiente al tipo
de nodo LHT65 que puede servir como referencia:

```html
<funciones>
    [{"nombre":"inicializacion","codigo":"$scope.vm.configuracion.___fechaHora=new Date($scope.vm.configuracion.___fechaHora);"}]
</funciones>
<md-dialog-content>
    <div class="md-dialog-content" flex>
        <md-expansion-panel-group>
            <md-expansion-panel md-component-id="panelGeneral" id="panelGeneral">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Configuraci&oacute;n general</div>
                    <div class="md-summary">Configurar atributos de la entidad</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Configuraci&oacute;n general</div>
                        <div class="md-summary">Configurar atributos de la entidad</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionGeneral" class="configure-entity-form" ng-submit="vm.configurar()">
                            <sustituir-coordenadas class="ng-scope"></sustituir-coordenadas>
                            <sustituir-chirpstack class="ng-scope"></sustituir-chirpstack>
                            <div class="md-body-1" style="padding-bottom: 10px; color: rgba(0, 0, 0, 0.57);">Alarmas</div>
                            <div class="body md-whiteframe-1dp" style="margin-bottom: 25px;">
                                <label class="checkbox-label">Activar alarma de temperatura</label>
                                <md-checkbox ng-model="vm.configuracion.__alarmas.temperatura.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.temperatura.enable ? "value.true" : "value.false") | translate}}</md-checkbox>
                                <md-content class="md-padding" layout-xs="column" layout="row">
                                    <div flex-xs flex-gt-xs="50" layout="column">
                                        <md-card>
                                            <div flex layout="column">
                                                Configuraci&oacute;n
                                                <md-input-container flex class="md-block">
                                                    <label>Umbral Tª mínima (ºC): </label>
                                                    <input
                                                        type="decimal"
                                                        size="5"
                                                        autocomplete="off"
                                                        ng-disabled="!vm.configuracion.__alarmas.temperatura.enable "
                                                        ng-model="vm.configuracion.__alarmas.temperatura.umbralMinima"
                                                        ng-required="vm.configuracion.__alarmas.temperatura.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Histéresis Tª mínima (ºC): </label>
                                                    <input
                                                        type="decimal"
                                                        autocomplete="off"
                                                        size="5"
                                                        ng-disabled="!vm.configuracion.__alarmas.temperatura.enable "
                                                        ng-model="vm.configuracion.__alarmas.temperatura.histeresisMinima"
                                                        ng-required="vm.configuracion.__alarmas.temperatura.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Umbral Tª máxima (ºC): </label>
                                                    <input
                                                        type="decimal"
                                                        size="5"
                                                        autocomplete="off"
                                                        ng-disabled="!vm.configuracion.__alarmas.temperatura.enable "
                                                        ng-model="vm.configuracion.__alarmas.temperatura.umbralMaxima"
                                                        ng-required="vm.configuracion.__alarmas.temperatura.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Histéresis Tª máxima (ºC): </label>
                                                    <input
                                                        type="decimal"
                                                        autocomplete="off"
                                                        size="5"
                                                        ng-disabled="!vm.configuracion.__alarmas.temperatura.enable "
                                                        ng-model="vm.configuracion.__alarmas.temperatura.histeresisMaxima"
                                                        ng-required="vm.configuracion.__alarmas.temperatura.enable"
                                                    />
                                                </md-input-container>
                                            </div>
                                        </md-card>
                                    </div>
                                    <div flex-xs flex-gt-xs="50" layout="column"><sustituir-notificaciones class="ng-scope">temperatura</sustituir-notificaciones></div>
                                </md-content>
                            </div>
                            <div class="body md-whiteframe-1dp" style="margin-bottom: 25px;">
                                <label class="checkbox-label">Activar alarma de humedad</label>
                                <md-checkbox ng-model="vm.configuracion.__alarmas.humedad.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.humedad.enable ? "value.true" : "value.false") | translate}}</md-checkbox>
                                <md-content class="md-padding" layout-xs="column" layout="row">
                                    <div flex-xs flex-gt-xs="50" layout="column">
                                        <md-card>
                                            <div flex layout="column">
                                                Configuraci&oacute;n
                                                <md-input-container flex class="md-block">
                                                    <label>Umbral humedad mínima (%): </label>
                                                    <input
                                                        type="decimal"
                                                        size="5"
                                                        autocomplete="off"
                                                        ng-disabled="!vm.configuracion.__alarmas.humedad.enable "
                                                        ng-model="vm.configuracion.__alarmas.humedad.umbralMinima"
                                                        ng-required="vm.configuracion.__alarmas.humedad.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Histéresis humedad mínima (%): </label>
                                                    <input
                                                        type="decimal"
                                                        autocomplete="off"
                                                        size="5"
                                                        ng-disabled="!vm.configuracion.__alarmas.humedad.enable "
                                                        ng-model="vm.configuracion.__alarmas.humedad.histeresisMinima"
                                                        ng-required="vm.configuracion.__alarmas.humedad.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Umbral humedad máxima (%): </label>
                                                    <input
                                                        type="decimal"
                                                        size="5"
                                                        autocomplete="off"
                                                        ng-disabled="!vm.configuracion.__alarmas.humedad.enable "
                                                        ng-model="vm.configuracion.__alarmas.humedad.umbralMaxima"
                                                        ng-required="vm.configuracion.__alarmas.humedad.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Histéresis humedad máxima (%): </label>
                                                    <input
                                                        type="decimal"
                                                        autocomplete="off"
                                                        size="10"
                                                        ng-disabled="!vm.configuracion.__alarmas.humedad.enable "
                                                        ng-model="vm.configuracion.__alarmas.humedad.histeresisMaxima"
                                                        ng-required="vm.configuracion.__alarmas.humedad.enable"
                                                    />
                                                </md-input-container>
                                            </div>
                                        </md-card>
                                    </div>
                                    <div flex-xs flex-gt-xs="50" layout="column"><sustituir-notificaciones class="ng-scope">humedad</sustituir-notificaciones></div>
                                </md-content>
                            </div>
                            <div class="body md-whiteframe-1dp" style="margin-bottom: 25px;">
                                <label class="checkbox-label">Activar alarma de nivel bajo de batería</label>
                                <md-checkbox ng-model="vm.configuracion.__alarmas.nivelDeBateria.enable" style="margin-bottom: 10px;">
                                    {{(vm.configuracion.__alarmas.nivelDeBateria.enable ? "value.true" : "value.false") | translate}}
                                </md-checkbox>
                                <md-content class="md-padding" layout-xs="column" layout="row">
                                    <div flex-xs flex-gt-xs="50" layout="column">
                                        <md-card>
                                            <div flex layout="column">
                                                Configuraci&oacute;n
                                                <md-input-container flex class="md-block">
                                                    <label>Umbral (V)</label>
                                                    <input
                                                        type="decimal"
                                                        size="10"
                                                        ng-disabled="!vm.configuracion.__alarmas.nivelDeBateria.enable "
                                                        ng-model="vm.configuracion.__alarmas.nivelDeBateria.umbralBateria"
                                                        ng-required="vm.configuracion.__alarmas.nivelDeBateria.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Histéresis</label>
                                                    <input
                                                        type="decimal"
                                                        autocomplete="off"
                                                        size="10"
                                                        ng-disabled="!vm.configuracion.__alarmas.nivelDeBateria.enable "
                                                        ng-model="vm.configuracion.__alarmas.nivelDeBateria.histeresis"
                                                        ng-required="vm.configuracion.__alarmas.nivelDeBateria.enable"
                                                    />
                                                </md-input-container>
                                            </div>
                                        </md-card>
                                    </div>
                                    <div flex-xs flex-gt-xs="50" layout="column"><sustituir-notificaciones class="ng-scope">nivelDeBateria</sustituir-notificaciones></div>
                                </md-content>
                            </div>
                            <div class="body md-whiteframe-1dp" style="margin-bottom: 25px;">
                                <label class="checkbox-label">Activar alarma de sensor externo</label>
                                <md-checkbox ng-model="vm.configuracion.__alarmas.sensorExterno.enable" style="margin-bottom: 10px;">
                                    {{(vm.configuracion.__alarmas.sensorExterno.enable ? "value.true" : "value.false") | translate}}
                                </md-checkbox>
                                <md-content class="md-padding" layout-xs="column" layout="row">
                                    <div flex-xs flex-gt-xs="50" layout="column">
                                        <md-card>
                                            <div flex layout="column">
                                                Configuraci&oacute;n
                                                <md-input-container flex class="md-block">
                                                    <label>Umbral de mínimo: </label>
                                                    <input
                                                        type="decimal"
                                                        size="10"
                                                        autocomplete="off"
                                                        ng-disabled="!vm.configuracion.__alarmas.sensorExterno.enable "
                                                        ng-model="vm.configuracion.__alarmas.sensorExterno.umbralMinima"
                                                        ng-required="vm.configuracion.__alarmas.sensorExterno.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Histéresis de mínimo: </label>
                                                    <input
                                                        type="decimal"
                                                        autocomplete="off"
                                                        size="10"
                                                        ng-disabled="!vm.configuracion.__alarmas.sensorExterno.enable "
                                                        ng-model="vm.configuracion.__alarmas.sensorExterno.histeresisMinima"
                                                        ng-required="vm.configuracion.__alarmas.sensorExterno.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Umbral de maximo: </label>
                                                    <input
                                                        type="decimal"
                                                        size="10"
                                                        autocomplete="off"
                                                        ng-disabled="!vm.configuracion.__alarmas.sensorExterno.enable "
                                                        ng-model="vm.configuracion.__alarmas.sensorExterno.umbralMaxima"
                                                        ng-required="vm.configuracion.__alarmas.sensorExterno.enable"
                                                    />
                                                </md-input-container>
                                                <md-input-container flex class="md-block">
                                                    <label>Histéresis de máximo: </label>
                                                    <input
                                                        type="decimal"
                                                        autocomplete="off"
                                                        size="10"
                                                        ng-disabled="!vm.configuracion.__alarmas.sensorExterno.enable "
                                                        ng-model="vm.configuracion.__alarmas.sensorExterno.histeresisMaxima"
                                                        ng-required="vm.configuracion.__alarmas.sensorExterno.enable"
                                                    />
                                                </md-input-container>
                                            </div>
                                        </md-card>
                                    </div>
                                    <div flex-xs flex-gt-xs="50" layout="column"><sustituir-notificaciones class="ng-scope">sensorExterno</sustituir-notificaciones></div>
                                </md-content>
                            </div>
                            <div class="body md-whiteframe-1dp" style="margin-bottom: 25px;">
                                <label class="checkbox-label">Activar alarma de inactividad</label>
                                <md-checkbox ng-model="vm.configuracion.__alarmas.inactividad.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.inactividad.enable ? "value.true" : "value.false") | translate}}</md-checkbox>
                                <md-content class="md-padding" layout-xs="column" layout="row">
                                    <div flex-xs flex-gt-xs="50" layout="column">
                                        <md-card>
                                            <div flex layout="column">
                                                Configuraci&oacute;n
                                                <md-input-container flex class="md-block">
                                                    <label>Umbral en segundos</label>
                                                    <input
                                                        type="number"
                                                        size="10"
                                                        ng-disabled="!vm.configuracion.__alarmas.inactividad.enable "
                                                        ng-model="vm.configuracion.__alarmas.inactividad.umbralInactividad"
                                                        ng-required="vm.configuracion.__alarmas.inactividad.enable"
                                                    />
                                                </md-input-container>
                                            </div>
                                        </md-card>
                                    </div>
                                    <div flex-xs flex-gt-xs="50" layout="column"><sustituir-notificaciones class="ng-scope">inactividad</sustituir-notificaciones></div>
                                </md-content>
                            </div>
                            <md-button type="submit" ng-disabled="form.configuracionGeneral.$invalid || !form.configuracionGeneral.$dirty" class="md-raised md-primary">
                                Configurar
                            </md-button>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>
            <md-expansion-panel md-component-id="panelHeartbeat" id="panelHeartbeat">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Periodo de transmisi&oacute;n en segundos</div>
                    <div class="md-summary">Configurar Periodo de transmisi&oacute;n en segundos</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Periodo de transmisi&oacute;n en segundos</div>
                        <div class="md-summary">Configurar Periodo de transmisi&oacute;n en segundos</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionPeriodo" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="row" layout="row">
                                <md-input-container style="margin: 0px; margin-top: 10px;">
                                    <label>Periodo de transmisi&oacute;n</label> <input type="number" step="1" min="0" max="‭167772165" ng-model="vm.configuracion.___periodo" />

                                    <md-button type="submit" ng-disabled="form.configuracionPeriodo.$invalid || !form.configuracionPeriodo.$dirty" class="md-raised md-primary" ng-click="vm.configuracion.___ultimoDownlink='periodo'">
                                        Configurar
                                    </md-button>
                                </md-input-container>
                            </div>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>

            <md-expansion-panel md-component-id="panelResetear" id="panelResetear">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Resetear</div>
                    <div class="md-summary">Resetear dispositivo</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Resetear</div>
                        <div class="md-summary">Resetear dispositivo</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionResetear" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="row" layout="row">
                                <md-input-container style="margin: 0px; margin-top: 10px;">
                                    <md-input-container style="margin: 0px; margin-top: 10px;">
                                        <md-button class="md-raised md-warn" type="submit" ng-click="vm.configuracion.___ultimoDownlink='resetear'">Resetear</md-button>
                                    </md-input-container>
                                </md-input-container>
                            </div>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>

            <md-expansion-panel md-component-id="panelconfirmacionUplink" id="panelconfirmacionUplink">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Confirmación de uplinks</div>
                    <div class="md-summary">Enviar uplinks con o sin ACK</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Confirmación de uplinks</div>
                        <div class="md-summary">Enviar uplinks con o sin ACK</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionConfirmacionUplink" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="row" layout="row">
                                <md-select ng-model="vm.configuracion.___confirmarUplinks" placeholder="Confirmación de uplink" class="md-no-underline">
                                    <md-option value="activada">Activar</md-option> <md-option value="desactivada">Desactivar</md-option>
                                </md-select>
                            </div>
                            <md-button
                                type="submit"
                                ng-disabled="form.configuracionConfirmacionUplink.$invalid || !form.configuracionConfirmacionUplink.$dirty"
                                class="md-raised md-primary"
                                ng-click="vm.configuracion.___ultimoDownlink='confirmacionUplink'"
                            >
                                Configurar
                            </md-button>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>

            <md-expansion-panel md-component-id="panelSubbanda" id="panelSubbanda">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Configurar Sub-banda</div>
                    <div class="md-summary">Sólo para US915, AU915, y CN470; no para Europa</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Configurar Sub-banda</div>
                        <div class="md-summary">Sólo para US915, AU915, y CN470; no para Europa</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionSubbanda" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="row" layout="row">
                                <md-select ng-model="vm.configuracion.___subbanda" placeholder="Subbanda" class="md-no-underline">
                                    <md-option value="0">0</md-option> <md-option value="1">1</md-option> <md-option value="2">2</md-option> <md-option value="3">3</md-option> <md-option value="4">4</md-option>
                                    <md-option value="5">5</md-option> <md-option value="6">6</md-option> <md-option value="7">7</md-option> <md-option value="8">8</md-option>
                                </md-select>
                            </div>
                            <md-button type="submit" ng-disabled="form.configuracionSubbanda.$invalid || !form.configuracionSubbanda.$dirty" class="md-raised md-primary" ng-click="vm.configuracion.___ultimoDownlink='subbanda'">
                                Configurar
                            </md-button>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>
            <md-expansion-panel md-component-id="panelFechaHora" id="panelFechaHora">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Fecha y hora</div>
                    <div class="md-summary">Configurar fecha y hora</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Fecha y hora</div>
                        <div class="md-summary">Configurar fecha y hora</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionFechaHora" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="row" layout="row">
                                <section layout="row" layout-align="start start">
                                    <mdp-date-picker ng-model="vm.configuracion.___fechaHora" mdp-placeholder="Fecha"></mdp-date-picker>
                                    <mdp-time-picker ng-model="vm.configuracion.___fechaHora" mdp-placeholder="Hora" mdp-auto-switch="true"></mdp-time-picker>
                                </section>
                            </div>
                            <md-button type="submit" ng-disabled="form.configuracionFechaHora.$invalid || !form.configuracionFechaHora.$dirty" class="md-raised md-primary" ng-click="vm.configuracion.___ultimoDownlink='fechaHora'">
                                Configurar
                            </md-button>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>

            <md-expansion-panel md-component-id="panelSensorExterno" id="panelSensorExterno">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Sensor externo</div>
                    <div class="md-summary">Elegir el tipo de sensor externo</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Sensor externo</div>
                        <div class="md-summary">Elegir el tipo de sensor externo</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionSensorExterno" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="row" layout="row">
                                <md-input-container style="margin: 0px; margin-top: 10px;">
                                    <label>Tipo de sensor</label>
                                    <md-select ng-model="vm.configuracion.___sensorExterno" placeholder="Tipo de sensor" class="md-no-underline">
                                        <md-option value="1">Temperatura</md-option> <md-option value="4">Interruptor</md-option> <md-option value="5">Iluminaci&oacute;n</md-option> <md-option value="6">Conversor ADC</md-option>
                                        <md-option value="7">Contador de pulsos</md-option>
                                    </md-select>
                                </md-input-container>
                                <md-input-container ng-if="vm.configuracion.___sensorExterno=='4'" style="margin: 0px; margin-top: 10px;">
                                    <label>Flanco</label>
                                    <md-select ng-required="vm.configuracion.___sensorExterno=='4'" ng-model="vm.configuracion.___flanco" placeholder="Flanco" class="md-no-underline">
                                        <md-option value="1">Ambos</md-option> <md-option value="2">Bajada</md-option> <md-option value="3">Subida</md-option>
                                    </md-select>
                                </md-input-container>
                                <md-input-container ng-if="vm.configuracion.___sensorExterno=='6'" style="margin: 0px; margin-top: 10px;">
                                    <label>Precalentamiento (ms)</label>
                                    <input type="number" step="1" min="0" max="65535" ng-model="vm.configuracion.___precalentamiento" ng-required="vm.configuracion.___sensorExterno=='6'" />
                                </md-input-container>
                                <md-input-container ng-if="vm.configuracion.___sensorExterno=='7'" style="margin: 0px; margin-top: 10px;">
                                    <label>Flanco o pulsos iniciales</label>
                                    <md-select ng-model="vm.configuracion.___flancoPulsos" placeholder="Flanco" class="md-no-underline" ng-required="vm.configuracion.___sensorExterno=='7'">
                                        <md-option value="0">Bajada</md-option> <md-option value="1">Subida</md-option><md-option value="2">Establecer pulsos iniciales</md-option>
                                    </md-select>
                                </md-input-container>
                                <md-input-container ng-if="vm.configuracion.___sensorExterno=='7' && vm.configuracion.___flancoPulsos=='2'" style="margin: 0px; margin-top: 10px;">
                                    <label>Pulsos iniciales </label>
                                    <input type="number" step="1" min="-1" max="65535" ng-model="vm.configuracion.___pulsosIniciales" ng-required="vm.configuracion.___sensorExterno=='7' && vm.configuracion.___flancoPulsos=='2'" />
                                </md-input-container>
                            </div>
                            <md-button
                                type="submit"
                                ng-disabled="form.configuracionSensorExterno.$invalid || !form.configuracionSensorExterno.$dirty"
                                class="md-raised md-primary"
                                ng-click="vm.configuracion.___ultimoDownlink='sensorExterno'"
                            >
                                Configurar
                            </md-button>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>

            <md-expansion-panel md-component-id="almacenamientoInterno" id="almacenamientoInterno">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Borrar almacenamiento interno</div>
                    <div class="md-summary">Eliminar los registros almacenados en el dispositivo</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Borrar almacenamiento interno</div>
                        <div class="md-summary">Eliminar los registros almacenados en el dispositivo</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionAlmacenamientoInterno" class="configure-entity-form" ng-submit="vm.configurar()">
                            <md-input-container style="margin: 0px; margin-top: 10px;">
                                <md-button class="md-raised md-warn" type="submit" ng-click="vm.configuracion.___ultimoDownlink='borrarAlmacenamientoInterno'">Borrar</md-button>
                            </md-input-container>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>

            <md-expansion-panel md-component-id="panelPeriodoLogging" id="panelPeriodoLogging">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Periodo logging</div>
                    <div class="md-summary">Periodo de almacenamiento de registros en minutos</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Periodo logging</div>
                        <div class="md-summary">Periodo de almacenamiento de registros en minutos</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionPeriodoLogging" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="row" layout="row">
                                <md-input-container style="margin: 0px; margin-top: 10px;">
                                    <label>Periodo de grabaci&oacute;n</label> <input type="number" step="1" min="0" max="65535" ng-model="vm.configuracion.___periodoGrabacion" />

                                    <md-button
                                        type="submit"
                                        ng-disabled="form.configuracionPeriodoLogging.$invalid || !form.configuracionPeriodoLogging.$dirty"
                                        class="md-raised md-primary"
                                        ng-click="vm.configuracion.___ultimoDownlink='periodoGrabacion'"
                                    >
                                        Configurar
                                    </md-button>
                                </md-input-container>
                            </div>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>
        </md-expansion-panel-group>
    </div>
</md-dialog-content>

<md-dialog-actions>
    <md-button ng-click="vm.cancel()" class="md-primary">Cancelar </md-button>
</md-dialog-actions>

```
Fundamentalmente está compuesto por 4 secciones:

* Funciones: Deben aparecer al principio del bloque de configuración. Es un array JSON con objetos que se convertirán en funciones al acceder al panel de configuración. Generalmente utilizaremos estas funciones para declarar atributos que requieren un tipo diferente al que se utiliza para almacenarlo internamente (recordemos que ThingsBoard sólo admite atributos en formato boolean, número, string o JSON). Por ejemplo, en el código anterior correspondiente al LHT65 necesitamos un atributo en formato Date para los controles de tipo Date y Time Picker, pero este atributo se almacena internamente como una cadena.
  
  Una de las funciones de este bloque, llamada "inicializacion", tiene un comportamiento especial; se ejecuta al acceder al cuadro de diálogo, permitiéndonos realizar tareas como declarar variables específicas para el tipo de dispositivo/activo. 
    
* Sustituciones: Son bloques de código genéricos para la mayoría de tipos de dispositivos y que reemplaza automáticamente el sistema por el código correspondiente. Por ejemplo ```<sustituir-coordenadas class="ng-scope"></sustituir-coordenadas>``` será sustituido por el código necesario para que el usuario seleccione el tipo de coordenadas (imagen o mapa) que quiere asignar al dispositivo. Actualmente sólo existen 3 sustituciones posibles (coordenadas, chirpstack y notificaciones), pero poco a poco iremos añadiendo más, especialmente relacionadas con las alarmas.
* Parámetros: Son bloques de código para configurar parámetros concretos, como __heartbeat.
* Alarmas: Son los bloques de código para configurar las alarmas, que se explican con más detalle a continuación.

Para crear una alarma hay que elegir un nombre para ella; por ejemplo,
cambioDeEstado

![](.//media/image2.png)

Y un nombre para la variable en la que se quiera almacenar el umbral o
disparador de ese tipo de alarma; por ejemplo, para el caso de
cambioDeEstado en el tipo V02\_001 se ha llamado trigger

![](.//media/image3.png)

Otro ejemplo; en el tipo V02\_001 se ha creado una alarma llamada
nivelDeBateria y la variable para su umbral se ha llamado umbralBateria.

![](.//media/image4.png)

Y lo único peculiar que queda por explicar de este bloque de código es
que, al final de cada alarma, se tiene que incluir un elemento de este
tipo en el que el texto debe coincidir con el nombre que se eligió para
la alarma; en este caso nivelDeBateria. Es uno de los tipos de sustituiciones que se explicaron anteriormente.

![](.//media/image5.png)

El sistema reconocerá este elemento y lo sustituirá automáticamente por
el código que permite configurar las notificaciones:

![](.//media/image6.png)

4.  Crear la cadena de reglas para gestionar tu dispositivo en
    particular; conviene que se llame como el tipo de dispositivo (en nuestro ejemplo V02\_002). Puede usarse como
    referencia la del dispositivo V02\_001, que muestro debajo

![](.//media/image7.png)

Esto realmente es muy sencillo; básicamente lo único que hace es cargar
las alarmas (cuando el customer configura una alarma usando el
formulario que se creó en el paso anterior, se añade automáticamente un
atributo en el dispositivo llamado alarma\_V02\_001), guardar las telemetrías y, opcionalmente, actualizar el tooltip o gestionar los downlinks. 

Para la gestión de las alarmas, el primer
nodo carga el atributo __alarma.

![](.//media/image8.png)

El nodo siguiente es el principal; en él se comparan los umbrales de las
alarmas con los valores recibidos en la telemetría

![](.//media/image9.png)

Y el resto es muy simple; básicamente en cada rama de cada tipo de
alarma se verifica si la alarma está activada, se configura el nombre
del tipo de alarma, se determina si hay que crear la alarma o borrarla,
se crea o borra la alarma, se construye el mensaje para las
notificaciones Telegram, IFTTT y email (que son las únicas que tenemos
de momento), y se pasa a la cadena de reglas "notificaciones" que se
encarga del resto.

Para el desarrollo, lo ideal es que se use una cuenta de tenant
configurándola como se explica en el README principal del repositorio.
