# 1. Asignar un entero a cada telemetría
Por ejemplo, en el caso del dispositivo TTNMAD_PLANT
1. Moisture
2. Luminosity
3. Temp
4. Hum
5. Pressure
6. Bat

# 2. El archivo \[TIPO DISPOSITIVO]_KEYBOARD.php
Añadir al array inline_keyboard los elementos correspondientes a las telemetrías, tanto en la acción home como en la acción telemetry.
```php
switch ($informacion->action) {
        case "home":
            array_push($keyboard["inline_keyboard"], [["text" => "Moisture", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"1\",\"q\":\"1\",\"r\":\"" . time() . "\"}"], ["text" => "Luminosity", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"2\",\"q\":\"1\",\"r\":\"" . time() . "\"}"]]);
            array_push($keyboard["inline_keyboard"], [["text" => "Temp", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"3\",\"q\":\"1\",\"r\":\"" . time() . "\"}"], ["text" => "Hum", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"4\",\"q\":\"1\",\"r\":\"" . time() . "\"}"], ["text" => "Bat", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"5\",\"q\":\"1\",\"r\":\"" . time() . "\"}"]]);
            array_push($keyboard["inline_keyboard"], [["text" => "Config", "callback_data" => "{\"action\":\"toConfig\"}"], ["text" => "Alarms", "callback_data" => "{\"action\":\"toAlarm\"}"]]);
            break;
        case "telemetry":
            array_push($keyboard["inline_keyboard"][0], ["text" => "💾", "callback_data" => "{\"action\":\"download\"}"]);
            array_push($keyboard["inline_keyboard"], [["text" => "Prev", "callback_data" => "{\"action\":\"prev\",\"s\":\"prev\"}"],["text" => "1h", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"".$informacion->p."\",\"q\":\"1\",\"r\":\"" . $informacion->r . "\"}"],["text" => "1d", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"".$informacion->p."\",\"q\":\"24\",\"r\":\"" . $informacion->r . "\"}"],["text" => "7d", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"".$informacion->p."\",\"q\":\"168\",\"r\":\"" . $informacion->r . "\"}"],["text" => "30d", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"".$informacion->p."\",\"q\":\"720\",\"r\":\"" . $informacion->r . "\"}"], ["text" => "Next", "callback_data" => "{\"action\":\"next\",\"s\":\"next\"}"]]);
            array_push($keyboard["inline_keyboard"], [["text" => "Moisture", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"1\",\"q\":\"".$informacion->q."\",\"r\":\"".$informacion->r."\"}"], ["text" => "Luminosity", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"2\",\"q\":\"".$informacion->q."\",\"r\":\"".$informacion->r."\"}"]]);
            array_push($keyboard["inline_keyboard"], [["text" => "Temp", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"3\",\"q\":\"".$informacion->q."\",\"r\":\"".$informacion->r."\"}"], ["text" => "Hum", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"4\",\"q\":\"".$informacion->q."\",\"r\":\"".$informacion->r."\"}"], ["text" => "Bat", "callback_data" => "{\"action\":\"telemetry\",\"p\":\"5\",\"q\":\"".$informacion->q."\",\"r\":\"".$informacion->r."\"}"]]);            
            array_push($keyboard["inline_keyboard"], [["text" => "Config", "callback_data" => "{\"action\":\"toConfig\"}"], ["text" => "Alarms", "callback_data" => "{\"action\":\"toAlarm\"}"]]);
            break;
    }
```
    
# 3. El archivo \[TIPO DISPOSITIVO]_GUI
Aquí básicamente hay que sustituir el nombre de la telemetría en cada case.
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
