//
// Created by Dawid Makowski on 30.11.2021.
//

#include "klasy_zaklady.h"

Zaklad::Zaklad(string a, unsigned int b) : nazwa(move(a)), pojemnosc(b) { //Konstruktor Fabryki
    Prototypy = new Lokomocja*[pojemnosc]; //Tablica wskaźników
    for (int i = 0; i < pojemnosc; i++) {
        Prototypy[i] = nullptr;
    }
}

Zaklad::Zaklad(const Zaklad &Z) : nazwa(Z.nazwa), pojemnosc(Z.pojemnosc) { //Konstruktor kopiujący
    Prototypy = new Lokomocja*[pojemnosc];
    for (int i = 0; i < pojemnosc; i++) {
        Prototypy[i] = Z.Prototypy[i];
    }
}

void Lakiernik::przelakieruj(Lokomocja *L, double &saldo) const {

    wyswietl_info("Wybierz nowy kolor pojazdu");

    Lokomocja::Kolory[Lokomocja::K + Lokomocja::r] = "Dodaj kolory";
    wyswietl_ekran(Lokomocja::K + Lokomocja::r + 1, Lokomocja::Kolory, "Wybierz kolor"); //Ekran wyboru koloru
    int w;
    cin >> w;
    w = czy_dobra_wartosc(w, Lokomocja::K + Lokomocja::r + 1, 1);

    while (w == Lokomocja::K + Lokomocja::r + 1) {
        Lokomocja::r += Lokomocja::dodaj_kolor(); //Ile dodano nowych kolorów
        wyswietl_ekran(Lokomocja::K + Lokomocja::r + 1, Lokomocja::Kolory, "Wybierz kolor");
        cin >> w;
        w = czy_dobra_wartosc(w, Lokomocja::K + Lokomocja::r + 1, 1);
    }

    double cena = w * staz;

    if(przelew(saldo, cena, "Nie udało się zmienić koloru pojazdu! Za mało środków na koncie.")) {
        string przed = L->kolor; //Kolor przed zmianą

        if (przed[przed.length() - 1] == ' ') przed.pop_back();
        if (przed[przed.length() - 1] == 'y') przed.pop_back();//Właściwa odmiana kolorów (kogo / czego)

        L->kolor = Lokomocja::Kolory[w - 1]; //Zmiana koloru

        string s[3];
        s[0] = "Zmieniono kolor pojazdu z " + przed + "ego na " + L->kolor;
        s[1] = "Koszt operacji wyniósł " + prec(cena, 2) + "zł";
        s[2] = "Saldo po wykonaniu transakcji wynosi " + prec(saldo, 2) + "zł";
        czy_ok(3, s);
    }
}