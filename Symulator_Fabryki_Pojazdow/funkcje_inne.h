//
// Created by Dawid Makowski on 29.12.2021.
//

#ifndef FUNKCJE_INNE_H
#define FUNKCJE_INNE_H

#include <string> //dla std:string
#include <utility> //dla std::move
#include <vector> //dla std::vector
#include <iostream>
#include <chrono> //dla generowania liczb losowych
#include <random> //dla generowania liczb losowych
#include <cmath> //dla funkcji pow()

using namespace std;

void wyswietl_menu(string info = "Wybierz akcję"); //funkcja służy do wydruku menu głównego
void wyswietl_ekran(unsigned int ile, string* opcje, string nazwa, string info = "###", char znak = '#'); //wyświetla ekran z opcjami
void wyswietl_ekran(vector<string> opcje, string nazwa, string info = "###", char znak = '#');
void wyswietl_info(int ile, string *info, char znak = '#'); //wyświetla ekran z tekstem linijka po linijce
void wyswietl_info(string info, char znak = '#'); //wyświetla tekst w jednej linijce
void wyswietl_ok(string tekst, char znak = '#'); //wyświetla tekst w jednej linijce, a pod nim opcję 'ok'
void wyswietl_ok(unsigned int rozmiar, string* tekst, char znak = '#'); //wyświetla tekst w wielu linijkach, a pod nim opcję 'ok'
bool przelew(double &saldo, double kwota, const string &blad = "Błąd! Za mało środków na koncie"); //wykonuje przelew na określoną kwotę
bool przelew(double &saldo, double kwota, string &&blad = "Błąd! Za mało środków na koncie");

bool plik_czy_cin(int ile, char** plik); //czytanie z pliku lub z klawiatury
//Funkcje losowe
void los();
unsigned int pobierz_los(unsigned int min, unsigned int max);
//Funkcja sprawdzająca, czy użytkownik podał wartość z oczekiwanego przedziału
template<class X, class Y, class Z = int>
inline X czy_dobra_wartosc(X i, Y max, Z min = 0, const string &opis = "Błąd podano złą wartość") { //funkcja sprawdzająca, czy użytkownik wprowadził dobrą wartość
    string tab[2];
    tab[0] = opis;
    tab[1] = "Podaj wartość z zakresu od " + to_string(min) + " do " + to_string(max);
    if (max == min) {
        tab[1] = "Podaj wartość " + to_string(min);
    }
    while (i > max || i < min || cin.fail()) { //dopóki i jest poza oczekiwanym zakresem liczbowym lub nie jest typem liczbowym
        wyswietl_info(2,tab);
        cin.clear();
        cin.ignore(256, '\n');
        cin >> i;
    }
    return i;
}
//funkcja zwraca string z typu zmiennoprzecinkowego, który zawiera określoną liczbę cyfr po przecinku
template<class X>
inline string prec(X x, unsigned int d) {
    string nowy = to_string((int)x) + '.';
    for (int i = 0; i < d; i++) {
        auto p = int(x * pow(10, i + 1)) % 10;
        nowy += to_string(p);
    }
    return nowy;
}

inline int tak_czy_nie(string &&tekst, string info = "###") { //Funkcja wyświetlająca wybór między tak, a nie z danym zapytaniem
    string koniec[2] = {"Tak", "Nie"};
    wyswietl_ekran(2, koniec, tekst, move(info));
    int w = 0;
    cin >> w;
    w = czy_dobra_wartosc(w, 2, 1);
    return 2 - w; //Jeśli nie to return 0, jeśli tak to return 1
}

inline int tak_czy_nie(const string &tekst, string info = "###") { //Funkcja wyświetlająca wybór między tak, a nie z danym zapytaniem
    string koniec[2] = {"Tak", "Nie"};
    wyswietl_ekran(2, koniec, tekst, info);
    int w = 0;
    cin >> w;
    w = czy_dobra_wartosc(w, 2, 1);
    return 2 - w; //Jeśli nie to return 0, jeśli tak to return 1
}
//uzupełnienie funkcji wyświetl_ok(), sprawdza, czy użytkownik nie wybrał jakiejś dziwnej wartości
inline void czy_ok(const string &tekst = "Wybierz \"OK\" aby przejść dalej") {
    wyswietl_ok(tekst);
    int i = 0;
    cin >> i;
    i = czy_dobra_wartosc(i, 1, 1);
}

inline void czy_ok(unsigned int rozmiar, string* tekst) {
    wyswietl_ok(rozmiar, tekst);
    int i = 0;
    cin >> i;
    i = czy_dobra_wartosc(i, 1, 1);
}
//używana w konstruktorach pojazdów, zamienia trzycyfrową liczbę na string
inline string licznik_to_string(unsigned int licznik) {
    string s = "000";
    if (licznik > 999) {
        licznik -= 1000;
    }
    s = to_string(static_cast<int>(licznik / 100)) +
        to_string(static_cast<int>((licznik / 10) % 10)) +
        to_string(static_cast<int>((licznik % 100) % 10));
    return s;
}
//używana w konstruktorach zakładów, zamienia spacje na '-' (minus)
inline string nazwa_bez_spacji(string zaklad) {
    for (char &i : zaklad) {
        if (i == ' ') {
            i = '-';
        }
    }
    return zaklad;
}
//wykonuje przelew na daną kwotę
inline bool przelew(double &saldo, double kwota, const string &blad) {
    if (saldo - kwota < 0.0) {
        czy_ok(blad);
        return false;
    }
    saldo -= kwota;
    return true;
}

inline bool przelew(double &saldo, double kwota, string &&blad) {
    if (saldo - kwota < 0.0) {
        czy_ok(blad);
        return false;
    }
    saldo -= kwota;
    return true;
}


#endif //FUNKCJE_INNE_H
