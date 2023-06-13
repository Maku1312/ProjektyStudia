//
// Created by Dawid Makowski on 30.11.2021.
//

#include "klasy_zaklady.h"

void Fabryka::praca_start(int ile, bool &czy_pojazd, bool &czy_rower, double &saldo) { //funkcja operuje na funkcji konstrukcji aut, wykonując ją określoną ilość razy
    int x = 0; //ile pojazdów udało się wyprodukować
    for (int i = 0; i < ile; i++) //ile-ile pojazdów użytkownik chce wyprodukować
    {
        unsigned int puste = znajdz_puste();
        if (puste >= pojemnosc) {
            string tab[2];
            tab[0] = "Fabryka jest pełna";
            tab[1] = "Zanim wyprodukujesz kolejne pojazdy, wpierw przekaż je do komisu";
            wyswietl_info(2, tab);
            return;
        }
        Lokomocja *L;
        double saldo_przed = saldo;
        string wybor[3] = {"Samochód", "Motocykl", "Rower"};
        int w = 0;
        wyswietl_ekran(3, wybor, "Wybierz pojazd, który chcesz wyprodukować");
        cin >> w;
        w = czy_dobra_wartosc(w, 3, 1);
        switch(w) {
            case 1: {
                Samochody[puste] = Samochod::konstrukcja(saldo, nazwa);
                L = &Samochody[puste];
                czy_pojazd = true;
                break;
            }
            case 2: {
                Motocykle[puste] = Motocykl::konstrukcja(saldo, nazwa);
                L = &Motocykle[puste];
                czy_pojazd = true;
                break;
            }
            case 3: {
                Rowery[puste] = Rower::konstrukcja(saldo, nazwa);
                L = &Rowery[puste];
                czy_rower = true;
                break;
            }
            default: {
                czy_ok("Błąd! Podano zły wybór");
                L = &Rowery[puste];
                break;
            }
        }
        Prototypy[puste] = L;
        string info[5];
        info[0] = "Wyprodukowano pojazd:";
        info[1] = L->seria(); //Wyświetlenie nr_seryjnego nowego pojazdu
        info[2] = "Pojazd dodano na " + to_string(puste + 1) + " miejsce w fabryce"; //Informacja gdzie dodano pojazd
        info[3] = "Koszt produkcji pojazdu wynosił " + prec(saldo_przed - saldo, 2) + "zł";
        info[4] = "Obecne saldo wynosi " + prec(saldo, 2) + "zł"; //Pokazuje ile zostało środków po wyprodukowaniu danego pojazdu
        czy_ok(5,info);
        x++;
    }
    //Jeśli produkowane było więcej niż jeden pojazd, to wyświetla się komunikat końcowy o pomyślnym wyprodukowaniu 'ile' pojazdów
    if (ile > 1 && ( ile % 10 == 2 || ile % 10 == 3 || ile % 10 == 4)) {
        string t = "Udało się wyprodukować " + to_string(x) + " pojazdy";
        czy_ok(t);
    }
    else if (ile > 1) {
        string t = "Udało się wyprodukować " + to_string(x) + " pojazdów";
        czy_ok(t);
    }
}

Fabryka tworz_fabryke() {
    string nazwa; //nazwa fabryki
    int pojemnosc = 0; //pojemność fabryki
    wyswietl_info("Wprowadź nazwę fabryki");
    IG;
    getline(cin, nazwa);
    wyswietl_info("Jaką pojemność ma mieć fabryka?");
    cin >> pojemnosc;
    pojemnosc = czy_dobra_wartosc(pojemnosc, 100, 1);
    Fabryka Nowa(nazwa, pojemnosc);
    return Nowa; //funkcja zwraca stworzoną fabrykę
}

Komis tworz_komis() {
    string nazwa; //nazwa komisu
    int pojemnosc = 0; //pojemność komisu
    float marza = 1.0; //marża komisu
    wyswietl_info("Wprowadź nazwę komisu");
    IG;
    getline(cin, nazwa);
    wyswietl_info("Jaką pojemność ma mieć komis?");
    cin >> pojemnosc;
    pojemnosc = czy_dobra_wartosc(pojemnosc, 100, 1);
    wyswietl_info("Podaj marżę komisu (od 1 do 1.5)");
    cin >> marza;
    marza = czy_dobra_wartosc(marza, 1.5, 1.0);
    Komis Nowy(nazwa, pojemnosc, marza); //Tworzy komis
    return Nowy; //funkcja zwraca stworzony komis
}

SklepRowerowy tworz_sklep() {
    string nazwa; //nazwa sklepu
    int pojemnosc = 0; //pojemność sklepu
    float marza = 1.0; //marża sklepu
    wyswietl_info("Wprowadź nazwę sklepu");
    IG;
    getline(cin, nazwa);
    wyswietl_info("Jaką pojemność ma mieć sklep?");
    cin >> pojemnosc;
    pojemnosc = czy_dobra_wartosc(pojemnosc, 100, 1);
    wyswietl_info("Podaj marżę sklepu (od 1 do 1.5)");
    cin >> marza;
    marza = czy_dobra_wartosc(marza, 1.5, 1.0);
    SklepRowerowy Nowy(nazwa, pojemnosc, marza);
    return Nowy;
}