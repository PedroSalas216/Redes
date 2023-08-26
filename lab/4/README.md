# Simulacion de redes complejas

## Autores
- [Juan Pablo Ludueña Zakka](mailto:juan.luduena.161@mi.unc.edu.ar)
- [Pedro Salas Piñero](mailto:pedrosalaspinero@mi.unc.edu.ar)
- [Lautaro Ebner](mailto:lautaro.ebner@mi.unc.edu.ar)

## Introduccion

En este laboratorio experimentamos sobre un algoritmo de enrutamiento sobre una red con topografia de anillo, es decir, una cadena simple de nodos, con el ultimo nodo conectado al primero. En este modelo de juguete vamos a poner a prueba dos algoritmos de enrutamiento, uno naive "clockwise" y uno mas complejo "shortest-path".

En este modelo, los nodos tienen una capa de aplicacion propia, y un vecino a izquierda y otro a derecha. Ademas, tenemos dos tipos de paquetes: `HelloPacket` y `Packet`. El primero tiene una lista de nodos.


### Clockwise: 
Solo usa un tipo de paquete, `Packet`. El algoritmo de enrutamiento es el siguiente: Cuando llega un paquete a un nodo, ese nodo chequea si le corresponde, si le corresponde lo manda para su capa de aplicacion, sino se lo envia al vecino a su derecha. 

### Shortest-Path: 
La idea es reconocer la red en la que se encuentran los nodos, asumiendo solamente que los nodos estan organizados en anillo. La forma de hacer esto es que cada nodo envie un paquete `HelloPacket` con la estrategia *"Clockwise+"* con si mismo como destinatario. *"Clockwise+"* agregaria la siguiente accion: cuando a cada nodo le llega un `HelloPacket` que no es suyo, pone su nombre al final de la lista. De esta forma, cuando los paquetes de reconocimiento llegan a cada nodo, cada nuevo paquete que salga desde la capa de aplicacion sale con la direccion que minimiza la distancia a su destino. Luego, si un paquete va hacia la derecha, sigue hacia la derecha hasta que llega a destino, y si va hacia la izquierda, sigue hacia la izquierda hasta que llega a destino. 

Mientras no tenemos la red reconocida (i.e. antes de que llegue el `HelloPacket` propio a cada nodo) tomamos la decision de usar el algoritmo anterior (*Clockwise*). En un principio nos surgio la idea de no mandar paquetes hasta tener reconocida la red, pero decidimos hacerlo asi para no tener un delay tan alto.


### Primer caso de estudio
> ¿Qué métricas se obtienen? 

En el primer caso de estudio vamos a ver cómo se comportan el delay promedio de los paquetes y el tamaño de buffer de cada nodo. El escenario que vamos a ver va a ser con los nodos (0,1,2,3,4,5,6,7) donde el nodo 0 y el nodo 2 envían paquetes al nodo 5. 

> ¿Cómo es el uso de los recursos de la red? ¿Se puede mejorar?

En este caso, el algoritmo *Clockwise* sufre un mayor delay, porque los paquetes que salen del nodo 0 dan toda la vuelta, mientras que usando *Shortest-Path* los paquetes usan el camino mas eficiente. Sin embargo, variando la cantidad de nodos, pero no variando quien manda o recibe, vimos que es posible que una gran parte de la red no se use, mientras que se congestionan las conecciones entre nodos mas cercanos al destino.

A continuacion veremos las metricas de este caso, donde la columna de la izquierda representa las metricas sacadas con el algoritmo *Clockwise* y la de la derecha con el algoritmo *Shortest-path*.


| Algoritmo **Clockwise**| Algoritmo **Shortest-Path**|
|:----------------------:|:--------------------------:|
|![](/assets/AlgoritmoProfe-Caso1-delay.svg)| ![](/assets/ShortestPath-Caso1-delay.svg) |
| ![](/assets/AlgoritmoProfe-Caso1-buffersize.svg) | ![](/assets/ShortestPath-Caso1-buffersize.svg) |


Ademas nuestro algoritmo implementado busca minimizar el camino que realiza cada paquete, es por esto que podemos ver cómo se comparan los algoritmos en la siguiente métrica "hopCount".



 ![](/assets/C1-hopcount.svg)


### Segundo caso de estudio

En el segundo caso de estudio vamos a ver cómo se comportan el delay promedio de los paquetes y el tamaño de buffer de cada nodo. El escenario que vamos a ver va a ser con los nodos (0,1,2,3,4,5,6,7) donde el nodo 5 recibe paquetes de todo el resto. 

Ahora veremos de la misma forma el uso de los recursos de la red, con los mismos parametros.


| Algoritmo **Clockwise**| Algoritmo **Shortest-Path**|
|:----------------------:|:--------------------------:|
|![](/assets/AlgoritmoProfe-Caso2-delay.svg)| ![](/assets/ShortestPath-Caso2-delay.svg) |
| ![](/assets/AlgoritmoProfe-Caso2-buffersize.svg) | ![](/assets/ShortestPath-Caso2-buffersize.svg) |



 ![](/assets/C2-hopcount.svg)

Como es de esperarse, el delay de los paquetes es mayor y tiene mas valores posibles dependiendo de cual es la distancia entre el nodo emisor y nodo 5.

> Explore y determine a partir de qué valor de interArrivalTime se puede garantizar un equilibrio o estabilidad en la red. 

Para esta consigna, decidimos evaluar los algoritmos en las mismas condiciones pero evaluando el uso efectivo (paquetes recibidos / paquetes enviados) de la red con respecto a `interArrivalTime`. Los graficos de abajo son resultado de estas mediciones.


| Algoritmo **Clockwise**| Algoritmo **Shortest-Path**|
|:-----:|:------:|
|![](/assets/ultimo-clockwise.svg)|![](/assets/ultimo-shortestpath.svg)|

De estos datos podemos concluir que el intervalo de generacion optimo para el algoritmo *Shortest-Path* esta entre el intervalo `(3,5)` porque vemos que es entre estas barras que se acerca mas al 100% del uso de la red, mientras que para el algoritmo *Clockwise* este sweet spot se encuentra fuera del rango de intervalos que medimos. Esto es una mejora, ya que significa que nuestro algoritmo puede manejar mayor velocidad de transmision de datos.


## Tarea diseño

Como es evidente, quisimos mantener la rama principal de la entrega limpia. Esta decision viene de la sugerencia que se nos dio en el coloquio previo. Sin embargo creemos que vale la pena notar 1) de dónde sacamos las mediciones y 2) cuál commit es el que consideramos como entrega. 

El commit que consideramos entrega de la tarea de diseño es [este](https://bitbucket.org/redes-famaf/redes23lab4g28/src/86f29d958777ddc1de1b79d1ea752e0a498cdd3e), de la rama [develop](https://bitbucket.org/redes-famaf/redes23lab4g28/src/develop/) sacamos las mediciones de *Shortest-Path*, mientras que de la rama [algoritmo-profe](https://bitbucket.org/redes-famaf/redes23lab4g28/src/algoritmo-profe/) sacamos las mediciones de *Clockwise*.



## Recursos y documentacion del proceso
 - [Documentacion omnet++](https://doc.omnetpp.org/omnetpp/manual/)
 - [Analisis del segundo caso](https://docs.google.com/spreadsheets/d/1Np0t0l80PH_x20gNaKZVG-qXYueYkEa6Wwn10FTkw-0/edit?usp=sharing)
 - [Pizzarra](https://excalidraw.com/#room=1fa9a964870662e4895e,C3v74kYm0ZonF9yZzJE7tQ)