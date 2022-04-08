# 1. Asignar un entero a cada telemetría
Por ejemplo, en el caso del dispositivo TTNMAD_PLANT_FREE
1. Moisture
2. Luminosity
3. Temp
4. Hum
5. Pressure
6. Bat

# 2. El archivo \[TIPO DISPOSITIVO]_KEYBOARD.php
Modificar el switch de acuerdo a los números asignados en el paso anterior, y añadir al array inline_keyboard los elementos correspondientes a las telemetrías, tanto en la acción home como en la acción telemetry.
```php
switch ($informacion->action) {
    case "home":
        array_push($keyboard["inline_keyboard"], [["text" => "Moisture", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"1\",\"q\":\"1\",\"r\":\"" . time() . "\"}"], ["text" => "Luminosity", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"2\",\"q\":\"1\",\"r\":\"" . time() . "\"}"], ["text" => "Temp", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"3\",\"q\":\"1\",\"r\":\"" . time() . "\"}"]]);
        array_push($keyboard["inline_keyboard"], [["text" => "Hum", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"4\",\"q\":\"1\",\"r\":\"" . time() . "\"}"], ["text" => "Pressure", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"5\",\"q\":\"1\",\"r\":\"" . time() . "\"}"], ["text" => "Bat", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"6\",\"q\":\"1\",\"r\":\"" . time() . "\"}"]]);
        array_push($keyboard["inline_keyboard"], [["text" => "Config", "callback_data" => "{\"action\":\"toConfig\"}"], ["text" => "Alarms", "callback_data" => "{\"action\":\"toAlarm\"}"]]);
        break;
    case "telemetry":
        //Si no hay telemetrías previas hay que indicarlo en el botón Prev
        switch ($informacion->p) {
            case 1:
                $nombreTelemetria = "Moisture";
                break;
            case 2:
                $nombreTelemetria = "LDR";
                break;
            case 3:
                $nombreTelemetria = "Temperatura";
                break;
            case 4:
                $nombreTelemetria = "Humedad";
                break;
            case 5:
                $nombreTelemetria = "Presion";
                break;
            case 6:
                $nombreTelemetria = "Bateria";
                break;
        }
        $telemetriaPrevia = json_decode(obtenerTelemetriaDispositivo($informacion->id, $nombreTelemetria, (intval($informacion->r) * 1000) - (intval($informacion->q) * 3600000), null));
        if (isset($telemetriaPrevia->$nombreTelemetria)) {
            //Hay telemetrías previas
            $prevText = "Prev";
            $prevCallbackData = "{\"action\":\"prev\",\"s\":\"prev\"}";
        } else {
            //No hay telemetrías previas
            $prevText = "🚫Prev";
            $prevCallbackData = "{\"action\":\"telemetry\",\"p\":\"" . $informacion->p . "\",\"q\":\"" . $informacion->q . "\",\"r\":\"" . $informacion->r . "\"}";
        }
        array_push($keyboard["inline_keyboard"][0], ["text" => "💾", "callback_data" => "{\"action\":\"download\"}"]);
        array_push($keyboard["inline_keyboard"][0], ["text" => "📝", "callback_data" => "{\"action\":\"text\"}"]);
        array_push($keyboard["inline_keyboard"], [["text" => "Prev", "callback_data" => "{\"action\":\"prev\",\"s\":\"prev\"}"], ["text" => "1h", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"" . $informacion->p . "\",\"q\":\"1\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "1d", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"" . $informacion->p . "\",\"q\":\"24\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "7d", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"" . $informacion->p . "\",\"q\":\"168\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "30d", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"" . $informacion->p . "\",\"q\":\"720\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "Next", "callback_data" => "{\"action\":\"next\",\"s\":\"next\"}"]]);
        array_push($keyboard["inline_keyboard"], [["text" => "Moisture", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"1\",\"q\":\"" . $informacion->q . "\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "Luminosity", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"2\",\"q\":\"" . $informacion->q . "\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "Temp", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"3\",\"q\":\"" . $informacion->q . "\",\"r\":\"" . $informacion->r . "\"}"]]);
        array_push($keyboard["inline_keyboard"], [["text" => "Hum", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"4\",\"q\":\"" . $informacion->q . "\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "Pressure", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"5\",\"q\":\"" . $informacion->q . "\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "Bat", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"6\",\"q\":\"" . $informacion->q . "\",\"r\":\"" . $informacion->r . "\"}"]]);
        array_push($keyboard["inline_keyboard"], [["text" => "Config", "callback_data" => "{\"action\":\"toConfig\"}"], ["text" => "Alarms", "callback_data" => "{\"action\":\"toAlarm\"}"]]);
        break;
}
```
    
# 3. El archivo \[TIPO DISPOSITIVO]_GUI
Aquí básicamente hay que sustituir el nombre de la telemetría en cada case.
```php
case "2"://Bat 
    $nombreTelemetria = "Bateria"; //Nombre de la telemetría en myIoT
    $rotuloTelemetria = "Bat";
```
Si alguna telemetría requiere redondear los rótulos del eje Y se puede usar una función de este estilo:
```php
function YAxisFormat($Value) {
    return(round($Value, 3));
}
```
Que se utiliza desde el método setAxisDisplay:
```php
$myPicture->myData->setAxisDisplay(0, AXIS_FORMAT_CUSTOM, "YAxisFormat"); //Redondeo a 3 decimales
```

# 4. La propiedad \[TIPO DISPOSITIVO]_config de myIoT
Hay que añadir el bloque funciones al principio:
```javascript
<funciones>[{"nombre":"myiotlegrampassadm","codigo":"var result= '';var characters= 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';var charactersLength = characters.length;for ( var i = 0; i != 20; i++ ) {result += characters.charAt(Math.floor(Math.random()* charactersLength));}$scope.vm.configuracion.__myiotlegram_passadm=result;$scope.form.configuracionmyiotlegram.$setDirty()"},{"nombre":"myiotlegrampassobs","codigo":"var result= '';var characters= 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';var charactersLength = characters.length;for ( var i = 0; i != 20; i++ ) {result += characters.charAt(Math.floor(Math.random()* charactersLength));}$scope.vm.configuracion.__myiotlegram_passobs=result;$scope.form.configuracionmyiotlegram.$setDirty()"}]</funciones>
```
Y un panel (por encima del pandel Configuración general):
```xml
<panel titulo="myIoTlegram" resumen="Si regenera un código, los dispositivos que estén usando el actual quedarán desvinculados de myIoTlegram" nombreFormulario="myiotlegram" ng-if="!vm.attributes.hasOwnProperty('esDelegado') || (vm.attributes.esDelegado===false)">
		<md-input-container  style="display: none;">
			<input type="text" name="passadm" ng-value="vm.configuracion.__myiotlegram_passadm"/>
		</md-input-container>
		<div class="md-whiteframe-1dp" layout="column" style="padding-left: 5px; margin-bottom: 3px;">
			<div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);">Código de administrador</div>
			<div class="row" layout="column">
				<div  ng-if="vm.configuracion.__myiotlegram_passadm!=undefined">{{vm.entityId.id}}:{{vm.configuracion.__myiotlegram_passadm}}-A
					<md-button type="button" class="md-raised md-primary md-button md-ink-ripple" ng-href="https://chart.googleapis.com/chart?chs=350x350&amp;cht=qr&amp;chl={{vm.entityId.id}}:{{vm.configuracion.__myiotlegram_passadm}}-A" target="_blank">Ver código QR</md-button>
				</div>
				<div  ng-if="vm.configuracion.__myiotlegram_passadm==undefined">NO GENERADO</div>
				<div width="20%">
					<md-input-container  style="margin: 0px;">
						<input type="button" value="Regenerar código" ng-click="myiotlegrampassadm()" class="md-raised md-primary md-button md-ink-ripple"/>
					</md-input-container>
				</div>
			</div>
		</div>
		<md-input-container  style="display: none;">
			<input type="text" name="passobs" ng-value="vm.configuracion.__myiotlegram_passobs"/>
		</md-input-container>
		<div class="md-whiteframe-1dp" layout="column" style="padding-left: 5px; margin-bottom: 3px;">
			<div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);">Código de observador</div>
			<div class="row" layout="column">
				<div  ng-if="vm.configuracion.__myiotlegram_passobs!=undefined">{{vm.entityId.id}}:{{vm.configuracion.__myiotlegram_passobs}}-O
					<md-button type="button" class="md-raised md-primary md-button md-ink-ripple" ng-href="https://chart.googleapis.com/chart?chs=350x350&amp;cht=qr&amp;chl={{vm.entityId.id}}:{{vm.configuracion.__myiotlegram_passobs}}-O" target="_blank">Ver código QR</md-button>
				</div>
				<div  ng-if="vm.configuracion.__myiotlegram_passobs==undefined">NO GENERADO</div>
				<div width="20%">
					<md-input-container  style="margin: 0px;">
						<input type="button" value="Regenerar código" ng-click="myiotlegrampassobs()" class="md-raised md-primary md-button md-ink-ripple"/>
					</md-input-container>
				</div>
			</div>
		</div>
	</panel>
```
En este punto ya podríamos depurar las telemetrías del nuevo tipo de dispositivo directamente en myIoTlegram.

# 5. El archivo \[TIPO DISPOSITIVO]_CONFIG.php
En este archivo se genera el menú y las acciones de configuración.
Son muy dependientes del tipo de dispositivo; pero se gestionan mediante los case de un switch($informacion_estado_actual_chat->p).
El case list muestra el estado actual de la configuración y ofrece el menú para modificarla, en el que el rol administrador tiene acceso a todas las opciones, mientras que el rol observador sólo puede cambiar el nombre del dispositivo (el nombre es local para cada instancia de myIoTlegram; no afecta al nombre real del dispositivo en myIoY, ni al de otros usuarios que lo tengan vinculado en su myIoTlegram), o desvincular el dispositivo.
Otros case genéricos son toNombre y toDesvincular.
En general, los dispositivos de la serie TTNMAD tienen opciones para configurar:

* Periodo de heartbeat
* ACK de los uplinks
* Resetear el dispositivo

# 6. El archivo \[TIPO DISPOSITIVO]_ALARM.php
Este archivo es muy similar al anterior, y es el responsable de mostrar las alarmas y su menú de configuración.
El rol observador no puede configurar las alarmas (asume la configuración establecida por el rol administrador).

# 7. El archivo \[TIPO DISPOSITIVO]_DOWNLOAD.php
Este archivo permite descargar las telemetrías en formato CSV del gráfico que estemos visualizando.
Sólo requiere indicar el nombre de la telemetría en función del número que le hayamos asignado a cada una en el paso 1.
```php
switch ($informacion_estado_actual_chat->p) {
    case "1"://Correo       
        $telemetrias = json_decode(obtenerTelemetriaDispositivo($informacion_estado_actual_chat->id, "Correo", intval($informacion_estado_actual_chat->r) * 1000, intval($informacion_estado_actual_chat->q) * 3600000));
        $telemetrias = array_reverse($telemetrias->Correo);
        $nombreArchivo = $nombre_dispositivo . "_correo_" . $informacion_estado_actual_chat->r . ".csv";
        break;    
    case "2"://Bateria       
        $telemetrias = json_decode(obtenerTelemetriaDispositivo($informacion_estado_actual_chat->id, "Bateria", intval($informacion_estado_actual_chat->r) * 1000, intval($informacion_estado_actual_chat->q) * 3600000));
        $telemetrias = array_reverse($telemetrias->Bateria);
        $nombreArchivo = $nombre_dispositivo . "_bat_" . $informacion_estado_actual_chat->r . ".csv";
        break;
}
```
# 8. El archivo \[TIPO DISPOSITIVO]_TEXT.php
Este archivo permite obtener en un mensaje de Telegram las variaciones de la telemetría que estemos visualizando (con un límite de unos 65000 caracteres impuesto por Telegram).
Básicamente, como en el caso del archivo DOWNLOAD, sólo requiere asociar a cada número el nombre correspondiente de telemetría de acuerdo al paso 1.
```php
switch ($informacion_estado_actual_chat->p) {
    case "1"://Correo       
        $telemetrias = json_decode(obtenerTelemetriaDispositivo($informacion_estado_actual_chat->id, "Correo", intval($informacion_estado_actual_chat->r) * 1000, intval($informacion_estado_actual_chat->q) * 3600000));
        $telemetrias = array_reverse($telemetrias->Correo);
        $txt = $nombre_dispositivo . ": Variaciones de Correo" . chr(10);
        break;    
    case "2"://Bateria       
        $telemetrias = json_decode(obtenerTelemetriaDispositivo($informacion_estado_actual_chat->id, "Bateria", intval($informacion_estado_actual_chat->r) * 1000, intval($informacion_estado_actual_chat->q) * 3600000));
        $telemetrias = array_reverse($telemetrias->Bateria);
        $txt = $nombre_dispositivo . ": Variaciones de Bat" . chr(10);
        break;
}
```
# 9. Icono para las notificaciones
En el archivo notificar.php hay que indicar el icono correspondiente al tipo de dispositivo.
```php
    switch ($tipoDispositivo){
        case "TTNMAD_DOOR":
            $icono="🚪 ";
            break;
        case "TTNMAD_PLANT_FREE":
            $icono="🪴 ";
            break;
        case "TTNMAD_MAILBOX":
            $icono="📬 ";
            break;
    }
```
#10. Localización y zona horaria
La localización afecta a myIoT en:
* tooltip (se usa en última telemetría)
* asuntoEmail (se usa en última alarma)
* mensajeTelegram (se usa en la comunicación de las alarmas)

A nivel del propio myIoTlegram:
* keyboards (botones)
* Gráficas (leyenda y rótulos de ejes)
* Intrucciones
