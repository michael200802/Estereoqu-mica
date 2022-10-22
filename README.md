# Mochi
Why doing your stereochemistry homework alone when you can do it with Mochi?

**Español**

[Como funciona?](#how_does_it_work_spn)

[Como compilar?](#how_to_compile_spn)
	
[Como usar?](#how_to_use_spn)


**English**
<!--[How does it work?](#how_does_it_work_spn)-->
[How to compile?](#how_to_compile_eng)

[How to use?](#how_to_use_eng)

		
# Version en español

**<a name="how_does_it_work_spn">Como funciona?</a>**
	
Hay 4 estructuras vitales para la logica del programa: 

[elem_t](#elem_t_spn)

[substance_t](#substance_t_spn)

[matrix_t](#matrix_t_spn)

[reaction_t](#reaction_t_spn)

Con las instancias de estas estructuras, podemos guardar informacion de:

	elem_t: de un elemento quimico.
	substance_t: de un compuesto o sustancia
	reaction_t: de una reaccion quimica expresada como ecuacion quimica.
	matrix_t: una matriz.

Se procedera a explicar como se guarda informacion y obtiene informacion con las instancias de estas estructuras. Ademas, al final se explicara como estas estructuras son usadas por el programa, es decir, su importancia y interrelacion.

### <a name="elem_t_spn">elem_t</a>

Se puede inicializar una instancia de la estructura elem_t usando la funcion:
```C
bool init_elem(const elem_symbol_t symbol, elem_t * const restrict elem);
```
Simplemente se debe pasar una instancia de elem_symbol_t y la direccion de memoria de la instancia de elem_t a la funcion.
La instancia de elem_symbol_t debe albergar el simbolo del elemento en cuestion. Entonces, para guardar este simbolo en la instancia de elem_symbol_t, previamente se llama a la funcion:
```C
bool init_elem_symbol(const char * const restrict symbol, elem_symbol_t * const restrict elem_symbol);
```
Esta funcion requiere un puntero a una cadena de caracteres que debe ser el simbolo en cuestion y la direccion de memoria a la instancia de elem_symbol_t que queremos usar para guardar el simbolo.

Estas dos funciones fallaran (y regresaran el valor booleano false) si el primer argumento pasado es incorrecto. Particularmente, init_elem_symbol() revisara que el primer caracter sea una mayuscula y que el segundo, si existe, sea una minuscula.
Pero entonces queda la duda, ¿Como pueden saber estas funciones si el simbolo pasado corresponde a un elemento quimico o no?
Es simple, ambas llaman a la siguiente funcion:
```C
atomic_num_t get_atomicnum(const elem_symbol_t elem_symbol);
```
get_atomicnum() solo requiere un argumento, una instancia de elem_symbol_t que alberge un simbolo, y devuelve a cambio el numero atomico correspondiente al elemento quimico o devuelve -1 si el simbolo pasado no corresponde a algun elemento quimico existente. Esto lo consigue gracias a que esta funcion es una funcion del tipo hash. Lo que la hace merecedora de llamarse asi es la peculiaridad de que la funcion convierte la cadena de caracteres pasada (el simbolo) en un numero, un indice valido (o posicion) en una tabla hash llamada sym_table (la cual solo es un arreglo de atomic_num_t, es decir, numeros atomicos). Con este indice/posicion, la funcion obtiene el numero atomico guardado en la posicion obtenida previamente. Esto se ha diseñado para optimizar a la funcion init_elem() y a su adjunta init_elem_symbol().

Entonces, ¿Como se obtiene informacion del elemento en cuestion una vez que la instancia de elem_t ha sido inicializada?

El numero atomico del elemento ya esta guardado en la estructura elem_t y se puede obtener mediante el uso de la siguiente macro:
```C
//Toma como argumento una instancia de elem_t
#define elem_get_atomicnum(elem) elem.atomic_num
```
Con este numero atomico tambien podemos obtener otros atributos del elemento usando a las siguientes funciones:
```C
oxistates_t get_oxistates(atomic_num_t atomic_num);//devuelve un arreglo con los posibles estados de oxidacion de un elemento
elem_symbol_t get_elem_symbol(atomic_num_t num);//devuelve el simbolo del elemento
num_t get_uma(atomic_num_t atomic_num);//devuelve el peso atomico en uma del elemento
```
Las tres solo requieren un argumento, el numero atomico del elemento en cuestion. Luego, le restan 1 a ese numero atomico y asi obtienen un indice (o posicion) valido para un arreglo en cuestion. Entonces, usando el indice mencionado previamente, obtienen un elemento del arreglo y devuelven este elemento, el cual es el atributo (estados de oxidacion, simbolo o peso atomico) del elemento en cuestion. Cada funcion accede a un diferente arreglo/array: get_oxistates() accede a oxistates_table, get_elem_symbol() accede a table y get_uma accede a uma_table. 

Para llamar a estas funciones facilmente se han definido a las siguientes macros:
```C
//Al igual que elem_get_atomicnum(), estas tambien toman como argumento una instancia de elem_t
/*
  Las tres macros luego llaman a elem_get_atomicnum() para obtener el numero atomico
  y asi luego pasar este numero a la funcion en cuestion
*/

#define elem_get_oxistates(elem) get_oxistates(elem_get_atomicnum(elem))

#define elem_get_symbol(elem) get_elem_symbol(elem_get_atomicnum(elem))

#define elem_get_uma(elem) get_uma(elem_get_atomicnum(elem))
```
Realmente, el proposito de estas macros es hacer facil de leer y escribir codigo.

### <a name="substance_t_spn">substance_t</a>

Esta estructura sirve para guardar guardar la informacion de una sustancia. Para esto se llama a la siguiente funcion:
```C
bool init_substance(const char * str, substance_t * const restrict sub);
```
Esta funcion toma como argumentos a un puntero a una cadena de caracteres y un puntero a la instancia de substance_t que sera usada para guardar la informacion de la sustancia. La cadena de caracteres debe ser la formula molecular, donde se agrupen elementos solo usando parentesis (no se reconocen los corchetes u otros simbolos) mas un coeficiente que indique la cantidad de moleculas que conforman la sustancia. Por ejemplo, 2C6H12O6 seria la formula de una sustancia donde hay dos moleculas de C6H12O6, es decir, glucosa. La funcion regresa false si la cadena de caracteres no era una formula valida, caso contrario, regresa true.

Antes de presentar las funciones adjuntas a la estructura substance_t, debemos aclarar como se guardar una sustancia cualquiera en una instancia de substance_t al llamar a la funcion init_substance(). En primer lugar, init_substance() no usa solo una instancia de substance_t (la que es pasada como argumento) para guardar la informacion, sino que usa esta y mas; lo que pasa es que se forma un arbol o grafo donde cada elemento es una instancia de substance_t, donde el primer nodo/elemento o mejor llamado root/raiz es la instancia pasada como argumento a la funcion init_substance(). En segundo lugar, cada elemento/nodo del arbol/grafo es una sustancia por si misma, solo que estas se divididen en las sustancias que las forman. Por ejemplo, 2H2O se divide en H2O, y este ultimo se divide en 2H y O, o tambien 4Fe(OH)2 el cual se divide en Fe(OH)2, luego este ultimo se divide en Fe y en 2OH, el cual se divide en O y H. Dos representaciones graficas de este tipo de estructura de datos son:

file:///home/MICHU_NEET/tree1.jpg![imagen](https://user-images.githubusercontent.com/69653355/173372889-844623b0-43d4-472f-8970-815c7c821814.png)



file:///home/MICHU_NEET/tree2.jpg![imagen](https://user-images.githubusercontent.com/69653355/173373703-fa7ce484-ca6d-4539-8df1-010c9e1fcdb8.png)


Todos los nodos/elementos del arbol tienen siempre estos tres datos: su cantidad de moleculas o tambien llamado coeficiente, su masa molar y un valor booleano (false/falso o true/verdadero) que indica si el elemento/nodo solo esta conformado por un unico elemento quimico, una sustancia simple, o es una sustancia compuesta por otras sustancias, una sustancia compleja; dependiendo de si el valor es verdadero o falso un cuarto dato es: si la sustancia es simple (el valor es verdadero/true), el cuarto dato es una instancia de elem_t, que guarda la informacion del elemento quimico en cuestion, caso contrario, el cuarto dato es un puntero que apunta al arreglo que alberga a las sustancias/elementos/nodos que conforman a la sustancia en cuestion. Por ultimo, cabe aclarar que esto se diseño asi pues hace que sea muy facil obtener la sustancia expresada en una formula quimica pasada como argumento a init_substance() dado que estas formulas pueden tener multiples parejas de parentesis recursivas (es decir, una dentro de la otra). 

Lo mencionado explica porque se reserva mucha memoria para guardar una sustancia en substance_t. Por eso es de vital importancia la funcion:
```C
void destroy_substance(substance_t * const restrict sub);
```
Esta funcion libera toda la memoria reservada para la instancia de substance_t y para saber a que instancia efectuar esta operacion toma como argumento un puntero a la instancia de substance_t en cuestion. Para poder efectuar esta operacion se hace uso de la recursividad: una vez llamada la funcion, esta se llama a si misma (es decir, la funcion destroy_substance() ejecuta otra vez a la funcion destroy_substance()) usando como argumentos a cada uno de las sustancias/elementos/nodos que componen a la sustancia pasada como argumento y luego libera la memoria donde se guardan estas sustancias que componen a la sustancia/elemento/nodo en cuestion. 

Por otro lado, para imprimir la formula quimica de una sustancia guardada en una instancia de substance_t, se usa la siguiente funcion:
```C
size_t print_substance(const substance_t * const restrict sub, char * buffer, size_t buffer_len);
```
Esta funcion requiere tres argumentos: un puntero a la instancia de substance_t en cuestion, un puntero a un buffer (el espacio en memoria donde se imprimira), y la longitud del buffer. En cuestion, la funcion recorre el grafo/arbol de izquierda a derecha e imprime a cada elemento del grafo/arbol como si fuera una substancia en si misma (a excepcion del primer elemento/nodo o tambien llamado raiz/root). En realidad, lo que pasa es que esta funcion primero imprime el coeficiente de la sustancia y luego prosigue a llamar a la funcion \_print_substance(), funcion que toma los mismos argumentos que print_substance() y es la que se encarga de imprimir el resto de la formula. En esta primera llamada a la funcion \_print_substance() se le pasa como argumento el puntero al siguiente nodo conectado a la raiz del arbol (un puntero a una instancia de substance_t). Luego, es esta funcion la que recorre el arbol de izquierda a derecha y cada vez que pasa por un elemento/sustancia del arbol hace lo siguiente: si la sustancia es un compuesto de otras sustancias (sustancia compleja) y tiene un coeficiente diferente de uno, abre parentesis, prosigue a recorrer las sustancias que conforman a este compuesto (mediante recursividad, es decir, de la misma manera que usa destroy_substance()) y luego cierra los parentesis seguido del coeficiente de la sustancia; y si la sustancia es solo conformada por un solo elemento (sustancia simple), es decir, no es un compuesto de otras sustancias, se imprime el simbolo del elemento (el cual se obtiene mediante una llamada a elem_get_symbol()) seguido del coefciente. 

Ademas, es posible obtener la cantidad de atomos de cada elemento quimico en una sustancia guardada en una instancia de substance_t (si el elemento quimico no esta presente, la cantidad seria cero). Para esto se usan dos cosas:
```C
typedef struct
{
    size_t bucket[NUMBER_OF_ELEMENTS+1];
    size_t ncomponents;
}components_of_substance_t;//estructura para guardar la cantidad de cada elemento en una sustancia.

//funcion para obtener la cantidad de atomos de cada elemento en una sustancia (que esta guardada en una instancia de substance_t)
void get_components_of_substance(const substance_t * const restrict sub, components_of_substance_t * const restrict comp);
```
Esta funcion toma como argumentos dos cosas: la direccion de memoria o puntero a la instancia de substance_t en cuestion y un puntero a la instancia de la estructura components_of_substance_t donde se guardaran las cantidades. Para obtener estas cantidades se debe recorrer el arbol o grafo en cuestion, y esto se hace otra vez recurriendo a la recursividad, eso si, la recursividad no se llevara a cabo usando a la funcion get_components_of_substance(). En realidad, esta funcion cuando es ejecutada llama a otra funcion para hacer el trabajo sucio:
```C
void _get_components_of_substance(const substance_t * const restrict sub, components_of_substance_t * const restrict comp, size_t multiplier)
```
A diferencia de la primera funcion, esta funcion toma un tercer argumento, un multiplicador. Cuando la funcion get_components_of_substance() es llamada, la funcion hace que los contadores de la instancia de components_of_substance_t sean iniciados en cero y luego esta funcion/rutina instanteaneamente llama a \_get_components_of_substance(), pasandole sus mismos argumentos (el puntero a la instancia de substance_t y el puntero a la instancia de components_of_substance_t) y pasa a 1 como el multiplicador. Cuando la funcion \_get_components_of_substance() es llamada, esta luego se llama a si misma varias veces usando como argumento a cada una de las sustancias que componen a la sustancia actual, pero eso si, se pasa el mismo puntero a una instancia de components_of_substance_t y el multplicador pasado como argumento es el producto del multiplicador actual por el coeficiente de la sustancia actual; sin embargo, en el caso de que la sustancia sea conformada por un solo elemento (es decir, no se ramifique en otras sustancias, lo que se llama sustancia simple en quimica), lo que se hace es usar el numero atomico del elemento en cuestion (se llama a la macro elem_get_atomicnum() usando como argumento a la instancia de elem_t guardada en la instancia de substance_t) para acceder a una posicion valida en el arreglo de numeros enteros albergado en la instancia de components_of_substance_t (si, lo que se usa para guardar las cantidades, no es mas que un arreglo de numeros enteros sin signo donde cada uno es un contador que corresponde a un alemento en cuestion) y luego al contador en la posicion obtenida se le suma la cantidad de atomos del elemento en la sustancia (el coeficiente o cantidad de moleculas) en cuestion multiplicada por el multiplicador pasado como argumento. 

Luego se pueden hacer otras operaciones con la instancia de components_of_substance_t, usando a las siguientes funciones:
```C
bool compare_components_of_substance(const components_of_substance_t * const comp1, const components_of_substance_t * const comp2);

void sum_components_of_substance(
	const components_of_substance_t * const comp1, const components_of_substance_t * const comp2, components_of_substance_t * const result
	);
```
La primera funcion, compare_components_of_substance(), compara a dos instancias de components_of_substance_t, requieriendo como argumentos a las direcciones de memoria o punteros de cada instancia, y devuelve el valor booleano false si no son iguales o true si son iguales. Mientras que la segunda funcion, sum_components_of_substance(), toma dos instancias de components_of_substance_t, las suma y guarda el resultado de la suma en una tercera instancia de components_of_substance_t, requieriendo esta vez los punteros de las tres instancias.
Por ultimo, hay una funcion que obtiene el numero de equivalente quimico de una sustancia que usa a la funcion get_components_of_substance():
```C
num_t get_equivalent_number_of_substance(const substance_t * restrict const sub);
```
Esta funcion solo requiere un argumento: un puntero a una instancia de substance_t. Para cumplir su trabajo esta funcion llama a get_components_of_substance() para obtener la cantidad de atomos de hidrogeno que hay en la sustancia dada y luego usa este numero para dividir al producto de la multiplicacion entre la masa molar de la sustancia y la cantidad de la sustancia; el cociente de esta division seria el numero de equivalente.

La recursividad usada aqui puede ser ejemplificada con un programa que se ejecuta a si mismo otra vez. Es decir, un programa que se esta ejecutando (una funcion que ha sido llamada y aun no termina su ejecucion) crea otra instancia del mismo (la funcion llama/ejecuta otra vez a si misma pero con diferentes argumentos), como cuando presionamos dos veces al icono del navegador y nos salen dos pestañas de este navegador, luego la instancia original del programa hace que la otra instancia cumpla una tarea dadas ciertas indicaciones (los argumentos de la funcion) y, por ultimo, se cierra la segunda instancia del programa una vez que la tarea se cumple y el programa original puede seguir trabajando en lo que estaba (la funcion llamada termino y la funcion original, la que llamo a la segunda, sigue con lo que estaba haciendo). Ademas, cabe recalcar que la segunda instancia puede que cree otra instancia del mismo programa para cumplir con su tarea. 

La recursividad no ha sido usada por mero capricho de mi, el Michu; en realidad, dado que los arboles o grafos son estructuras de datos recursivas, la recursividad es la mejor forma de recorrer y trabajar con estas. 

### <a name="matrix_t_spn">matrix_t</a>

Esta estructura sirve para poder crear y guardar matrices. Entonces, para crear una matriz se debe usar la siguiente funcion:

```C
bool create_matrix(matrix_t* const restrict matrix, size_t nrows, size_t ncols);
```
Esta toma como argumento tres cosas: un puntero a la instancia de matrix_t, el numero de filas y el numero de columnas.
Luego, con estos tres argumentos, esta funcion crea una matriz y luego guarda esta matriz en la instancia de matrix_t.

Una vez que la matriz ya no sea necesaria, se usa la siguiente funcion para liberar la memoria consumida por la matriz:
```C
void delete_matrix(matrix_t* const restrict matrix);
```

Esta estructura esta definida de esta manera:
```C
typedef long long integer_t;

typedef struct
{
    integer_t* arr;
    size_t len;
}row_t;

typedef struct
{
    row_t* rows;//puntero a un arreglo/array de instancias de la estructura row_t, es decir, a un arreglo de filas.
    size_t nrows;
    size_t ncols;
}matrix_t;
```
Como se puede ver, cada instancia incializada de matrix_t alberga el numero de filas, el numero de columnas y un arreglo de filas, que seria en si la matriz. Por lo tanto, para acceder al numero de filas o al numero de columnas simplemente se accede a estos de directamente, a excepción de los elementos de la matriz y las filas. Para acceder a un elemento o a una fila de una matriz (con el objetivo de obtener un valor o modificarlo), se usan estas macros:
```C
#define matrix_get_row(matrix,i) matrix.rows[i]

#define matrix_get_elem(matrix,i,j) matrix.rows[i].arr[j]
```
Estas macros solo requieren de la instancia de matrix_t que alberga la matriz y la posicion del elemento o fila en cuestion. Usar estas macros hace que el codigo sea mas legible.

Por ultimo, se puede hacer que la matriz se convierta en una matriz RREF o una matriz REF usando las siguientes funciones:
```C
void make_matrix_REF(matrix_t* const restrict matrix);

void make_matrix_RREF(matrix_t* const restrict matrix);
```
La funcion make_matrix_REF() aplica eliminacion de Gauss sobre la matriz, mientras que make_matrix_RREF() aplica eliminación de Gauss-Jordan sobre la matriz. Ambos algoritmos funcionan para matrices cuadradas y rectangulares puesto que no usan como referencia a una diagonal principal, sino que estos usan los pivotes de cada fila (si es que la fila tiene uno), es decir, ambas funciones usan los algoritmos originales (a diferencia de lo que enseñan en el cole jsjjsj). Sin embargo, make_matrix_RREF() no puede asegurar de que todos los pivotes seran iguales a uno en caso de que al dividir una fila por un escalar se cree un numero no entero, puesto que una matriz guardada en una instancia de matrix_t solo puede albergar numeros enteros. Ademas, make_matrix_REF(), tras convertir la matriz a REF, elimina las filas cuyos elementos sean todos ceros.

### <a name="reaction_t_spn">reaction_t</a>

Las instancias de esta estructura se encargan de guardar a reacciones quimicas (expresadas como ecuacion quimica). Para esto poder guardar una reaccion quimica en una instancia de reaction_t hay dos funciones: 
```C
reaction_err_t init_reaction(const char * restrict _reactants, const char * restrict _products, reaction_t * restrict const react, bool balance);

reaction_err_t init_balanced_reaction(const char * restrict _reactants, const char * restrict _products, reaction_t * restrict const react);
```
Ambas funciones requieren tres argumentos: un puntero a una cadena de caracteres donde este la parte de la ecuacion quimica de los reactivos, un puntero a la cadena de caracteres donde este la parte de la ecuacion de los productos y un puntero a una instancia de reaction_t donde se guardara la reaccion quimica. Sin embargo, la segunda funcion, init_reaction(), requiere un cuarto argumento: un valor booleano (false/falso o true/verdadero) que indique si se debe balancear la ecuacion quimica o, caso contrario, fallar si la ecuacion no esta balanceada. Para guardar a la ecuacion quimica se crean dos arreglos de instancias de substance_t, la primera para los reactivos y la segunda para los productos, y en cada de esas instancias se guarda a cada reactivo/producto (que son sustancias) llamando a la funcion init_substance(), a la cual se le pasa como argumento a la formula quimica correspondiente a la sustancia que es encontrada en una de las cadenas de caracteres pasadas como argumentos. 

Estas funciones al ser llamadas pueden devolver uno de los siguientes valores:

	REACTION_ERR_INIT_UNKNOWN_SYMBOL (0)
		Indica que hay un simbolo desconocido o un error en la sintaxis.

	REACTION_ERR_INIT_SUCCESS (1)
		Indica que la operacion fue exitosa.

	REACTION_ERR_INIT_UNBALANCED (2)
		Se fallo pues la ecuacion quimica no esta balanceada y no se nos ha indicado que se balancee.

	REACTION_ERR_INIT_CANNOT_BALANCE (3)
		Se trato de balancear pero es imposible.

Para balancear se usa el metodo de balanceo de ecuaciones quimicas llamado metodo algebraico para crear un sistema de ecuaciones y luego se resuelve este sistema mediante Gauss-Jordan. El sistema de ecuaciones se expresa como una matriz que es guardada en una instancia de matrix_t, que es creada mediante una llamada a create_matrix() a la cual se le pasa como numero de filas (el numero de ecuaciones) el numero de diferentes elementos que actuan en la reaccion quimica mas 1 y como numero de columnas (numero de variables) a el numero de reactivos mas el numero de productos. Para obtener al numero de filas se recorren ambos arreglos de instancias de substance_t para se obtiener la cantidad de atomos de cada elemento quimico usando a la funcion get_components_of_substance() y luego sumar el resultado a otra instancia de components_of_substance_t usando a la funcion de sum_components_of_substance() para asi al final que obtener el total guardado en una instancia de components_of_substance_t, asi luego simplemente contar la cantidad de contadores que son diferentes de cero y asi obtener el numero de elementos que participan en la reaccion. Por otro lado, el numero de reactivos y el numero de productos se obtienen directamente dentro en la misma instancia de reaction_t que ya ha sido inicializada por una de las dos funciones ya mencionadas. Entonces, una vez que tenemos la matriz hecha, lo siguiente es rellenarla y para eso se va recorriendo otra vez los arreglos de instancias de substance_t (como se hizo previamente para obtener el numero de filas) y se va obteniendo la cantidad de atomos de cada elemento en cada reactivo/producto usando a la funcion get_components_of_substance(), pero previamente haciendo que el coeficiente de la sustancia sea uno, luego con los valores albergados en la instancia de components_of_substance_t donde se han guardado las cantidades de atomos de cada elemento se rellena la columna correspondiente al reactivo o producto en cuestion pero sin tocar a la ultima fila (no solo porque esta no corresponde a una ecuacion de uno de los elementos, tambien porque con esta podremos romper la simetria de la matriz), ademas, si se trata de un reactivo, el numero ingresado en la matriz debe ser multiplicado por -1. Posteriormente, al primer elemento de la ultima fila (que estaria en la primera columna) se le hace uno (lo cual le hace el unico elemento diferente de cero en la fila) y luego se hace uno al elemento ultimo de la ultima fila (el elemento estaria en la ultima columna). Una vez que la matriz ha sido rellenada, se llama a la funcion make_matrix_RREF() para convertir a la matriz en una matriz RREF (basicamente Gauss-Jordan). Esto nos daria una respuesta satisfactoria, pero a veces se debe seguir una serie de pasos extras, para asi hacer que los coeficientes sean enteros, a continuacion se da un ejemplo para mostrar esto:

Para balancear a: CO2 + H2O --> C6H12O6 + O2
	
	1. Primero se forma la matriz:
	
		0 0 0 0 0

		0 0 0 0 0
		
		0 0 0 0 0
	
	2. Luego se rellena la matriz.
	
		0 2 -12  0 0
	
		1 0  -6  0 0
	
		2 1  -6 -2 0
	
		1 0   0  0 1

		Tomando en cuenta que el sistema de ecuaciones seria:

		H:  0a + 2b - 12c + 0d = 0 
		C:  1a + 0b - 6c + 0d = 0 
		O:  2a + 1b - 6c - 2d = 0 
		    1a + 0b + 0c + 0d = 1
	
	3. Luego se usa Gauss-Jordan para simplificar a la matriz:
	
		1 0 0 0 1
	
		0 1 0 0 1
	
		0 0 6 0 1
	
		0 0 0 1 1

	4. En este caso, como uno de los elementos de la diagonal principal no es 1, se multiplica a toda la ultima columna 
	(la que corresponde a los terminos independientes de cada ecuacion) el minimo comun multiplo entre el termino independiente 
	y el termino diferente de 1 de la fila 3, al ultimo elemento de la fila se le divide por el elemento diferente de 1 
	y luego se se hace a este numero diferente de uno ser igual a 1.

		1 0 0 0 6
	
		0 1 0 0 6
	
		0 0 1 0 1
	
		0 0 0 1 6

	5. Recorriendo a cada ecuacion/fila se sacan los nuevos coeficientes.

		En caso de ser posible, tambien se simplifica los coeficientes dividiendolos para un minimo comun divisor que tengan.

		6CO2 + 6H2O ---> C6H12O6 + 6O2

**<a name="how_to_compile_spn">Como compilar?</a>**
	
	Debes tener las siguentes herramientas:
	
		python 3.9.10 
		mendeleev 0.9.0
		gcc de mingw (con el paquete de los POSIX threads instalado)
		make
	
	Luego, en la terminal, ejecuta el comando "make wmain.exe"
	wmain.exe sera el programa resultante.
	Despues de esto, puedes usar el comando "make clean" para borrar los object files.

**<a name="how_to_use_spn">Como usar?</a>**
	
	Primero, ingresa la reaccion quimica.
	Segundo, ingresa la informacion que tengas de cada reactivo/producto en la reaccion.
	Tercero, presiona el boton debajo de la reaccion quimica.
	(Cuando ingreses la informacion, puede que cometas errores, asi que toma atencion a los mensajes de error que apareceran).
	
	
	Los calculos resultantes incluyen:
	
		Lo que necesita un reactivo/producto para reaccionar o ser producido.
		Dada informacion de mas de un reactivo, se puede determinar el reactivo limitante y el en exceso.
		La masa y los moles de los reactivos/productos en la reaccion real y la teorica.

# English version (para los gringos)

<a name="how_to_compile_eng">How to compile?</a>
	
	You must have these tools:
	
		python 3.9.10 
		mendeleev 0.9.0
		a C/C++ compiler (which must include WINAPI and POSIX threads, such as gcc of mingw)
		make
	
	
	Then, you just have to type "make wmain.exe".
	wmain.exe will be the final program.
	Don't forget to type "make clean" to clean all object files (which are basically trash).

<a name="how_to_use_eng">How to use Mochi?</a>

First of all, you ought to put the reaction and pay attention to the error messages.
	Then, put the information that you have of each reactive or product (and also pay attention to the error messages).
	Then, click the button below the reaction that you write.
	The final results (output) are the calculations of:

		Given enough data:
			The requirements of a product to be created (if the substance is a product).
			The requirements of a reactant to react (if the substance is a reactant).
		Given enought data of two or more reactants:
			Which the limiting reagent is.
			Which the excess reagents are.
		The mass and mol of each substance in the:	
			The theoretical reaction.
			The real reaction.

		

