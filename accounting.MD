Cada dispositivo consume un crédito diario.

Cada usuario puede incrementar su crédito por algún sistema a determinar (inicialmente todos los usuarios poseen 365 créditos).

Un usuario puede tener dispositivos propios y/o dispositivos a su cargo (por ejemplo, un delegador decide hacerse cargo del coste de la delegación).

Todos los días se descuentan automáticamente los créditos de los dispositivo propios y a cargo de cada usuario por adelantado si el saldo de créditos es mayoro igual que 0 (se cobra el día por adelantado). Si es menor que 0, no se resta, porque ya tiene anuladas las telemetrías para ese día. En otras palabras: al tener un saldo deudor, no se sigue cobrando porque tampoco se presta servicio de telemetrías.

Cada customer posee 3 atributos **(credito, dispositivosPropios y dispositivosAsumidos)**, y sus correspondientes telemetrías (duplico los atributos en telemetrías para poder mostrar la evolución en gráficos del dashboard Configuracion).

Como medida de backup voy a mantener en el cliente_patron un atributo llamado accounting con la siguiente estructura:

"5334c2d0-88bb-11ea-823d-a96a9e8c2286":{"dispositivosAsumidos":0,"dispositivosPropios":20,"credito":306}

Al crear un dispositivo (o reclamarlo, si es un delegado por cuenta propia) se resta inmediatamente un crédito. Al borrar un dispositivo, si es por cuenta propia, se recupera automáticamente un crédito.

Si un usuario tiene un saldo nulo o negativo de créditos, no podrá crear más dispositivos ni delegaciones (ni por cuenta propia ni por cuenta del delegado).

El saldo de créditos puede pasar a negativo al comienzo del día o si un delegado reclama su dispositivo por cuenta del delegador. No puede pasar a negativo por la creación de dispositivos ni de delegados por cuenta propia porque no está permitido. Si el balance de créditos de un usuario pasa a ser negativo (rojo) se actualizará su atributo fechaDeudora y se le avisará por mail. Podrá añadir créditos para poner su cuenta en "negro" de nuevo y borrar el atributo fechaDeudora. El día siguiente a la fechaDeudora dejarán de tramitarse sus telemetrías.
