//
// Created by Dawid Makowski on 02.01.2022.
//

#include <utility>

#include "klasy_zaklady.h"

Fabryka::Fabryka(string nazwa, unsigned int pojemnosc) : Zaklad(move(nazwa), pojemnosc) {
    Rowery = new Rower[pojemnosc];
    Motocykle = new Motocykl[pojemnosc];
    Samochody = new Samochod[pojemnosc];
    for (int i = 0; i < pojemnosc; i++) {
        Prototypy[i] = &Samochody[i];
    }
}

Fabryka::Fabryka(const Fabryka& F) {
    nazwa = F.nazwa;
    pojemnosc = F.pojemnosc;
    Rowery = new Rower[pojemnosc];
    Motocykle = new Motocykl[pojemnosc];
    Samochody = new Samochod[pojemnosc];
    Prototypy = new Lokomocja*[pojemnosc];
    for (int i = 0; i < pojemnosc; i++) {
        if (F.Samochody[i].seria().substr(3,4) == "Auto") {
            Samochody[i] = F.Samochody[i];
            Prototypy[i] = &Samochody[i];
        }
        else if (F.Motocykle[i].seria().substr(3,8) == "Motocykl") {
            Motocykle[i] = F.Motocykle[i];
            Prototypy[i] = &Motocykle[i];
        }
        else {
            Rowery[i] = F.Rowery[i];
            Prototypy[i] = &Rowery[i];
        }
    }
}

Fabryka::Fabryka(Fabryka&& F) noexcept {
    nazwa = F.nazwa;
    pojemnosc = F.pojemnosc;
    Prototypy = F.Prototypy;
    Rowery = F.Rowery;
    Motocykle = F.Motocykle;
    Samochody = F.Samochody;
    F.Prototypy = nullptr;
    F.Samochody = nullptr;
    F.Motocykle = nullptr;
    F.Rowery = nullptr;
}

ostream& operator<<(ostream& os, const Fabryka& F) { //przeładowanie "cout" dla Fabryki korzysta z funkcji wyswietl()
    os << F.Fabryka::wyswietl("", true);
    return os;
}

Fabryka& Fabryka::operator=(const Fabryka &F) {
    if(&F != this) {
        delete[] Motocykle;
        delete[] Samochody;
        delete[] Rowery;
        delete[] Prototypy;
        nazwa = F.nazwa;
        pojemnosc = F.pojemnosc;
        Prototypy = new Lokomocja*[pojemnosc];
        Rowery = new Rower[pojemnosc];
        Motocykle = new Motocykl[pojemnosc];
        Samochody = new Samochod[pojemnosc];
        for (int i = 0; i < pojemnosc; i++) {
            if (F.Samochody[i].seria().substr(3,4) == "Auto") {
                Samochody[i] = F.Samochody[i];
                Prototypy[i] = &Samochody[i];
            }
            else if (F.Motocykle[i].seria().substr(3,8) == "Motocykl") {
                Motocykle[i] = F.Motocykle[i];
                Prototypy[i] = &Motocykle[i];
            }
            else {
                Rowery[i] = F.Rowery[i];
                Prototypy[i] = &Rowery[i];
            }
        }
    }
    return *this;
}

Fabryka& Fabryka::operator=(Fabryka &&F) noexcept {
    if(&F != this) {
        delete[] Motocykle;
        delete[] Samochody;
        delete[] Rowery;
        delete[] Prototypy;
        nazwa = F.nazwa;
        pojemnosc = F.pojemnosc;
        Rowery = F.Rowery;
        Motocykle = F.Motocykle;
        Samochody = F.Samochody;
        Prototypy = F.Prototypy;
        F.Prototypy = nullptr;
        F.Rowery = nullptr;
        F.Motocykle = nullptr;
        F.Samochody = nullptr;
    }
    return *this;
}

string Fabryka::wyswietl(string info, bool druk) const { //wyświetla zawartość fabryki
    auto *pojazdy = new string[pojemnosc + 1];
    string calosc = "Fabryka pojazdów " + nazwa + "\n" + "Wyprodukowane pojazdy:" + "\n"; //Nagłówek dla calosc

    for (int i = 0; i < pojemnosc; i++) {
        if ((Prototypy[i]->seria()).substr(0,2) == "PL") pojazdy[i] = Prototypy[i]->seria(); //Numery seryjne wyprodukowanych aut
        else pojazdy[i] = "...........Puste Miejsce...........";
        calosc += to_string(i + 1) + ". " + pojazdy[i] + "\n"; //funkcja zwraca tę wartość dla cout'a
    }

    pojazdy[pojemnosc] = "Powrót"; //Jako ostatnią rubrykę wyświetla opcję powrót do menu
    string fabr = "Fabryka pojazdów " + nazwa; //Nagłówek dla okienka
    if (!druk) wyswietl_ekran(pojemnosc + 1, pojazdy, fabr, move(info)); //Wydruk okienka zawartości fabryki
    delete[] pojazdy;
    return calosc;
}

unsigned int Fabryka::znajdz_puste() {
    unsigned int k = 0;
    while (Prototypy[k]->seria() != "...........Puste Miejsce...........") {
        k++;
        if (k == pojemnosc) {
            return pojemnosc;
        }
    }
    return k;
}

void Fabryka::przekaz(Komis &K) {

    wyswietl("Wybierz numer pojazdu do przeniesienia");
    int pojazd = 0;

    cin >> pojazd;
    pojazd = czy_dobra_wartosc(pojazd, pojemnosc, 1);
    Lokomocja *L = Prototypy[pojazd - 1];
    unsigned int pusty = K.znajdz_puste();

    if(L->seria().substr(3, 4) == "Auto") {
        K.Samochody[pusty] = Samochody[pojazd - 1];
        K.Prototypy[pusty] = &K.Samochody[pusty];
    }
    else if (L->seria().substr(3, 4) == "Moto") {
        K.Motocykle[pusty] = Motocykle[pojazd - 1];
        K.Prototypy[pusty] = &K.Motocykle[pusty];
    }
    else if (L->seria().substr(3, 5) == "Rower") {
        czy_ok("Komis nie może posiadać roweru!");
        return;
    }
    else {
        czy_ok("Nie można przekazać tego elementu do komisu");
        return;
    }

    usun_pojazd(pojazd - 1);

    string info[2];
    info[0] = "Przekazano do komisu " + K.nazwa + " pojazd";
    info[1] = K.Prototypy[pusty]->seria();
    czy_ok(2, info);
}

void Fabryka::przekaz(SklepRowerowy &S) {

    wyswietl("Wybierz numer roweru do przeniesienia");
    int pojazd = 0;

    cin >> pojazd;
    pojazd = czy_dobra_wartosc(pojazd, pojemnosc, 1);
    Lokomocja *L = Prototypy[pojazd - 1];
    unsigned int pusty = S.znajdz_puste();

    if(L->seria().substr(3, 4) == "Auto") {
        czy_ok("Sklep rowerowy nie może posiadać samochodu!");
        return;
    }
    else if (L->seria().substr(3, 4) == "Moto") {
        czy_ok("Sklep rowerowy nie może posiadać motocyklu!");
        return;
    }
    else if (L->seria().substr(3, 5) == "Rower") {
        S.Rowery[pusty] = Rowery[pojazd - 1];
    }
    else {
        czy_ok("Nie można przekazać tego elementu do sklepu!");
        return;
    }

    usun_pojazd(pojazd - 1);

    string info[2];
    info[0] = "Przekazano do sklepu " + S.nazwa + " rower";
    info[1] = S.Rowery[pusty].seria();
    czy_ok(2, info);
}