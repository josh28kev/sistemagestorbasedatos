#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// ---------------------------------ESTRUCTURAS-------------------------------------------------------
struct tupla_dato {
    int tabla_perteneciente;
    int numero_columna;
    int donde_empieza;
    int numero_bloque;
    int siguiente_bloque;
    char dato[256];
} tupla_dato;

struct tupla_catalogo {
    int identificador_unico;
    int numero_columna;
    char dato[256];
} tupla_catalogo;

typedef struct nodo_lista {struct nodo_lista *sig; char dato[256];} nodo;
// ---------------------------------------------------------------------------------------------------
// ------------------------------------------------MÉTODOS--------------------------------------------------------------
char* num_a_carac(int num){ // Transforma un número de int a escritura en char.
    int i=0, j;char a;
    char* result=(char*) malloc(sizeof(char*));
    while(num > 0){
   	j = num % 10 + 48;
   	result[i] = j;
   	num /= 10;
    }
    i = 0;
    j = strlen(result)-5;
    while(j>1){
   	a = result[i];
   	result[i] = result[j];
   	result[j] = a;
   	i++;j--;
    }
    return result;
}

nodo* lista_columnas_tabla(char* nombre_tabla){  // Devuelve un puntero a una lista de columnas de la tabla a insertar
    FILE *fa=fopen("catalog.dat","a");fclose(fa);
    bool fin = false; int codigo_tabla_a_insertar = 0;
    nodo* primero = (struct nodo_lista*) malloc(sizeof(nodo)); // Se crean nodos para recorrer una lista.
    nodo* actual = (struct nodo_lista*) malloc(sizeof(nodo));
    nodo* aux = (struct nodo_lista*) malloc(sizeof(nodo));
    aux->sig=NULL;
    actual=primero;
    strcpy(primero->dato,"-1");
    FILE *fp=fopen("catalog.dat","r");
    while(!fin && fread(&tupla_catalogo,sizeof(tupla_catalogo),1,fp)){
   	 if(strcmp(tupla_catalogo.dato,nombre_tabla) == 0){
   			codigo_tabla_a_insertar = tupla_catalogo.identificador_unico;
   			strcpy(primero->dato,num_a_carac(codigo_tabla_a_insertar));
   			while(fread(&tupla_catalogo,sizeof(tupla_catalogo),1,fp) && tupla_catalogo.identificador_unico == codigo_tabla_a_insertar){
   			 strcpy(aux->dato,tupla_catalogo.dato);
   			 actual->sig = aux;
   			 actual = actual->sig;
   			 aux = (struct nodo_lista*) malloc(sizeof(nodo));
   			}
   	fin = true;
   	 }
    }
    fclose(fp);
    return primero;
}

int quitar_tipo_dato_palabra(char* palabra){// Remueve el tipo de dato de una palabra con forma tipo-nombre.
    int i=0;
    for(i; palabra[i] && palabra[i] != '-'; i++){}// Avanza hasta que encuentre un guión o hasta que termine la palabra.
    if(palabra[i]){
   	strncpy(palabra, palabra+i+1 , strlen(palabra)-1);//Cambia la variable palabra por lo que esta contenía del "-" hacia delante.
  	 }
  	 return 0;
}

int aislar_tipo_dato(char* palabra){// Deja solo el tipo de dato en una palabra con forma tipo-nombre.
   palabra = (char*)strtok(palabra,"-");
}

nodo* lista_orden_argumentos(char** argumentos){ // Devuelve un puntero a una lista con el orden en el que se va a insertar en las columnas
    int i=3;
    nodo* primero = (struct nodo_lista*) malloc(sizeof(nodo));// Se crean los nodos para la lista.
    nodo* actual = (struct nodo_lista*) malloc(sizeof(nodo));
    nodo* aux = (struct nodo_lista*) malloc(sizeof(nodo));
    aux->sig=NULL;
    actual=primero;
    for(i; argumentos[i] && strcmp(argumentos[i],"values") != 0; i++){
   	 strcpy(aux->dato,argumentos[i]);// Se obtiene el valor de la columna a buscar.
   	 actual->sig = aux;
   	 actual = actual->sig;
   	 aux = (struct nodo_lista*) malloc(sizeof(nodo));// Se crea un nuevo nodo para la lista.
    }
  	 return primero->sig;// Regresa el puntero al primer elemento.
}

nodo* lista_valores_argumentos(char** argumentos){ // Devuelve un puntero a una lista con los valores que se va a insertar en las columnas
   int i=0;
   for(i; argumentos[i] && strcmp(argumentos[i],"values")!=0; i++){}// Se avanza en la instrucción hasta estar después de la palabra values.
   i += 1;
   nodo* primero = (struct nodo_lista*) malloc(sizeof(nodo));// Se crean los nodos para la lista.
   nodo* actual = (struct nodo_lista*) malloc(sizeof(nodo));
   nodo* aux = (struct nodo_lista*) malloc(sizeof(nodo));
   aux->sig=NULL;
   actual=primero;
   for(i; argumentos[i]; i++){
    strcpy(aux->dato,argumentos[i]);
    actual->sig = aux;// Se obtiene el valor del valor de columna a insertar.
    actual = actual->sig;
    aux = (struct nodo_lista*) malloc(sizeof(nodo));// Se crea un nuevo nodo para la lista.
   }
   return primero->sig;// Se retorna el puntero al primer elemento.
}

nodo* lista_tipos_de_datos(nodo* lista_columnas){// Se genera una lista con los tipos de dato de todos
   nodo* primero = (struct nodo_lista*) malloc(sizeof(nodo));// los valores, de forma secuencial.
   nodo* actual = (struct nodo_lista*) malloc(sizeof(nodo));
   nodo* aux = (struct nodo_lista*) malloc(sizeof(nodo));
   nodo* lista_cols = lista_columnas;
   aux->sig=NULL;
   actual=primero;
   while(lista_cols){
    strcpy(aux->dato,lista_cols->dato);
    aislar_tipo_dato(aux->dato);
    quitar_tipo_dato_palabra(lista_cols->dato);
    actual->sig = aux;
    actual = actual->sig;
    aux = (struct nodo_lista*) malloc(sizeof(nodo));
    lista_cols = lista_cols->sig;
   }
   return primero->sig;// Se retorna el puntero al primer elemento de la lista.
}

char* obtener_argumento_requerido(char* argumento_requerido, nodo* orden, nodo* valores){
    nodo* n1 = orden; // Este método obtiene un argumento requerido comparando el orden en el
  nodo* n2 = valores;// que esta con su valor y lo retorna. (Ej: nombre -> nombre -> Juan)
    while(n1 && n2 && strcmp(n1->dato,argumento_requerido)!=0){
   	 n1=n1->sig;n2=n2->sig;
    	}

    if(n1 && n2)
   	return n2->dato;

    return "null";
}

nodo* lista_valores_argumetos_ordenada(nodo* lista_columnas, nodo* lista_orden_args, nodo* lista_valores_args){
   nodo* lista_cols = lista_columnas;// Este método construye una lista con todos los argumentos requeridos, es decir
   nodo* primero = (struct nodo_lista*) malloc(sizeof(nodo));// pasa de Nombre, Apellido a Juan, Ramírez.
   nodo* actual = (struct nodo_lista*) malloc(sizeof(nodo));
   nodo* aux = (struct nodo_lista*) malloc(sizeof(nodo));
   aux->sig=NULL;
   actual=primero;
   while(lista_cols){
    strcpy(aux->dato,obtener_argumento_requerido(lista_cols->dato,lista_orden_args,lista_valores_args));
    actual->sig = aux;
    actual = actual->sig;
    aux = (struct nodo_lista*) malloc(sizeof(nodo));
    lista_cols = lista_cols->sig;
   }
   return primero->sig;
}

int cantidad_bloques_archivo(){// Retorna el numero de bloque al cual se podrá insertar el elemento.
   FILE *fp2=fopen("users.dat","a");fclose(fp2);
   FILE *fp=fopen("users.dat","r");
   int sig_tupla_dato = 1;
   while(fread(&tupla_dato,sizeof(tupla_dato),1,fp)){
    if(tupla_dato.numero_bloque != -1)
   	sig_tupla_dato = tupla_dato.siguiente_bloque;
   }
   fclose(fp);
   return sig_tupla_dato;
}

bool validar_int(char* texto){ //Revisa que un campo sea valido como tipo int.
   int i = 0;
   if(strcmp(texto,"null")==0)
  	return true;
   for(i; texto[i]; i++){
    if(texto[i]!='0'&&texto[i]!='1'&&texto[i]!='2'&&texto[i]!='3'&&texto[i]!='4'&&
   	texto[i]!='5'&&texto[i]!='6'&&texto[i]!='7'&&texto[i]!='8'&&texto[i]!='9')
   	return false;
   }
   return true;
}

bool validar_float(char* texto){ //Revisa que un campo sea valido como tipo float.
   if(strcmp(texto,"null")==0)
  	return true;
   bool hay_punto = false; int i=0;
   while(texto[i] && !hay_punto){
    if(texto[i]=='.')
   	hay_punto = true;
    i++;
   }
   if(!hay_punto)
  	return validar_int(texto);
   char *a,*b;
   char aux[strlen(texto)];
   strcpy(aux, texto);
   a = (char*)strtok(aux,".");//Recupera el primer elemento de argv segun '.'.
   b = (char*)strtok(NULL,".");//Recupera el segundo elemento de argv segun '.'.
   if(!validar_int(a) || !validar_int(b))
    return false;
   return true;
}

bool validar_tipos_datos(nodo* lista_tipos_datos, nodo* lista_datos_a_ingresar){// Se encarga de toda
   nodo* primero_tipos_datos = lista_tipos_datos;// la validación entre los tipos de dato y el dato en sí.
   nodo* primero_datos_nuevos = lista_datos_a_ingresar;
   while(primero_datos_nuevos && primero_tipos_datos){
    if(strcmp(primero_tipos_datos->dato,"int")==0)
   	if(!validar_int(primero_datos_nuevos->dato)){
       	printf("\nERROR: algun(os) tipo(s) de dato(s) no coincide(n) con el(los) requerido(s)\n");
       	return false;
   	}
    if(strcmp(primero_tipos_datos->dato,"float")==0)
   	if(!validar_float(primero_datos_nuevos->dato)){
       	printf("\nERROR: algun(os) tipo(s) de dato(s) no coincide(n) con el(los) requerido(s)\n");
       	return false;
       	}

    primero_tipos_datos = primero_tipos_datos->sig;
    primero_datos_nuevos = primero_datos_nuevos->sig;
   }
   return true;
}

bool encontrar_Values(int argc, char ** argv){// Verifica que exista la palabra values.
    int indice = 0;
    while(indice<argc){
    if(strcmp (argv[indice],"values") == 0)    {
   	 return true;
    }
   	 indice ++;
    }
    return false;
}

bool existe_Tabla(char ** argv){ // Verifica la existencia de la tabla buscada.
    FILE *fp2=fopen("catalog.dat","r");
  	 while(fread(&tupla_catalogo,sizeof(tupla_catalogo),1,fp2)){
   	if(strcmp (tupla_catalogo.dato,argv[2])==0){
         return true;
   	}
  	 }
    fclose(fp2);
    return false;
}

bool numeroArgumentos(int argc){// Verifica que la cantidad de argumentos en la función sea correcta.
    int total = argc-4;
    if(total%2==0){
    return true;
    }
    else {
    return false;
    }
}

int numero_Columnas (char ** argv){// Retorna el numero de columnas.
    int cantidad= 0;
    int indice = 3;
    while(strcmp(argv[indice], "values") != 0){
   	 indice ++;
   	 cantidad ++;
    }
    return cantidad;
}

bool existe_Columna (char* nombre_tabla,char* col){// Verifica la existencia de una columna.
	FILE *fp=fopen("catalog.dat","a+");int cod_tabla = -1;
	char *aux =(char*) malloc(sizeof(char*));
	while(fread(&tupla_catalogo,sizeof(tupla_catalogo),1,fp)){
    if(cod_tabla < 0)
   	if(strcmp(tupla_catalogo.dato,nombre_tabla)==0)
      	cod_tabla=tupla_catalogo.identificador_unico;
    if(cod_tabla > 0){
   	strcpy(aux,tupla_catalogo.dato);
   	quitar_tipo_dato_palabra(aux);
   	if(strcmp(aux,col)==0 && tupla_catalogo.identificador_unico == cod_tabla){
      	fclose(fp);
      	return true;
   	}
    }
	}
	fclose(fp);
	return false;
}

bool parseador(int argc, char ** argv){
    FILE *fp1=fopen("catalog.dat","a");fclose(fp1);// Además de crear catalog.dat y users.dat si no existian, verifica
   	 FILE *fp2=fopen("users.dat","a");fclose(fp2);//la sintaxis de la llamada de función.

    if(strcmp(argv [1],"into")!=0){
    printf("Excepcion en %s.\n", argv[1]);
    printf("Error de sintaxis, la palabra correcta es: into. \n");
    return false;
    }

    if(encontrar_Values(argc,argv)==false){
   	 printf("Error de sintaxis.\n");
   	 printf("No se encontro la palabra reservada values.\n");
   	 return false;
    }

    if (existe_Tabla (argv)==false){
   	 printf("Exepcion en %s.\n", argv[2]);
   	 printf("Error: no se encontro la tabla ingresada.\n");
   	 return false;
    }

    if (numeroArgumentos(argc)==false){
   	 printf("Excepcion en %s.\n", argv[3]);
   	 printf("Error: el numero de columnas no corresponde con el numero de valores ingresados o viceversa.\n");
   	 return false;
    }
    int i = 3;
    while(strcmp(argv[i],"values")!=0){
   	if (existe_Columna(argv[2],argv[i])==false){
      	printf("\nError: no se encontro la columna %s en la tabla %s\n",argv[i],argv[2]);
      	return false;
   	}
   	i++;
    }
    return true;
}

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------MAIN--------------------------------------------------------------------------------------
int main(int argc, char ** argv){
  	 if(!parseador(argc,argv)){// Primero se parsean los elementos.
   	printf("=== INSERCION DE ELEMENTO RECHAZADA ===\n");
   	return 0;
    }
    char* aux = (char*) malloc(sizeof(char*));
    char *numero_tabla = (char*) malloc(sizeof(char*));
    int numero_columna = 1, i = 0;
    int num_tupla_dato = cantidad_bloques_archivo();
    FILE *fp = fopen("users.dat","a");
    nodo* primero_columnas_tabla = lista_columnas_tabla(argv[2]); // Lista de columnas de la tabla a insertar
    strcpy(numero_tabla,primero_columnas_tabla->dato);
    primero_columnas_tabla = primero_columnas_tabla->sig;
    nodo* primero_tipos_datos = lista_tipos_de_datos(primero_columnas_tabla);
    nodo* primero_orden_args = lista_orden_argumentos(argv); // Orden en que van a ingresar los argumentos
    nodo* primero_valores_args = lista_valores_argumentos(argv); // Valores de los argumentos a ingresar
    nodo* primero_valores_a_insertar_ordenados = lista_valores_argumetos_ordenada(primero_columnas_tabla,primero_orden_args,primero_valores_args);
    if(!validar_tipos_datos(primero_tipos_datos,primero_valores_a_insertar_ordenados)){
    	printf("=== INSERCION DE ELEMENTO RECHAZADA ===\n");
    	return 0;
    }

    if(num_tupla_dato == 1){  // Si no hay ningún tupla_dato en el archivo, guarda el tupla_dato número 0
   	 tupla_dato.tabla_perteneciente = 0;
   	 tupla_dato.numero_columna = 0;
   	 tupla_dato.donde_empieza = 0;
   	 tupla_dato.numero_bloque = 0;
   	 tupla_dato.siguiente_bloque = 1;
   	 strcpy(tupla_dato.dato,"Bloque cero");
   	 fwrite(&tupla_dato,sizeof(tupla_dato),1,fp);
    }
    while(primero_valores_a_insertar_ordenados){
   	 aux = primero_valores_a_insertar_ordenados->dato;
    // ---------------------GUARDAR---------------------
   	 tupla_dato.tabla_perteneciente = atoi(numero_tabla);
   	 tupla_dato.numero_columna = numero_columna;
   	 tupla_dato.donde_empieza = num_tupla_dato;
   	 tupla_dato.numero_bloque = num_tupla_dato + i;
   	 tupla_dato.siguiente_bloque = num_tupla_dato + i + 1;
   	 strcpy(tupla_dato.dato,aux);
   	 fwrite(&tupla_dato,sizeof(tupla_dato),1,fp);
   	 primero_valores_a_insertar_ordenados = primero_valores_a_insertar_ordenados->sig;
   	 i++; numero_columna++;
    }
    fclose(fp);
    printf("=== ELEMENTO INSERTADO CORRECTAMENTE ===\n");
    return 0;
}

