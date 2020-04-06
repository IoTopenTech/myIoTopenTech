<?php
	define('DBUSER','xxx');
	define('DBPWD','xxx');
	define('DBHOST','xxx');
	define('DBNAME','xxx');
	define('CAPTCHA_WEB','xxx');
	define('CAPTCHA_PRIVATE','xxx');
	define('TENANT_NAME','xxx');
	define('TENANT_PWD','xxx');
	define('TENANT_ID',"xxx");	
	define('TIPOS_DE_ACTIVOS','MAP01,IMAGE01');
	define('TIPOS_DE_DISPOSITIVOS','V02_001,V02_002');
	define('V02_001_CONFIG','<div class="md-body-1" style="padding-bottom: 10px; color: rgba(0,0,0,0.57);"> Alarmas</div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de cambio de estado</label> <md-checkbox ng-model="vm.attributes.alarmas.cambioDeEstado.enable" style="margin-bottom: 10px;">{{(vm.attributes.alarmas.cambioDeEstado.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div><div class="row" layout="row"> <md-input-container class="md-block" style="min-width: 100px;"> <label>Disparar al </label> <md-select ng-disabled="!vm.attributes.alarmas.cambioDeEstado.enable" ng-required="vm.attributes.alarmas.cambioDeEstado.enable" name="cambioDeEstadoTrigger" ng-model="vm.attributes.alarmas.cambioDeEstado.trigger"> <md-option value="abrir"> abrir </md-option> <md-option value="cerrar"> cerrar </md-option> </md-select> <div ng-messages="editEntityForm.cambioDeEstadoTrigger.$error"> <div ng-message="required">Este dato es obligatorio. </div></div></md-input-container> <sustituir-notificaciones class="ng-scope">cambioDeEstado</sustituir-notificaciones> </div></div></div></div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de nivel bajo de batería</label> <md-checkbox ng-model="vm.attributes.alarmas.nivelDeBateria.enable" style="margin-bottom: 10px;">{{(vm.attributes.alarmas.nivelDeBateria.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div><div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Umbral (V)</label> <input type="decimal" size="10" ng-disabled="!vm.attributes.alarmas.nivelDeBateria.enable " ng-model="vm.attributes.alarmas.nivelDeBateria.umbralBateria" ng-required="vm.attributes.alarmas.nivelDeBateria.enable" > </md-input-container> <sustituir-notificaciones class="ng-scope">nivelDeBateria</sustituir-notificaciones> </div></div></div></div><div class="body"> <div class="row" layout="row" layout-align="start center"> <div class="md-whiteframe-1dp" flex layout="column" style="padding-left: 5px; margin-bottom: 3px;"> <div flex layout="column"> <label class="checkbox-label">Activar alarma de inactividad</label> <md-checkbox ng-model="vm.attributes.alarmas.inactividad.enable" style="margin-bottom: 10px;">{{(vm.attributes.alarmas.inactividad.enable ? "value.true" : "value.false") | translate}}</md-checkbox> </div><div class="row" layout="row"> <md-input-container flex class="md-block"> <label>Umbral en segundos</label> <input type="number" size="10" ng-disabled="!vm.attributes.alarmas.inactividad.enable " ng-model="vm.attributes.alarmas.inactividad.umbralInactividad" ng-required="vm.attributes.alarmas.inactividad.enable" > </md-input-container> <sustituir-notificaciones class="ng-scope">inactividad</sustituir-notificaciones> </div></div></div></div>');
	define('DASHBOARDS', array(
    'xxx',
    'xxx',
    'xxx',
	'xxx'
));
