Cada dispositivo consume un crédito diario.

Cada usuario puede incrementar su crédito por algún sistema a determinar (inicialmente todos los usuarios poseen 365 créditos).

Un usuario puede tener dispositivos propios y/o dispositivos a su cargo (por ejemplo, un delegador decide hacerse cargo del coste de la delegación).

Todos los días se descuentan automáticamente los créditos de los dispositivo propios y a cargo de cada usuario.

Cada customer posee 3 atributos **(credito, dispositivosPropios y dispositivosAsumidos)**, y sus correspondientes telemetrías (duplico los atributos en telemetrías para poder mostrar la evolución en gráficos del dashboard Configuracion).

Como medida de backup voy a mantener en el cliente_patron un atributo llamado accounting con la siguiente estructura:

"5334c2d0-88bb-11ea-823d-a96a9e8c2286":{"dispositivosAsumidos":0,"dispositivosPropios":20,"credito":306}

Al crear un dispositivo (o reclamarlo, si es un delegado por cuenta propia) se resta inmediatamente un crédito. Al borrar un dispositivo, si es por cuenta propia, se recupera automáticamente un crédito.

Si un usuario tiene un saldo nulo o negativo de créditos, no podrá crear más dispositivos ni delegaciones (ni por cuenta propia ni por cuenta del delegado).

Si el balance de créditos de un usuario pasa a ser negativo (rojo) dejarán de tramitarse sus telemetrías. Convendría avisar de esta eventualidad por email unos días antes. Podrá añadir créditos para poner su cuenta en "negro" de nuevo. Aquí podría valorarse una medida de gracia del estilo "el primer día que pase a negativo siguen procesándose sus telemetrías, pero el segundo ya no".
