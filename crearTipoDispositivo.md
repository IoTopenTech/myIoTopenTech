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
<funciones>[{"nombre":"actualizarHeartbeat","codigo":"$scope.vm.configuracion.___0700=parseInt($scope.vm.configuracion.__heartbeat).toString(16);console.log($scope.vm.configuracion.___0700);"}]</funciones>
<sustituir-coordenadas class="ng-scope"></sustituir-coordenadas>
<div class="md-body-1" style="padding-bottom: 10px; color: rgba(0, 0, 0, 0.57);">Par&aacute;metros configurables mediante downlink</div>
<div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
    <div class="md-body-1" style="padding-bottom: 10px; color: rgba(0, 0, 0, 0.57);">Heartbeat</div>
    <div class="body">
        <div class="row" layout="row" layout-align="start center">
            <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
                <div class="row" layout="row">
                    <md-input-container flex class="md-block">
                        <label>Número de minutos entre heartbeats</label> <input type="number" size="10" step="1" min="0" max="60" ng-model="vm.configuracion.__heartbeat" ng-change="actualizarHeartbeat()" />
                        <input type="hidden" ng-model="vm.configuracion.___0700" />
                        <md-button aria-label="CONFIGURAR" type="submit" ng-click="vm.configuracion.__ultimoDownlink='heartbeat'"> <md-icon>check</md-icon> <md-tooltip md-direction="top"> Configurar heartbeat </md-tooltip> </md-button>
                    </md-input-container>
                </div>
            </div>
        </div>
    </div>
</div>
<div class="md-body-1" style="padding-bottom: 10px; color: rgba(0, 0, 0, 0.57);">Alarmas</div>
<div class="body">
    <div class="row" layout="row" layout-align="start center">
        <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
            <div flex layout="column">
                <label class="checkbox-label">Activar alarma de cambio de estado</label>
                <md-checkbox ng-model="vm.configuracion.__alarmas.cambioDeEstado.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.cambioDeEstado.enable ? "value.true" : "value.false") | translate}}</md-checkbox>
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
                <md-checkbox ng-model="vm.configuracion.__alarmas.nivelDeBateria.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.nivelDeBateria.enable ? "value.true" : "value.false") | translate}}</md-checkbox>
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
                <md-checkbox ng-model="vm.configuracion.__alarmas.inactividad.enable" style="margin-bottom: 10px;">{{(vm.configuracion.__alarmas.inactividad.enable ? "value.true" : "value.false") | translate}}</md-checkbox>
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
```
Fundamentalmente está compuesto por 4 secciones:

* Funciones: Deben aparecer al principio del bloque de configuración. Es un array JSON con objetos que se convertirán en funciones al acceder al panel de configuración. Generalmente utilizaremos estas funciones para realizar conversiones de datos. Por ejemplo, si queremos configurar el periodo de heartbeats enviando un downlink al dispositivo necesitaremos 2 parámetros:

  * __heartbeat: Este parámetro (empieza con doble guión bajo) se almacenará en un atributo del dispositivo y contendrá el valor decimal en segundos del periodo del heartbeat, que es como le resultará más cómodo al usuario expresarlo. Está asociado a un control input de tipo number del bloque de configuración.
  * ____0700: Este parámetro (empieza con triple guión bajo) se enviará por downlink al dispositivo, que requiere que el valor del periodo esté expresado en hexadecimal. Está asociado a un control input de tipo hidden del bloque de configuración. Cuando el usuario modifique el parámetro __heartbeat, se ejecutará la función actualizarHeartbeat(), que convertirá el valor a hexadecimal y lo almacenará en ___0700.
  
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
atributo en el dispositivo llamado alarma\_V02\_001). Por eso, el primer
nodo carga ese atributo.

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
