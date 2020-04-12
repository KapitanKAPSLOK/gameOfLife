# gameOfLife
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

