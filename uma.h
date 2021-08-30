#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char unidad[5];

//LISTA STR
typedef struct nodo{
	int x;
	struct nodo *sig, *prev;
}str;

str*node(unsigned char x)
{
	str*n = (str*)malloc(sizeof(str));
	n->x = x;
	n->prev = NULL;
	n->sig = NULL;
	return n;
}

void add(str**i,str**f,int x)
{
	if(*i == NULL)
	{
		*i = node(x);
		*f = *i;
	}
	else
	{
		(*f)->sig = node(x);
		(*f)->sig->prev = *f;
		*f = (*f)->sig;
	}
}

int sizel(str*p)
{
	int i = 0;
	while (p != NULL)
	{
		i++;
		p = p->sig;
	}
	return i;
}

char *lts(str*p)
{
	int c = sizel(p);
	char *r = (char*)malloc(sizeof(char)*(c+1)),*aux = r;
	while (p != NULL)
	{
		*r = p->x;
		p = p->sig;
		r++;
		c--;
	}
	*r = '\0';
	return aux;
}

void del(str*p)
{
	while (p->sig != NULL)
	{
		p = p->sig;
		free(p->prev);
	}
	free(p);
}


//FUNCIONES ARIMETICAS

typedef double ld;

int ne, nr;

double seedo, seedn, *original;

void start(ld *originalp, int num_elementos, int num_reactivos)
{
	original = originalp;
	ne = num_elementos;
	nr = num_reactivos;
	strcpy(unidad,"g");
}

void set(double seed_original, double seed_new)
{
	seedo = seed_original;
	seedn = seed_new;
}

double fun(double original_result)
{	
	return (original_result*seedn)/seedo;
}

void put(double *v, int pos, double content)
{
	ld *o = original;
	int size = ne;
	set(o[pos],content);
	while(size)
	{
		if(pos == 0)
		{
			*v = content;
		}
		else
		{
			*v = fun(*o);
		}
		pos--;
		v++;
		o++;
		size--;
	}
}

//IMPLEMENTACION

void sigf(FILE *f, char x)
{
	int k;
	while((k = fgetc(f)) != x && k != -1);
}

int nume(FILE *f, int* r)
{
	int c = 0, k;
	while ( !((k = fgetc(f)) == 'v' && fgetc(f) == 'a') && k != -1)
	{
		if(k == '\n')
		{
			c++;
		}
		else
		{
			if(k == 'p')
			{
				*r = c-1;
			}
		}
	}
	rewind(f);
	return c-2;
}

ld find(const char *l)
{
	char x[3];
	ld v;
	FILE *f = fopen("const.txt","r");
	while (fscanf(f,"%s",&x) != -1)
	{
		if(strcmp(l,x) == 0)
		{
			fscanf(f,"%Lf",&v);
			fclose(f);
			return v;
		}
		sigf(f,'\n');
	}
	fclose(f);
	return 0;
}

int readnum(FILE*f)
{
	int r = 0, k, pos;
	while(1)
	{
		pos = ftell(f);
		if((k = fgetc(f)) != -1 && isdigit(k))
		{
			if(r != 0)
			{
				r = r*10;
			}
			r += k-48;
		}
		else
		{
			break;
		}
	}
	fseek(f,pos,SEEK_SET);
	if(r == 0)
	{
		r = 1;
	}
	return r;
}

ld readsimb(FILE*file)
{
	str*i = NULL, *f = NULL;
	int k, pos, lon = 0;
	while(1)
	{
		pos = ftell(file);
		if((k = fgetc(file)) != -1 && isalpha(k) && lon < 2  && !(lon == 1 && isupper(k)) )
		{
			add(&i,&f,k);
			lon++;
		}
		else
		{
			break;
		}
	}
	fseek(file,pos,SEEK_SET);
	char *c = lts(i);
	del(i);
	ld r = find(c);
	free(c);
	return r;
}

ld readc(FILE *f)
{
	ld r = 0, v = readnum(f);
	int k;
	while ((k = fgetc(f)) != -1 && k != '\n')
	{
		if(k == ' ')
		{
			sigf(f,'\n');
			break;
		}
		fseek(f,ftell(f)-1,SEEK_SET);
		r += readsimb(f)*readnum(f);
	}
	return r*v;
}

ld *readr(FILE *f, int*r, int*size)
{
	int i = 0;
	*size = nume(f,r);
	ld *v = (ld*)malloc(sizeof(ld)*(*size));
	start(v,*size,*r);
	sigf(f,'\n');
	for(; i < *r; i++)
	{
		v[i] = readc(f);
	}
	sigf(f,'\n');
	for(; i < *size; i++)
	{
		
		v[i] = readc(f);
	}
	return v;
}

int lendf(FILE*f)
{
	int i = 0, k = fgetc(f);
	if(k != -1)
	{
		i = 1;
	}
	while (k != -1)
	{
		if(k == '\n')
		{
			i++;
		}
		k = fgetc(f);
	}
	return i;
}

char *getcom(FILE*file,int pos, int numr)
{
	int aux = ftell(file), k;
	rewind(file);
	
	if(pos < numr)
	{
		pos++;
	}
	else
	{
		pos+=2;
	}

	for(int i = 0; i < pos; i++)
	{
		sigf(file,'\n');
	}

	readnum(file);

	str*i = NULL, *f = NULL;

	while ((k = fgetc(file)) != -1 && k != '\n')
	{
		add(&i,&f,k);
	}
	
	char *r = lts(i);
	del(i);

	fseek(file,aux,SEEK_SET);
	return r;
}

ld**readv(FILE*f, int *nv, int size)
{
	{
		int aux = ftell(f);
		*nv = lendf(f);
		fseek(f,aux,SEEK_SET);
	}
	if(*nv == 0)
	{
		return NULL;
	}
	ld **v = (ld**)malloc(sizeof(ld*)*(*nv)), content;
	int pos;
	for(int i = 0; i < *nv; i++)
	{
		v[i] = (ld*)malloc(sizeof(ld)*(size+1));
		pos = readnum(f)-1;
		*(*(v+i)+size) = (ld)pos;
		fscanf(f,"%Lf",&content);
		switch (fgetc(f))
		{
		case -1:
			/* code */
			break;
		case '\n':
			/* code */
			break;
		default:
			{
				char uni[6];
				fscanf(f,"%s",&uni);
				if(strcmp("mol",uni) == 0 || strcmp("moles",uni) == 0)
				{
					char *comp = getcom(f,pos,nr);
					FILE *tempf = tmpfile();
					fputs(comp,tempf);
					rewind(tempf);
					content *= readc(tempf);
					fclose(tempf);
					free(comp);	
				}
				sigf(f,'\n');
			}
			break;
		}
		put(v[i],pos,content);
	}
	return v;
}

void setuni(const char *unid)
{
	strcpy(unidad,unid);
}

void show(FILE*f, double *v, const char* message)
{
	puts(message);
	int size = 0, rnum = nr;
	char *r;
	while(size < ne)
	{
		r = getcom(f,size,nr);
		printf("%Lf%s %s",*v,unidad,r);
		free(r);
		if(size != ne -1)
		{
			if(rnum == 1)
			{
				printf(" ----> ");
			}
			else
			{
				printf(" + ");
			}
		}
		rnum--;
		v++;
		size++;
	}
	puts("\n");
}


#define S 32.06
#define Li 6.94 
#define Fe 55.845 
#define O 15.999
#define H 1.008
#define C 12.011 
#define N 14.007
#define P 30.974
#define Mg 24.305
#define K 39.098
#define Cl 35.45
#define Mn 54.938
#define Pb 207.2
#define Ca 40.078
#define F 18.998
#define K 39.098
#define LEY "LA LEY DE CONSERVACION DE MASAS SE CUMPLE"

/*
reactivos:
S8
4Cl2
productos:
4S2Cl2
variables:
1 4.06 
2 6.24
*/