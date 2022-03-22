# 1. Asignar un entero a cada telemetría
Por ejemplo, en el caso del dispositivo TTNMAD_PLANT
1. Moisture
2. Luminosity
3. Temp
4. Hum
5. Bat

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
    
