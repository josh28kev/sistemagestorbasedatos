#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// ---------------------------------ESTRUCTURAS------------------------------------------------------
struct tupla_dato {int tabla_perteneciente; int numero_columna; int donde_empieza;
   		   int numero_bloque; int siguiente_bloque; char dato[256];} tupla_dato;

struct tupla_catalogo {int identificador_unico;
    int numero_columna; char dato[256];} tupla_catalogo;

typedef struct coordenadas_col_a_modificar {int tabla; int col;} coordenadas;
// ---------------------------------------------------------------------------------------------------
// ------------------------------------------------MÉTODOS--------------------------------------------
int quitar_tipo_dato_palabra(char* palabra){ // Convierte por ejemplo int-cedula en cedula
   int i=0;
   for(i; palabra[i] && palabra[i] != '-'; i++){}
   if(palabra[i])
    strncpy(palabra, palabra+i+1 , strlen(palabra)-1);
   return 0;
}

int obtener_tipo_dato(char* palabra,char* tipo_dato){ // Recupera por ejemplo el int de int-cedula
  char aux[strlen(palabra)];
  strcpy(aux, palabra);
  strcpy(tipo_dato,(char*)strtok(aux,"-"));
  return 0;
}

coordenadas* coordenadas_columna_a_modificar(char* nombre_tabla, char* nombre_col, char* tipo_dato){ // Obtiene y guarda en una estructura de tipo coordenadas
   FILE *fp=fopen("catalog.dat","a+"); bool salir = false;                                       	// el número de tabla y el número de columna de los
   int num_tab = -1, num_col = -1; char* aux = (char*) malloc(sizeof(char*));                    	// bloques que coinciden con el atributo que el
   while(fread(&tupla_catalogo,sizeof(tupla_catalogo),1,fp) && !salir){                          	// usuario desea modificar
  	 if(strcmp(tupla_catalogo.dato,nombre_tabla)==0)
    	num_tab = tupla_catalogo.identificador_unico;
    else {
     	if(num_tab != -1){
   	 strcpy(aux,tupla_catalogo.dato);
   	 obtener_tipo_dato(aux,tipo_dato);
        	quitar_tipo_dato_palabra(aux);
      	 if(strcmp(aux,nombre_col) == 0 && tupla_catalogo.identificador_unico == num_tab){
           	num_col = tupla_catalogo.numero_columna;
           	salir = true;
   	 }
    	}
    	}
   }
   fclose(fp);
   coordenadas* coord = (struct coordenadas_col_a_modificar*) malloc(sizeof(coordenadas));
   coord->tabla = num_tab;
   coord->col = num_col;
   return coord;
}

bool modificar_bloques(coordenadas* coord, char* dato_viejo, char* dato_nuevo){ // Obtiene el numero de tabla y de columna de la estructura de tipo coordenadas
	FILE *fp2=fopen("users.dat","a");fclose(fp2);                           	// y busca en todos los bloques que coinciden con estos datos, el dato que el
	FILE *fp=fopen("users.dat","r+");bool modifico = false;                 	// usuario desea modificar; en caso de coincidir, reescribe el bloque con el
	while(fread(&tupla_dato,sizeof(tupla_dato),1,fp)){                      	// nuevo dato que el usuario digitó.
    if(tupla_dato.tabla_perteneciente == coord->tabla && tupla_dato.numero_columna == coord->col && strcmp(tupla_dato.dato,dato_viejo) == 0){
   	strcpy(tupla_dato.dato,dato_nuevo);
   	fseek(fp, tupla_dato.numero_bloque*sizeof(struct tupla_dato), SEEK_SET);
   	fwrite(&tupla_dato,sizeof(tupla_dato),1,fp);
 			modifico = true;
    }
	}
	fclose (fp);
	return modifico;
}
// --------------------------------------------------------------------------------------------
// ------------------------------------------VALIDACIÓN----------------------------------------------
int busqueda_de_tabla(char* tabla) {  // Revisa que la tabla a insertar exista en el diccionario de datos
   FILE *fp=fopen("catalog.dat","a+");
   while(fread(&tupla_catalogo, sizeof(tupla_catalogo), 1, fp))
  	if(strcmp(tupla_catalogo.dato, tabla) == 0){
    fclose(fp);
    	return tupla_catalogo.identificador_unico;
  	}
   fclose(fp);
   return -1;
}

int busqueda_de_columna(int numero_tabla, char* columna) { // Revisa que la columna a insertar exista en la tabla especificada
   FILE *fb=fopen("catalog.dat","a+");
   char* aux = (char*) malloc(sizeof(char*));
   while(fread(&tupla_catalogo, sizeof(tupla_catalogo), 1, fb)) {
  	if(tupla_catalogo.identificador_unico == numero_tabla) {
     	if(tupla_catalogo.numero_columna != 0) {
    	strcpy(aux,tupla_catalogo.dato);quitar_tipo_dato_palabra(aux);
    	if(strcmp(aux,columna) == 0) {
   	 fclose(fb);
   	 return tupla_catalogo.numero_columna;
        	}
     }
  	}
   }
   fclose(fb);
   return -1;
}

bool validar_int(char* texto){  // Valida que una cadena de texto sólo posea números
   int i = 0;
   for(i; texto[i]; i++){
    if(texto[i]!='0'&&texto[i]!='1'&&texto[i]!='2'&&texto[i]!='3'&&texto[i]!='4'&&
   	texto[i]!='5'&&texto[i]!='6'&&texto[i]!='7'&&texto[i]!='8'&&texto[i]!='9')
   	return false;
   }
   return true;
}

bool validar_float(char* texto){ // Valida que una cadena de texto posea sólo números y a lo sumo un punto
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
   a = (char*)strtok(aux,".");//Recupera el primer elemento de argv segun '.'
   b = (char*)strtok(NULL,".");//Recupera el segundo elemento de argv segun '.'
   if(!validar_int(a) || !validar_int(b))
    return false;
   return true;
}

bool validar_tipo_dato(char* palabra, char* tipo_dato, char* atributo){  // Método que gestiona la validación del tipo de dato a ingresar
   if(strcmp(tipo_dato,"int")==0)
   	if(!validar_int(palabra)){
       	printf("\nERROR: El atributo %s solo puede contener numeros enteros\n",atributo);
   	return false;
   	}
   if(strcmp(tipo_dato,"float")==0)
   	if(!validar_float(palabra)){
   	printf("\nERROR: El atributo %s solo puede contener numeros\n",atributo);
   	return false;
   	}
   return true;
}

bool validar(int argc, char** argv){  // Método que se encarga de validar los argumentos

    	// ---------------  Mínimo de campos requeridos  ---------------
    if(argc < 8) {
   	printf("\nERROR: No hay sufcientes argumentos para la funcion.\n");
   	printf("La sintaxis requerida es:\nmodify from <tabla> <dato nuevo> where <nombre columna> = <dato viejo>\n");
   	return false;
    }
    // ---------------  	Indicador from  	---------------
    	if(strcmp(argv[1],"from") != 0){
   	printf("\nERROR: Indicador from no encontrado en la posicion esperada.\n");
   	printf("La sintaxis requerida es:\nmodify from <tabla> <dato nuevo> where <nombre columna> = <dato viejo>\n");
   	return false;
    }
    // ---------------  	Indicador where  	---------------
    	if(strcmp(argv[4],"where") != 0){
   	printf("\nERROR: Indicador where no encontrado en la posicion esperada.\n");
   	printf("La sintaxis requerida es:\nmodify from <tabla> <dato nuevo> where <nombre columna> = <dato viejo>\n");
   	return false;
    }
    // ---------------  Existencia de tabla buscada  ---------------
    int id = busqueda_de_tabla(argv[2]);
    if(id < 0){
   	printf("\nERROR: La tabla %s no existe.\n",argv[2]);
   	return false;
    }
    // ---------------  Existencia de columna buscada  ---------------
    int id2 = busqueda_de_columna(id, argv[5]);
    if(id2 < 0){
   	printf("\nERROR: La columna %s no existe en la tabla %s.\n",argv[5],argv[2]);
   	return false;
    }
    return true;
}
// --------------------------------------------------------------------------------------------------
// ------------------------------------------MAIN----------------------------------------------
int main(int argc, char ** argv){

   if(!validar(argc,argv)){ // En caso de que la validación detecte algún problema, termina el programa.
  	printf("=== MODIFICACION DE ELEMENTO RECHAZADA ===\n");
  	return 0;
   }

   char* tipo_dato = (char*) malloc(sizeof(char*));
   coordenadas* coord = coordenadas_columna_a_modificar(argv[2],argv[5],tipo_dato); // Obtiene las "coordenadas" de los bloques a modificar

   if(!validar_tipo_dato(argv[3],tipo_dato,argv[5])) // Valida el tipo de dato del nuevo atributo digitado por el usuario
  	return 0;

   if(!modificar_bloques(coord,argv[7],argv[3])){  // Invoca al método que modifica los bloques
  	printf("\nADVERTENCIA: No se ha encontrado un objeto "); // Muestra una advertencia si no se encontró nungún bloque que cumpla la condición para ser modificado
  	printf("con el(la) %s igual a %s\n",argv[5],argv[7]);
   }

   else
  	printf("\n=== MODIFICACIÓN REALIZADA CORRECTAMENTE ===\n"); // Mensaje de éxito

   return 0; // Termina el programa
}
// -------------------------------------------------------------------------------------------

