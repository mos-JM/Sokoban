

#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <cmath>
#include <iomanip>
//#include "colores.h"


using namespace std;
const int MAX = 50;
const int MAXH = 10;
const int MAXE = 100;
typedef enum {Libre, Muro, DestinoL, DestinoC, DestinoJ, Jugador, Caja} tCasilla; //Destino L C y J representan pos detino libres con caja o con jugador
typedef enum {Arriba, Abajo, Derecha, Izquierda, Salir, Nada, Deshacer} tTecla;

typedef tCasilla tTablero[MAX][MAX];

typedef struct {
	tTablero tablero;
	int nFilas; // <= MAX
	int nColumnas;
	int jFila; // fila y columna donde esta colocado el jugador
	int jColumna;
	int cajas;
	int cajasDestino;
} tSokoban;

typedef tSokoban tTableros[MAXH];

typedef struct {
	tTableros tableros;
	int punteroWrite;
	int contDeshacer;
	
} tHistoria;

typedef struct {
	tSokoban sokoban;
	int numMovimientos;
	string nFichero;
	int nivel;
	tHistoria historia;
} tJuego;

typedef struct {
	string nFichero;
	int nivel;
	int numMovimientos;
} tPartida;

typedef tPartida tExito[MAXE];

typedef struct {
	string nombreJugador;
	tExito exitoJuegos;
	int contResueltas;
} tInfo;

// cabecera funciones
int menu();

//visualizacion del tablero
void colorFondo(int color); //color para el fondo ,cambia cada vez que se dibuja una casilla
void dibujarCasilla(tCasilla casilla); // muestra una casilla del tablero
void dibujar(const tJuego &juego); //muestra el tablero del juego , el nombre del fichero del que se cargo el nivel, su nivel y el numero de mov

//carga de un nivel del juego
void inicializa(tJuego &juego); //inicializa el tablero haciendo que todas las MAX x MAX esten libres y el numMov a 0;
bool cargarJuego(tJuego &juego); //solicita al usuario el fichero y el nivel que desea jugar y lo carga desde dicho fichero
bool cargarNivel(ifstream &fichero, tSokoban &sokoban, int nivel); //busca el fichero el nivel correspondiente y carga el tablero correspondiente, devuelve true si lo ha encontrado

// acciones del juego
tTecla leerTecla(); // devuelve un valor de tipo tTecla, que puede ser una de la 4 direcciones si se pulsan , el valor salir (esc) y Nada si se pulsa cualquier otra tecla
void hacerMovimiento(tJuego &juego, tTecla tecla); //realiza el mov a partir de la direccion escogida, si no se puede realizar el mov , no tiene efecto y contador mov no incrementa
bool aplicarMovimiento(tJuego &juego, int iDestino, int jDestino, int iCaja, int jCaja);

//funcion bloqueado y deshacerMovimiento
bool bloqueado(const tJuego &juego); //indica si alguna de las cajas no colocadas en destino ha quedado atrapada en una esquina
void guardarMovimiento(tJuego &juego, tSokoban &sokoban); // guarda los movimientos que se realizaron antes de aplicar movimiento
bool deshacerMovimiento(tJuego &juego); //Ofrecer al jugador la posibilidad de deshacer hasta los últimos MAXH=10 movimientos.

// jugar
void jugar(tJuego &juego, tInfo &info); // lleva la secuencia del juego
bool comprobacionTipo(int tipo);  //comprueba si cin es un entero
bool isWinner(tJuego &juego);

//getion de niveles superados
bool cargaInformacion(tInfo &info);
bool guardarInformacion(tJuego &juego, tInfo &info);
void mostrarInformacion(tInfo &info);
void guardaInfoArchivo(tInfo &info);
bool operator>(tPartida &opIzq, tPartida &opDer);
bool operator ==(tPartida &opIzq, tPartida &opDer);

void main() {

	tJuego juego;
	tInfo info;
	
	cout << "Introduzca su nombre: " ;
	cin >> info.nombreJugador;
	
	if (!cargaInformacion(info))
		cout << "Se ha creado el jugador " << info.nombreJugador << endl;
	cout << "Bienvenido " << info.nombreJugador << endl;

	int op = menu();
	while (op != 0) {
		switch (op) {
		case 1: jugar(juego, info);
			break;
		case 2: 
			mostrarInformacion(info);
			break;
		}
		op = menu();
	}
	system("pause");

}

int menu(){
	int op;
	do {
		cout << "1 . Jugar partida" << endl;
		cout << "2 . Mostrar partidas ganadas" << endl;
		cout << "0 . Salir" << endl;
		cout << "opcion: ";
		cin >> op;
		comprobacionTipo(op);
	} while (op < 0 || op > 2);

	return op;
}

void colorFondo(int color){
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, 15 | (color << 4));
}

void dibujarCasilla(tCasilla casilla){
	switch (casilla) {
	case Libre: 
		colorFondo(1);
		cout << "  ";
	
		break;
	case Muro: 
		colorFondo(2);
		cout << "  ";
	
		break;
	case DestinoL: 
		colorFondo(4);
		cout << "..";
	
		break;
	case DestinoC: 
		colorFondo(11);
		cout << "()";
	
		break;
	case DestinoJ: 
		colorFondo(14);
		cout << "00";
		break;
	case Jugador: 
		colorFondo(6);
		cout << "00";
	
		break;
	case Caja: 
		colorFondo(8);
		cout << "()";
		break;
	
	}
}

void dibujar(const tJuego & juego){
	cout << "Fichero: " << juego.nFichero << " Nivel: " << juego.nivel << endl;
	for (int i = 0; i < juego.sokoban.nFilas; i++){
		for (int j = 0; j < juego.sokoban.nColumnas; j++){
			dibujarCasilla(juego.sokoban.tablero[i][j]);
		}
		cout << endl;
	}
	colorFondo(0);
	cout << "Numero de Movimientos: " << juego.numMovimientos << endl;
}

void inicializa(tJuego &juego){
	for (int i = 0; i < MAX ; i++){
		for (int j = 0; j < MAX; j++){
			juego.sokoban.tablero[i][j] = Libre;
		}
	}
	juego.numMovimientos = 0;
	juego.sokoban.cajasDestino = 0;
	juego.historia.punteroWrite = 0;
	juego.historia.contDeshacer =0;
}

bool cargarJuego(tJuego & juego){
	bool carga = true;
	ifstream fichero;
	
	cout << "Introduce el nombre del fichero del cual desea cargar: ";
	cin >> juego.nFichero;
	fichero.open(juego.nFichero + ".txt");

	if (!fichero.is_open())
		carga = false;
	else {
		do {
			cout << "Introduce el nivel al que quieres jugar: ";
			cin >> juego.nivel;
		}while (!comprobacionTipo(juego.nivel));
		if (!cargarNivel(fichero, juego.sokoban, juego.nivel))
			carga = false;
	}
	fichero.close();
	return carga;
}

bool cargarNivel(ifstream &fichero, tSokoban & sokoban, int nivel) {
	bool encontrado = false;
	string level = "Level " + to_string((long long)nivel);
	string cargaLevel;
	while (!encontrado && !fichero.eof()) {
		getline(fichero, cargaLevel);
		if (cargaLevel == level)
			encontrado = true;
	}
	if (encontrado) {
		sokoban.nFilas = 0;
		sokoban.nColumnas = 0;
		sokoban.cajas = 0;

		getline(fichero, cargaLevel);
		while (cargaLevel.length() != 0) {
			for (int i = 0; i < cargaLevel.length(); i++) {  //<---------------------------------------------------------------
				if (cargaLevel[i] == '#')
					sokoban.tablero[sokoban.nFilas][i] = Muro;
				else if (cargaLevel[i] == ' ')
					sokoban.tablero[sokoban.nFilas][i] = Libre;
				else if (cargaLevel[i] == '.')
					sokoban.tablero[sokoban.nFilas][i] = DestinoL; //detino libre
				else if (cargaLevel[i] == '$') {
					sokoban.tablero[sokoban.nFilas][i] = Caja;
					sokoban.cajas++;
				}
				else if (cargaLevel[i] == '@') {
					sokoban.tablero[sokoban.nFilas][i] = Jugador;
					sokoban.jFila = sokoban.nFilas;
					sokoban.jColumna = i;
				}
				else if (cargaLevel[i] == '*') {
					sokoban.tablero[sokoban.nFilas][i] = DestinoC; // caja en destino
					sokoban.cajasDestino++;
					sokoban.cajas++;
				}
				else if (cargaLevel[i] == '+')
					sokoban.tablero[sokoban.nFilas][i] = DestinoJ; // jugador en casilla destino
				if (sokoban.nColumnas < i)
					sokoban.nColumnas = i;
			}
			sokoban.nFilas++;
			getline(fichero, cargaLevel);
		}
		sokoban.nColumnas += 1;
	}
		
	return encontrado;
}

tTecla leerTecla(){
	tTecla tecla;
	cin.sync();
	int dir = _getch(); //dir tipo int
	

	if (dir == 0xe0) { // son flechas
		dir = _getch(); // leo el siguiente
		switch (dir) {
		case 72: tecla = Arriba;
			break;
		case 80: tecla = Abajo;
			break;
		case 77: tecla = Derecha;
			break;
		case 75: tecla = Izquierda;
			break;
		}
	}
	else {
		if (dir == 27)
			tecla = Salir;
		else if (dir == 100 || dir == 68)
			tecla = Deshacer;
		else
			tecla = Nada;
	}

	return tecla;
}

void hacerMovimiento(tJuego & juego, tTecla tecla) {
	int i = juego.sokoban.jFila;
	int j = juego.sokoban.jColumna;
	int iDestino, jDestino, iCaja, jCaja;
	tSokoban sokoban2 = juego.sokoban; // guardo estado actual

	switch (tecla) {
	case Izquierda: 
		iDestino = i; jDestino = j - 1;
		iCaja = i; jCaja = j - 2;
		break;
	case Derecha:
		iDestino = i; jDestino = j + 1;
		iCaja = i; jCaja = j + 2;
		break;
	case Arriba:
		iDestino = i - 1; jDestino = j;
		iCaja = i -2; jCaja = j;
		break;
	case Abajo:
		iDestino = i + 1; jDestino = j;
		iCaja = i + 2; jCaja = j;
		break;
	}

	if (aplicarMovimiento(juego, iDestino, jDestino, iCaja, jCaja)) {
		guardarMovimiento(juego, sokoban2);
		juego.numMovimientos++;
		if(juego.sokoban.tablero[i][j] == Jugador)
			juego.sokoban.tablero[i][j] = Libre;
		else if (juego.sokoban.tablero[i][j] == DestinoJ)
			juego.sokoban.tablero[i][j] = DestinoL;
		juego.sokoban.jFila = iDestino;
		juego.sokoban.jColumna = jDestino;
	}
}

bool aplicarMovimiento(tJuego & juego, int iDestino, int jDestino, int iCaja, int jCaja){
	bool mov = true;
	
	switch (juego.sokoban.tablero[iDestino][jDestino]) {
	case Muro: mov = false;
		break;
	case Libre: 
		juego.sokoban.tablero[iDestino][jDestino] = Jugador;
		break;
	case DestinoL: 
		juego.sokoban.tablero[iDestino][jDestino] = DestinoJ;
		break;
	case Caja:
		if (juego.sokoban.tablero[iCaja][jCaja] == Libre) {
			juego.sokoban.tablero[iCaja][jCaja] = Caja;
			juego.sokoban.tablero[iDestino][jDestino] = Jugador;
		}
		else if (juego.sokoban.tablero[iCaja][jCaja] == DestinoL) {
			juego.sokoban.tablero[iCaja][jCaja] = DestinoC;
			juego.sokoban.cajasDestino++;
			juego.sokoban.tablero[iDestino][jDestino] = Jugador;
		}
		else if (juego.sokoban.tablero[iCaja][jCaja] == Caja || juego.sokoban.tablero[iCaja][jCaja] == Muro || juego.sokoban.tablero[iCaja][jCaja] == DestinoC)
			mov = false;
		break;
	case DestinoC:
		if (juego.sokoban.tablero[iCaja][jCaja] == Libre) {
			juego.sokoban.tablero[iCaja][jCaja] = Caja;
			juego.sokoban.cajasDestino--;
			juego.sokoban.tablero[iDestino][jDestino] = DestinoJ;
		}
		else if (juego.sokoban.tablero[iCaja][jCaja] == DestinoL) {
			juego.sokoban.tablero[iCaja][jCaja] = DestinoC;
			juego.sokoban.tablero[iDestino][jDestino] = DestinoJ;
		}
		else if (juego.sokoban.tablero[iCaja][jCaja] == DestinoC || juego.sokoban.tablero[iCaja][jCaja] == Muro|| juego.sokoban.tablero[iCaja][jCaja] == Caja)
			mov = false;
		break;
	}
	return mov;
}

bool bloqueado(const tJuego & juego){
	bool bloqueado = false;
	int f = juego.sokoban.nFilas;
	int c = juego.sokoban.nColumnas;
	for (int i = 0; i < f; i++){
		for (int j = 0; j < c; j++){
			if (juego.sokoban.tablero[i][j] == Caja) { 
				if ((juego.sokoban.tablero[i - 1][j] == Muro && juego.sokoban.tablero[i][j - 1] == Muro) ||
				   (juego.sokoban.tablero[i - 1][j] == Muro && juego.sokoban.tablero[i][j + 1] == Muro) ||
				   (juego.sokoban.tablero[i + 1][j] == Muro && juego.sokoban.tablero[i][j + 1] == Muro) ||
				   (juego.sokoban.tablero[i + 1][j] == Muro && juego.sokoban.tablero[i][j - 1] == Muro))
						bloqueado = true;
			}
		}
	}
	return bloqueado;
}

void guardarMovimiento(tJuego &juego, tSokoban &sokoban) {

	juego.historia.tableros[juego.historia.punteroWrite] = sokoban;
	juego.historia.punteroWrite = (juego.historia.punteroWrite + 1) % MAXH;
	if (juego.historia.contDeshacer < MAXH)
		juego.historia.contDeshacer++;
}

bool deshacerMovimiento(tJuego & juego) {
	bool deshacer = true;
	
	if (juego.historia.contDeshacer > 0) {
		if (juego.historia.punteroWrite == 0)
			juego.historia.punteroWrite = MAXH-1;
		else
			juego.historia.punteroWrite--;
		juego.sokoban = juego.historia.tableros[juego.historia.punteroWrite];
		juego.numMovimientos--;	
		juego.historia.contDeshacer--;
	} 
	else
		deshacer = false;
	return deshacer;
}


void jugar(tJuego & juego, tInfo &info){
	tTecla tecla;
	bool won = false;
	bool encontrado = false;
	inicializa(juego);
	if (!cargarJuego(juego))
		cout << "El archivo no se puedo cargar o no existe" << endl;
	else {
		system("cls");
		dibujar(juego);
		tecla = leerTecla();
		do {
			switch (tecla){
			case Arriba:
			case Abajo:
			case Derecha:
			case Izquierda:
				hacerMovimiento(juego, tecla);
				break;
			case Salir:
				break;
			case Nada:
				break;
			case Deshacer:
				deshacerMovimiento(juego);
				break;
			}
			system("cls");
			dibujar(juego);
			if(bloqueado(juego))
				cout << "Algunas de las cajas esta bloqueada" << endl;
			won = isWinner(juego);
			if (!won)
				tecla = leerTecla();
		} while (tecla != Salir && !won);
		
		if (won) {
			cout << "Felicidades!! " << info.nombreJugador << endl;
			system("pause");
			system("cls");
			guardarInformacion(juego, info);
		}
		guardaInfoArchivo(info);
	}
}

bool comprobacionTipo(int tipo) {  //comprueba si cin es un entero
	bool ok = true;
	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		ok = false;
	}
	return ok;
}

bool isWinner(tJuego & juego){
	bool won = false;
	int i = 0;

	while (i < juego.sokoban.nFilas && !won) {
		int j = 0;
		while (j < juego.sokoban.nColumnas && !won) {
			if (juego.sokoban.cajasDestino == juego.sokoban.cajas)
				won = true;
			else
				j++;
		}
		if (!won)
			i++;
	}
	return won;;
}

bool cargaInformacion(tInfo & info){
	bool carga = true;
	ifstream archivo;
	ofstream creaArchivo;
	tPartida partida;
	char aux;

	info.contResueltas = 0;
	archivo.open(info.nombreJugador + ".txt");
	if (archivo.fail()) {
		creaArchivo.open(info.nombreJugador + ".txt");
		carga = false;
	}
	else {
		archivo >> partida.nFichero;
		while (!archivo.eof()) {
			archivo >> partida.nivel;
			archivo >> partida.numMovimientos;
			archivo.get(aux);
			info.exitoJuegos[info.contResueltas] = partida;
			info.contResueltas++;
			archivo >> partida.nFichero;
		}
		archivo.close();
	}
	
	return carga;
}

bool guardarInformacion(tJuego & juego, tInfo & info){
	tPartida auxPartida;
	bool guarda = true;
	bool encontrado = false;
	if (info.contResueltas == MAXE)
		guarda = false;
	else {
		auxPartida.nFichero = juego.nFichero;
		auxPartida.nivel = juego.nivel;
		auxPartida.numMovimientos = juego.numMovimientos;
		int i = 0;
		while ((i < info.contResueltas) && (!encontrado)) {
			if (info.exitoJuegos[i] == auxPartida) {
				if (juego.numMovimientos < info.exitoJuegos[i].numMovimientos) {
					info.exitoJuegos[i].numMovimientos = juego.numMovimientos;
					encontrado = true;
				}
				else
					guarda = false;

			}
			else if ((info.exitoJuegos[i] > auxPartida) && guarda) {
				encontrado = true;
				for (int j = info.contResueltas; j > i; j--) {
					info.exitoJuegos[j] = info.exitoJuegos[j - 1];
				}
				info.exitoJuegos[i].nFichero = juego.nFichero;
				info.exitoJuegos[i].nivel = juego.nivel;
				info.exitoJuegos[i].numMovimientos = juego.numMovimientos;
				info.contResueltas++;
			}
			i++;
		}
		if (!encontrado) {
			info.exitoJuegos[i].nFichero = juego.nFichero;
			info.exitoJuegos[i].nivel = juego.nivel;
			info.exitoJuegos[i].numMovimientos = juego.numMovimientos;
			info.contResueltas++;
		}
	}
	return guarda;
}

void mostrarInformacion(tInfo & info){
	cout << setw(10) << "Fichero" << setw(20) << "Nivel" << setw(22)<<  "Movimientos" << endl;
	cout << "-----------------------------------------------------" << endl;
	for (int i = 0; i < info.contResueltas; i++){
		cout  << setw(10) <<  info.exitoJuegos[i].nFichero << setw(18) << info.exitoJuegos[i].nivel << setw(20) << info.exitoJuegos[i].numMovimientos << endl;
	}
	cout << endl;
}

void guardaInfoArchivo(tInfo & info){
	ofstream archivo;

	archivo.open(info.nombreJugador + ".txt");
	for (int i = 0; i < info.contResueltas; i++){
		archivo << info.exitoJuegos[i].nFichero << " "
				<< info.exitoJuegos[i].nivel << " " 
				<< info.exitoJuegos[i].numMovimientos << endl;
	}
	archivo.close();
}

bool operator>(tPartida &opIzq, tPartida &opDer) {
	return ((opIzq.nFichero > opDer.nFichero) ||
		(opIzq.nFichero == opDer.nFichero && opIzq.nivel > opDer.nivel));
}
bool operator ==(tPartida &opIzq, tPartida &opDer) {
	return ((opIzq.nFichero == opDer.nFichero) && (opIzq.nivel == opDer.nivel));
}



