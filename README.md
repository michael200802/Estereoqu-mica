# Mochi
Why doing your stereochemistry homework alone when you can do it with Mochi?

**Version en español**

Como compilar?
	Debes tener las siguentes herramientas:
		python 3.9.10 
		mendeleev 0.9.0
		gcc (which must include WINAPI and POSIX threads)
		make
	Luego, en la terminal, ejecuta el comando "make wmain.exe"
	wmain.exe sera el programa resultante.
	Despues de esto, puedes usar el comando "make clean" para borrar los object files.

Como usar?
	Primero, ingresa la reaccion quimica.
	Segundo, ingresa la informacion que tengas de cada reactivo/producto en la reaccion.
	Tercero, presiona el boton debajo de la reaccion quimica.
	(Cuando ingreses la informacion, puede que cometas errores, asi que toma atencion a los mensajes de error que apareceran).
	Los calculos resultantes incluyen:
		Lo que necesita un reactivo/producto para reaccionar o ser producido.
		Dada informacion de mas de un reactivo, se puede determinar el reactivo limitante y el en exceso.
		La masa y los moles de los reactivos/productos en la reaccion real y la teorica.

**English version (para los gringos)**

How to compile?
	You must have these tools:
		python 3.9.10 
		mendeleev 0.9.0
		gcc (which must include WINAPI and POSIX threads)
		make
	Then, you just have to type "make wmain.exe".
	wmain.exe will be the final program.
	Don't forget to type "make clean" to clean all object files (which are basically trash).

How to use Mochi?
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

		

