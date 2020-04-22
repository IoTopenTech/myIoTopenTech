1.  Notificar que quieres integrar un tipo de dispositivo nuevo para que
    se te asigne un identificador de dispositivo, del tipo VXX\_XXX (por
    ejemplo, V02\_002).

2.  Crear un dashboard para tu tipo de dispositivo llamado
    Dispositivo\_V02\_002. Puedes usar como referencia el del nodo
    V02\_001 que hay en el repositorio
    ([[https://github.com/IoTopenTech/ThingsBoard-TTN-Edition/tree/master/dashboards]](https://github.com/IoTopenTech/ThingsBoard-TTN-Edition/tree/master/dashboards)).
    El sistema está preparado para que cuando el usuario haga clic en
    el dashboard \"Panel de control\" sobre un dispositivo de tipo
    \"identificador\" se abra automáticamente el dashboard
    \"Dispositivo\_identificador\", que es el que tú vas a crear.
    Asignar el dashboard al customer.
    

3.  Crear un bloque de HTML para la configuración del dispositivo. Por
    ejemplo, para configurar, el periodo de tiempo de los heartbeat, las coordenadas, o los umbrales de las alarmas. Este bloque
    de código se almacenará en un atributo de cada customer con el
    nombre, por ejemplo, config\_V02\_002, y el sistema lo tomará de ahí para
    mostrarlo en el panel de configuración de este modo. Cada parámetro será una propiedad del objeto vm.configuracion.
    Existen 3 tipos de parámetros configurables:
    
    * Los que simplemente se almacenan en un atributo del dispositivo, como __xPos o __yPos que se utilizan para posicionar un dispositivo en un widget de tipo IMAGE.
    * Los de alarmas, que se agrupan dentro del atributo __alarmas para distinguirlos de los demás y poder gestionarlos en las reglas para determinar si es necesario enviar algún tipo de notificación.
    * Los que además de almacenarse en un atributo, se envían por downlink al dispositivo, como ___0700. Éstos deben empezar por tres guiones bajos, seguidos del canal y el tipo de dato (en concordancia con el formato Cayenne LPP).

![](.//media/image1.png)

A continuación, se muestra el bloque de código correspondiente al tipo
de nodo V02\_001 que puede servir como referencia:

```html
<div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);">Heartbeat</div>
<div class="body">
    <div class="row" layout="row" layout-align="start center">
        <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
            <div class="row" layout="row">
                <md-input-container flex class="md-block">
                    <label>Número de minutos entre heartbeats</label>
                    <input type="number" size="10" step="1" min="0" max="60" ng-model="vm.configuracion.___0700"> </md-input-container>
            </div>
        </div>
    </div>
</div>
<div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);">Coordenadas</div>
<div class="body">
    <div class="row" layout="row" layout-align="start center">
        <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
            <div class="row" layout="row">
                <md-input-container flex class="md-block">
                    <label>Posicion X</label>
                    <input type="number" size="10" step=".01" min="0" max="1" ng-model="vm.configuracion.__xPos"> </md-input-container>
                <md-input-container flex class="md-block">
                    <label>Posicion Y</label>
                    <input type="number" size="10" step="0.01" min="0.00" max="1.00" ng-model="vm.configuracion.__yPos"> </md-input-container>
            </div>
        </div>
    </div>
</div>
<div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);">Alarmas</div>
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
                    <md-select ng-disabled="!vm.configuracion.__alarmas.cambioDeEstado.enable" ng-required="vm.configuracion.__alarmas.cambioDeEstado.enable" name="cambioDeEstadoTrigger" ng-model="vm.configuracion.__alarmas.cambioDeEstado.trigger">
                        <md-option value="abrir"> abrir </md-option>
                        <md-option value="cerrar"> cerrar </md-option>
                    </md-select>
                    <div ng-messages="editEntityForm.cambioDeEstadoTrigger.$error">
                        <div ng-message="required">Este dato es obligatorio. </div>
                    </div>
                </md-input-container>
                <sustituir-notificaciones class="ng-scope">cambioDeEstado</sustituir-notificaciones>
            </div>
        </div>
    </div>
</div>
<div class="body">
    <div class="row" layout="row" layout-align="start center">
        <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;">
            <div class="row" layout="row">
                <md-input-container flex class="md-block">
                    <label>Umbral (V)</label>
                    <input type="decimal" size="10" ng-disabled="!vm.configuracion.__alarmas.nivelDeBateria.enable " ng-model="vm.configuracion.__alarmas.nivelDeBateria.umbralBateria" ng-required="vm.configuracion.__alarmas.nivelDeBateria.enable"> </md-input-container>
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
                    <input type="number" size="10" ng-disabled="!vm.configuracion.__alarmas.inactividad.enable " ng-model="vm.configuracion.__alarmas.inactividad.umbralInactividad" ng-required="vm.configuracion.__alarmas.inactividad.enable"> </md-input-container>
                <sustituir-notificaciones class="ng-scope">inactividad</sustituir-notificaciones>
            </div>
        </div>
    </div>
</div>
```

Básicamente, hay que inventarse un nombre para cada alarma; por ejemplo,
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
la alarma; en este caso nivelDeBateria.

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
