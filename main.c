#include "uma.h"

void _main()
{
	//Abrimos input.txt
	FILE *f = fopen("input.txt","r");
	//Redirigimos el buffer
	freopen("output.txt","w",stdout);

	
	int r, t, nv, flag = 0;
	//Reaccion original
	ld *react = readr(f,&r,&t);
	show(f,react,"Reaccion original");
	sigf(f,'\n');
	//Reacciones hipoteticas
	ld **v = readv(f,&nv,t);
	if(v == NULL)
	{
		return;
	}
	//Mostramos la info
	char *name;
	{

		for(int i = 0; i < nv; i++)
		{
			name = getcom(f,(int)*(*(v+i)+t),r);
			printf("Si hay %Lf gramos de %s", *(*(v+i) + (int)*(*(v+i)+t)), name);
			show(f,v[i]," pasa: ");
			free(name);
		}
	}


	//Busco el reactivo limitante
	for (int i = 1, sum; i < nv ; i++)
	{
		sum = (int)*(*(v+i)+ t);
		if(*(*(v+i) + sum) < *(*(v+i-1) + sum))
		{
			flag = i;
		}
	}
	name = getcom(f,(int)*(*(v+flag)+ t),r);
	printf("%Lfg %s es el limitante\n\n", *(*(v+flag) + (int)*(*(v+flag)+ t)),name);
	free(name);
	show(f,v[flag],"Reaccion real (teorica): ");
	{
		FILE *temp;
		for (int i = 0; i < t; i++)
		{
			temp = tmpfile();
			name = getcom(f,i,r);
			fputs(name,temp);
			rewind(temp);
			free(name);
			*(*(v+flag)+ i) = *(*(v+flag)+ i) / readc(temp);
			fclose(temp);
		}
	}
	setuni(" mol");
	show(f,v[flag],"Reaccion real en moles (teorica): ");


	//Final

	free(react);

	for (int i = 0; i < nv; i++)
	{
		free(*(v+i));
	}
	free(v);

	fclose(f);
}

int main()
{
	_main();
	fclose(stdout);
	system("output.txt");
	return 0;
}
