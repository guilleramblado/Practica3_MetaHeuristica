#include <iostream> 
#include <fstream> //para ifstream
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <random>
#include <numeric>
#include <map>
#include <cassert>
using namespace std;

#ifndef PROGRAMA //evitamos que se inserte vairas veces el mismo archivo de cabecera
#define PROGRAMA

//definimos un struct que almacenará las matrices de flujo/distancia leidas del fichero
struct Matrices{
    vector<vector<int>> matriz_flujo;
    vector<vector<int>> matriz_distancia;
};

//lectura del fichero de datos del problema
Matrices lectura_datos(string nombre_fichero){

    ifstream fichero(nombre_fichero);
    if (!fichero){
        cout<<"Error al abrir el fichero";
        exit(EXIT_FAILURE);
    }

    //leemos el valor de 'n'
    int n;
    fichero>>n;

    Matrices salida;
    //especificamos el tamaño de las dos matrices del struct
    salida.matriz_flujo.resize(n,vector<int>(n));
    salida.matriz_distancia.resize(n,vector<int>(n));

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            fichero>>salida.matriz_flujo[i][j];
        }
    }
    //lectura matriz de distancias
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
           fichero>>salida.matriz_distancia[i][j];
        }
    }

    //terminamos de trabajar con el fichero...
    fichero.close();

    return salida;

}

/*
Funcion fitness -> evalúa cómo de buena es una solución al problema (su calidad)
En este caso, una solución será mejor cuanto menor sea su valor fitness, es decir, cuanto menor sea el coste total de asignacion
La funcion fitness devolverá el coste total de las asignaciones
Parámetros:
1.Solucion cuya calidad queremos conocer: vector de n componentes
2.Matriz de distancia
3.Matriz de flujo
4. Numero de unidades a asignar entre las localizaciones
*/

int fitness(vector<int> solucion,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    int coste_total=0;
    int distancia=0;
    int flujo=0;

    int n=matriz_flujo.size();

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            distancia=matriz_distancia[solucion[i]][solucion[j]];
            flujo=matriz_flujo[i][j];
            coste_total+=distancia*flujo;
        }
    }

    return coste_total;

}


/*
Cálculo del fi de cada unidad//di de cada localizacion
fi: entero calculado como la suma del flujo de la unidad i-resto de unidades
di: entero calculado como la suma de la distancia de la localizacion i-resto de localizaciones
la función devolvera un array con dos vectores de tamaño 'n':
1) Flujo total de cada unidad
2) Distancia total de cada localizacion
*/

struct DosVectores{
    //definimos los vectores de potenciales de flujo / distancia
    vector<int> potenciales_flujo;
    vector<int> potenciales_distancia;
    
    DosVectores(int n){
        potenciales_flujo.resize(n);
        potenciales_distancia.resize(n);
    }
    
};

DosVectores potenciales(vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){

   int n=matriz_flujo.size();
   DosVectores salida(n);

   int flujo;
   int distancia;

   for (int i = 0; i < n; i++) {
        flujo = 0;
        distancia = 0;

        for (int j = 0; j < n; j++) {
            flujo += matriz_flujo[i][j];
            distancia += matriz_distancia[i][j];
        }

        salida.potenciales_flujo[i] = flujo;
        salida.potenciales_distancia[i] = distancia;
    }

    //ya tenemos los potenciales de flujo y distancia, los devolvemos

    return salida;

}


/*
algoritmo Greedy, construimos la solucion paso a paso, tal que la localizacion a la
que se asignará la siguiente unidad 'i' será la mejor a nivel local
*/

bool comparar_flujos(pair<int,int> a,pair<int,int> b){
    //return(a.second>=b.second);
    //recordemos que tiene que definir un ordenamiento estrictamente debil
    //esto implica que debe definir una relacion irreflexiva y antisimetrica
    /*
    -irreflexiva: un elemento no es menor que sí mismo (devuelve false para dos mismos elementos)
    -antisimetrica: si devuelve 'true' para 'x' e 'y', devolverá 'false' para 'y e 'x'
    (dicho de otra forma: si x<=y y y<=x ---> x=y [parejas iguales])
    */
    if(a.second>b.second){
        return true; //primero va el potencial de flujo mayor
    }
    else if(a.second==b.second){
        //mismo potencial, colocamos por ej la pareja tal que el indice de la unidad sea mayor
        return(a.first>b.first);
    }
    else{
        //potencial de 'a' menor estricto que potencial de 'b' -> 'b' va antes que 'a'
        return false;
    }
}

//¿Como se ordenan los potenciales de distancia? --> de menor a mayor
//Si devuelve true (primer potencial de distancia < sgundo potencial de distancia) -> 'a' irá antes que 'b'
bool comparar_distancias(pair<int,int> a,pair<int,int> b){
    if(a.second<b.second){
        return true; //primero va el potencial de distancia menor
    }
    else if(a.second==b.second){
        //mismo potencial, colocamos por ej la pareja tal que el indice de la unidad sea menor
        return(a.first<b.first);
    }
    else{
        //potencial de distancia de 'a' mayor estricto que el de 'b'
        return false;
    }
}

vector<int> algoritmo_greedy(vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //primero calculamos los potenciales para cada unidad y distancia...
    DosVectores matrices=potenciales(matriz_flujo,matriz_distancia);
    vector<int> potenciales_flujo=matrices.potenciales_flujo;
    vector<int> potenciales_distancia=matrices.potenciales_distancia;
    /*
    cout<<"Imprimo el vector de potenciales de distancia... de tamanio"<<potenciales_distancia.size()<<"\n";
    for(int i=0;i<potenciales_distancia.size();i++){
        cout<<potenciales_distancia[i]<<"-";
    }
    */
    
    //ordenamos primeramente los vectores de potenciales
    //si ordenamos directamente los potenciales de flujo, perdemos la referencia de la unidad con ese flujo
    //por tanto definimos una pareja unidad-fi correspondiente
    int n=potenciales_flujo.size();

    pair<int,int> pareja;
    vector<pair<int,int>> flujos;
    vector<pair<int,int>> distancias;
    //recorremos 
    for(int i=0;i<n;i++){
        //unidad-potencial de flujo
        pareja.first=i;
        pareja.second=potenciales_flujo[i];
        flujos.push_back(pareja);
        //localizacion-potencial de distancia
        pareja.second=potenciales_distancia[i];
        distancias.push_back(pareja);
    }
    /*
    cout<<"\nImprimo parejas (unidad,potencial flujo), de tamanio "<<flujos.size()<<"\n";
    for(int i=0;i<n;i++){
        cout<<flujos[i].first<<"-"<<flujos[i].second<<"\n";
    }
    */
    
    /*
    //imprimo para comprobar
    cout<<"\nPAREJAS UNIDAD-POTENCIAL DE FLUJO...\n";
    for(int i=0;i<n;i++){
        cout<<flujos[i].first<<"-"<<flujos[i].second<<"\n";
    }

    cout<<"\nPAREJAS UNIDAD-POTENCIAL DE FLUJO...\n";
    for(int i=0;i<n;i++){
        cout<<distancias[i].first<<"-"<<distancias[i].second<<"\n";
    }
    */
    
    //procedemos a ordenar
    
    sort(flujos.begin(),flujos.end(),comparar_flujos);
    sort(distancias.begin(),distancias.end(),comparar_distancias);
    //compruebo si se han ordenado bien...
    /*
    cout<<"Imprimo el vector de potenciales de flujo... de tamanio"<<flujos.size()<<"\n";
    for(int i=0;i<flujos.size();i++){
        cout<<flujos[i].second<<"-";
    }
    cout<<"\nImprimo el vector de potenciales de distancia... de tamanio"<<distancias.size()<<"\n";
    for(int i=0;i<distancias.size();i++){
        cout<<distancias[i].second<<"-";
    }
    */
    
    //construimos la solucion paso a paso...
    vector<int> solucion;
    solucion.resize(n);

    
    for(int i=0;i<n;i++){
        //tomamos la unidad con mayor potencial de flujo y le asignamos la distancia con menor potencial
        //flujos[i].first con distancias[i].first 
        solucion[flujos[i].first]=distancias[i].first;
    }

    /*
    cout<<"\nSOLUCION OBTENIDA:\n";
    for(int i=0;i<n;i++){
        cout<<"(unidad "<<i<<" ,localizacion "<<solucion[i]<<")\n";
    }
    */
    
    return solucion;
}

//procedemos a implementar la busqueda local, haciendo uso de las siguientes funciones

//funcion que nos devuelve la diferencia de coste entre la nueva solucion vecina y la solucion actual
//si el valor devuelto es negativo -> hemos encontrado una solucion de mayor calidad
//podemos hacer Coste(solucion_nueva)-Coste(solucion-actual), pero en este caso lo que haremos
//será realizar el menor numero de operaciones posibles factorizando...
//recibe como entrada la solucion actual, y las unidades (entre 0 y n-1) cuyas localizaciones queremos intercambiar
int diferencia_coste(vector<int> solucion_actual,int r,int s,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    int n=solucion_actual.size();
    //los indices de las unidades deben estar en [0,n-1]
    assert(r>=0 && r<n && r!=s);
    assert(s>=0 && s<n);

    int diferencia=0;
    for(int i=0;i<n;i++){
        if(i!=r && i!=s){
            diferencia+=matriz_flujo[r][i]*(matriz_distancia[solucion_actual[s]][solucion_actual[i]]-matriz_distancia[solucion_actual[r]][solucion_actual[i]])+
                        matriz_flujo[s][i]*(matriz_distancia[solucion_actual[r]][solucion_actual[i]]-matriz_distancia[solucion_actual[s]][solucion_actual[i]])+
                        matriz_flujo[i][r]*(matriz_distancia[solucion_actual[i]][solucion_actual[s]]-matriz_distancia[solucion_actual[i]][solucion_actual[r]])+
                        matriz_flujo[i][s]*(matriz_distancia[solucion_actual[i]][solucion_actual[r]]-matriz_distancia[solucion_actual[i]][solucion_actual[s]]);
        }
    }

    return diferencia;
}

//bucle interno de la BL que se encarga de explorar el vecindario
/*
EL proceso a seguir es el siguiente:
1. Se generan las soluciones vecinas a la actual UNA A UNA hasta que:
    1.1 Se consiga una solucion vecina mejor que la actual -> seguimos iterando
        con esta nueva solucion vecina
    1.2 Generamos el vecindario por completo sin encontrar nada mejor -> FIN

Pseudocodigo:
Bucle externo (cada iteracion equivale a generar el vecindario de cada una de las diversas soluciones que vayamos considerando hasta que generemos el vecindario por completo)
Bucle interno (genera el vecindario de la determinada solucion actual que estamos considerando, frenando si conseguimos una solucion vecina mejor o recorrer todo el vecindario sin exito)
Bucle externo, en resumen, itera los entornos de las diferentes soluciones actuales que vayamos considerando
Bucle interno, en resumen, se focaliza en una determinada solucion actual, y genera su vecindario
*/
vector<int> busqueda_vecindario(vector<bool> &dlb,vector<int> solucion_actual,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    int n=dlb.size();
    //bool mejora=false;

    //fijando la solucion actual, generamos su vecindario, generando primero las soluciones vecinas
    //intercambiando la localizacion asignada a una determinada unidad 'i' con el resto...
    //Si ninguna de las soluciones vecinas obtenidas es mejor, se desactiva dicha unidad
    for(int i=0;i<n;i++){
        if(!dlb[i]){
        //soluciones vecinas que podemos generar utilizando la unidad 'i'
           // mejora=false;
            for(int j=0;j<n;j++){
                //intercambiamos la localizacion de la unidad 'i' con la del resto de unidades
                if(j==i) continue;
                //intercambiamos localizaciones siempre y cuando produzca una mejora
                if(diferencia_coste(solucion_actual,i,j,matriz_flujo,matriz_distancia)<0){
                    //solucion genera es mejor...
                    swap(solucion_actual[i],solucion_actual[j]);
                    //dlb[i]=false;
                    dlb[j]=false;
                    //mejora=true;
                    return solucion_actual;
                }
            }
            /*
            if(!mejora){
                //la unidad 'i' no nos ha permitido generar soluciones vecinas de mayor calidad, no nos sirve
                dlb[i]=true;
            }
            */
           //si hemos llegado hasta aqui es porque no hemos encontrado ninguna mejor solucion vecina utilizando la unidad 'i'
           dlb[i]=true;
        }
    }
    //devolvemos la última solucion vecina generada en el vecindario de la solucion inicial considerada...
    return solucion_actual;
}

//GENERADOR DE UN NUMERO ALEATORIO EN [0,n-1]
int myrandom(int i) {
    return rand()%i;
}

//FUNCION QUE DEVUELVE UNA SOLUCION INICIAL
/*
Usaremos las siguientes funciones:
- srand(), para inicializar la semilla que usa la funcion rand()
- rand()%n para generar numeros aleatorios en [0,n)
*/

vector<int> generar_solucion_aleatoria(unsigned int semilla,int tamanio){
    //creamos un vector de n componentes que almacenen por defecto '0'
    vector<int> solucion_inicial(tamanio);
    //usamos la funcion iota para que almacene los enteros de 0 a tamanio-1
    iota(solucion_inicial.begin(),solucion_inicial.end(),0);
    /*
    cout<<"\nSOLUCION INICIAL SIN PERMUTAR --> ";
    for (auto n : solucion_inicial) {
            cout << n << " ";
    }
    */

    //inicializamos la semilla del generador de numeros aleatorios
    srand(semilla);
    //permutamos usando la funcion random_shuffle()
    random_shuffle(solucion_inicial.begin(),solucion_inicial.end(),myrandom);
    /*
    cout<<"\nSOLUCION INICIAL TRAS PERMUTAR --> ";
    for (auto n : solucion_inicial) {
            cout << n << " ";
    }
    */

    return solucion_inicial;
}

//bucle externo de la busqueda local
vector<int> busqueda_local(vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia,unsigned int semilla){
    //generamos una solucion inicial de la cual partir...
    vector<int> solucion_actual=generar_solucion_aleatoria(semilla,matriz_flujo.size());

    vector<bool> dlb(matriz_flujo.size(),false);
    vector<int> solucion_obtenida;
    bool seguir_iterando=false;

    //bucle externo...
    do{
        //la busqueda interna modifica la solucion_actual siempre y cuando se consiga una mejor solucion
        //por lo que si devuelve un vector solucion identico a la solucion_actual pasada como parametro,
        //quiere decir que no se ha encontrado una mejor solucion vecina en dicho vecindario, y finaliza la bsuqueda local
        solucion_obtenida=busqueda_vecindario(dlb,solucion_actual,matriz_flujo,matriz_distancia);
        seguir_iterando=(solucion_obtenida!=solucion_actual); 
        //actualizamos la solucion actual
        solucion_actual=solucion_obtenida;
    }while(seguir_iterando); //mientras que la solucion vecina sea de mayor calidad

    return solucion_actual;
}

//CODIGO CORRESPONDIENTE A LA PRACTICA 2

/*
Funcion que genera una poblacion totalmente nueva especificando los siguientes parámetros:
1. Nº de unidades/localizaciones del problema QAP a resolver --> tamaño de cada solucion
2. Nº de individuos de la población (=nº de soluciones a generar)

En mi caso, almacenaré cada individuo junto con su correspondiente valor fitness para evitar recalcular el fitness cada vez que lo necesite a lo largo
de las diferentes etapas que se llevan a cabo.
*/
vector<pair<vector<int>,int>> generar_poblacion(int n, int tam_poblacion,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    vector<pair<vector<int>,int>> poblacion;
    pair <vector<int>,int> pareja;
    //generamos soluciones aleatorias
    for(int i=0;i<tam_poblacion;i++){
        vector<int> solucion_inicial(n);
        //usamos la funcion iota para que almacene los enteros de 0 a tamanio-1
        iota(solucion_inicial.begin(),solucion_inicial.end(),0);
        //permutamos para generar una solucion aleatoria
        random_shuffle(solucion_inicial.begin(),solucion_inicial.end(),myrandom);
        //calculamos su fitness y definimos la nueva pareja a almacenar
        pareja=make_pair(solucion_inicial,fitness(solucion_inicial,matriz_flujo,matriz_distancia));
        //almacenamos el nuevo individuo generado en la poblacion de partida
        poblacion.push_back(pareja);
    }
    return poblacion;   
}

/*
Funcion que devuelve el indice sobre la poblacion de todos aquellos individuos que han sigo escogidos en el proceso de seleccion
*/
vector<int> seleccion_padres(vector<pair<vector<int>,int>> poblacion,int num_padres){
        vector<int> padres; //vector que almacena el indice de los individuos que han sido seleccionados
        int padre1,padre2 = -1;
        int num_individuos=poblacion.size();
        for(int i=0;i<num_padres;i++){
            //escogemos dos individuos de forma aleatoria
            padre1=rand()%num_individuos;
            padre2=rand()%num_individuos;
            if(poblacion[padre1].second<poblacion[padre2].second){
                //el individuo 'padre1' es mejor que 'padre2', lo seleccionamos
                padres.push_back(padre1);
            }
            else{
                padres.push_back(padre2);
            }
        }
        return padres;

}

/*
Funcion que se encarga de cruzar dos padres entre sí usando el cruce basado en posición
Como no nos importa el coste de ambos padres, requerimos únicamente los vectores solución
Se devuelve el hijo generado, expresado mediante una pareja (solucion_generada,coste de la solucion)
*/
pair<vector<int>,int> cruce_posicion(vector<int> padre1, vector<int> padre2,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //primeramente rellenamos aquellos genes del hijo utilizando los genes que coincidan en posicion y valor en ambos padres
    int num_genes=padre1.size();
    vector<int> posiciones_no_coinciden; //almacena la posicion de aquellos genes que no coinciden
    vector<int> valores_posiciones_no_coinciden;
    vector<int> hijo(num_genes);

    for(int i=0;i<num_genes;i++){
        //comparamos genes en dicha posicion
        if(padre1[i]==padre2[i]){
            hijo[i]=padre1[i];
            //cout<<"Se repite el gen "<<padre1[i];
        }
        else{
            //guardamos la posicion en la que los genes de ambos padres no coinciden
            posiciones_no_coinciden.push_back(i); //posiciones libres del hijo
            valores_posiciones_no_coinciden.push_back(padre1[i]); //si son diferentes, nos quedamos con el gen del padre1
        }

    }

    /*
    cout<<"\nPosiciones que no coinciden...\n";
    for(int i=0;i<posiciones_no_coinciden.size();i++){
        cout<<posiciones_no_coinciden[i]<<"-";
    }
    cout<<"\nValores para las posiciones previas...\n";
    for(int i=0;i<valores_posiciones_no_coinciden.size();i++){
        cout<<valores_posiciones_no_coinciden[i]<<"-";
    }
    */
    //permutamos los genes que no han coincido
    random_shuffle(valores_posiciones_no_coinciden.begin(),valores_posiciones_no_coinciden.end(),myrandom);

    /*
    cout<<"\nValores tras permutar...\n";
    for(int i=0;i<valores_posiciones_no_coinciden.size();i++){
        cout<<valores_posiciones_no_coinciden[i]<<"-";
    }
    */

    //almacenamos en las posiciones libres del hijo
    for(int j=0;j<posiciones_no_coinciden.size();j++){
        hijo[posiciones_no_coinciden[j]]=valores_posiciones_no_coinciden[j];
    }

    //ya hemos generado el hijo, lo devolvemos
    pair <vector<int>,int> pareja=make_pair(hijo,fitness(hijo,matriz_flujo,matriz_distancia));

    return pareja;

}

/*
Segundo operador de cruce: PMX
El funcionamiento es el siguiente: se selecciona una subcadena central en ambos padres
El primer hijo copia la subcadena central del segundo  padre, y el resto de genes serán los genes del padre1 fuera de la subseccion. Si ese gen a almacenar está en la seccion del hijo, se almacena el siguiente gen de la otra subseccion 
Devuelve dos hijos con su correspondiente valor fitness
*/
pair< pair<vector<int>,int>, pair<vector<int>,int> > cruce_PMX(vector<int> padre1,vector<int> padre2,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //el tamaño de la seccion central irá desde 1 hasta n-1, no puede ser 'n' porque en ese caso los hijos generados serían idénticos a los padres
    //n: nº de genes de cada padre
    int num_genes=padre1.size();
    int tam_seccion=rand()%(num_genes-1)+1; //la seccion tendra entre 1 n-1 genes
    //cout<<"NUM_GENES:"<<num_genes<<"//TAMANIO DE LA SECCION:"<<tam_seccion;

    //calculamos el indice del gen que delimita por la izquierda la seccion central (=indice del primer gen de la seccion central)
    //ese indice podrá tomar cualquier valor entero entre 0 y n-tamaño_seccion
    //asi nos aseguramos que la seccion central comenzando por ese primer indice sea una sección válida, encaje perfectamente dentro del vector
    int maximo_indice_izquierda=num_genes-tam_seccion;
    int extremo_izquierdo=rand()%(maximo_indice_izquierda+1); //valor entero en [0,maximo_indice_izquierda]
    //cout<<"\nEXTREMO IZQUIERDO:"<<extremo_izquierdo;

    //calculamos ahora el indice del ultimo gen de la seccion central, el que delimita por la derecha dicha sección
    int extremo_derecho=extremo_izquierdo+tam_seccion-1;
    //cout<<"\nEXTREMO DERECHO:"<<extremo_derecho<<"\n";

    //almacenamos la seccion central del padre1 en el hijo2, y la del padre2 en el hijo1
    //ademas, guardamos los genes de la seccion central tanto del padre1 como del padre2 para el relleno que se hará a posteriori
    vector<int> hijo1(num_genes),hijo2(num_genes);
    
    //las correspondencias se almacenaran en dos vectores del mismo tamaño que las secciones
    //por ej, este primer vector almacen
    vector<int> correspondencia_padre1_padre2 (num_genes,-1);
    vector<int> correspondencia_padre2_padre1 (num_genes,-1);


    for(int i=extremo_izquierdo;i<=extremo_derecho;i++){
        hijo1[i]=padre2[i];
        hijo2[i]=padre1[i];
        correspondencia_padre1_padre2[padre1[i]]=padre2[i];
        correspondencia_padre2_padre1[padre2[i]]=padre1[i];
    }

    /*
    cout<<"\nImprimo los genes del centro del padre 1 y del padre 2...\n";
    for(int i=0;i<num_genes;i++){
        cout<<correspondencia_padre1_padre2[i]<<"//";
    }
    cout<<"\n";
    for(int i=0;i<num_genes;i++){
        cout<<correspondencia_padre2_padre1[i]<<"//";
    }
    */


    //falta rellenar el resto de genes de los hijos
    //para ello generaré un vector que almacene los indices de los genes que faltan por rellenar para ambos hijos
    vector<int> indices(num_genes);
    iota(indices.begin(),indices.end(),0);
    indices.erase(indices.begin()+extremo_izquierdo,indices.begin()+extremo_derecho+1);
    /*
    cout<<"\nINDICES DE GENES A RELLENAR --->\n";
    for(int i=0;i<indices.size();i++){
        cout<<indices[i]<<"-";
    }
    */

    //rellenamos los genes que faltan
    int gen_a_almacenar;
    for(int i=0;i<indices.size();i++){
        gen_a_almacenar=padre1[indices[i]];
        //¿Ese gen aparece ya en la seccion del padre2 == tiene una correspondencia?
        while(correspondencia_padre2_padre1[gen_a_almacenar]!=-1){
            //se repite
            gen_a_almacenar=correspondencia_padre2_padre1[gen_a_almacenar];
        }
        hijo1[indices[i]]=gen_a_almacenar;

        gen_a_almacenar=padre2[indices[i]];
        //¿Ese gen aparece ya en la seccion del padre1 == tiene una correspondencia?
        while(correspondencia_padre1_padre2[gen_a_almacenar]!=-1){
            //se repite
            gen_a_almacenar=correspondencia_padre1_padre2[gen_a_almacenar];
        }
        hijo2[indices[i]]=gen_a_almacenar;
        

    }

    /*
    cout<<"\nHIJO 1 --->\n";
    for(int i=0;i<num_genes;i++){
        cout<<hijo1[i]<<"-";
    }
    cout<<"\nHIJO 2 --->\n";
    for(int i=0;i<num_genes;i++){
        cout<<hijo2[i]<<"-";
    }
    */
    
    

    //se devuelven los dos hijos creados con su correspondiente valor fitness
    return make_pair(make_pair(hijo1,fitness(hijo1,matriz_flujo,matriz_distancia)),make_pair(hijo2,fitness(hijo2,matriz_flujo,matriz_distancia)));

}


/*
Funcion que se encarga de cruzar las parejas entre sí usando el cruce basado en posición o el cruce PMX en función de si se le pasa 0 o 1
Parámetros de entrada:
1. Poblacion de partida
2. Indices de los individuos que han sido seleccionados
3. Probabilidad de cruce a nivel de pareja
4. Entero {0,1} para indicar el tipo de cruce a realizar: 0-->posicion // 1-->PMX
4. Matriz de flujo
5. Matriz de distancia
La salida se corresponde con una nueva población formada únicamente por los hijos que se han generado tras el cruce
*/
vector<pair<vector<int>,int>> cruzar_padres(vector<pair<vector<int>,int>> poblacion,vector<int> indices_padres,float prob_cruce,int tipo_cruce,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //primero calculamos el nº de parejas que se van a cruzar, conociendo el total de parejas seleccionadas
    int total_parejas=indices_padres.size()/2;
    int num_cruces=prob_cruce*total_parejas; //numero de parejas que se van a cruzar
    int indices=num_cruces*2; //indices de los padres que se van a cruzar entre sí
    //cout<<"\nDe un total de "<<total_parejas<<", se van a cruzar solo "<<num_cruces<< " parejas.";

    //definimos la nueva poblacion de hijos generada
    vector<pair<vector<int>,int>> hijos;
    //hijos generados por cruce basado en posicion
    pair<vector<int>,int> hijo1;
    pair<vector<int>,int> hijo2;
    //valor generado por PMX
    pair< pair<vector<int>,int>, pair<vector<int>,int> > dos_hijos;

    //para eliminar la componente aleatoria en el cruce, fijamos un orden de cruce determinado: primer padre seleccionado se cruzará con el segundo, el tercero con el cuarto...
    //para evitar decidir de forma aleatoria las parejas que se van a cruzar, si de 'n' parejas se cruzan 'x', cruzamos las primeras 'x' parejas
    
    for(int i=0;i<indices;i+=2)
    {
        //cruzamos la pareja correspondiente
        //comprobamos el cruce a realizar
        if(tipo_cruce==0){
            //cruzamos usando el cruce basado en posicion
            //como este mecanismo de cruce devuelve un hijo por pareja, se invoca dos veces por pareja para generar dos hijos por pareja
            hijo1=cruce_posicion(poblacion[indices_padres[i]].first,poblacion[indices_padres[i+1]].first,matriz_flujo,matriz_distancia);
            hijo2=cruce_posicion(poblacion[indices_padres[i]].first,poblacion[indices_padres[i+1]].first,matriz_flujo,matriz_distancia);
        
        }
        else{
            //cruzamos usando PMX,que ya devuelve dos hijos
            dos_hijos=cruce_PMX(poblacion[indices_padres[i]].first,poblacion[indices_padres[i+1]].first,matriz_flujo,matriz_distancia);
            hijo1=dos_hijos.first;
            hijo2=dos_hijos.second;
        }

        //añadimos los dos hijos generados por esa pareja a la poblacion de hijos
        hijos.push_back(hijo1);
        hijos.push_back(hijo2);

    }
    
    for(int i=indices;i<indices_padres.size();i++){
        //almacenamos el resto de padres que se han seleccionado pero que no se han terminado cruzando
        hijos.push_back(poblacion[indices_padres[i]]);
    }
    

    return hijos;

}

/*
Funcion que se encarga de mutar los genes de los hijos generados tras el proceso de cruce
Se requieren los siguientes parámetros:
1. La poblacion a mutar a nivel de gen
2. La probabilidad de que un gen mute
*/
void mutacion(vector<pair<vector<int>,int>>& poblacion,float prob,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //primero calculamos el nº total de genes que van a mutar
    int tamanio=poblacion.size(); //nº de individuos
    int num_genes=poblacion[0].first.size(); //genes por cada invidiuo

    int num_mutaciones=(tamanio*num_genes)*prob;

    int cromosoma_a_mutar;
    int gen_a_mutar;
    int nueva_posicion;
    int indice_random;
    for(int i=0;i<num_mutaciones;i++){
        //decidimos el cromosoma y el gen a mutar
        /*
        cromosoma_a_mutar=rand()%tamanio;
        gen_a_mutar=rand()%num_genes;
        */
        indice_random=rand()%(tamanio*num_genes); 
        //dividiendo los genes en trozos de tamaño 'n=num_genes'... vemos el trozo en el que se encuentra, lo que equivale al cromosoma
        cromosoma_a_mutar=indice_random/num_genes;
        gen_a_mutar=indice_random%num_genes;
        //mutamos --> intercambiar dicho gen con otro 
        do{
            nueva_posicion=rand()%num_genes;

        }while(gen_a_mutar==nueva_posicion);

        //ya hemos determinado los genes del cromosoma a intercambiar, esto implica cambiar las localizaciones a las que se han asignado dos unidades --> nuevo valor fitness
        int coste_extra=diferencia_coste(poblacion[cromosoma_a_mutar].first,gen_a_mutar,nueva_posicion,matriz_flujo,matriz_distancia);

        swap((poblacion[cromosoma_a_mutar].first)[gen_a_mutar],(poblacion[cromosoma_a_mutar].first)[nueva_posicion]);
        poblacion[cromosoma_a_mutar].second+=coste_extra;
    }
}

/*
Funcion que nos devuelve el indice del mejor individuo
Parámetro de entrada:
1. Poblacion de individuos
*/
int mejor_individuo(vector<pair<vector<int>,int>> poblacion){
    int mejor_coste=std::numeric_limits<int>::max();
    int indice_mejor=-1;
    //recorremos la poblacion
    for(int i=0;i<poblacion.size();i++){
        //individuo de indice 'i'
        if(poblacion[i].second<mejor_coste){
            //hemos encontrado una mejor solucion, actualizamos el mejor coste encontrado, junto con el indice del individuo
            mejor_coste=poblacion[i].second;
            indice_mejor=i;
        }

    }
    return indice_mejor;
}

/*
Funcion que nos devuelve el indice del peor individuo
Parámetro de entrada:
1. Poblacion de individuos
*/
int peor_individuo(vector<pair<vector<int>,int>> poblacion){
    int peor_coste=-1;
    int indice_peor=-1;
    //recorremos la poblacion
    for(int i=0;i<poblacion.size();i++){
        //individuo de indice 'i'
        if(poblacion[i].second>peor_coste){
            //hemos encontrado una peor solucion, actualizamos el peor coste encontrado, junto con el indice del individuo
            peor_coste=poblacion[i].second;
            indice_peor=i;
        }

    }
    return indice_peor;
}


/*
ALGORITMO GENETICO GENERACIONAL (nueva poblacion por cada época)
->Parámetros de entrada:
1. Numero de unidades del problema QAP a resolver (=numero de genes de cada invididuo)
2. Numero de cromosomas o individuos de la poblacion inicial
3. Valor entero [0-1] para indicar el operador de cruce que se desea utilizar (0:posicion, 1:PMX)
4. Probabilidad de cruce a nivel de pareja
5. Probabilidad de mutar a nivel de INDIVIDUO
6. Matriz de flujo
7. Matriz de distancia
->Salida: solución generada con su correspondiente valor fitness
*/

pair<vector<int>,int> AGG(int n,int num_cromosomas,int operador_cruce,float pc, float pmut,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //antes de nada calculamos la probabilidad de mutacion a nivel de gen
    float pm=pmut/n;
    //fijamos el numero maximo de ejecuciones de la funcion fitness
    int max_iteraciones=50000;
    int invocaciones=0;
    vector<int> indices_padres_seleccionados;
    vector< pair< vector<int>, int>  > hijos; //poblacion de hijos generada tras el cruce

    //generamos la poblacion de partida
    vector<pair<vector<int>,int>> poblacion=generar_poblacion(n,num_cromosomas,matriz_flujo,matriz_distancia);
    //como cada individuo de la poblacion inicial trae consigo su valor fitness->se ejecuta una vez la funcion objetivo por cada individuo
    invocaciones+=poblacion.size();

    /*
    cout<<"El tamaño de la poblacion inicial generada es "<<poblacion.size();
    for(int i=0;i<poblacion.size();i++){
        cout<<"\nINDIVIDIUO DE INDICE "<<i<<" ...\n";
        for(int j=0;j<n;j++){
            cout<<poblacion[i].first[j]<<"-";
        }
        cout<<"\nValor fitness asociado:"<<poblacion[i].second<<"\n";
    }
    */

    int peor_hijo,mejor_padre;
    
    
    while(invocaciones<max_iteraciones){
        //SELECCION DE LOS PADRES
        indices_padres_seleccionados=seleccion_padres(poblacion,poblacion.size());

        //CRUZO LOS PADRES PARA GENERAR LOS HIJOS
        hijos=cruzar_padres(poblacion,indices_padres_seleccionados,pc,operador_cruce,matriz_flujo,matriz_distancia);
        //cuando cruzamos, generamos hijos que implican volver a evaluar sobre ellos la funcion objetivo
        //tomando únicamente las parejas que se cruzan, cada pareja genera dos hijos, y por ende, dos invocaciones de la funcion fitness
        invocaciones+=2*(pc*(indices_padres_seleccionados.size()/2));

        //MUTAMOS
        mutacion(hijos,pm,matriz_flujo,matriz_distancia);
        //es verdad que al mutar un gen de un cromosoma, obtenemos una solucion nueva para la que necesitamos conocer su valor fitness,
        //pero en este caso, no utilizamos la función fitness, sino la factorización que consideramos en la anterior practica
        

       //REEMPLAZAMIENTO
        //ahora reemplazamos la poblacion inicial por la poblacion nueva (=hijos)
        //reemplazamos el peor hijo por el mejor individuo de la poblacion inicial
        peor_hijo=peor_individuo(hijos);
        mejor_padre=mejor_individuo(poblacion);
        //cout<<"\nLos indices del peor hijo y del mejor padre son: "<<peor_hijo<<" y "<<mejor_padre;

        hijos[peor_hijo]=poblacion[mejor_padre];

        //cambiamos la poblacion de partida en la siguiente generación
        poblacion=hijos;

    }

    //fuera del while...
    //seleccionamos el mejor individuo de la última población de soluciones generada, y lo devolvemos junto con su valor fitness
    mejor_padre=mejor_individuo(poblacion);
    return poblacion[mejor_padre];
    
}

/*
La siguiente funcion recibe como entrada la poblacion, y devuelve el indice de los dos peores padres como un pair
tal que el primer valor del pair es el indice del primer peor padre, y el segundo valor el indice del segundo peor padre
*/
pair<int,int> dos_peores_padres(vector<pair<vector<int>,int>>& poblacion){
    //inicialmente asumimos que los dos primeros padres son los peores, el primero incluso peor que el segundo
    int primero_peor=0;
    int segundo_peor=1;
    //comprobamos si el segundo padre es incluso peor que el primero
    if(poblacion[primero_peor].second<poblacion[segundo_peor].second){
        //segundo invididuo cuesta mas que el primero
        swap(primero_peor,segundo_peor);
    }

    //comprobamos si el resto de inviduos son peores que los dos peores padres iniciales
    for(int i=2;i<poblacion.size();i++){
        //para cada individuo de indice 'i'...
        //¿es peor que el segundo peor encontrado?
        if(poblacion[i].second>poblacion[segundo_peor].second){
            //es peor que el segundo peor padre --> ¿es peor tambien que el primero peor?
            if(poblacion[i].second>poblacion[primero_peor].second){
                //el primer peor padre será ahora el segundo peor
                segundo_peor=primero_peor;
                //es incluso peor que el primero peor ---> nuevo primer peor padre
                primero_peor=i;
            }
            else{
                //es peor que el segundo peor, pero mejor que el primero peor
                segundo_peor=i;
            }
        } 
    }

    return make_pair(primero_peor,segundo_peor);

}

/*
Funcion que nos devuelve el indice de los dos mejores padres
*/
pair<int,int> dos_mejores_padres(vector<pair<vector<int>,int>>& poblacion){
    //inicialmente asumimos que los dos primeros padres son los mejores, el primero incluso mejor que el segundo
    int primero_mejor=0;
    int segundo_mejor=1;
    //comprobamos si el segundo padre es incluso mejor que el primero
    if(poblacion[segundo_mejor].second<poblacion[primero_mejor].second){
        //segundo invididuo cuesta menos que el primero
        swap(primero_mejor,segundo_mejor);
    }

    //comprobamos si el resto de inviduos son peores que los dos peores padres iniciales
    for(int i=2;i<poblacion.size();i++){
        //para cada individuo de indice 'i'...
        //¿es mejor que el segundo mejor encontrado?
        if(poblacion[i].second<poblacion[segundo_mejor].second){
            //es mejor que el segundo mejor padre --> ¿es mejor tambien que el primero mejor?
            if(poblacion[i].second<poblacion[primero_mejor].second){
                //el primer mejor padre será ahora el segundo mejor
                segundo_mejor=primero_mejor;
                //es incluso mejor que el primero mejor ---> nuevo primer mejor padre
                primero_mejor=i;
            }
            else{
                //es mejor que el segundo mejor, pero peor que el primero mejor
                segundo_mejor=i;
            }
        } 
    }

    return make_pair(primero_mejor,segundo_mejor);
}

void reemplazo_en_AGE(vector<pair<vector<int>,int>>& poblacion,vector<pair<vector<int>,int>> hijos){
   pair<int,int> peores_padres=dos_peores_padres(poblacion); //Indice de los dos peores padres

   //genero una poblacion formada por esos 4 individuos, sobre la cual evaluo la funcion 'dos_peores_padres'
   vector<pair<vector<int>,int>> poblacion_auxiliar;
   poblacion_auxiliar.push_back(poblacion[peores_padres.first]);
   poblacion_auxiliar.push_back(poblacion[peores_padres.second]);
   poblacion_auxiliar.push_back(hijos[0]);
   poblacion_auxiliar.push_back(hijos[1]);

   pair<int,int> mejores=dos_mejores_padres(poblacion_auxiliar);
   poblacion[peores_padres.first]=poblacion_auxiliar[mejores.first];
   poblacion[peores_padres.second]=poblacion_auxiliar[mejores.second];

}

/*
ALGORITMO GENETICO ESTACIONARIO
*Diferencias con respecto al generacional..
--> En el proceso de seleccion se seleccionan únicamente dos padres, cruzandose entre si y generando 2 hijos que se mutarán según prob_mut
--> Ambos compiten por entrar en la población, tal que considerando los dos hijos y los dos peores individuos de la población inicial, acabaremos tomando de esos 4 individuos, los dos mejores.
--> Por otra parte, la probabilidad de cruce siempre será 1 ya que disponemos únicamente de una pareja de padres
*/
pair<vector<int>,int> AGE(int n,int num_cromosomas,int operador_cruce, float pmut,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    float pm=pmut/n;//calculamos la probabilidad de mutacion a nivel de gen
    //fijamos el numero maximo de ejecuciones de la funcion fitness
    int max_iteraciones=50000;
    int invocaciones=0;
    vector<int> indices_padres_seleccionados;
    vector< pair< vector<int>, int>  > hijos; //poblacion de hijos generada tras el cruce
    //generamos la poblacion de partida
    vector<pair<vector<int>,int>> poblacion=generar_poblacion(n,num_cromosomas,matriz_flujo,matriz_distancia);
    //como cada individuo de la poblacion inicial trae consigo su valor fitness->se ejecuta una vez la funcion objetivo por cada individuo
    invocaciones+=poblacion.size();

    int peor_hijo,mejor_padre;

    
    while(invocaciones<max_iteraciones){
        //SELECCION DE LOS PADRES
        indices_padres_seleccionados=seleccion_padres(poblacion,2); //seleccionamos 2 padres

        //CRUZO LOS PADRES PARA GENERAR LOS HIJOS
        hijos=cruzar_padres(poblacion,indices_padres_seleccionados,1,operador_cruce,matriz_flujo,matriz_distancia);
        invocaciones+=2; //como siempre se cruzan los dos padres, se generan dos hijos que implican 2 evaluaciones

        //MUTAMOS
        mutacion(hijos,pm,matriz_flujo,matriz_distancia);
        //es verdad que al mutar un gen de un cromosoma, obtenemos una solucion nueva para la que necesitamos conocer su valor ftiness,
        //pero en este caso, no utilizamos la función fitness, sino la factorización que consideramos en la anterior practica
        

       //tras mutar los dos hijos, mantenemos la poblacion de partida realizando ciertos reemplazos
       reemplazo_en_AGE(poblacion,hijos);

    }

    //fuera del while...
    //seleccionamos el mejor individuo de la última población de soluciones generada, y lo devolvemos junto con su valor fitness
    mejor_padre=mejor_individuo(poblacion);
    
    return poblacion[mejor_padre];
}


/*
Busqueda Local modificada para su correcto funcionamiento cuando utiliceos memeticos...
Se añaden dos nuevos parametros respecto de la version inicial:
1. Punto_partida: individuo sobre el cual comenzaremos a ejecutar la búsqueda local
2. Max_exploraciones: nº maximo de evaluaciones de la funcion 'busqueda_vecindario'
*/
vector<int> busqueda_local_memeticos(vector<int> punto_partida,int max_exploraciones,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //solucion inicial de la cual partimos...
    vector<int> solucion_actual=punto_partida;

    vector<bool> dlb(matriz_flujo.size(),false);
    vector<int> solucion_obtenida;
    bool seguir_iterando=false;
    int num_exploraciones=0;

    //bucle externo...
    do{
        //nueva exploracion del vecindario
        num_exploraciones++;
        //la busqueda interna modifica la solucion_actual siempre y cuando se consiga una mejor solucion
        //por lo que si devuelve un vector solucion identico a la solucion_actual pasada como parametro,
        //quiere decir que no se ha encontrado una mejor solucion vecina en dicho vecindario, y finaliza la bsuqueda local
        solucion_obtenida=busqueda_vecindario(dlb,solucion_actual,matriz_flujo,matriz_distancia);
        seguir_iterando=(solucion_obtenida!=solucion_actual); 
        //actualizamos la solucion actual
        solucion_actual=solucion_obtenida;
    }while(seguir_iterando and (num_exploraciones<max_exploraciones)); //mientras que la solucion vecina sea de mayor calidad

    return solucion_actual;
}

/*
Primera version propuesta del algoritmo memetico, tal que cada 10 generaciones, ejecutamos la BL sobre cada individuo, reemplazandolo
por el obtenido con la BL.
*/
void AM_All(vector<pair<vector<int>,int>>& poblacion,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    vector<int> nueva_solucion;
    for(int i=0;i<poblacion.size();i++){
        //ejecutamos la BL partiendo de dicho invididuo como solucion de partida
        nueva_solucion=busqueda_local_memeticos(poblacion[i].first,400,matriz_flujo,matriz_distancia);
        poblacion[i].first=nueva_solucion;
        //calculamos el coste de la nueva solucion
        poblacion[i].second=fitness(nueva_solucion,matriz_flujo,matriz_distancia);
    }
}

/*
Segunda version propuesta del algoritmo memetico, tal que cada 10 generaciones, ejecutamos la BL sobre un subconjunto de invididuos de la poblacion, no sobre todos
*/
void AM_Rand(vector<pair<vector<int>,int>>& poblacion,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //probabilidad de que un individuo sea escogido...
    float pls=0.1;
    //calculamos primeramente el nº de individuos que van a ser escogidos
    int num_seleccionados=poblacion.size()*pls;
    //permutamos los indices de los individuos y nos quedamos con los 'num_seleccinados' primeros individuos
    vector<int> indices_individuos(poblacion.size());
    iota(indices_individuos.begin(),indices_individuos.end(),0);
    random_shuffle(indices_individuos.begin(),indices_individuos.end(),myrandom);

    vector<int> nueva_solucion;
    for(int i=0;i<num_seleccionados;i++){
        //ejecutamos la BL partiendo de dicho invididuo como solucion de partida
        nueva_solucion=busqueda_local_memeticos(poblacion[indices_individuos[i]].first,400,matriz_flujo,matriz_distancia);
        poblacion[indices_individuos[i]].first=nueva_solucion;
        //calculamos el coste de la nueva solucion
        poblacion[indices_individuos[i]].second=fitness(nueva_solucion,matriz_flujo,matriz_distancia);
    }
}

/*
Funcion que nos permite ordenar dos individuos (dos pair<...>), tal que primero irá el de menor coste
Devuelve 'true' cuando 'a' sea menos costoso que 'b' --> 'a' irá antes que 'b'
*/
bool comparar_costes(pair<int,int> a,pair<int,int> b){
    //recordemos que tiene que definir un ordenamiento estrictamente debil
    //esto implica que debe definir una relacion irreflexiva y antisimetrica
    /*
    -irreflexiva: un elemento no es menor que sí mismo (devuelve false para dos mismos elementos)
    -antisimetrica: si devuelve 'true' para 'x' e 'y', devolverá 'false' para 'y e 'x'
    (dicho de otra forma: si x<=y y y<=x ---> x=y [parejas iguales])
    */
    if(a.second<b.second){
        return true;
    }
    else if(a.second==b.second){
        //utilizamos los indices de los individuos
        return(a.first<b.first);
    }
    else{
        //'a' cuesta mas que 'b' --> irá despues
        return false;
    }
}

/*
Tercera version propuesta del algoritmo memetico, tal que cada 10 generaciones, ejecutamos la BL 
*/
void AM_Best(vector<pair<vector<int>,int>>& poblacion,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //probabilidad de que un individuo sea escogido...
    float pls=0.1;
    //calculamos primeramente el nº de individuos que van a ser escogidos
    int num_seleccionados=poblacion.size()*pls;
    
    //primero ordenamos los invididuos de menor a mayor coste
    //para mantener la referencia de ellos, asociamos, para cada invididuo, su indice con su coste
    vector<pair<int,int>> indice_coste;
    pair<int,int> pareja;
    for(int i=0;i<poblacion.size();i++){
        pareja.first=i;
        pareja.second=poblacion[i].second;
        indice_coste.push_back(pareja);
    }
    //ahora ordenamos de menor a mayor coste
    sort(indice_coste.begin(),indice_coste.end(),comparar_costes);

    vector<int> nueva_solucion;
    for(int i=0;i<num_seleccionados;i++){
        //ejecutamos la BL partiendo de dicho invididuo como solucion de partida
        nueva_solucion=busqueda_local_memeticos(poblacion[indice_coste[i].first].first,400,matriz_flujo,matriz_distancia);
        poblacion[indice_coste[i].first].first=nueva_solucion;
        //calculamos el coste de la nueva solucion
        poblacion[indice_coste[i].first].second=fitness(nueva_solucion,matriz_flujo,matriz_distancia);
    }
}

/*
ALGORITMOS MEMETICOS
*/
pair<vector<int>,int> memeticos(int n,int num_cromosomas,int operador_cruce,float pc, float pmut,int tipo_memetico,vector<vector<int>> matriz_flujo,vector<vector<int>> matriz_distancia){
    //probabilidad de mutacion a nivel de gen
    float pm=pmut/n;
    //cout<<"\nEjecutando memetico version "<<tipo_memetico;
    //fijamos el numero maximo de ejecuciones de la funcion fitness
    int max_iteraciones=50000;
    int invocaciones=0;
    vector<int> indices_padres_seleccionados;
    vector< pair< vector<int>, int>  > hijos; //poblacion de hijos generada tras el cruce

    //generamos la poblacion de partida
    vector<pair<vector<int>,int>> poblacion=generar_poblacion(n,num_cromosomas,matriz_flujo,matriz_distancia);
    //como cada individuo de la poblacion inicial trae consigo su valor fitness->se ejecuta una vez la funcion objetivo por cada individuo
    invocaciones+=poblacion.size();

    //ahora contabilizamos el nº de generaciones
    int generaciones=0;

    int peor_hijo,mejor_padre;
    
    
    while(invocaciones<max_iteraciones){
        generaciones++;

        if(generaciones==10){
            switch(tipo_memetico){
                case 1:
                    AM_All(poblacion,matriz_flujo,matriz_distancia);
                    invocaciones+=poblacion.size();
                    break;
                case 2:
                    AM_Rand(poblacion,matriz_flujo,matriz_distancia);
                    invocaciones+=(0.1*poblacion.size());
                    break;
                case 3:
                    AM_Best(poblacion,matriz_flujo,matriz_distancia);
                    invocaciones+=(0.1*poblacion.size());
                    break;
            }
            //volvemos a poner a 0 el nº de generaciones
            generaciones=0;
        }
        //SELECCION DE LOS PADRES
        indices_padres_seleccionados=seleccion_padres(poblacion,poblacion.size());

        //CRUZO LOS PADRES PARA GENERAR LOS HIJOS
        hijos=cruzar_padres(poblacion,indices_padres_seleccionados,pc,operador_cruce,matriz_flujo,matriz_distancia);
        //cuando cruzamos, generamos hijos que implican volver a evaluar sobre ellos la funcion objetivo
        //tomando únicamente las parejas que se cruzan, cada pareja genera dos hijos, y por ende, dos invocaciones de la funcion fitness
        invocaciones+=2*(pc*(indices_padres_seleccionados.size()/2));

        //MUTAMOS
        mutacion(hijos,pm,matriz_flujo,matriz_distancia);
        //es verdad que al mutar un gen de un cromosoma, obtenemos una solucion nueva para la que necesitamos conocer su valor fitness,
        //pero en este caso, no utilizamos la función fitness, sino la factorización que consideramos en la anterior practica
        

       //REEMPLAZAMIENTO
        //ahora reemplazamos la poblacion inicial por la poblacion nueva (=hijos)
        //reemplazamos el peor hijo por el mejor individuo de la poblacion inicial
        peor_hijo=peor_individuo(hijos);
        mejor_padre=mejor_individuo(poblacion);
        //cout<<"\nLos indices del peor hijo y del mejor padre son: "<<peor_hijo<<" y "<<mejor_padre;

        hijos[peor_hijo]=poblacion[mejor_padre];

        //cambiamos la poblacion de partida en la siguiente generación
        poblacion=hijos;

    }

    //fuera del while...
    //seleccionamos el mejor individuo de la última población de soluciones generada, y lo devolvemos junto con su valor fitness
    mejor_padre=mejor_individuo(poblacion);
    return poblacion[mejor_padre];
    
}

//-----------------------------------------------------------P3----------------------------------------------
//Antes de comenzar a escribir el código asociado a la P3, optimizaré las funciones usadas en la BL

int fitness_optimizado(vector<int> solucion,const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    int coste_total=0;
    int distancia=0;
    int flujo=0;

    int n=matriz_flujo.size();

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            distancia=matriz_distancia[solucion[i]][solucion[j]];
            flujo=matriz_flujo[i][j];
            coste_total+=distancia*flujo;
        }
    }

    return coste_total;

}

int diferencia_coste_optimizado(const vector<int>& solucion_actual,int r,int s,const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    int n=solucion_actual.size();
    //los indices de las unidades deben estar en [0,n-1]
    assert(r>=0 && r<n && r!=s);
    assert(s>=0 && s<n);

    int diferencia=0;
    for(int i=0;i<n;i++){
        if(i!=r && i!=s){
            diferencia+=matriz_flujo[r][i]*(matriz_distancia[solucion_actual[s]][solucion_actual[i]]-matriz_distancia[solucion_actual[r]][solucion_actual[i]])+
                        matriz_flujo[s][i]*(matriz_distancia[solucion_actual[r]][solucion_actual[i]]-matriz_distancia[solucion_actual[s]][solucion_actual[i]])+
                        matriz_flujo[i][r]*(matriz_distancia[solucion_actual[i]][solucion_actual[s]]-matriz_distancia[solucion_actual[i]][solucion_actual[r]])+
                        matriz_flujo[i][s]*(matriz_distancia[solucion_actual[i]][solucion_actual[r]]-matriz_distancia[solucion_actual[i]][solucion_actual[s]]);
        }
    }

    return diferencia;
}


//funcion que recibe como parámetro un entero 'n', y devuelve una pareja de enteros diferentes entre 0 y n-1
pair<int,int> dos_posiciones_aleatorias(int n){
    int i,j;

    do{
    int random=rand()%(n*n);
    i=random/n;
    j=random%n;
    }while(i==j);

    return make_pair(i,j);
}

/*
1. Búsqueda Local Modificada
A diferencia de la busqueda local inicial, NO usamos la mascara de bits, y además, se genera un solo vecino en cada iteracion del bucle
*/

vector<int> busqueda_local_modificada(vector<int> solucion_partida,const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    //generamos una solucion inicial de la cual partir...
    vector<int> solucion_actual=solucion_partida;

    int vecindarios_explorados=0;
    pair<int,int> posiciones_a_intercambiar;
    //bucle externo...
    do{
        vecindarios_explorados++;
        //generamos un vecino y vemos si es mejor que la solucion actual
        posiciones_a_intercambiar=dos_posiciones_aleatorias(solucion_actual.size());

        if(diferencia_coste_optimizado(solucion_actual,posiciones_a_intercambiar.first,posiciones_a_intercambiar.second,matriz_flujo,matriz_distancia)<0){
            //el unico vecino del vecindario actual generado es mejor que la solucion actual
            swap(solucion_actual[posiciones_a_intercambiar.first],solucion_actual[posiciones_a_intercambiar.second]);
        }

    }while(vecindarios_explorados<2000); //mientras que hayamos encontrado en el vecindario algun vecino mejor que la solucion actual...

    return solucion_actual;
}

/*
2. BMB (Búsqueda Multiarranque Básica)
--------------------------------------
Consiste en generar un determinado nº de soluciones aleatorias de partida, y ejecutar la BL sobre cada una de ellas
Se devuelve la mejor de las soluciones obtenidas...


Parámetros: matriz de flujo y salida
Salida: pareja formada por la mejor solución conseguida, y su valor fitness
En este caso ejecutaremos 25 veces la BL (una vez sobre cada una de las 25 soluciones iniciales generadas aleatoriamente)
*/

pair<vector<int>,int> BMB(const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    vector<int> mejor_solucion(matriz_flujo.size());
    //el coste de la mejor solucion inicialmente es +infinito
    int mejor_coste=numeric_limits<int>::max();
    int coste;
    
    vector<int> solucion_inicial(matriz_flujo.size());
    for(int i=0;i<25;i++){
        //solucion de partida aleatoria
        iota(solucion_inicial.begin(),solucion_inicial.end(),0);
        random_shuffle(solucion_inicial.begin(),solucion_inicial.end(),myrandom);
        //ejecutamos la BL
        solucion_inicial=busqueda_local_modificada(solucion_inicial,matriz_flujo,matriz_distancia);
        //¿es mejor que la mejor solucion conseguida hasta el momento?
        coste=fitness_optimizado(solucion_inicial,matriz_flujo,matriz_distancia);
        if(coste<mejor_coste){
            //hemos encontrado una nueva mejor solucion
            mejor_coste=coste;
            mejor_solucion=solucion_inicial;
        }
        //siguiente iteracion
    }

    return make_pair(mejor_solucion,mejor_coste);
}


vector<int> mutacion_ILS(vector<int> solucion){
    int n=solucion.size();
    //escogemos de forma aleatoria la posicion de la primera componente del subvector a permutar
    int pos_partida=rand()%n;
    //calculamos el nº de componentes que queremos mutar = tamaño de subvector a permutar
    int num_mutaciones=n/3;

    //genero el subvector con los elementos a permutar
    vector<int> elementos_a_permutar(num_mutaciones);
    for(int i=0;i<num_mutaciones;i++){
        elementos_a_permutar[i]=solucion[(pos_partida+i)%n];
    }
    //permutamos
    random_shuffle(elementos_a_permutar.begin(),elementos_a_permutar.end());
    //almacenamos el subvector permutado en la solucion original
    for(int i=0;i<num_mutaciones;i++){
        solucion[(pos_partida+i)%n]=elementos_a_permutar[i];
    }

    return solucion;

}

pair<vector<int>,int> ILS(const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    //comenzamos generado una solucion aleatoria
    vector<int> solucion_inicial(matriz_flujo.size());
    iota(solucion_inicial.begin(),solucion_inicial.end(),0);
    random_shuffle(solucion_inicial.begin(),solucion_inicial.end(),myrandom);

    //aplicamos sobre ella la BL
    vector<int> solucion_optimizada=busqueda_local_modificada(solucion_inicial,matriz_flujo,matriz_distancia);
    int coste_solucion_optimizada;
    //esa solucion será inicialmente la mejor solucion encontrada hasta el momento
    vector<int> mejor_solucion=solucion_optimizada;
    int coste_mejor_solucion=fitness_optimizado(solucion_optimizada,matriz_flujo,matriz_distancia);

    int max_iteraciones=24;
    int iteracion=0;
    while(iteracion<max_iteraciones){
        //mutamos la mejor solucion encontrada hasta el momento
        solucion_optimizada=mutacion_ILS(mejor_solucion);
        //aplicamos busqueda local sobre la mejor solucion una vez mutada y actualizamos el coste
        solucion_optimizada=busqueda_local_modificada(solucion_optimizada,matriz_flujo,matriz_distancia);
        coste_solucion_optimizada=fitness_optimizado(solucion_optimizada,matriz_flujo,matriz_distancia);
        //comprobamos si hemos conseguido una solucion mejor que la mejor conseguida hasta el momento
        if(coste_solucion_optimizada<coste_mejor_solucion){
            mejor_solucion=solucion_optimizada;
           
            coste_mejor_solucion=coste_solucion_optimizada;
        }

        iteracion++;
    }

    return make_pair(mejor_solucion,coste_mejor_solucion);

}

vector<int> busqueda_local_VNS(vector<int> solucion_partida,int k,const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    int n=solucion_partida.size();
    //generamos una solucion inicial de la cual partir...
    vector<int> mejor_sol=solucion_partida;
    int coste_mejor_sol=fitness(mejor_sol,matriz_flujo,matriz_distancia);

    int vecindarios_explorados=0;
    pair<int,int> posiciones_a_intercambiar;
    vector<int> solucion_vecina;
    int coste_vecino;
    //bucle externo...
    do{
        vecindarios_explorados++;
        solucion_vecina=mejor_sol;
        //generamos un vecino y vemos si es mejor que la solucion actual
        //el vecino se generará intercambiando k parejas de localizaciones escogidas de forma aleatoria
        for(int i=0;i<k;i++){
            posiciones_a_intercambiar=dos_posiciones_aleatorias(n);
            //cout<<"-------cambiando posiciones--------";
            swap(solucion_vecina[posiciones_a_intercambiar.first],solucion_vecina[posiciones_a_intercambiar.second]);
        }
        //cout<<"-------saliendo del bucle que cambia posiciones--------";

        //calculamos el coste de la solucion vecina y  la actual
        coste_vecino=fitness(solucion_vecina,matriz_flujo,matriz_distancia);

        if(coste_vecino<coste_mejor_sol){
            //el unico vecino del vecindario actual generado es mejor que la solucion actual
            mejor_sol=solucion_vecina;
            coste_mejor_sol=coste_vecino;
        }

    }while(vecindarios_explorados<2000); //mientras que no se hayan generado mas de 2000 vecinos

    return mejor_sol;
}


pair<vector<int>,int> VNS(const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    //nº maximo de parejas existentes para intercambiar
    int kmax=5;
    
    //generamos una solucion de partida
    vector<int> solucion_inicial(matriz_flujo.size());
    iota(solucion_inicial.begin(),solucion_inicial.end(),0);
    random_shuffle(solucion_inicial.begin(),solucion_inicial.end(),myrandom);
    //aplicamos la BL generando el vecindario para k=1
    int k=1;
    vector<int> sol_opt=busqueda_local_VNS(solucion_inicial,k,matriz_flujo,matriz_distancia); 

    //la mejor solucion es esta inicialmente
    vector<int> mejor_solucion=sol_opt;
    int coste_mejor_solucion=fitness_optimizado(sol_opt,matriz_flujo,matriz_distancia);

    int max_iteraciones=24;
    int iteracion=0;
    vector<int> sol_mutada;
    int coste_sol_opt;
    
    while(iteracion<max_iteraciones){
        iteracion++;
        //mutamos la mejor solucion encontrada y aplicamos BL
        sol_mutada=mutacion_ILS(mejor_solucion);
        sol_opt=busqueda_local_VNS(sol_mutada,k,matriz_flujo,matriz_distancia);  
        coste_sol_opt=fitness_optimizado(sol_opt,matriz_flujo,matriz_distancia);
        //comprobamos si hemos conseguido un vecino mejor que la solucion sobre la que hemos aplicado la BL
        if(sol_opt!=sol_mutada){
            //hemos encontrado una solucion mejor, volvemos al primer tipo de vecindario
            k=1;
        }
        else{
            //probamos con otro tipo de vecindario
            k++;
        }
        //nos aseguramos de que k no supere el max nº de parejas posibles a intercambiar
        if(k>kmax){
            k=1;
        }
        //actualizamos la mejor solucion
        if(coste_sol_opt<coste_mejor_solucion){
            mejor_solucion=sol_opt;
            coste_mejor_solucion=coste_sol_opt;
        }
    }
    

    return make_pair(mejor_solucion,coste_mejor_solucion);

}

pair<vector<int>,int> ES(const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    uniform_real_distribution<double> unif(0,1);
    default_random_engine gen;
    //primero generamos una solucion de partida de forma aleatoria y calculamos su coste
    int n=matriz_flujo.size();
    vector<int> solucion_inicial(n);
    iota(solucion_inicial.begin(),solucion_inicial.end(),0);
    random_shuffle(solucion_inicial.begin(),solucion_inicial.end(),myrandom);
    int coste=fitness_optimizado(solucion_inicial,matriz_flujo,matriz_distancia);

    vector<int> mejor_solucion=solucion_inicial;
    int coste_mejor_solucion=coste;

    //definimos la temperatura inicial y final
    float T_0=(0.3*coste)/(-log(0.2));
    float T_f=0.0001;
    float T_k=T_0;
    //la final debe ser menor que la inicial
    assert(T_f<T_0);

    //parámetros del BUCLE INTERNO
    int max_vecinos=15*n;
    int max_exitos=0.1*max_vecinos;
    //parametros del BUCLE EXTERNO
    int max_iter=50000/max_vecinos;

    //calculamos el valor de beta (valor fijo) necesario para enfriar
    float valor_beta=(T_0-T_f)/(max_iter*T_0*T_f);

    int exitos,vecinos;
    int iteracion=0;
    pair<int,int> posiciones_a_intercambiar;
    int dif_coste;
    vector<int> sol_actual=solucion_inicial;
    do{
        vecinos=0;
        exitos=0;
        do{
            //generamos una unica solucion vecina utilizando solo dos unidades
            posiciones_a_intercambiar=dos_posiciones_aleatorias(n);
            dif_coste=diferencia_coste_optimizado(sol_actual,posiciones_a_intercambiar.first,posiciones_a_intercambiar.second,matriz_flujo,matriz_distancia);
            if(dif_coste<0 || ( unif(gen) <= (exp(-dif_coste / T_k)) ) ){
                exitos++;
                //tomamos ese vecino como solucion actual
                swap(sol_actual[posiciones_a_intercambiar.first],sol_actual[posiciones_a_intercambiar.second]);
                coste=coste+dif_coste; 
                //comprobamos si el vecino es mejor
                if(dif_coste<0){
                    mejor_solucion=sol_actual;
                    coste_mejor_solucion=coste;
                }
            }
            vecinos++;

        }while(vecinos<max_vecinos && exitos<max_exitos);

        iteracion++;
        //enfriamos...
        T_k=T_k/(1+valor_beta*T_k);

    }while(iteracion<max_iter && exitos!=0);

    return make_pair(mejor_solucion,coste_mejor_solucion);

}

//igual que ES pero en lugar de generar nosotros la solucion de partida, se requiere pasar como parámetro
pair<vector<int>,int> ES_2(vector<int> solucion_inicial,const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    uniform_real_distribution<double> unif(0,1);
    default_random_engine gen;
    int n=matriz_flujo.size();
    int coste=fitness_optimizado(solucion_inicial,matriz_flujo,matriz_distancia);

    vector<int> mejor_solucion=solucion_inicial;
    int coste_mejor_solucion=coste;

    //definimos la temperatura inicial y final
    float T_0=(0.3*coste)/(-log(0.2));
    float T_f=0.0001;
    float T_k=T_0;
    //la final debe ser menor que la inicial
    assert(T_f<T_0);

    //parámetros del BUCLE INTERNO
    int max_vecinos=15*n;
    int max_exitos=0.1*max_vecinos;
    //parametros del BUCLE EXTERNO
    int max_iter=50000/max_vecinos;

    //calculamos el valor de beta (valor fijo) necesario para enfriar
    float valor_beta=(T_0-T_f)/(max_iter*T_0*T_f);

    int exitos,vecinos;
    int iteracion=0;
    pair<int,int> posiciones_a_intercambiar;
    int dif_coste;
    vector<int> sol_actual=solucion_inicial;
    do{
        vecinos=0;
        exitos=0;
        do{
            //generamos una unica solucion vecina utilizando solo dos unidades
            posiciones_a_intercambiar=dos_posiciones_aleatorias(n);
            dif_coste=diferencia_coste_optimizado(sol_actual,posiciones_a_intercambiar.first,posiciones_a_intercambiar.second,matriz_flujo,matriz_distancia);
            if(dif_coste<0 || ( unif(gen) <= (exp(-dif_coste / T_k)) ) ){
                exitos++;
                //tomamos ese vecino como solucion actual
                swap(sol_actual[posiciones_a_intercambiar.first],sol_actual[posiciones_a_intercambiar.second]);
                coste=coste+dif_coste; 
                //comprobamos si el vecino es mejor
                if(dif_coste<0){
                    mejor_solucion=sol_actual;
                    coste_mejor_solucion=coste;
                }
            }
            vecinos++;

        }while(vecinos<max_vecinos && exitos<max_exitos);

        iteracion++;
        //enfriamos...
        T_k=T_k/(1+valor_beta*T_k);

    }while(iteracion<max_iter && exitos!=0);

    return make_pair(mejor_solucion,coste_mejor_solucion);

}

pair<vector<int>,int> ILS_ES(const vector<vector<int>>& matriz_flujo,const vector<vector<int>>& matriz_distancia){
    //comenzamos generado una solucion aleatoria
    vector<int> solucion_inicial(matriz_flujo.size());
    iota(solucion_inicial.begin(),solucion_inicial.end(),0);
    random_shuffle(solucion_inicial.begin(),solucion_inicial.end(),myrandom);

    pair<vector<int>,int> salida;
    //aplicamos sobre ella ES
    salida=ES_2(solucion_inicial,matriz_flujo,matriz_distancia);
    //esa solucion será inicialmente la mejor solucion encontrada hasta el momento
    vector<int> mejor_solucion=salida.first;
    int coste_mejor_solucion=salida.second;

    vector<int> solucion_optimizada;
    int coste_solucion_optimizada;
    int max_iteraciones=24;
    int iteracion=0;
    while(iteracion<max_iteraciones){
        //mutamos la mejor solucion encontrada hasta el momento
        solucion_optimizada=mutacion_ILS(mejor_solucion);
        //aplicamos ES sobre la mejor solucion una vez mutada y actualizamos el coste
        salida=ES_2(solucion_optimizada,matriz_flujo,matriz_distancia);
        solucion_optimizada=salida.first;
        coste_solucion_optimizada=salida.second;
        //comprobamos si hemos conseguido una solucion mejor que la mejor conseguida hasta el momento
        if(coste_solucion_optimizada<coste_mejor_solucion){
            mejor_solucion=solucion_optimizada;
           
            coste_mejor_solucion=coste_solucion_optimizada;
        }

        iteracion++;
    }

    return make_pair(mejor_solucion,coste_mejor_solucion);


}


#endif
