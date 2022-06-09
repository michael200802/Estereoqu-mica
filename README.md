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

**<a name="how_does_it_work_spn">Como funciona</a>**
	
Hay 4 estructuras vitales para la logica del programa: 
	
	elem_t, substance_t, reaction_t y matrix_t.

Con las instancias de estas estructuras, podemos guardar informacion de:

	elem_t: de un elemento quimico.
	substance_t: de un compuesto o sustancia
	reaction_t: de una reaccion quimica expresada como ecuacion quimica.
	matrix_t: una matriz.

Se procedera a explicar como se guarda informacion y obtiene informacion con las instancias de estas estructuras.

## elem_t

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
get_atomicnum() solo requiere un argumento, una instancia de elem_symbol_t que alberge un simbolo, y devuelve a cambio el numero atomico correspondiente al elemento quimico o devuelve -1 si el simbolo pasado no corresponde a algun elemento quimico existente. Esto lo consigue gracias a que esta funcion es una funcion del tipo hash. Lo que la hace merecedora de llamarse asi es la peculiaridad de que la funcion convierte la cadena de caracteres pasada (el simbolo) en un numero, un indice valido (o posicion) en una tabla hash llamada sym_table (la cual solo es un arreglo de atomic_num_t, es decir, numeros atomicos). Con este indice/posicion, la funcion obtiene el numero atomico guardado en la posicion obtenida previamente. Esto se ha diseñado para optimizar la funcion init_elem().



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

		

