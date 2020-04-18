<!DOCTYPE html>
<html lang="es">
    <head>
        <meta charset="utf-8">
        <title>IoT open Tech</title>
        <meta name="description" content="">
        <?php
        include('config.php');

        if ($_SERVER['REQUEST_METHOD'] === 'POST' && !empty($_POST) && isset($_POST['ENVIAR']) && isset($_POST['email'])) {    //Procesar el formulario
            echo "</head>";
            echo '<body style="text-align: center;">';
            if (isset($_POST['recaptcha_response'])) {
                // Build POST request:
                $recaptcha_url = 'https://www.google.com/recaptcha/api/siteverify';
                $recaptcha_secret = CAPTCHA_PRIVATE;
                $recaptcha_response = $_POST['recaptcha_response'];

                // Make and decode POST request:
                $recaptcha = file_get_contents($recaptcha_url . '?secret=' . $recaptcha_secret . '&response=' . $recaptcha_response);
                $recaptcha = json_decode($recaptcha);

                // Take action based on the score returned:
                if ($recaptcha->score >= 0.5) {
                    // Verified - send email
                } else {
                    // Not verified - show form error
                    die("No ha demostrado ser lo suficientemente humanno. Lo siento si es un error, no si no.");
                }
            } else {
                die("Ha fallado el sistema de ReCaptcha. Lo sentimos. Inténtelo más tarde.");
            }



            //Procesar formulario
            //Obtengo el jwt de la base de datos y, si ha caducado, requiero otro
            $mysqli = new mysqli(DBHOST, DBUSER, DBPWD, DBNAME);
            if ($mysqli->connect_errno) {
                die("Falló la conexión con la base de datos");
            }
            $mysqli->set_charset("utf8");

            $query = "SELECT token FROM token LIMIT 1";
            $resultado = $mysqli->query($query);
            $fila = $resultado->fetch_row();
            $JWT = $fila[0];
            //echo $JWT;

            $curl = curl_init();
            $headers = [
                "X-Authorization: Bearer $JWT",
                "Accept: application/json"
            ];

            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_URL, 'https://tb.iotopentech.io/api/tenant/' . TENANT_ID);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                //$info = curl_getinfo($curl);
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 0');
            }
            curl_close($curl);
            $decoded = json_decode($curl_response);
            //var_dump($decoded); 
            if (isset($decoded->status) && $decoded->status == 401) {
                //El token ha expirado
                $url = 'https://tb.iotopentech.io/api/auth/login';
                $options = array(
                    'http' => array(
                        'header' => "Content-type: application/json\r\n",
                        'method' => 'POST',
                        'content' => '{"username":"' . TENANT_NAME . '", "password":"' . TENANT_PWD . '"}'
                    ),
                );
                $context = stream_context_create($options);
                $result = file_get_contents($url, false, $context);
                $obj = json_decode($result);
                if (!isset($obj->token)) {
                    die('SE HA PRODUCIDO UN ERROR DE TIPO 1');
                }

                $query = "UPDATE token SET token='" . $obj->token . "' WHERE 1";
                $mysqli->query($query);
                $JWT = $obj->token;
                //var_dump($JWT);
            } else if (isset($decoded->id) && $decoded->id->entityType == "TENANT") {
                //Todo bien; no hago nada. El token sigue vigente
            } else {
                die('SE HA PRODUCIDO UN ERROR DE TIPO 2');
            }


            //Comprobar si el usuario existe
            //Al comprobarlo directamente le manda un mail de activación
            $email = $mysqli->real_escape_string($_POST['email']);
            $service_url = 'https://tb.iotopentech.io/api/user/sendActivationMail?email=' . urlencode($email);
            $curl = curl_init($service_url);
            $curl_post_data = array(
                'message' => '{}'
            );
            $headers = [
                "Content-type: application/json",
                "X-Authorization: Bearer $JWT",
                "Accept: application/json"
            ];
            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_POST, true);
            curl_setopt($curl, CURLOPT_POSTFIELDS, $curl_post_data);
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                //$info = curl_getinfo($curl);
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 3');
            }
            curl_close($curl);
            $decoded = json_decode($curl_response);
            //Si no devuelve nada quiere decir que el usuario ya existe pero aún no está activo
            if ($decoded == null) {
                die("LE HEMOS ENVIADO UN MENSAJE A LA DIRECCIÓN $email.<br/>GRACIAS");
            } else if(isset($decoded->status) && isset($decoded->errorCode) && $decoded->errorCode==31){
                //Ya está activado
                mail ( $email , 'Activación cuenta' , "Se ha solicitado la activación de su cuenta en ThingsBoard TTN Edition, pero ya está activada." );
                die("LE HEMOS ENVIADO UN MENSAJE A LA DIRECCIÓN $email.<br/>GRACIAS");
            }            
            if (!isset($decoded->errorCode) || $decoded->errorCode != '32') {
                die('SE HA PRODUCIDO UN ERROR DE TIPO 4');
            }

            //Crear el customer					
            $query = "INSERT INTO clientes VALUES (NULL, '$email','" . date("Y-m-d H:i:s") . "', '','')";

            $mysqli->query($query);
            $mysqli->error;
            $customerID = $mysqli->insert_id;
            $customerName = str_pad($customerID, 8, "0", STR_PAD_LEFT);

            $service_url = 'https://tb.iotopentech.io/api/customer';
            $curl = curl_init($service_url);
            $curl_post_data = array(
                "id" => null,
                "title" => "$customerName",
                "tenantId" => array(
                    "id" => TENANT_ID,
                    "entityType" => "TENANT"
                )
            );
            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_POST, true);
            curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($curl_post_data));
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 5');
            }
            curl_close($curl);
            $decoded = json_decode($curl_response);
            if (!isset($decoded->id) || !isset($decoded->id->id)) {
                die('SE HA PRODUCIDO UN ERROR DE TIPO 6');
            }
            $customerTBID = $decoded->id->id;
            //Almacenar el id del customer				
            $query = "UPDATE clientes SET customerID='" . $customerTBID . "' WHERE id=$customerID";
            $mysqli->query($query);
            
            //Crear los atributos del customer
            $service_url = "https://tb.iotopentech.io/api/plugins/telemetry/CUSTOMER/$customerTBID/attributes/SERVER_SCOPE";
            $curl = curl_init($service_url);
            $curl_post_data = array(
                "tiposDeDispositivos" => TIPOS_DE_DISPOSITIVOS,
                "tiposDeActivos" => TIPOS_DE_ACTIVOS,
				"V02_001_config"=> V02_001_CONFIG
            );

            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_POST, true);
            curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($curl_post_data));
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 7');
            }
            curl_close($curl);
            $decoded = json_decode($curl_response);
            
            
            //Crear el activo ROOT
            $service_url = 'https://tb.iotopentech.io/api/asset';
            $curl = curl_init($service_url);
            $curl_post_data = array("customerId" => array(
                    "id" => "$customerTBID",
                    "entityType" => "CUSTOMER"
                ),
                "id" => null,
                "name" => $customerName . '_ROOT',
                "tenantId" => array(
                    "id" => TENANT_ID,
                    "entityType" => "TENANT"
                ),
                "type" => "ROOT"
            );

            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_POST, true);
            curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($curl_post_data));
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 8');
            }
            curl_close($curl);
            $decoded = json_decode($curl_response);
            if (!isset($decoded->id) || !isset($decoded->id->id)) {
                die('SE HA PRODUCIDO UN ERROR DE TIPO 9');
            }
            $rootID = $decoded->id->id;

            //Crear la relacion con el customer
            $service_url = 'https://tb.iotopentech.io/api/relation';
            $curl = curl_init($service_url);
            $curl_post_data = array(
                "from" => array(
                    "entityType" => "CUSTOMER",
                    "id" => "$customerTBID"
                ),
                "to" => array(
                    "entityType" => "ASSET",
                    "id" => "$rootID"
                ),
                "type" => "$rootID",
                "typeGroup" => "COMMON"
            );
            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_POST, true);
            curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($curl_post_data));
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 10');
            }
            curl_close($curl);

            //Crear los atributos del asset root
            $service_url = "https://tb.iotopentech.io/api/plugins/telemetry/ASSET/$rootID/attributes/SERVER_SCOPE";
            $curl = curl_init($service_url);
            $curl_post_data = array(
                "nombreEntidad" => "_ROOT",
                "tipoEntidad" => "ROOT"
            );

            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_POST, true);
            curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($curl_post_data));
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 11');
            }
            curl_close($curl);
            $decoded = json_decode($curl_response);
            


            //Crear el dispositivo CONTROL
            $service_url = 'https://tb.iotopentech.io/api/device';
            $curl = curl_init($service_url);
            $curl_post_data = array(
                "customerId" => array(
                    "id" => "$customerTBID",
                    "entityType" => "CUSTOMER"
                ),
                "id" => null,
                "name" => $customerName . '_CONTROL',
                "tenantId" => array(
                    "id" => TENANT_ID,
                    "entityType" => "TENANT"
                ),
                "type" => "SYSTEM"
            );
            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_POST, true);
            curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($curl_post_data));
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 12');
            }
            curl_close($curl);
            $decoded = json_decode($curl_response);

            //Crear el usuario
            $service_url = 'https://tb.iotopentech.io/api/user?sendActivationMail=true';
            $curl = curl_init($service_url);
            $curl_post_data = array(
                "authority" => "CUSTOMER_USER",
                "customerId" => array(
                    "id" => "$customerTBID",
                    "entityType" => "CUSTOMER"
                ),
                "email" => "$email",
                "id" => null,
                "tenantId" => array(
                    "id" => TENANT_ID,
                    "entityType" => "TENANT"
                )
            );
            curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
            curl_setopt($curl, CURLOPT_POST, true);
            curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($curl_post_data));
            curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
            $curl_response = curl_exec($curl);
            if ($curl_response === false) {
                curl_close($curl);
                die('SE HA PRODUCIDO UN ERROR DE TIPO 13');
            }
            curl_close($curl);
            $decoded = json_decode($curl_response);
            
            //Asignar todos los dashboards al cliente
            foreach (DASHBOARDS as $dashboardID){
                
                $service_url = "https://tb.iotopentech.io/api/customer/$customerTBID/dashboard/$dashboardID";
                $curl = curl_init($service_url);
                $curl_post_data = array( );
                curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
                curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
                curl_setopt($curl, CURLOPT_POST, true);
                curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($curl_post_data));
                curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
                $curl_response = curl_exec($curl);
                if ($curl_response === false) {
                    curl_close($curl);
                    die('SE HA PRODUCIDO UN ERROR DE TIPO 13');
                }
                curl_close($curl);
            }

            //Guardr el resultado en la base de datos
            $query = "UPDATE clientes SET resultado='CORRECTO' WHERE id=$customerID";
            $mysqli->query($query);
            echo "LE HEMOS ENVIADO UN MENSAJE A LA DIRECCIÓN $email.<br/>GRACIAS";
        } else {
            //Mostrar el formulario
            ?>
            <script src="https://www.google.com/recaptcha/api.js?render=<?php echo CAPTCHA_WEB; ?>"></script>
            <script>
                grecaptcha.ready(function () {
                    grecaptcha.execute('<?php echo CAPTCHA_WEB; ?>', {action: 'contact'}).then(function (token) {
                        var recaptchaResponse = document.getElementById('recaptchaResponse');
                        recaptchaResponse.value = token;
                    });
                });
            </script>
        </head>
        <body style="text-align: center;">	
            <h1>IoT open Tech</h1>
            <h2>Crear cuenta de cliente en ThingsBoard TTN Edtition</h2>
            <p>Indique la dirección de correo electrónico que desee asociar a su cuenta de cliente.</p>
            <p>Recibirá un mensaje en la dirección anterior con el resultado de la operación.</p>
            <form method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>">
                <label for="email">email:</label>
                <input type="email" name="email" required/>
                <br/>
                <input type="submit" name="ENVIAR" value="ENVIAR"/>
                <input type="hidden" name="recaptcha_response" id="recaptchaResponse">
            </form>
        </body>
        <?php
    }
    ?>
</html>




