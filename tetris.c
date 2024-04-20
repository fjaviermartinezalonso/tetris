/*PROYECTO PROGRAMACIÓN I 2013 - TETRIS
GRUPO 7 DE PRÁCTICAS
ALUMNOS:
	FCO. JAVIER MARTÍNEZ ALONSO
	MIGUEL ÁNGEL MOJÓN ARTEAGA
*/

//LIBRERIAS USADAS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Estructura usada durante el juego con los datos actuales.
typedef struct {
	int ficha; //"Letra" de la ficha, L, O, I...
	char simbolo; //Caracter que forma la pieza, #, %, o ...
	int rotacion; //Índice de la rotación
	int j; //Coordenada Y
	int i; //Coordenada X
	int lineas; //Número de lineas completadas.
	long puntuacion;
} estado;

typedef struct srank {
	long puntuacion;
	int lineas;
	struct srank *sig;
} rank;

//Inicio Prototipos de las funciones definidas
	//Funciones que manejan el tablero
		char **crearTablero(int alto, int ancho);
		void nuevoTablero(char **tablero, int alto, int ancho);
		void pintarTablero(char **tablero, int alto, int ancho);
		void liberarTablero(char **tablero, int alto);
	//Funciones del menú "Principal" 
		int menuPrincipal(void);
		void nuevoJuego(estado *estadoJuego, int pieza[2][4], char **tablero, int alto, int ancho);
		int cargarJuego(estado *estadoJuego, char **tablero, int alto, int ancho);
		int guardarJuego(estado *estadoJuego, char **tablero, int alto, int ancho);
		void salirJuego(char **tablero, int alto, int ancho, rank *ranking);
	//Funciones del menú de juego
		int menuJuego(estado *estadoJuego);
		int moverPieza(int pieza[2][4], estado *estadoJuego, char **tablero, int dy, int dx, int rotar);
	//Otras funciones
		int nuevaPieza(int pieza[2][4], estado *estadoJuego, char **tablero);
		void comprobarLineas(int pieza[2][4], estado *estadoJuego, char **tablero, int ancho, int alto);
		void mostrarRanking(rank *ranking);
		rank *cargarRanking(void);
		rank *comprobarRanking(rank *ranking, estado *estadoJuego);
		void guardarRanking(rank *ranking);
		void liberarRanking(rank *ranking);
//Fin Prototipos

/*main
Contiene el bucle principal de juego, llama a los menús y actua en consecuencia*/
int main (void) {
	int alto = 21, ancho = 12; //Variables con el alto y el ancho
	int pieza[2][4]; //Matriz de enteros con el tetramino actual
	char **tablero; //Puntero al tablero
	int opcion, flagMenu = 1, valido = 1, fin = 0, cargaFichero = 1; //Variables de opcion y flags
	estado estadoJuego = {0, 0, 0, 0, 4, 0, 0}; //Variable con el estado de juego
	rank *ranking = NULL;
	char comando[31]; //Cadena para el comando de redimensionar

	//Reservamos memoria para el tablero y lo inicializamos (Ponerlo blanco con los bordes)
	tablero = crearTablero(alto, ancho);
	nuevoTablero(tablero, alto, ancho);
	//Establecemos una semilla para rand()
	srand(time(NULL));
	ranking = cargarRanking();
	//MEJORA: Redimensionar la ventana mediante comando del sistema
	sprintf(comando, "resize -s %d %d", 2*alto , 4*ancho);
	system(comando);
	system("echo \"\\033[?25l\"");
	//Bucle principal - inicio
	do {	
		pintarTablero(tablero, alto, ancho); //Imprimimos el tablero
		//if de menús, si flagMenu == 1, se muestra el menu princiapl, si no, el de juego
		if(flagMenu) {
			opcion = menuPrincipal(); //Imprimimos el menú y obtenemos una opción valida
			//switch con la opcion
			switch(opcion) {
				case '1': //Juego nuevo
					nuevoJuego(&estadoJuego, pieza, tablero, alto, ancho);
					flagMenu = 0; //Se debe pasar al menu de juego
					break;
				case '2': //Cargar juego
					cargaFichero = cargarJuego(&estadoJuego, tablero, alto, ancho);
					//Si se cargo correctamente o no
					if(cargaFichero) {
						printf("PARTIDA CARGADA\n");
						nuevaPieza(pieza, &estadoJuego, tablero);
						flagMenu = 0; //Se debe pasar al menu de juego
					} else {
						printf("\tError cargando fichero.");
					}
					valido = 1;
					break;
				case '3': //Guardar juego
					guardarJuego(&estadoJuego, tablero, alto, ancho);
					break;
				case '4':
					pintarTablero(tablero, alto, ancho);
					mostrarRanking(ranking);
					break;
				case '5': //Salir juego
					salirJuego(tablero, alto, ancho, ranking);
					break;
				default:
					break;
			}
		} else {
			//Si el movimiento no ha sido valido, por colisión al bajar


			opcion = menuJuego(&estadoJuego); //Pintamos el menú de juego y obtenemos una opción valida
			switch(opcion) {
				case '0': //caso 22... Abajo del todo
					//Movemos hacia abajo mientras sea valido
					do {
						valido = moverPieza(pieza, &estadoJuego, tablero, 1, 0, 0);
					} while(valido);
					break;
				case '2': //Abajo
					valido = moverPieza(pieza, &estadoJuego, tablero, 1, 0, 0);
					break;
				case '4': //Izquierda
					moverPieza(pieza, &estadoJuego, tablero, 0, -1, 0);
					break;
				case '6': //Derecha
					moverPieza(pieza, &estadoJuego, tablero, 0, 1, 0);
					break;
				case '8': //Rotar
					valido = moverPieza(pieza, &estadoJuego, tablero, 0, 0, 1); //Quita la pieza y comprueba si se puede rotar
					//Si se puede rotar
					if(valido) {
						estadoJuego.rotacion++; //Pasamos a la siguiente rotación
						nuevaPieza(pieza, &estadoJuego, tablero); //Creamos la misma pieza rotada
					} else {
						valido = moverPieza(pieza, &estadoJuego, tablero, 0, 0, 0); //Volvemos a pintar la misma pieza
					}
					break;
				case 'm': case 'M': //Volver al menú principal
					flagMenu = -1; //Activamos el flag
					break;
				default:
					break;
			}
			if(valido == 0) {
				valido = 1; //Desactivamos el flag
				comprobarLineas(pieza, &estadoJuego, tablero, ancho, alto); //Comprobamos si hay que eliminar fichas
				estadoJuego.ficha = rand() % 7; //Obtenemos una nueva pieza
				estadoJuego.i=4; //Inicializamos coordenada x
				estadoJuego.j=0; //Inicializamos coordenada y
				estadoJuego.rotacion=0; //Inicializamos rotación
				fin = nuevaPieza(pieza, &estadoJuego, tablero); //Creamos una nueva pieza
				//Si no se pudo colocar (no había espacio)
				if(fin) {
					printf("\n\t\tHAS PERDIDO\n"); //Mostramos un mensaje
					system("sleep 1");
					flagMenu = 1; //Se debe pasar al menu de juego
					fin = 0; //Desactivamos el flag
					ranking = comprobarRanking(ranking, &estadoJuego);
					continue; //Pasamos a la siguiente iteración
					
				}		
				pintarTablero(tablero, alto, ancho); //Pintamos el tablero con la nueva pieza
			}
		}
	} while (!flagMenu || opcion != '5'); //Bucle principal - fin, mientras no sea el menu princiapl y no sea opcion 4

	system("echo \"\\033[?25h\"");
	return 1;
}

/*CREAR TABLERO
Reserva memoria para el tablero.
PARÁMETROS: Alto y ancho del tablero.
RETURN: Retorna el puntero al tablero.*/
char **crearTablero(int alto, int ancho) {
	char **pTablero = NULL; //Puntero inicializado a NULL
	int j; //Coordenada Y

	pTablero = (char**)malloc(alto * sizeof(pTablero)); //Reservamos un puntero para cada fila
	//Reservamos un ancho punteros a char para cada fila
	for(j=0; j<alto; j++) 
		pTablero[j] = (char *)malloc(ancho * sizeof(pTablero[j]));

	return pTablero; //Retornamos el puntero
}

/*NUEVO TABLERO
Inicializa el tablero con los bordes pintados y el resto blanco.
PARÁMETROS: El puntero al tablero, alto y ancho del mismo.
RETORNO: Void*/
void nuevoTablero(char **tablero, int alto, int ancho) {
	int j, i;

	for(j=0; j<alto; j++) //Bucle fila a fila
		for(i=0; i<ancho; i++) //Bucle anidado columna a columna
			tablero[j][i] = ' '; //Ponemos un blanco
	for(j=0; j<alto; j++) { //Bucle fila a fila
		tablero[j][0] = '|'; //Primera columna es un borde
		tablero[j][ancho-1] = '|'; //Última columna es un borde
	}
	for(i=0; i<ancho; i++) //Bucle columna a columna
		tablero[alto-1][i] = '='; //Última fila es un borde
}

/*PINTAR TABLERO
Imprime el tablero por pantalla.
PARÁMETROS: El puntero al tablero, alto y ancho del mismo.
RETORNO: Void*/
void pintarTablero(char **tablero, int alto, int ancho) { 
	int j, i;

	system("clear"); //Limpiamos la pantalla
	printf("\n\n"); //Damos un poco de espacio
	for(j=0; j<alto; j++) { //Bucle fila a fila
		printf("\t"); //Tabulamos
		for(i=0; i<ancho; i++) //Bucle
			printf("%2c", tablero[j][i]); //Imprimimos el caracter, en 2 espacios para que no quede muy junto
		printf("\n"); //Pasamos a la siguiente linea
	}
}

/*LIBERAR TABLERO
Libera la memoria reservada por crearTablero.
PARÁMETROS: El puntero al tablero, alto y ancho del mismo.
RETORNO: Void*/
void liberarTablero(char **tablero, int alto) {
	int j;

	for(j=0; j<alto; j++) //Liberamos cada fila
		free(tablero[j]);
	free(tablero); //Liberamos el puntero
}

/*MENU PRINCIPAL
Imprime el menú principal y otiene una opción.
PARAMETROS: Void
RETURN: La opción si es valida, -1 si es una opción no válida, como int*/
int menuPrincipal(void) {
	static int error = 0; //Flag error, para imprimir un mensaje
	char opcion = '\0', subOpcion = '\0'; //Variable para la opción y subopción

	printf("\n\tJuego Tetris:\n\t1. Juego nuevo.\n\t2. Recuperar juego guardado.\n\t3. Guardar juego.\n\t4. Ranking\n\t5. Salir.");
	//Si hubo error mostramos un mensaje
	if(error) {
		printf("\n\tOpcion incorrecta.");
		error = 0; //Desactivamos el flag
	}

	printf("\n\tOpción: ");
	opcion = getchar(); //Obtenemos una opción
	switch(opcion) {
		case '1': case '2': case '3': case '4': case '5'://Si esta dentro de las válidas
			subOpcion = getchar(); //Comprobamos que el siguiente caracter sea un enter
			if(subOpcion != '\n')
				error = 1; //Si no, es un error
			break;
		default:
			error = 1; //En otro caso también es un error
			break;
	}
	if(opcion!='\n' && subOpcion != '\n') //Si no se leyó un enter se limpia el stdin
		while(getchar() != '\n');
	//Si hubo error retornamos -1
	if(error)
		return -1;
	return (int) opcion; //Retornamos la opción como int
}

/*MENU JUEGO
Imprime el menú de juego y otiene una opción.
PARAMETROS: Void
RETURN: La opción si es valida, -1 si es una opción no válida, como int*/
int menuJuego(estado *estadoJuego) {
	static int error = 0; //Flag error, para imprimir un mensaje
	char opcion = '\0', subOpcion = '\0'; //Variable para la opción y subopción

	printf("\n\t* Puntuacion *** Lineas *\n\t*% 13ld * %6d *\n\t*************************\n", estadoJuego->puntuacion ,estadoJuego->lineas);
	printf("\n\tPARA JUGAR PULSAR 8, 6, 4 ó 2\n\tDesplazar DCHA: 6\n\tDesplazar IZDA: 4\n\tDesplazar ABAJO: 2\n\tGirar: 8\n\tPARA VOLVER AL MENÚ, PULSAR m ó M");
	//Si hubo error mostramos un mensaje
	if(error) {
		printf("\n\tOpcion incorrecta.");
		error = 0; //Desactivamos el flag
	}

	printf("\n\tOpción: ");
	opcion = getchar(); //Obtenemos una opción
	switch(opcion) {
		case '2': //Caso abajo
			subOpcion = getchar(); //Obtenemos una segunda opción
			if(subOpcion == '2') //Si la segunda opción es otro 2
				opcion = '0';  //Opcion 0 = bajar del todo
			else if(subOpcion != '\n') //Comprobamos que el siguiente caracter sea un enter
				error = 1; //Si no, es error
			break;
		case '4': case '6': case '8': case 'm': case 'M': //Otros casos válidos
			subOpcion = getchar(); 
			if(subOpcion != '\n') //Comprobamos que el siguiente caracter sea un enter
				error = 1; //Si no, es error
			break;
		default:
			error = -1; //En otro caso también es un error
			break;
	}

	if(opcion !='\n' && subOpcion != '\n') //Si no se leyó un enter se limpia el stdin
		while(getchar() != '\n');
	//Si hubo error retornamos -1
	if(error)
		return -1;

	return (int) opcion; //Retornamos la opción como int
}

/*NUEVO JUEGO
Inicializa el estado del juego, limpia el tablero, y coloca una nueva pieza aleatoria
PARÁMETROS: el estado del juego, la pieza, el tablero y alto y ancho del mismo
RETURN: Void*/
void nuevoJuego(estado *estadoJuego, int pieza[2][4], char **tablero, int alto, int ancho) {
	estado estadoNuevo = {0, 0, 0, 0, 4, 0, 0}; //Creamos un estado nuevo, inicializado
	nuevoTablero(tablero, alto, ancho); //Limpiamos el tablero
	estadoNuevo.ficha = rand() % 7; //Obtenemos una pieza aleatoria
	nuevaPieza(pieza, &estadoNuevo, tablero); //Creamos la nueva pieza

	*estadoJuego = estadoNuevo; //Asignamos el estado nuevo al estado de juego
}

/*CARGAR JUEGO
Inicializa el estado del juego, limpia el tablero, y coloca una nueva pieza aleatoria
PARÁMETROS: el estado del juego, el tablero y alto y ancho del mismo
RETURN: 1 exito, 0 no se pudo abrir*/
int cargarJuego(estado *estadoJuego, char **tablero, int alto, int ancho) {
	FILE *pf = NULL; //Puntero al fichero
	int i,j; //Variables para los bucles for
	estado estadoNuevo;

	pf = fopen("juego_guardado.txt", "r" ); //Intentamos abrir el fichero en modo lectura
	
	if(!pf) //Si no se puede abrir acaba la función retornando 0
		return 0;
	rewind(pf); //Se vuelve al principio del fichero
	fread(&estadoNuevo, sizeof(estado), 1, pf); //Se escanean los datos del estado de juego
	//Leemos el tablero caracter a caracter
	for(j=0; j<alto; j++) 
		for(i=0; i<ancho; i++)
			fscanf(pf, "%c", &tablero[j][i]);
	*estadoJuego = estadoNuevo; //Asignamos el nuevo estado al viejo
	fclose(pf); //Cerramos el fichero
	return 1; //Retornamos éxito
}

/*GUARDAR JUEGO
Inicializa el estado del juego, limpia el tablero, y coloca una nueva pieza aleatoria
PARÁMETROS: el estado del juego, el tablero y alto y ancho del mismo
RETURN: 1 exito, 0 no se pudo abrir*/
int guardarJuego(estado *estadoJuego, char **tablero, int alto, int ancho) {
	FILE *pf = NULL; //Puntero al fichero
	int i,j; //Variables para los bucles for

	pf = fopen("juego_guardado.txt", "w" ); //Intentamos abrir el fichero en modo sobreescribir
	if(!pf) //Si no se puede abrir acaba la función retornando 0
		return 0;

	fwrite(estadoJuego, sizeof(estado), 1, pf); //Escribimos en el fichero los datos de estadoJuego separados por *
	//Escribimos el tablero caracter a caracter
	for(j=0; j<alto; j++) 
		for(i=0; i<ancho; i++)
			fprintf(pf, "%c", tablero[j][i]);
	fclose(pf); //Cerramos el fichero
	return 1;
}

/*SALIR JUEGO
Se ejecuta al salir del juego, en caso de que haya que hacer alguna tarea antes de salir
PARÁMETROS: el tablero, su alto y ancho
RETURN: Void*/
void salirJuego(char **tablero, int alto, int ancho, rank *ranking) {
	liberarTablero(tablero,alto);
	guardarRanking(ranking);
	liberarRanking(ranking);
}

/*NUEVA PIEZA 
Crea las piezas, dependiendo del estadoJuego y las situa en el tablero, las piezas están guardadas como lo que son, tetraminós, cada pieza es una matriz tridimensional, de las coordenadas de cada tetramino con respecto a la esquina superior izquierda respecto a sus reticulas de giro, para cada rotación
PARÁMETROS: la matriz bidimensional de la pieza, pieza[0] contiene las coordenadas X tecada minó, pieza [1] las Y, el estado de juego y el tablero.
RETURN: 0 si se pudo colocar, 1 si no se puede colocar (acaba el juego)*/
int nuevaPieza(int pieza[2][4], estado *estadoJuego, char **tablero) {
	
	int n, m, retorno=0; //n y m, indices para el bucle for, retorno para el valor de retorno.
	char tetraminos[] = "LOIJSZT"; //Con tiene la "forma" de las piezas, para comodidad en el switch

	//Cada pieza, como se describe arriba.
	int L[4][2][4] = {{{0,1,2,0},{0,0,0,1}},{{0,0,0,1},{0,1,2,2}},{{2,0,1,2},{0,1,1,1}},{{0,1,1,1},{0,0,1,2}}};
	int O[1][2][4] = {{{0,0,1,1},{0,1,0,1}}};
	int I[2][2][4] = {{{0,1,2,3},{0,0,0,0}},{{0,0,0,0},{0,1,2,3}}};
	int J[4][2][4] = {{{0,1,2,2},{0,0,0,1}},{{0,1,1,1},{2,0,1,2}},{{0,0,1,2},{0,1,1,1}},{{0,0,0,1},{0,1,2,0}}};
	int S[2][2][4] = {{{0,1,1,2},{1,0,1,0}},{{0,0,1,1},{0,1,1,2}}};
	int Z[2][2][4] = {{{0,1,1,2},{0,0,1,1}},{{0,0,1,1},{1,2,0,1}}};
	int T[4][2][4] = {{{0,1,1,2},{0,0,1,0}},{{0,0,0,1},{0,1,2,1}},{{0,1,1,2},{1,0,1,1}},{{0,1,1,1},{1,0,1,2}}};

	//Switch para elegir la pieza segun estadoJuego->ficha
	switch(tetraminos[estadoJuego->ficha]) {
		case 'L':
			if(estadoJuego->rotacion >= sizeof(L)/sizeof(*L)) //Comprueba que no se pasa del número de rotaciones de la pieza
				estadoJuego->rotacion = 0; //Si se pasa vuelve a la primera
			for(n=0;n<2;n++)
				for(m=0;m<4;m++)
					pieza[n][m] = L[estadoJuego->rotacion][n][m]; //Se guarda la pieza deseada en la pieza del main
			estadoJuego->simbolo = '#'; //Se guarda con que símbolo se va a pintar
			break;
		case 'O': //Idem a L	
			if(estadoJuego->rotacion >= sizeof(O)/sizeof(*O))
				estadoJuego->rotacion = 0;
			for(n=0;n<2;n++)
				for(m=0;m<4;m++)
					pieza[n][m] = O[estadoJuego->rotacion][n][m];
			estadoJuego->simbolo = 'O';		
			break;	
		case 'I': //Idem a L
			if(estadoJuego->rotacion >= sizeof(I)/sizeof(*I))
				estadoJuego->rotacion = 0;
			for(n=0;n<2;n++)
				for(m=0;m<4;m++)
					pieza[n][m] = I[estadoJuego->rotacion][n][m];
			estadoJuego->simbolo = '%';
			break;
		case 'J':

			if(estadoJuego->rotacion >= sizeof(J)/sizeof(*J))
				estadoJuego->rotacion = 0;
			for(n=0;n<2;n++)
				for(m=0;m<4;m++)
					pieza[n][m] = J[estadoJuego->rotacion][n][m];
			estadoJuego->simbolo = '#';
			break;
		case 'S':

			if(estadoJuego->rotacion >= sizeof(S)/sizeof(*S))
				estadoJuego->rotacion = 0;
			for(n=0;n<2;n++)
				for(m=0;m<4;m++)
					pieza[n][m] = S[estadoJuego->rotacion][n][m];
			estadoJuego->simbolo = '@';
			break;
		case 'Z':

			if(estadoJuego->rotacion >= sizeof(Z)/sizeof(*Z))
				estadoJuego->rotacion = 0;
			for(n=0;n<2;n++)
				for(m=0;m<4;m++)
					pieza[n][m] = Z[estadoJuego->rotacion][n][m];
			estadoJuego->simbolo = '@';
			break;
		case 'T':

			if(estadoJuego->rotacion >= sizeof(T)/sizeof(*T))
				estadoJuego->rotacion = 0;
			for(n=0;n<2;n++)
				for(m=0;m<4;m++)
					pieza[n][m] = T[estadoJuego->rotacion][n][m];
			estadoJuego->simbolo = '&';
			break;
	}
	//Comprobamos que se pueda colocar la pieza.
	for(m=0;m<4;m++) 
		if(tablero[(estadoJuego->j) + pieza[1][m]][(estadoJuego->i) + pieza[0][m]] != ' ') 
			retorno = 1;
	//Si se puede colocar, la colocamos.
	if(retorno!=1)
		for(m=0;m<4;m++) 
			tablero[(estadoJuego->j) + pieza[1][m]][(estadoJuego->i) + pieza[0][m]] = estadoJuego->simbolo;
			
	return retorno;
}

/*MOVER PIEZA
Mueve y rota las piezas de acuerdo a los parametros recividos, en realidad no rota, solo comprueba si es posible rotar
PARÁMETROS: La pieza, el estado de juego, el tablero, desplazamiento de y y x, y un flag para rotar
RETURN: 1 si el movimiento/rotación es valido, 0 caso contrario*/
int moverPieza(int pieza[2][4], estado *estadoJuego, char **tablero, int dy, int dx, int rotar) {

	int n, m, valido = 1, tam = 0; //n, m indices para bucles, valido para retornar, tam para tamaño reticula de giro.
	//Quitamos la pieza para evitar colisiones consigo misma
	for(n=0;n<4;n++) 
		tablero[estadoJuego->j + pieza[1][n]][estadoJuego->i + pieza[0][n]] = ' ';

	//Si hay que rotar
	if(rotar) {
		//Obtenemos la mayor coordenada(x o y) de la pieza, que dará el tamaño de la reticula
		for(n=0;n<4;n++) {
			if(pieza[0][n] > tam)
				tam = pieza[0][n];
			if(pieza[1][n] > tam)
				tam = pieza[1][n];
		}
		//Comprobamos que el area de la retícula este vacia
		for(n=0;n<=tam;n++)
			for(m=0;m<=tam;m++) {
				if(tablero[estadoJuego->j + n][estadoJuego->i + m] != ' ') { 
					valido = 0; //Si no lo está ponemos valido a 0
				}
			}
	} 
	//Si no hay que rotar
	else {
		//Comprobamos que podemos colocar la pieza en la posición + desplazamientos
		for(n=0;n<4;n++) 
			if(tablero[estadoJuego->j + pieza[1][n] + dy][estadoJuego->i + pieza[0][n] + dx] != ' ') 
				valido = 0;
		//Si se puede colocar, la nueva coordenada es la coordenada anterior + desplazmientos
		if(valido) {
			estadoJuego->i += dx;
			estadoJuego->j += dy;
		}
		//Volvemos a pintar la pieza con las nuevas coordonedas
		for(n=0;n<4;n++) 
			tablero[estadoJuego->j + pieza[1][n]][estadoJuego->i + pieza[0][n]] = estadoJuego->simbolo;
	}
	return valido; //Retornamos valido.
}
/*COMPROBAR LINEAS
Comprueba que hay lineas completadas solo donde se puedan haber completado, es decir, las cordenadas Y de la pieza actual, y las elimina bajando las superiores*/
void comprobarLineas(int pieza[2][4], estado *estadoJuego, char **tablero, int ancho, int alto) {
	int tamy=0, n, m, o, p, valido=1, bonus = 10, contador = 0;

	//Obtenemos la mayor coordenada Y
	for(n=0;n<4;n++) 
		if(pieza[1][n] > tamy)
			tamy = pieza[1][n];
	//Para cada linea de la ficha
	for(m=0;m<=tamy;m++) {
		//Comprobamos que la linea de referencia + desplazamiento contenga algun espacio
		for(n=1;n<ancho-1;n++)
			if(tablero[estadoJuego->j + m][n] == ' ') 
				valido = 0; //Si lo tiene no hay que eliminar nada
		//Si no se encontro un espacio eliminamos la linea
		if(valido)  {
			//Cada fila pasa a ser la de arriba, empezando de abajo a arriba
			for(o=estadoJuego->j;o>0;o--) 
				for(p=0;p<ancho;p++) 
					tablero[o + m][p] = tablero[o + m - 1][p];
			estadoJuego->lineas++; //Aumentamos el núnero de lineas completadas
			contador++;
			bonus += 2;
		}
		valido = 1;	//Activamos el flag de valido
	}
	estadoJuego->puntuacion += bonus * contador;
}

//MEJORA
rank *cargarRanking(void) {
	FILE *pf;
	rank *nodo, *aux;
	int i=0;

	nodo = malloc(sizeof(rank));
	aux = nodo;
	while(i<5) {
		aux->puntuacion = 0;
		aux->lineas = 0;
		aux->sig = NULL;
		if(i < 4)
			aux->sig = malloc(sizeof(rank));
		aux = aux->sig;
		i++;
	}
	/*aux = nodo;
	pf = fopen("ranking.txt", "rb");
	if(!pf) {
		pf = fopen("ranking.txt", "wb");
		while(aux) {
			fwrite(aux, sizeof(rank), 1, pf);
			aux = aux->sig;
		}
		//return -1;
	} else {
		while(aux) {
			fread(aux, sizeof(rank), 1, pf);
			aux = aux->sig;
		}
	}
	fclose(pf);*/
	return nodo;
}

rank *comprobarRanking(rank *ranking, estado *estadoJuego) {
	rank *aux = NULL, *nuevo = NULL, *anterior = NULL, *pranking = ranking;
	aux = ranking;
	int i = 0;
	while(aux && estadoJuego->puntuacion < aux->puntuacion) {
		anterior = aux;
		aux = aux->sig;
	}
	nuevo = malloc(sizeof(rank));
	nuevo->puntuacion = estadoJuego->puntuacion;
	nuevo->lineas = estadoJuego->lineas;
	nuevo->sig = NULL;

	if(anterior == NULL) {
		nuevo->sig = pranking;
		pranking = nuevo;
	} else {
		anterior->sig = nuevo;
		nuevo->sig = aux;
	}
	aux = pranking;
	anterior = NULL;
	while(aux) {
		if(aux->sig == NULL) {
			anterior->sig = NULL;
			free(aux);
			break;
		}
		anterior = aux;
		aux = aux->sig;
	}
	anterior = NULL;
	/*printf("%ld\n", aux->puntuacion);
	system("sleep 0.5");*/
	return pranking;
}

void mostrarRanking(rank *ranking) {

	rank *aux = ranking;
	int i=1;

	printf("\t  # PUNTUACION\tLINEAS\n");
	while(aux) {
		printf("\t  %d. %9ld\t%6d\n", i, aux->puntuacion, aux->lineas);
		i++;
		aux = aux->sig;
	}
	printf("\n\t  Pulsa enter para salir\n");
	while(getchar()!='\n');
}

void guardarRanking(rank *ranking) {
	/*FILE *pf;
	rank *aux=ranking;
	pf=fopen("ranking.txt", "wb");
	if(!pf)
		return;
	while(aux) {
		fwrite(aux, sizeof(rank), 1, pf);
		aux = aux->sig;
	}*/
}

void liberarRanking(rank *ranking) {
	rank *aux;
	while(ranking)
	{
		aux=ranking;
		ranking=ranking->sig;
		free(aux);
	}
}




