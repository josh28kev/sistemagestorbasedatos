#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
// ---------------------------------ESTRUCTURAS-------------------------------------------------------
struct bloque_objeto {
    int tabla_perteneciente;
    int numero_columna;
    int donde_empieza;
    int numero_bloque;
    int siguiente_bloque;
    char dato[256];
} bloque;

struct bloque_tabla {
    int identificador_unico;
    int numero_columna;
    char dato[256];
} bloque_tabla;

typedef struct nodo_lista_columnas_tabla {
    struct nodo_lista_columnas_tabla *sig; char dato[256];
} nodo;
// ---------------------------------------------------------------------------------------------------
// ------------------------------------------------MÉTODOS--------------------------------------------
int busqueda_diccionario_datos(char * tabla) { // Existencia de la tabla buscada.
    FILE *fr=fopen("catalog.dat","a+");
    while(fread(&bloque_tabla, sizeof(bloque_tabla), 1, fr)) {
   	 if(bloque_tabla.numero_columna == 0 && strcmp(bloque_tabla.dato, tabla)==0) {
   		 fclose(fr);
   		 return bloque_tabla.identificador_unico;// Retorna el id de la tabla.
   	 }
    }
    fclose(fr);
    return -1;
}

int tamano_de_palabra(char * argv) {
  int i = 0;
  while(argv[i] != 0){ // Los argv[i] son los numeros de los caracteres
	i++;// El caracter \0 es el caracter nulo, o sea, el fin de la palabra.
  }
  return i;
}

bool revision_from(char * from) { // Permite que el from se digite en mayusculas
    char aux[5];//                 	o minusculas
    int i = 0;
    if(strlen(from) != 4) {
   	 return false;
    }
    while(i < 4) {
   	 aux[i] = from[i];
   	 aux[i] = (tolower(aux[i]));
   	 i++;
    }
    aux[4] = 0;
    if (strcmp(aux, "from")==0) return true;
    return false;
}

int posicion_from(int argc, char ** argv) { // Ubica la posicion del from en la
    int posicionF = 2; bool flag1 = false;// 	llamada de la funcion.
    while((posicionF < argc) && (flag1 == false)) {
   	 if(revision_from(argv[posicionF])==true){
   		 flag1 = true;
   	 }
   	 else posicionF++;
    }
    if(flag1 == false) {
   	 printf("ERROR: No se encontro indicador de ubicacion (FROM).\n");
   	 return 0;
    }
    return posicionF;
}

bool columnas_repetidas(char ** argv, int from) {// Busca si hay columnas que se
    int posicion = 1;// repiten en la llamada de funcion. Esto se hace para que no
    int posicion2 = 2;//se produzcan errores de impresion al final.
    while(posicion < from-1) {
   	 while(posicion2 < from) {
   		 if(strcmp(argv[posicion], argv[posicion2]) == 0) {
   			 printf("ERROR: Se repite la columna %s en la llamada a la funcion.\n",argv[posicion]);
   			 return false;
   		 }
   		 posicion2++;
   	 }
   	 posicion++;
   	 posicion2 = posicion + 1;
    }
    return true;
}

char* columna_sin_tipo(char* argv) {// Devuelve el nombre de alguna columna sin su tipo.
    char *a,*b;
    char aux[tamano_de_palabra(argv)];
  strcpy(aux, argv); // El metodo strtok destruye el char*.
  a = (char*)strtok(aux,"-");//Recupera el primer elemento de argv segun '-'.
  b = (char*)strtok(NULL,"\0");//Recupera el segundo elemento de argv segun '\0'.
    return b;
}

int busqueda_de_columna(char * argv, int idTabla) { // Retorna el numero de una columna
    FILE *fb=fopen("catalog.dat","a+");//            	segun la palabra buscada.
    while(fread(&bloque_tabla, sizeof(bloque_tabla), 1, fb)) {
   	 if(bloque_tabla.identificador_unico == idTabla) {
   		 if(bloque_tabla.numero_columna != 0) {
   			 if(strcmp(columna_sin_tipo(bloque_tabla.dato), argv)==0) {
   				 fclose(fb);
   				 return bloque_tabla.numero_columna;
   			 }
   		 }
   	 }
    }
    fclose(fb);
    return false;
}

char * nombre_columna(int id, int numero_columna, FILE * fr) {// Retorna el nombre de columna y
    while(fread(&bloque_tabla, sizeof(bloque_tabla), 1, fr)) {// su tipo, según el numero_columna.
   	 if(bloque_tabla.identificador_unico == id && bloque_tabla.numero_columna == numero_columna) {
   		 return bloque_tabla.dato;
   	 }
    }
    return "-desconocido";
}

void impresion_de_linea(int cantidad) { // Imprime líneas divisorias simples.
    while(cantidad > 0) {
   	 printf("----------------|-");
   	 cantidad--;
    }
    printf("\n");
}

int cantidad_elementos_tabla(int identificador, char * argv, int argc) {
    int contador = 0;
    if((strcmp(argv,"*") == 0)&&(argc == 4)) {
   	 FILE *fr = fopen("catalog.dat","a+");
   	 while(fread(&bloque_tabla, sizeof(bloque_tabla), 1, fr)) {
   		 if(bloque_tabla.identificador_unico == identificador && bloque_tabla.numero_columna != 0) {
   			 contador++;
   		 }
   	 }
   	 fclose(fr);
   	 return contador;
    }
    else {
   	 return argc-3;
    }
}

int error_argc() {
    printf("ERROR: No se encuentra la cantidad minima de elementos para realizar la operacion.\n");
    printf("La sintaxis minima es: select <nombre_columna_1> <nombre_columna_2>\n");
    printf("... from <nombre_tabla>.\n");
    return 0;
}

int error_no_tabla() {
    printf("ERROR: No se indico la tabla de busqueda.\n");
    return 0;
}

int error_tabla_no_encontrada(char * tabla) {
    printf("ERROR: No se encontro la tabla %s.\n", tabla);
    return 0;
}

bool limpiar_memoria(nodo* primero, nodo* actual) {
    actual = primero;
    while(primero != NULL) {
   	 primero = primero->sig;
   	 free(actual);
   	 actual = primero;
    }
    return true;
}
// ------------------------------------------------MAIN----------------------------------------------
int main(int argc, char ** argv) {
    printf("\n");
    // ------------------  Minimo de campos requeridos  -----------------
    if(argc < 4) return error_argc();
    // --------------------  	Indicador FROM  	--------------------
    int posicionF = posicion_from(argc,argv); //////Variable importante 1: Posición del from.
    if(posicionF == 0) return 0;
    // -------------------   Tabla despues del from   -------------------
    if(posicionF+1 != argc-1) return error_no_tabla();
    // ------------------  Existencia de tabla buscada  -----------------
    int id = busqueda_diccionario_datos(argv[posicionF+1]); //////Variable importante 2:
    if(id == -1) return error_tabla_no_encontrada(argv[posicionF+1]);//identificador_unico.
    // ---------------------  Columnas no repetidas  --------------------
    if(columnas_repetidas(argv,posicionF) == false) return 0;
    // -----------------  Existencia de columna buscada  ----------------
    int busqueda = 1;
    int cantidad_elementos = cantidad_elementos_tabla(id,argv[1],argc);
    int vectorNumeros[cantidad_elementos]; //Variable importante 3, columnas a revisar.
    int numeroTemporal;
    if((strcmp(argv[1],"*") == 0)&&(argc == 4)) {
   	 while(busqueda <= cantidad_elementos){
   		 vectorNumeros[busqueda-1] = busqueda;
   		 busqueda++;
   	 }
    }
    else {
   	 while(busqueda < posicionF) {
   		 numeroTemporal = busqueda_de_columna(argv[busqueda], id);
   		 if(numeroTemporal == 0) {
   			 printf("ERROR: No se encontro la columna %s en la tabla %s.\n",
   			 argv[busqueda], argv[posicionF+1]);
   			 return 0;
   		 }
   		 vectorNumeros[busqueda-1] = numeroTemporal;
   		 busqueda++;
   	 }
    }
    // ---------------  Ordenamiento del vector ---------------
    int bi,bj,baux; // El vector debe ordenarse debido a que en los datos siempre
    for(bi=0; bi < cantidad_elementos-1; bi++) { // se guardan los atributos en orden 1-2-3-...
   	 for(bj=0; bj < cantidad_elementos-1; bj++) {// Al buscar los datos siempre estarán en
   		 if(vectorNumeros[bj]>vectorNumeros[bj+1]) {// orden secuencial.
   			 baux = vectorNumeros[bj];
   			 vectorNumeros[bj] = vectorNumeros[bj+1];
   			 vectorNumeros[bj+1] = baux;
   		 }
   	 }
    }
    // ---------------  Creacion de lista enlazada  --------------
    FILE * lectura = fopen("users.dat", "a+");
    busqueda = 0;
    nodo *primero;
    nodo *actual;
    primero = (struct nodo_lista_columnas_tabla*)malloc(sizeof(nodo));
    actual = (struct nodo_lista_columnas_tabla*)malloc(sizeof(nodo));
    bool flag1 = false; // Indicador de que ya se establecio el primer nodo.
    bool flag2;
    while(fread(&bloque, sizeof(bloque), 1, lectura)) {
   	 flag2 = false; // Indicador de que ya se encontró la columna del elemento.
   	 if(bloque.tabla_perteneciente == id) {// Como los valores en el delete quedan en -1,
   		 while((busqueda < cantidad_elementos)&&(flag2 == false)) {//no se leerán los espacios
   			 if(bloque.numero_columna == vectorNumeros[busqueda]) {//  borrados.
   				 if(flag1 == false) {
   					 strcpy(primero->dato, bloque.dato);
   					 primero->sig = NULL;
   					 actual = primero;
   					 flag1 = true;
   				 }
   				 else {
   					 nodo* aux = (struct nodo_lista_columnas_tabla*)malloc(sizeof(nodo));
   					 strcpy(aux->dato, bloque.dato);
   					 actual->sig = aux;
   					 aux->sig = NULL;
   					 actual = aux;
   				 }
   				 flag2 = true;
   			 }
   			 else {
   				 busqueda++;
   			 }
   		 }
   		 busqueda = 0;
   	 }
    }
    fclose(lectura);
    // ---------------  Impresion de lista enlazada ---------------
    actual = primero;
    busqueda = 0;
    FILE * fi = fopen("catalog.dat","a+");
    while(busqueda < cantidad_elementos) {// Nombres de columnas
   	 printf("%15s | ", nombre_columna(id,vectorNumeros[busqueda],fi));
   	 busqueda++;
    }
    fclose(fi);
    printf("\n");
    impresion_de_linea(cantidad_elementos);// Linea divisoria
    busqueda = 1;
    while(actual != NULL) {
   	 printf("%15s | ", actual->dato);// Impresion de datos
   	 actual = actual->sig;
   	 if(busqueda == cantidad_elementos) {
   		 busqueda = 1;
   		 printf("\n");
   	 }
   	 else {
   		 busqueda++;
   	 }
    }
    // ---------------       	Final        	---------------
    limpiar_memoria(primero, actual);
    return 0;
}
