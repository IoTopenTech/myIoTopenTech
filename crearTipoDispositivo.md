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
    * Los que no se almacenan en un atributo, sino que se envían por downlink al dispositivo, que deben empezar con triple guión bajo,  como ___0700. El sistema tomará como carga de pago completa para el donwlink todo lo que haya a continuación de los tres guiones bajos concatenado con el valor del propio atributo; por ejemplo, si el atributo se llama ___01 y su valor es 64 (100 expresado en hexadecimal), la plataforma enviará un downlink con la carga de pago 0164. 
    
> Debemos tener en cuenta que si almacenamos en un atributo un valor que puede convertirse en un entero, ThingsBoard lo convertirá en un entero aunque no lo sea; por ejemplo, si queremos almacenar la cadena de texto "08", ThingsBoard la almacenará en el atributo como el entero 8.

![](.//media/image1.png)

A continuación, se muestra el bloque de código correspondiente al tipo
de nodo V02\_001 que puede servir como referencia:

```html
<funciones>[{"nombre":"actualizarHeartbeat","codigo":"var valor=parseInt($scope.vm.configuracion.___heartbeat).toString(16);var pad='00';$scope.vm.configuracion.___0700=pad.substring(0, pad.length - valor.length) + valor;"}]</funciones>
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
                            <div class="body">
                                <div class="row" layout="row" layout-align="start center">
                                    <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
                                        <div flex layout="column">
                                            <label class="checkbox-label">Activar alarma de cambio de estado</label>
                                            <md-checkbox ng-model="vm.configuracion.__alarmas.cambioDeEstado.enable" style="margin-bottom: 10px;">
                                                {{(vm.configuracion.__alarmas.cambioDeEstado.enable ? "value.true" : "value.false") | translate}}
                                            </md-checkbox>
                                        </div>
                                        <div class="row" layout="row">
                                            <md-input-container class="md-block" style="min-width: 100px;">
                                                <label>Disparar al </label>
                                                <md-select
                                                    ng-disabled="!vm.configuracion.__alarmas.cambioDeEstado.enable"
                                                    ng-required="vm.configuracion.__alarmas.cambioDeEstado.enable"
                                                    name="cambioDeEstadoTrigger"
                                                    ng-model="vm.configuracion.__alarmas.cambioDeEstado.trigger"
                                                >
                                                    <md-option value="abrir"> abrir </md-option> <md-option value="cerrar"> cerrar </md-option>
                                                </md-select>
                                                <div ng-messages="editEntityForm.cambioDeEstadoTrigger.$error"><div ng-message="required">Este dato es obligatorio.</div></div>
                                            </md-input-container>
                                            <sustituir-notificaciones class="ng-scope">cambioDeEstado</sustituir-notificaciones>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            <div class="body">
                                <div class="row" layout="row" layout-align="start center">
                                    <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
                                        <div flex layout="column">
                                            <label class="checkbox-label">Activar alarma de nivel bajo de batería</label>
                                            <md-checkbox ng-model="vm.configuracion.__alarmas.nivelDeBateria.enable" style="margin-bottom: 10px;">
                                                {{(vm.configuracion.__alarmas.nivelDeBateria.enable ? "value.true" : "value.false") | translate}}
                                            </md-checkbox>
                                        </div>
                                        <div class="row" layout="row">
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
                                            <sustituir-notificaciones class="ng-scope">nivelDeBateria</sustituir-notificaciones>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            <div class="body">
                                <div class="row" layout="row" layout-align="start center">
                                    <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
                                        <div flex layout="column">
                                            <label class="checkbox-label">Activar alarma de inactividad</label>
                                            <md-checkbox ng-model="vm.configuracion.__alarmas.inactividad.enable" style="margin-bottom: 10px;">
                                                {{(vm.configuracion.__alarmas.inactividad.enable ? "value.true" : "value.false") | translate}}
                                            </md-checkbox>
                                        </div>
                                        <div class="row" layout="row">
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
                                            <sustituir-notificaciones class="ng-scope">inactividad</sustituir-notificaciones>
                                        </div>
                                    </div>
                                </div>
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
                    <div class="md-title" translate>Hearbeat</div>
                    <div class="md-summary">Configurar periodo de envío de heartbeat</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Hearbeat</div>
                        <div class="md-summary">Configurar periodo de envío de heartbeat</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionDownlink" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="row" layout="row">
                                <md-input-container flex class="md-block">
                                    <label>Número de minutos entre heartbeats</label> <input type="number" size="10" step="1" min="0" max="60" ng-model="vm.configuracion.___heartbeat" ng-change="actualizarHeartbeat()" />
                                    <input type="hidden" ng-model="vm.configuracion.___0700" />

                                    <md-button type="submit" ng-disabled="form.configuracionDownlink.$invalid || !form.configuracionDownlink.$dirty" class="md-raised md-primary" ng-click="vm.configuracion.___ultimoDownlink='heartbeat'">
                                        Configurar
                                    </md-button>
                                </md-input-container>
                            </div>
                        </form>
                    </md-expansion-panel-content>
                </md-expansion-panel-expanded>
            </md-expansion-panel>
            <md-expansion-panel md-component-id="panelTomarPosesion" id="panelTomarPosesion" ng-if="vm.entityType.toLowerCase()=='device' && vm.attributes.hasOwnProperty('apropiable') && vm.attributes.apropiable==true">
                <md-expansion-panel-collapsed>
                    <div class="md-title" translate>Credenciales LoRaWAN</div>
                    <div class="md-summary">Configurar las credenciales LoRaWAN del dispositivo</div>
                    <md-expansion-panel-icon></md-expansion-panel-icon>
                </md-expansion-panel-collapsed>
                <md-expansion-panel-expanded>
                    <md-expansion-panel-header ng-click="$panel.collapse()">
                        <div class="md-title" translate>Credenciales LoRaWAN</div>
                        <div class="md-summary">Configurar las credenciales LoRaWAN del dispositivo</div>
                        <md-expansion-panel-icon></md-expansion-panel-icon>
                    </md-expansion-panel-header>
                    <md-expansion-panel-content>
                        <form name="form.configuracionEspecifica" class="configure-entity-form" ng-submit="vm.configurar()">
                            <div class="md-dialog-content">
                                <div layout="row">
                                    <md-input-container flex class="md-block" style="min-width: 100px; width: 150px;">
                                        <label>Método de activación</label>
                                        <md-select ng-model="vm.configuracion.___tomarPosesionMetodoActivacion" required>
                                            <md-option ng-if="vm.attributes.hasOwnProperty('admiteABP') && vm.attributes.admiteABP==true" value="A" ng-selected="">
                                                ABP
                                            </md-option>
                                            <md-option value="O" ng-selected="true">
                                                OTAA
                                            </md-option>
                                        </md-select>
                                    </md-input-container>
                                </div>
                                <div layout="row">
                                    <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='A' " class="md-block" style="min-width: 100px; width: 150px;">
                                        <label>Device Address (msb)</label>
                                        <input type="text" pattern="[0-9a-fA-F]{8}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam1" required />
                                    </md-input-container>
                                </div>
                                <div layout="row">
                                    <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='A'" class="md-block" style="min-width: 100px; width: 150px;">
                                        <label>Network Session Key (msb)</label>
                                        <input type="text" pattern="[0-9a-fA-F]{32}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam2" required />
                                    </md-input-container>
                                </div>
                                <div layout="row">
                                    <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='A' " class="md-block" style="min-width: 100px; width: 150px;">
                                        <label>Application Session Key (msb)</label>
                                        <input type="text" pattern="[0-9a-fA-F]{32}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam3" required />
                                    </md-input-container>
                                </div>
                                <div layout="row">
                                    <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='O' " class="md-block" style="min-width: 100px; width: 150px;">
                                        <label>Device EUI (msb)</label>
                                        <input type="text" pattern="[0-9a-fA-F]{16}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam1" name="vm.configuracion.___tomarPosesionParam1" required />
                                    </md-input-container>
                                </div>
                                <div layout="row">
                                    <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='O'" class="md-block" style="min-width: 100px; width: 150px;">
                                        <label>Application EUI (msb)</label>
                                        <input type="text" pattern="[0-9a-fA-F]{16}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam2" required />
                                    </md-input-container>
                                </div>
                                <div layout="row">
                                    <md-input-container flex ng-if="vm.configuracion.___tomarPosesionMetodoActivacion=='O'" class="md-block" style="min-width: 100px; width: 150px;">
                                        <label>Application Key (msb)</label>
                                        <input type="text" pattern="[0-9a-fA-F]{32}" autocomplete="off" ng-model="vm.configuracion.___tomarPosesionParam3" required />
                                    </md-input-container>
                                </div>
                                <md-button type="submit" ng-disabled="form.configuracionEspecifica.$invalid || !form.configuracionEspecifica.$dirty" class="md-raised md-primary" ng-click="vm.configuracion.___ultimoDownlink='tomarPosesion'">
                                    Configurar
                                </md-button>
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

* Funciones: Deben aparecer al principio del bloque de configuración. Es un array JSON con objetos que se convertirán en funciones al acceder al panel de configuración. Generalmente utilizaremos estas funciones para realizar conversiones de datos. Por ejemplo, si queremos configurar el periodo de heartbeats enviando un downlink al dispositivo necesitaremos 2 parámetros:

  * __heartbeat: Este parámetro (empieza con doble guión bajo) se almacenará en un atributo del dispositivo y contendrá el valor decimal en segundos del periodo del heartbeat, que es como le resultará más cómodo al usuario expresarlo. Está asociado a un control input de tipo number del bloque de configuración.
  * ____0700: Este parámetro (empieza con triple guión bajo) se enviará por downlink al dispositivo, que requiere que el valor del periodo esté expresado en hexadecimal. Está asociado a un control input de tipo hidden del bloque de configuración. Cuando el usuario modifique el parámetro __heartbeat, se ejecutará la función actualizarHeartbeat(), que convertirá el valor a hexadecimal y lo almacenará en ___0700.
  
  Una de las funciones de este bloque, llamada "inicializacion", tiene un comportamiento especial; se ejecuta al acceder al cuadro de diálogo, permitiéndonos realizar tareas como declarar variables específicas para el tipo de dispositivo/activo. 
  Algunos tipos de nodos ofrecen hasta 3 tipos diferentes de configuración: parámetros de atributos (a la que denominaremos configuración general), parámetros que se configuran mediante un único downlink (a la que denominaremos configuración por downlink), y parámetros que requieres un tratamiento especial, como enviar varios downlinks (a la que denominaremos configuración específica).
  La configuración general (parámetros que empiezan con doble guión bajo __) y la configuración por downlink (parámetros que empiezan con triple guión bajo ___) se gestionan directamente en la cadena de reglas configurarEntidad; los parámetros de la configuración específica también llegan a la misma cadena de reglas configurarEntidad, pero desde allí son reenviados a la cadena raíz como telemetría para que la gestione la regla del tipo de dispositivo/activo correspondiente.
  
* Sustituciones: Son bloques de código genéricos para la mayoría de tipos de dispositivos y que reemplaza automáticamente el sistema por el código correspondiente. Por ejemplo "<sustituir-coordenadas class="ng-scope"></sustituir-coordenadas>" será sustituido por el código necesario para que el usuario seleccione el tipo de coordenadas (imagen o mapa) que quiere asignar al dispositivo. Actualmente sólo existen 3 sustituciones posibles (coordenadas, chirpstack y notificaciones), pero poco a poco iremos añadiendo más, especialmente relacionadas con las alarmas.
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
