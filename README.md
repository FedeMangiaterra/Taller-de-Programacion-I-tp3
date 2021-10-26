# Taller de Programación I
## Ejercicio N°3: My Little Middleware
* Nombre: Federico Carlos Mangiaterra 
* Padrón: 105771 
* Link del repositorio de Github: https://github.com/FedeMangiaterra/Taller-de-Programacion-I-tp3
## Introducción
Este trabajo consiste en la implementación en C++ de un sistema de servidor y cliente, en el cual el servidor es capaz de crear colas de mensajes, y hacer operaciones de push y pop sobre ellas, según lo indiquen los clientes. Estos clientes deben poder estar conectados al servidor de forma simultanea, por lo que se hace uso de threads para lograr esto.<br>
Para la resolución del ejercicio se crearon las siguientes clases:
* Client_middleware
* Server_middleware
* Socket (Misma del TP1 pero más adaptada a C++)
* Socket_exception
* Thread (Misma del TP2)
* Accepting_thread
* Client_thread
* Message_queue (Misma cola bloqueante del TP2, pero con mensajes representados por strings)
* Message_queue_map
* Operation <br>

## Resolución del trabajo
Primero se describe el funcionamiento "normal" del sistema. Finalmente se describe lo que pasa cuando se quiere detener la ejecución.
### Server_Middleware
La clase del servidor contiene un socket, el cual es el encargado de aceptar las conexiones de los clientes y una variable atomica de tipo booleano que indica si se debe detener o no la ejecución. El método run del server unicamente lanza el Accepting_thread y se queda esperando a que el usuario ingrese la tecla "q" por consola.
### Accepting_Thread
La clase de accepting thread usa una referencia al socket del server y una referencia al booleano atomico stop, y además tiene un vector de Client_thread* y un contenedor de colas de mensajes, el cual es un hash con un mutex que evita las race conditions entre los hilos que quieran acceder a él. Va aceptando clientes y creando hilos de la clase Client_thread para realizar las operaciones, los cuales son agregados a un vector de punteros a Client_Thread. Hace uso de move semantics para pasarle el Socket de la conexion a los nuevos hilos, para poder seguir creando más sin problemas mientras los hilos trabajan. Tras aceptar un cliente, recorre todo el vector de clientes usando un iterador, y comprueba si hay alguno aceptado anteriormente que haya terminado. Si es asi, lo elimina. El ciclo de aceptar clientes se detiene cuando se cierra el socket del servidor. <br>
![](http://www.plantuml.com/plantuml/png/SoWkIImgAStDuKhEIImkLWZ9A4fDJ85o79EJKmjAClFIuqkGXPrpCbFpIc1Y41qADZMwkWWDM2JHj3cavgK0hGK0) <br>
### Client_thread
Los Client_thread tienen un Socket, el cual representa la conexión con el cliente, una referencia al contenedor de colas, y una variable booleana que indica si debe seguir trabajando o no. Primero se llama a create_operation. Lo que hace esta función es recibir primero un byte, el cual, debido al protocolo de comunicación de este trabajo, representa la operación a realizarse. Dependiendo del caracter representado por ese byte, se crea un puntero al tipo de operacion correspondiente.
#### Creación de operaciones
Se cuentan con 3 operaciones distintas: define, push y pop. Las tres se crean de la misma forma inicialmente, porque todas encesitan el nombre de la cola sobre la cual trabajar. Primero reciben dos bytes que indican el largo del nombre. Estos deben ser ajustados a la endianness correcta usando la funcion ntohs(). Cuando ya se tiene el tamaño, se hace otro receive con el que se obtiene el nombre. La creación de las operaciones define y pop termina con eso. En el caso de push, vuelve a hacer el mismo proceso de recibir dos bytes para conocer el tamaño del siguiente receive. Con eso obtiene el larg del mensaje a pushear y el mensaje en sí. <br>
![](http://www.plantuml.com/plantuml/png/SoWkIImgAStDuKfCAYufIamkKV0lI2rABCdCp-DApaaiBbPmIKtBp4kDpqSJ3YWjpi0Kp2z04eERf62ba5gHcbnQaggXYIHgKwEhYs4ycZpO7fooQ1OwfEQb0Fq20000) <br>
Como lo que se crea es un puntero a operación, se hace uso del polimorfismo y se llama a la funcion execute de la operación creada. Como pueden haber varios hilos intentando realizar operaciones a la vez, tanto el contenedor de colas como las mismas colas usan mutex para evitar race conditions. En la ejecucion de pop, se hace un send del mensaje, con el protocolo de que los dos primeros bytes indiquen el largo del mismo. <br>
### Client_Middleware
El cliente, por su lado, se ejecuta indicando a que host y servicio se quiere conectar. Si la conexion falla, se finaliza la ejecución. El cliente va recibiendo las operaciones por entrada estandar. Si se ingresa una inexistente o con el formato incorrecto, se detiene la ejecución. Si sí es válida, se crea el mensaje que se debe enviar al servidor, poniendo el caracter identificador de la operacion en el primer byte, el largo del nombre de la cola en el segundo, luego el nombre de la cola, y si se trata de un push, tambien el largo del texto y el texto a enviarse. Luego de la creación, se envía. Si la operación era un pop, el cliente se queda esperando a recibir el mensaje que se sacó de la cola correspondiente. Si al momento de hacer el pop no había nada en la cola, el cliente se queda epserando hasta que sí haya. <br>
Este ciclo se repite hasta que haya un error o el usuario escriba 'exit' en la consola. <br> <br>
![](http://www.plantuml.com/plantuml/png/bP712i8m44Jl-OhyG0_DKJnKlCfU-W4X9ej6abJYLa7yU8LIa6GZUDrvCpimmurCfZa6CHqlQ4dkWyT4Edhd0ZvCHjst9TMMO9L6p7VCNz8gxAp5A_bqqdJEQ5oHjHnrpvyLVmOXobYvVNLTtK9kvC6afn75kmMlGdXQ_KL3csusuVRu64QKTefpyjOajXCtrcyVj7p0-0fawLejntJWGCM0m84W1ankZk4D) <br>

### Cierre del programa
El programa hace uso de las excepciones lanzadas por la clase socket, las cuales están en una nueva clase llamada Socket_exception. Cuando el cliente escribe exit, el programa del cliente se cierra y por lo tanto su socket tambien, por lo que se va a lanzar una excepcion cuando el Client_thread quiera hacer un receive del mensaje que indique la siguiente operacióna realizarse. Se hace un catch de esa excepción y la variable que indica si se debe frenar ese thread pasa a estar en true. Tras esto, cuando el hilo aceptador itere sobre su vector de clientes, verá que el hilo terminó y liberará su memoria. <br>
Por otro lado, cuando en el hilo principal se presiona la tecla q, se cierra el socket aceptador y se espera a que el hilo aceptador termine, por lo que el Accepting_thread deberá hacer un catch de la excepcion lanzada cuando falle el accept. Este catch hace que salga del loop de acpetar, y pase a recorrer todo su vector de clientes, cerrando sus sockets uno por uno, lo que hará que se detengan el en el momento.
