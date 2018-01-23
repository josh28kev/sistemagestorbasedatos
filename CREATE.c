#include <stdio.h>
#include <string.h>
typedef int bool; //Definicion del tipo booleano en C.
#define true 1	//
#define false 0   //

// ---------------------------------ESTRUCTURAS------------------------------------------------------
struct tupla_catalogo {int identificador_unico;
    int numero_columna; char dato[256];} tupla_catalogo;
// ---------------------------------------------------------------------------------------------------
// ------------------------------------------------MÉTODOS--------------------------------------------
int cantidad_tablas(){  // Devuelve la cantidad de tablas presentes en el archivo catalog.dat
   int suma=0;
   FILE *fp=fopen("catalog.dat","r");
   while(fread(&tupla_catalogo,sizeof(tupla_catalogo),1,fp)){
    if(tupla_catalogo.identificador_unico != suma)
    	suma++;
   }
   fclose(fp);
   return suma;
}
//--------------------------VALIDACION-------------------------
int tamano_de_palabra(char* argv) { // Devuelve la cantidad de caracteres en un vector de char
  int i = 0;
  while(argv[i] != 0){ // Los argv[i] son los numeros de los caracteres
	i++;// El caracter \0 es el caracter nulo, o sea, el fin de la palabra.
  }
  return i;
}
//----------------------------------------------
bool validacion_de_columnas(char * argv) { // Valida que un argumento corresponda al formato
  int i = 0;                           	// <tipo_dato>-<nombre_atributo>
  int tam = tamano_de_palabra(argv);
  while (argv[i] != (int)'-' && i < tam)
	i++;
  if(i == 0) {
   	 printf("\n--> Excepcion en \"%s\"\n", argv);
	printf("ERROR: Debe indicar un nombre de parametro.\n");
	return false;
  }
  if(i == tam) {
   	 printf("\n--> Excepcion en \"%s\"\n", argv);
	printf("ERROR: No se encuentra separación '-' entre nombre y tipo.\n");
	return false;
  }
  if(i+1 == tam) {
   	 printf("\n--> Excepcion en \"%s\"\n", argv);
	printf("ERROR: El parametro %s no posee un identificador.\n", argv);
	return false;
  }
  char *a,*b;
  char aux[tam];
  strcpy(aux, argv); // El metodo strtok destruye el char*.
  a = (char*)strtok(aux,"-");//Recupera el primer elemento de argv segun '-'.
  b = (char*)strtok(NULL,"-");//Recupera el segundo elemento de argv segun '-'.
  if(validar_tipo(a) == 0) return false;
  return true;
}
//----------------------------------------------
bool validar_tipo(char * tipo, char * identificador) { // Valida que el tipo de dato digitado corresponda
                                                   	// a uno de los tres permitidos: int, float o text
  if(strcmp("text", tipo) == 0) {
  	return true;
  }

  if(strcmp("int", tipo) == 0){
  	return true;
  }

  if(strcmp("float", tipo) == 0) {
  	return true;
  }
    printf("\n--> Excepcion en \"%s-%s\"\n", tipo, identificador);
  printf("ERROR: El tipo %s no es un tipo valido.\n", tipo);
  return false;
}
//----------------------------------------------
bool administrador_validacion(int argc, char ** argv) { // Método principal para la validación
  if(argc<3) {
	printf("\nERROR: No se encontraron argumentos suficientes para");
	printf(" realizar la operacion.\n");
   	 printf("El formato requerido es:\n create nombre_de_tabla");
   	 printf(" tipo_de_columna_1-identificador_de_columna_1 ...\n");
	return false; // 3 argumentos: metodo, tabla y primera columna.
  }
  // El nombre de la tabla necesitara validacion cuando necesite revisar
  // si ese nombre de tabla ya existe
  int i;
  for(i = 2; i < argc; i++) {
	if(validacion_de_columnas(argv[i]) == 0) return false;
  }
  return true;
}
//--------------------------------------------------------------//
bool validacion(int argc, char ** argv)                  	// Invoca al método principal para la validación y muestra un mensaje
{                                                        	// en caso de que la peticion de creacion de tabla sea rechazada
   if(administrador_validacion(argc,argv) == 1) return true; // por la falta de algun dato requerido
   printf("=== CREACION DE TABLA RECHAZADA ===\n");
   return false;
}
//--------------------------------------------------------------//
//--------------------------MAIN--------------------------------//
int main(int argc, char ** argv){

   if(validacion(argc,argv) == 0) // En caso de que la validación detecte algún problema, termina el programa.
  	return 0;

   FILE *fp=fopen("catalog.dat","a"); // Abre catalog.dat para su escritura.

   int i, num_tab = cantidad_tablas()+1;

   for (i=1;i<argc;i++){	// Ciclo que graba en disco cada uno de los bloques necesarios para la tabla
  	tupla_catalogo.identificador_unico = num_tab;
  	tupla_catalogo.numero_columna = i-1;
  	strcpy(tupla_catalogo.dato,argv[i]);
  	fwrite(&tupla_catalogo,sizeof(tupla_catalogo),1,fp);
   }

   fclose(fp); // Cierra el flujo de datos al archivo

   printf("\n=== TABLA CREADA ===\n"); // Imprime un mensaje de éxito

   return 0; // Termina el programa
}
//--------------------------------------------------------------//
