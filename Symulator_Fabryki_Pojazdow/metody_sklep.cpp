//
// Created by Dawid Makowski on 03.01.2022.
//

#include "klasy_zaklady.h"

SklepRowerowy::SklepRowerowy(string nazwa, unsigned int pojemnosc, float m) : Zaklad(move(nazwa), pojemnosc), margin(m) {
    Rowery = new Rower[pojemnosc];
    Prototypy = new Lokomocja*[pojemnosc];
    for (int i = 0; i < pojemnosc; i++) {
        Prototypy[i] = &Rowery[i];
    }
}

SklepRowerowy::SklepRowerowy(const SklepRowerowy& S) {
    nazwa = S.nazwa;
    pojemnosc = S.pojemnosc;
    margin = S.margin;
    Rowery = new Rower[pojemnosc];
    Prototypy = new Lokomocja*[pojemnosc];
    for (int i = 0; i < pojemnosc; i++) {
        Rowery[i] = S.Rowery[i];
        Prototypy[i] = &Rowery[i];
    }
}

SklepRowerowy::SklepRowerowy(SklepRowerowy&& S) noexcept {
    nazwa = S.nazwa;
    pojemnosc = S.pojemnosc;
    margin = S.margin;
    Rowery = S.Rowery;
    Prototypy = S.Prototypy;
    S.Rowery = nullptr;
    S.Prototypy = nullptr;
}

SklepRowerowy& SklepRowerowy::operator=(const SklepRowerowy& S) {
    if(&S != this) {
        delete[] Rowery;
        delete[] Prototypy;
        nazwa = S.nazwa;
        pojemnosc = S.pojemnosc;
        margin = S.margin;
        Rowery = new Rower[pojemnosc];
        Prototypy = new Lokomocja*[pojemnosc];
        for (int i = 0; i < pojemnosc; i++) {
            Rowery[i] = S.Rowery[i];
            Prototypy[i] = &Rowery[i];
        }
    }
    return *this;
}

SklepRowerowy& SklepRowerowy::operator=(SklepRowerowy&& S) noexcept {
    if(&S != this) {
        delete[] Rowery;
        delete[] Prototypy;
        nazwa = S.nazwa;
        pojemnosc = S.pojemnosc;
        margin = S.margin;
        Rowery = S.Rowery;
        Prototypy = S.Prototypy;
        S.Prototypy = nullptr;
        S.Rowery = nullptr;
    }
    return *this;
}

ostream& operator<<(ostream& os, const SklepRowerowy& S) {
    os << S.SklepRowerowy::wyswietl("Posiadane rowery", true);
    return os;
}

string SklepRowerowy::wyswietl(string info, bool druk) const {
    auto *rowerki = new string[pojemnosc + 1];
    string calosc = "Komis " + nazwa + "\n" + "Posiadane rowery:" + "\n"; //Nagłówek dla "calosc"

    for (int i = 0; i < pojemnosc; i++) {
        if ((Rowery[i].seria()).substr(0,2) == "PL") rowerki[i] = Rowery[i].seria(); //Numery seryjne rowerów
        else rowerki[i] = "...........Puste Miejsce...........";
        calosc += to_string(i + 1) + ". " + rowerki[i] + "\n"; //funkcja zwraca tę wartość dla cout'a
    }

    rowerki[pojemnosc] = "Powrót"; //Jako ostatnią rubrykę wyświetla opcję powrót do menu
    string fabr = "Sklep rowerowy " + nazwa_bez_spacji(nazwa); //Nagłówek dla okienka
    if (!druk) wyswietl_ekran(pojemnosc + 1, rowerki, fabr, move(info)); //Wydruk okienka zawartości komisu
    delete[] rowerki;
    return calosc;
}

unsigned int SklepRowerowy::znajdz_puste() {
    unsigned int k = 0;
    while (Rowery[k].seria() != "...........Puste Miejsce...........") {
        k++;
        if (k == pojemnosc) {
            return pojemnosc;
        }
    }
    return k;
}

void SklepRowerowy::sprzedaz(double &saldo) {

    wyswietl("Podaj pozycję roweru, który chcesz sprzedać");

    int i = 0;
    cin >> i;
    i = czy_dobra_wartosc(i, pojemnosc, 1);

    string sprzedaz[2];
    sprzedaz[0] = "Czy chcesz sprzedać " + Rowery[i - 1].seria();
    sprzedaz[1] = " z pozycji nr " + to_string(i) + "?";
    double cena;
    if(tak_czy_nie(sprzedaz[0], sprzedaz[1])) {
        cena = Rowery[i - 1].ustal_cene();
        cena *= margin;
        if (Rowery[i - 1].seria().substr(3,5) == "Rower") Rowery[i - 1] = Rower();
        else {
            czy_ok("Nie można sprzedać roweru!");
            return; //Jeśli nr seryjny pojazdu jest niewłaściwy, nie dokonuje się jego sprzedaży
        }
    }
    else return; //Jeśli użytkownik nie chce sprzedać pojazdu to program wraca do menu
    saldo += cena;
    string komunikat[2];
    komunikat[0] = "Sprzedano rower za " + to_string((int)cena) + "zł";
    komunikat[1] = "Obecne saldo wynosi " + to_string((int)saldo) + "zł";
    czy_ok(2, komunikat); //okienko
}