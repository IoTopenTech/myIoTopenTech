```xml
<myIoT>
	<panel  titulo="" resumen="" nombreFormulario="" labelBotonSubmit="[Configurar]" ultimoDownlink="" [tipo="devEUI"]>
		<item tipo="coordenadas/chirpstack/alarma/atributoCompartido" 
		      
			nombreAlarma="" telemetria=""
		     	labelAlarma=""
		      	tipoAlarma="opciones/umbralMinimo/umbralMaximo/biUmbral"
		      	labelAuxAlarma="Disparar al/expresado en segundos..." 
		      	histeresis="no/min/max/bi"
			valores="A/B/C"
			
		      	nombreAtributoCompartido=""
		      	labelAtributoCompartido=""
		      	tipoAtributoCompartido="opciones/texto/numero/boton">
				
				<umbralMinimo min="" max="" step="" --> atributos estándar HTML />
				<umbralMaximo min="" max="" step="" --> atributos estándar HTML />
				<histeresisMinimo min="" max="" step="" --> atributos estándar HTML />
				<histeresisMaximo min="" max="" step="" --> atributos estándar HTML />
				
				<atributoCompartido min="" max="" step="" size="" pattern="" --> atributos estándar HTML />
					
		</item>
	</panel>
</myIoT>
```
telemetria se usa para saber si el usuario posee la delegación necesaria. Si llegamos a implementar alarmas que impliquen simultáneamente varias telemetrías, el atributo telemetría pasaría a ser un array como opciones.
