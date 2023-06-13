//
// Created by Dawid Makowski on 02.01.2022.
//

#include "klasy_zaklady.h"

Komis::Komis(string nazwa, unsigned int pojemnosc, float m) : Zaklad(move(nazwa), pojemnosc), margin(m) {
    Prototypy = new Pojazd*[pojemnosc];
    Motocykle = new Motocykl[pojemnosc];
    Samochody = new Samochod[pojemnosc];
    for (int i = 0; i < pojemnosc; i++) {
        Prototypy[i] = &Samochody[i];
    }
}

Komis::Komis(const Komis& K) {
    nazwa = K.nazwa;
    pojemnosc = K.pojemnosc;
    margin = K.margin;
    Prototypy = new Pojazd*[pojemnosc];
    Motocykle = new Motocykl[pojemnosc];
    Samochody = new Samochod[pojemnosc];
    for (int i = 0; i < pojemnosc; i++) {
        if (K.Samochody[i].seria().substr(3,4) == "Auto") {
            Samochody[i] = K.Samochody[i];
            Prototypy[i] = &Samochody[i];
        }
        else {
            Motocykle[i] = K.Motocykle[i];
            Prototypy[i] = &Motocykle[i];
        }
    }
}

Komis::Komis(Komis&& K) noexcept {
    nazwa = K.nazwa;
    pojemnosc = K.pojemnosc;
    margin = K.margin;
    Prototypy = K.Prototypy;
    Motocykle = K.Motocykle;
    Samochody = K.Samochody;
    K.Prototypy = nullptr;
    K.Samochody = nullptr;
    K.Motocykle = nullptr;
}

ostream& operator<<(ostream& os, const Komis& K) { //przeładowanie "cout" dla Komisu korzysta z funkcji wyswietl()
    os << K.Komis::wyswietl("Posiadane pojazdy", true);
    return os;
}

Komis& Komis::operator=(const Komis &K) {
    if(&K != this) {
        delete[] Prototypy;
        delete[] Motocykle;
        delete[] Samochody;
        nazwa = K.nazwa;
        pojemnosc = K.pojemnosc;
        margin = K.margin;
        Prototypy = new Pojazd*[pojemnosc];
        Motocykle = new Motocykl[pojemnosc];
        Samochody = new Samochod[pojemnosc];
        for (int i = 0; i < pojemnosc; i++) {
            if (K.Samochody[i].seria().substr(3,4) == "Auto") {
                Samochody[i] = K.Samochody[i];
                Prototypy[i] = &Samochody[i];
            }
            else {
                Motocykle[i] = K.Motocykle[i];
                Prototypy[i] = &Motocykle[i];
            }
        }
    }
    return *this;
}

Komis& Komis::operator=(Komis &&K) noexcept {
    if(&K != this) {
        delete[] Prototypy;
        delete[] Motocykle;
        delete[] Samochody;
        nazwa = K.nazwa;
        pojemnosc = K.pojemnosc;
        margin = K.margin;
        Motocykle = K.Motocykle;
        Samochody = K.Samochody;
        Prototypy = K.Prototypy;
        K.Prototypy = nullptr;
        K.Motocykle = nullptr;
        K.Samochody = nullptr;
    }
    return *this;
}

string Komis::wyswietl(string info, bool druk) const {
    auto *pojazdy = new string[pojemnosc + 1];
    string calosc = "Komis " + nazwa + "\n" + "Posiadane pojazdy:" + "\n"; //Nagłówek dla "calosc"

    for (int i = 0; i < pojemnosc; i++) {
        if ((Prototypy[i]->seria()).substr(0,2) == "PL") pojazdy[i] = Prototypy[i]->seria(); //Numery seryjne wyprodukowanych aut
        else pojazdy[i] = "...........Puste Miejsce...........";
        calosc += to_string(i + 1) + ". " + pojazdy[i] + "\n"; //funkcja zwraca tę wartość dla cout'a
    }

    pojazdy[pojemnosc] = "Powrót"; //Jako ostatnią rubrykę wyświetla opcję powrót do menu
    string fabr = "Komis " + nazwa; //Nagłówek dla okienka
    if (!druk) wyswietl_ekran(pojemnosc + 1, pojazdy, fabr, move(info)); //Wydruk okienka zawartości komisu
    delete[] pojazdy;
    return calosc;
}

unsigned int Komis::znajdz_puste() {
    unsigned int k = 0;
    while (Prototypy[k]->seria() != "...........Puste Miejsce...........") {
        k++;
        if (k == pojemnosc) {
            return pojemnosc;
        }
    }
    return k;
}

void Komis::cofnij_licznik(unsigned int i) { //50% prawdopodobieństwa, że przebieg zmniejszy się dwukrotnie
    bool krec = pobierz_los(0,1);
    if (krec) {
        Prototypy[i]->przebieg /= 2;
    }
}

void Komis::sprzedaz(double &saldo) { //sprzedaje wybrany pojazd z komisu

    wyswietl("Podaj pozycję pojazdu, który chcesz sprzedać");

    int i = 0;
    cin >> i;
    i = czy_dobra_wartosc(i, pojemnosc, 1);

    string sprzedaz[2];
    sprzedaz[0] = "Czy chcesz sprzedać " + Prototypy[i - 1]->seria();
    sprzedaz[1] = " z pozycji nr " + to_string(i) + "?";
    double cena;
    if(tak_czy_nie(sprzedaz[0], sprzedaz[1])) {
        cofnij_licznik(i - 1);
        cena = Prototypy[i - 1]->ustal_cene();
        cena *= margin;
        if (Prototypy[i - 1]->seria().substr(3,4) == "Auto") Samochody[i - 1] = Samochod();
        else if (Prototypy[i - 1]->seria().substr(3,8) == "Motocykl") Motocykle[i - 1] = Motocykl();
        else {
            czy_ok("Nie można sprzedać pojazdu!");
            return; //Jeśli nr seryjny pojazdu jest niewłaściwy, nie dokonuje się jego sprzedaży
        }
    }
    else return; //Jeśli użytkownik nie chce sprzedać pojazdu to program wraca do menu
    saldo += cena;
    string komunikat[2];
    komunikat[0] = "Sprzedano pojazd za " + to_string((int)cena) + "zł";
    komunikat[1] = "Obecne saldo wynosi " + to_string((int)saldo) + "zł";
    czy_ok(2, komunikat); //okienko
}

Samochod Komis::kup_auto(double &saldo, bool &czy) { //generuje pseudolosowy samochód i pyta o jego kupno
    do {
        unsigned int tab[4];
        tab[0] = pobierz_los(0, Samochod::M + Samochod::p);
        tab[1] = pobierz_los(0, Samochod::S);
        tab[2] = pobierz_los(0, Samochod::N);
        tab[3] = pobierz_los(0, Samochod::K + Samochod::r);
        double a = pobierz_los(70, 250);
        unsigned long przeb = pobierz_los(5000, 500000);
        Samochod Kupny(Samochod::Modele[tab[0]], Samochod::Silniki[tab[1]], Samochod::Nadwozia[tab[2]], Samochod::Kolory[tab[3]], a, 0, przeb);
        double cena = Kupny.ustal_cene();
        Kupny.cena_det = cena;
        string s[2];
        s[0] = "Czy chcesz kupić samochód " + Kupny.seria() + " za " + to_string((int)cena) + "zł?";
        s[1] = "Przebieg = " + to_string(przeb) + "km";
        if (tak_czy_nie(s[0], s[1])) {
            saldo -= cena;
            string info = "Zakupiono pojazd " + Kupny.seria();
            wyswietl_info(info);
            czy = true;
            return Kupny;
        }
        Kupny = Samochod();
    } while(!tak_czy_nie("Czy chcesz zakończyć operację kupna?"));
    Samochod Kupny;
    return Kupny;
}

Motocykl Komis::kup_motor(double &saldo, bool &czy) { //generuje pseudolosowy motocykl i pyta o jego kupno
    do {
        unsigned int tab[4];
        tab[0] = pobierz_los(0, Motocykl::M + Motocykl::p);
        tab[1] = pobierz_los(0, Motocykl::S);
        tab[2] = pobierz_los(0, Motocykl::T);
        tab[3] = pobierz_los(0, Motocykl::K + Motocykl::r);
        double a = pobierz_los(70, 250);
        unsigned long przeb = pobierz_los(5000, 500000);
        Motocykl Kupny(Motocykl::Modele[tab[0]], Motocykl::Silniki[tab[1]], Motocykl::Typy[tab[2]], Motocykl::Kolory[tab[3]], a, 0, przeb);
        double cena = Kupny.ustal_cene();
        Kupny.cena_det = cena;
        string s[2];
        s[0] = "Czy chcesz kupić pojazd " + Kupny.seria() + " za " + to_string((int)cena) + "zł?";
        s[1] = "Przebieg = " + to_string(przeb) + "km";
        if (tak_czy_nie(s[0], s[1])) {
            saldo -= cena;
            string info = "Zakupiono pojazd " + Kupny.seria();
            wyswietl_info(info);
            czy = true;
            return Kupny;
        }
        Kupny = Motocykl();
    } while(!tak_czy_nie("Czy chcesz zakończyć operację kupna?"));
    Motocykl Kupny;
    return Kupny;
}