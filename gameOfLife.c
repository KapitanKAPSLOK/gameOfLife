/*
autor: Mateusz Turowski
e-mail:maturowsk@gmail.com

Kompilacja poleceniem:
gcc -std=c11 -pedantic -Wall -Wextra -Werror -fstack-protector-strong -g przeksztalcenie.c -o przeksztalcenie

Program symuluje grę w życie
Czyta on z wejścia i pisze na wyjście opis generacji początkowej a następnie czyta i wykonuje polecenia użytkownika.

Program rozpoznaje polecenia postaci:
-polecenie puste,
Oblicz i wypisz następną generację.
-jedna, nieujemna, liczba całkowita N,
Oblicz i wypisz N-tą kolejną generację.
-dwie liczby całkowite A B,
Zmień dwie pierwsze współrzędne okna, czyli współrzędne lewego górnego rogu, na A B.
Jednocześnie zaktualizuj dwie pozostałe współrzędne okna, czyli współrzędne prawego dolnego rogu tak, rozmiar okna się nie zmienił.
Po zmianie okna, wypisz aktualną generację.
-cztery liczby całkowite A B C D.
Zmień współrzędne okna na A B C D.
Po zmianie okna, wypisz aktualną generację.
Polecenia zmiany okna nie powodują obliczenia kolejnej generacji.

Po każdej generacji program pisze wiersz ze statusem, czyli informacją o aktualnym stanie.
Status ma format:
A {B} (C:D) (E:F) [G:H] [I:J]
gdzie:
A to numer generacji, dla generacji początkowej równy 0,
B to liczba żywych komórek,
C to numer pierwszego wiersza na planszy, w którym jest żywa komórka,
D to numer pierwszej kolumny na planszy, w której jest żywa komórka,
E to numer ostatniego wiersza na planszy, w którym jest żywa komórka,
F to numer ostatniej kolumny na planszy, w której jest żywa komórka,
G H I J to aktualne współrzędne okna.
*/



#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>


enum Stan { ZYWA, MARTWA };

typedef struct {
	int x, y;
	enum Stan s;
	int ileSasiadow;
}Komorka;

typedef struct Lista {
	Komorka k;
	struct Lista* nast;
}Lista;

//hash mapa na koórki, w każdym wierszu tablicy jest lista żywych komórek
typedef struct {
	int A, B, C, D;
	long rozmiar;
	int a, b;
	int nrGeneracji;
	Lista** tab;
}Plansza;

//struktura pomocnicza przy inicjowaniu planszy
typedef struct List {
	int x, y;
	struct List* nast;
}List;

typedef struct {
	long rozmiar;
	struct List* nast;
}ListPocz;


Plansza zainicjuj(); //wczytuje i ustala stan początkowy symulacji				 
void symulacja(Plansza *p);  //wykonuje komendy urzytkownika
void skasujPlansze(Plansza *p);


int main() {
	Plansza p = zainicjuj();
	symulacja(&p);
	skasujPlansze(&p);
	return 0;
}


Plansza stworzPlansze(ListPocz* l, int C, int D);
Komorka* daj(Plansza p, int x, int y);  //zwraca komórkę o podanych współrzędnych (dodaje ją jeśli nie istnieje)									
void dodajSasiadow(Plansza p, Komorka k); //dodaje +1 do licznika sąsiadów komórką dookoło podanej komórki k									 
void wypisz(Plansza p, int x, int y);  //wypisuje "0" jeśli istnieje komórka o danych współrzędnych lub "." jeśli nie
void wyswietl(Plansza p);
void zmienWspolrzedne(Plansza *p, int a, int b, int c, int d);
void zmienWspolrzedne2(Plansza *p, int a, int b);
void status(Plansza *p, int n);  //wylicza i wypisuje aktualny status planszy i zwiększa numer generacji o n					 
void liczGeneracje1(Plansza* p);  //wylicza następną generację			 
void liczGeneracje(Plansza* p, int n);  //wylicza i wypisuje N-tą generację
Komorka nowaKomorka(int x, int y);  //tworzy nową martwą komórkę
long hash(unsigned int x, unsigned int y, int a, int b, int rozm); //funkcja tworzy liniową kombinacją obydwu współrzędnych
void add(Lista **l, int x, int y);  //przekształca element typu [List] na e;ement typu [Lista] i dodaje go do istniejącej listy l						
void zwolnij(ListPocz* l);  //zwalnia pamięć po liście
void zwolnij2(Lista* l);
void usunPlansze(Plansza *p);


void skasujPlansze(Plansza *p) {
	for (int i = 0; i<p->rozmiar; ++i) {
		zwolnij2(p->tab[i]);
	}
	usunPlansze(p);
}

//wczytuje i ustala stan początkowy symulacji
Plansza zainicjuj() {
	int ile = 0, x = 0, y;
	int maxX = 0;
	List* dane = malloc(sizeof(*dane));
	List* act = dane;
	dane->nast = NULL;
	char znak = getchar();
	y = 1;
	while (znak != EOF) {
		if (x > maxX)maxX = x;
		x = 1;
		while (znak != '\n') {
			if (znak == '0') {
				act->nast = malloc(sizeof(*act));
				act = act->nast;
				act->x = x;
				act->y = y;
				act->nast = NULL;
				++ile;
			}
			++x;
			znak = getchar();
		}
		if (x == 1)break;
		++y;
		znak = getchar();
	}
	ListPocz *l = malloc(sizeof(*l));
	l->rozmiar = ile;
	l->nast = dane->nast;
	free(dane);
	return stworzPlansze(l, y - 1, maxX - 1);
}

void usunPlansze(Plansza *p) {
	free(p->tab);
}

//wykonuje komendy użytkownika
void symulacja(Plansza *p) {
	int a, b, c, d;
	char znak = getchar();
	wyswietl(*p);
	status(p, 0);
	while (znak != EOF) {

		//rozpoznawanie komend użytkownika
		ungetc(znak, stdin);
		if (znak == '\n') { //polecenie puste
			liczGeneracje(p, 1);
			getchar();
		}
		else {
			scanf("%d", &a);
			if (getchar() == '\n') { //policz a-tą generację
				liczGeneracje(p, a);
			}
			else {
				scanf("%d", &b);
				if (getchar() == '\n') { //zmiana współrzędnych okna bez zmiany rozmiaru
					zmienWspolrzedne2(p, a, b);
					wyswietl(*p);
					status(p, 0);
				}
				else { //zmiana współrzędnych okna na zadane
					scanf("%d", &c);
					scanf("%d", &d);
					getchar();
					zmienWspolrzedne(p, a, b, c, d);
					wyswietl(*p);
					status(p, 0);
				}
			}
		}
		znak = getchar();
	}
}


//////////////funkcje wewnętrzne///////////////

//tworzy nową martwą komórkę
Komorka nowaKomorka(int x, int y) {
	Komorka k;
	k.ileSasiadow = 0;
	k.s = MARTWA;
	k.x = x;
	k.y = y;
	return k;
}

//funkcja tworzy liniową kombinacją obydwu współrzędnych
long hash(unsigned int x, unsigned int y, int a, int b, int rozm) {
	//2147483629 jest to największa liczba pierwsza mniejsza od INT_MAX, pozwala wrócić z większych liczb na zakres inta bez utraty liniowości
	return ((a*(((unsigned long long int)x << (8 * sizeof(int))) + y) + b) % 2147483629 % rozm); //w zależności od rozmiaru long long inta i inta może wystąpić overflow
}

//tworzy element typu [Lista] i dodaje go do istniejącej listy l
void add(Lista **l, int x, int y) {
	Lista *temp;
	temp = malloc(sizeof(*temp));
	if (*l != NULL) temp->nast = *l;
	else temp->nast = NULL;
	temp->k.ileSasiadow = 0;
	temp->k.x = x;
	temp->k.y = y;
	temp->k.s = ZYWA;

	*l = temp;
}

//zwalnia pamięć po liście
void zwolnij(ListPocz* l) {
	List *wsk = l->nast;
	List *temp;
	while (wsk != NULL) {
		temp = wsk->nast;
		free(wsk);
		wsk = temp;
	}
	free(l);
}

void zwolnij2(Lista* l) {
	if (l == NULL)return;
	Lista *wsk = l->nast;
	Lista *temp;
	while (wsk != NULL) {
		temp = wsk->nast;
		free(wsk);
		wsk = temp;
	}
	free(l);
}

void powiekszPlansze(Plansza *p, int n) {

	//tworzenie listy żywych komórek
	Lista *l;
	l = NULL;
	for (int i = 0; i < p->rozmiar; ++i) {
		Lista *act = p->tab[i];
		if (act != NULL) {
			while (act->nast != NULL) {
				act = act->nast;
			}
			act->nast = l;
			l = p->tab[i];
		}
	}

	Lista **tablica;
	Lista *kom;
	int *temp;

	//w celu możliwości lepszego hashowania elementów większy rozmiar tablicy niż liczba żywych komórek
	if (n > INT_MAX / 8 - 1) n = INT_MAX;
	else n *= 8;

	/*Losuje takie współczynniki funkcji liniowej a i b, które dają odchylenie kwadratowe ilości elementów w danym indeksie talicy <=4*rozm+1
	wykluczane są w ten sposób pesymistyczne przypadki, dla których otrzymamy nagromadzenie komórek w jednym indeksie i co za tym idzie
	spowolnienie złożoności algorytmu z liniowego do kwadratowego*/
	int a, b;
	temp = malloc(n * sizeof(*temp));
	long kwadraty;
	do {
		kwadraty = 0;
		kom = l;
		for (int i = 0; i < n; ++i) {
			temp[i] = 0;
		}
		a = rand() % 100 + 1;
		b = rand() % 100 + 1;
		while (kom != NULL) {
			++temp[hash(kom->k.x, kom->k.y, a, b, n)];
			kom = kom->nast;
		}
		for (int i = 0; i < n; ++i) {
			kwadraty += temp[i] * temp[i];
		}
	} while (kwadraty >= 2 * n + 1);


	free(temp);

	p->a = a;
	p->b = b;
	//dodawanie komórek do planszy dla optymalnych współczynników a i b
	p->rozmiar = n;
	tablica = malloc(n * sizeof(*tablica));
	kom = l;
	for (int i = 0; i < n; ++i) {
		tablica[i] = NULL;
	}
	while (kom != NULL) {
		add(&(tablica[hash(kom->k.x, kom->k.y, a, b, n)]), kom->k.x, kom->k.y);
		kom = kom->nast;
	}
	free(p->tab);
	zwolnij2(l);
	p->tab = tablica;
}

Plansza stworzPlansze(ListPocz* l, int C, int D) {
	Plansza plansza;
	List *kom;
	int *temp;
	int rozm = l->rozmiar;

	//w celu możliwości lepszego hashowania elementów większy rozmiar tablicy niż liczba żywych komórek
	if (rozm > INT_MAX / 8 - 1) rozm = INT_MAX;
	else rozm = 8 * l->rozmiar;

	//inicjalizacja ziarna dla funkcji rand czasem systemowym
	srand((unsigned)time(NULL));

	/*Losuje takie współczynniki funkcji liniowej a i b, które dają odchylenie kwadratowe ilości elementów w danym indeksie talicy <=4*rozm+1
	wykluczane są w ten sposób pesymistyczne przypadki, dla których otrzymamy nagromadzenie komórek w jednym indeksie i co za tym idzie
	spowolnienie złożoności algorytmu z liniowego do kwadratowego*/
	int a, b;
	temp = malloc(rozm * sizeof(*temp));
	long kwadraty;
	do {
		kwadraty = 0;
		kom = l->nast;
		for (int i = 0; i < rozm; ++i) {
			temp[i] = 0;
		}
		a = rand() % 100 + 1;
		b = rand() % 100 + 1;
		while (kom != NULL) {
			++temp[hash(kom->x, kom->y, a, b, rozm)];
			kom = kom->nast;
		}
		for (int i = 0; i < rozm; ++i) {
			kwadraty += temp[i] * temp[i];
		}
	} while (kwadraty >= 4 * l->rozmiar + 1);

	free(temp);

	//dodawanie komórek do planszy dla optymalnych współczynników a i b
	plansza.rozmiar = rozm;
	plansza.tab = malloc(rozm * sizeof(*plansza.tab));
	kom = l->nast;
	for (int i = 0; i < rozm; ++i) {
		plansza.tab[i] = NULL;
	}
	while (kom != NULL) {
		add(&(plansza.tab[hash(kom->x, kom->y, a, b, rozm)]), kom->x, kom->y);
		kom = kom->nast;
	}
	if (plansza.rozmiar > 0) {
		plansza.a = a;
		plansza.b = b;
	}

	zwolnij(l);

	plansza.nrGeneracji = 0;

	plansza.A = 1;
	plansza.B = 1;
	plansza.C = C;
	plansza.D = D;

	return plansza;
}


//zwraca komórkę o podanych współrzędnych (dodaje ją jeśli nie istnieje)
Komorka* daj(Plansza p, int x, int y) {
	int indeks = hash(x, y, p.a, p.b, p.rozmiar);
	if (p.tab[indeks] == NULL) { //indeks tablicy jest pusty, więc komórka na pewno nie istnieje
		p.tab[indeks] = malloc(sizeof(Lista));
		p.tab[indeks]->k = nowaKomorka(x, y);
		p.tab[indeks]->nast = NULL;
		return (&(p.tab[indeks]->k));
	}
	Lista* act = (p.tab[indeks]);
	while (act->nast != NULL) {
		if (act->k.x == x && act->k.y == y)return &(act->k);
		act = act->nast;
	}
	if (act->k.x == x && act->k.y == y)return &(act->k);

	//w tym momencie wiadomo, że nie ma takiej komórki
	act->nast = malloc(sizeof(Lista));
	act = act->nast;
	act->k = nowaKomorka(x, y);
	act->nast = NULL;
	return &(act->k);
}

//dodaje +1 do licznika sąsiadów komórką dookoło podanej komórki k
void dodajSasiadow(Plansza p, Komorka k) {
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (i != 0 || j != 0) {
				daj(p, k.x + i, k.y + j)->ileSasiadow++;
			}
		}
	}
}

//wypisuje "0" jeśli istnieje komórka o danych współrzędnych lub "." jeśli nie
void wypisz(Plansza p, int x, int y) {
	if (p.rozmiar == 0) {
		printf(".");
		return;
	}
	Lista *l = p.tab[hash(x, y, p.a, p.b, p.rozmiar)];
	while (l != NULL) {
		if (l->k.x == x && l->k.y == y) {
			printf("0");
			return;
		}
		l = l->nast;
	}
	printf(".");
}

void wyswietl(Plansza p) {
	for (int i = p.A; i <= p.C; ++i) {
		for (int j = p.B; j <= p.D; ++j) {
			wypisz(p, j, i);
		}
		printf("\n");
	}
}

void zmienWspolrzedne(Plansza *p, int a, int b, int c, int d) {
	p->A = a;
	p->B = b;
	p->C = c;
	p->D = d;
}

void zmienWspolrzedne2(Plansza *p, int a, int b) {
	p->C += a - p->A;
	p->D += b - p->B;
	p->A = a;
	p->B = b;
}

//wylicza i wypisuje aktualny status planszy i zwiększa numer generacji o n
void status(Plansza *p, int n) {
	p->nrGeneracji += n;
	int zywe = 0;
	int pierwszaW = INT_MAX, pierwszaK = INT_MAX;//numer wiersza pierwszej żywej kolumny i numer kolumny
	int ostatniaW = -INT_MAX, ostatniaK = -INT_MAX;

	for (int i = 0; i < p->rozmiar; ++i) {
		Lista *l = p->tab[i];
		while (l != NULL) {
			if (l->k.x < pierwszaK)pierwszaK = l->k.x;
			if (l->k.y < pierwszaW)pierwszaW = l->k.y;
			if (l->k.x > ostatniaK)ostatniaK = l->k.x;
			if (l->k.y > ostatniaW)ostatniaW = l->k.y;
			++zywe;
			l = l->nast;
		}
	}
	if (zywe == 0) {
		pierwszaW = 0;
		pierwszaK = 0;
		ostatniaW = 0;
		ostatniaK = 0;
	}
	printf("%d {%d} (%d:%d) (%d:%d) [%d:%d] [%d:%d]\n", p->nrGeneracji, zywe, pierwszaW, pierwszaK, ostatniaW, ostatniaK, p->A, p->B, p->C, p->D);
	if (zywe > p->rozmiar / 3) powiekszPlansze(p, zywe);
}

//wylicza następną generację
void liczGeneracje1(Plansza* p) {
	int zywe = 0;
	if (p->rozmiar == 0)return;
	//liczenie sąsiadów
	for (int i = 0; i < p->rozmiar; ++i) {
		Lista* temp = (p->tab[i]);
		while (temp != NULL) {
			if (temp->k.s == ZYWA) {
				dodajSasiadow(*p, temp->k);
				++zywe;
			}

			temp = temp->nast;
		}
	}

	//ożywia odpowiednie komórki i usuwa te martwe
	for (int i = 0; i < p->rozmiar; ++i) {
		Lista* temp = p->tab[i];
		if (temp != NULL) {
			while (temp->nast != NULL) {
				if (temp->nast->k.ileSasiadow == 3) {
					temp->nast->k.s = ZYWA;
					temp->nast->k.ileSasiadow = 0;
					temp = temp->nast;
				}
				else if (temp->nast->k.ileSasiadow == 2 && temp->nast->k.s == ZYWA) {
					temp->nast->k.ileSasiadow = 0;
					temp = temp->nast;
				}
				else {//w tym momencie wiadomo, że trzeba usunąć komórkę
					Lista *l = temp->nast->nast;
					free(temp->nast);
					temp->nast = l;
				}
			}

			//sprawdzanie początku listy
			temp = (p->tab[i]);//wracamy wskaźnikiem do pierwszego elementu
			if (temp->k.ileSasiadow == 3) {
				temp->k.s = ZYWA;
				temp->k.ileSasiadow = 0;
			}
			else if (temp->k.ileSasiadow != 2 || temp->k.s != ZYWA) { //w tym momencie wiadomo, że trzeba usunąć komórkę
				Lista *l = temp->nast;

				free(temp);
				p->tab[i] = l;
			}
			else {
				temp->k.ileSasiadow = 0;
			}
		}
	}
	if (zywe > p->rozmiar / 3)powiekszPlansze(p, zywe);
}

//wylicza i wypisuje N-tą generację
void liczGeneracje(Plansza* p, int n) {
	for (int i = 0; i < n; ++i) {
		liczGeneracje1(p);
	}
	wyswietl(*p);
	status(p, n);
}
