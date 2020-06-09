Éste es el formato de archivo que tendría que importar el tenant para pre-aprovisionar los dispositivos.
Se almecena el nombre, el tipo y las credenciales originales por si en algún momento el cliente borrase el dispositivo (accidentalmente o no) y necesitara reclamarlo de nuevo.

El nombre de los dispositivos tiene este aspecto P0000000100000001, siendo la primera parte un identificador del proveedor, y la segunda un identificador del dispositivo dentro de los pre-aprovisionados por ese proveedor.

Al reclamar el dispositivo, el cliente tendrá que escribir su nombre sin la "P" inicial.
