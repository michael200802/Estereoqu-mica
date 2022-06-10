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

[reaction_t](#reaction_t_spn)

[matrix_t](#matrix_t_spn)

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



### <a name="reaction_t_spn">reaction_t</a>

### <a name="matrix_t_spn">matrix_t</a>

Esta estructura sirve para poder crear y guardar matrices. Entonces, para crear una matriz se debe usar la siguiente funcion:

```C
bool create_matrix(matrix_t* const restrict matrix, size_t nrows, size_t ncols);
```
Esta toma como argumento tres cosas: un puntero a la instancia de matrix_t, el numero de filas y el numero de columnas.
Luego, con estos tres argumentos, crea una matriz y luego guarda esta matriz en la instancia de matrix_t.

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
Estas macros solo requieren de la instancia y la posicion del elemento o fila en cuestion. Usar estas macros hace que el codigo sea mas legible.

Por ultimo, se puede hacer que la matriz se convierta en una matriz RREF o una matriz REF usando las siguientes funciones:
```C
void make_matrix_REF(matrix_t* const restrict matrix);

void make_matrix_RREF(matrix_t* const restrict matrix);
```
La funcion make_matrix_REF() aplica eliminacion de Gauss sobre la matriz, mientras que make_matrix_RREF() aplica eliminación de Gauss-Jordan sobre la matriz. Ambos algoritmos funcionan para matrices cuadradas y rectangulares puesto que no usan como referencia a una diagonal principal, sino que estos usan los pivotes de cada fila (si es que la fila tiene uno), es decir, ambas funciones usan los algoritmos originales (a diferencia de lo que enseñan en el cole jsjjsj). Sin embargo, make_matrix_RREF() no puede asegurar de que todos los pivotes seran iguales a uno en caso de que al dividir una fila por un escalar se cree un numero no entero, puesto que una matriz guardada en una instancia de matrix_t solo puede albergar numeros enteros. Ademas, make_matrix_REF(), tras convertir la matriz a REF, elimina las filas cuyos elementos sean todos ceros.

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

		

