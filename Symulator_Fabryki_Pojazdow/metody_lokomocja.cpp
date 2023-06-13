//
// Created by Dawid Makowski on 30.12.2021.
//

#include "klasy_pojazdy.h"

unsigned int Lokomocja::r = 0;
string *Lokomocja::Kolory = new string[100] {"Biały", "Srebrny", "Czarny", "Niebieski", "Zielony", "Czerwony", "Różowy"};

Lokomocja::Lokomocja(string a, string b, double c, string d) :
        model(a), kolor(b), wyprodukowano(d), cena_det(c) {}

Lokomocja::Lokomocja() :
        model(), kolor(), wyprodukowano(), cena_det(0) {
    nr_seryjny = string(11, '.') + "Puste Miejsce" + string(11, '.');
}

ostream& operator<<(ostream& os, const Lokomocja& pojazd) { //Przeładowanie operatora "cout" dla Lokomocji wypisuje jego nr_seryjny
    os << pojazd.nr_seryjny;
    return os;
}

unsigned int Lokomocja::dodaj_kolor() { //dodawanie koloru zażyczonego sobie przez użytkownika
    wyswietl_info("Ile chcesz dodać kolorów?");
    unsigned int u = 0;
    cin >> u;
    string kolor;
    u = czy_dobra_wartosc(u, 10);


    for (int i = 0; i < u; i++) {
        string tekst = "Wprowadź kolor nr " + to_string(i + 1) + ".";
        wyswietl_info(tekst);
        IG;
        getline(cin, kolor);
        Kolory[K + i + r] = kolor;
    }
    Kolory[K + u + r] = "Dodaj kolory";
    return u;
}

string Lokomocja::dolacz_kolor(unsigned int &k) {
    Kolory[K + r] = "Dodaj kolory"; //K-domyślna liczba kolorów, r-liczba kolorów, które użytkownik dotychczas dodał
    wyswietl_ekran(K + r + 1, Kolory, "Wybierz kolor pojazdu"); //Ekran wyboru koloru
    cin >> k;
    k = czy_dobra_wartosc(k, K + r + 1, 1);

    while (k == K + r + 1) {
        r += dodaj_kolor(); //Ile dodano nowych kolorów
        wyswietl_ekran(K + r + 1, Kolory, "Wybierz kolor pojazdu");
        cin >> k;
        k = czy_dobra_wartosc(k, K + r + 1, 1);
    }
    return Kolory[k - 1];
}