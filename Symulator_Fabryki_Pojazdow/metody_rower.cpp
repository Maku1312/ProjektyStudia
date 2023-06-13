//
// Created by Dawid Makowski on 30.12.2021.
//

#include "klasy_zaklady.h"

float Rower::smar = 0.0;
unsigned int Rower::p = 0;
unsigned int Rower::licznik = 0;

string Rower::menu_zarzadzania[8] {"Jedź rowerem", "Nasmaruj rower", "Wyświetl bagażnik", "Załaduj bagażnik", "Rozładuj bagażnik", "Zmień kolor roweru", "Zejdź z roweru", "Zarządzanie rowerem"};
string *Rower::Marki = new string[30] {"Unibike", "Kross", "Cube", "Kellys", "Author"};
string Rower::Typy[5] {"miejski", "szosowy", "crossowy", "trekkingowy", "górski"};

unsigned int Rower::dodaj_marke() {//dodawanie modelu zażyczonego sobie przez użytkownika
    wyswietl_info("Ile chcesz dodac nowych modeli?");
    unsigned int n = 0;
    cin >> n;
    string marka;
    n = czy_dobra_wartosc(n, 10);

    for (int i = 0; i < n; i++)
    {
        string tekst = "Wprowadź model nr " + to_string(i+1) + ".";
        wyswietl_info(tekst); //okienko
        IG;
        getline(cin, marka);
        Marki[M+i+p] = marka; //nowe modele są dodawane do tablicy Modele
    }
    Marki[M+n+p] = "Dodaj marki";
    return n;
}

string Rower::dolacz_marke(unsigned int &k) {
    Marki[M + p] = "Dodaj marki";
    wyswietl_ekran(M + p + 1, Marki, "Wybierz markę roweru"); //Wybór modelu motocyklu
    cin >> k; //Wartość sczytywana, jeśli 17 to dodaje się nowy, zdefiniowany przez użytkownika model
    k = czy_dobra_wartosc(k, M + p + 1, 1);

    while (k == M + p + 1) {
        p += dodaj_marke(); //Ile dodano nowych modeli
        wyswietl_ekran(M+ p + 1, Marki, "Wybierz markę roweru");
        cin >> k;
        k = czy_dobra_wartosc(k, M + p + 1, 1);
    }
    return Marki[k - 1];
}

string Rower::dolacz_typ(unsigned int &k) {
    wyswietl_ekran(T, Typy, "Wybierz typ"); //Wybór nadwozia
    cin >> k;
    k = czy_dobra_wartosc(k, T, 1);
    return Typy[k - 1];
}

Rower::Rower(string a, string b, string c, double d, string e) : Lokomocja(a, c, d, e), typ(b) {
    nr_seryjny = "PL_Rower" + nazwa_bez_spacji(wyprodukowano) + licznik_to_string(licznik) + "_" + model + "_" + typ + "_" + kolor;
    licznik++;
}

Rower::Rower(string nr) {
    if(nr[4] != '.') {
        wyprodukowano = nr.substr(8, nr.find('0') - 8);
        licznik = stoi(nr.substr(8+wyprodukowano.size(), 3));
        model = nr.substr(12+wyprodukowano.size(), nr.find('_', 13+wyprodukowano.size())-12-wyprodukowano.size());
        typ = nr.substr(13+wyprodukowano.size()+model.size(), nr.find('_', 14+wyprodukowano.size()+model.size())-13-wyprodukowano.size()-model.size());
        kolor = nr.substr(14+wyprodukowano.size()+model.size()+typ.size());
        nr_seryjny = "PL_Rower" + nazwa_bez_spacji(wyprodukowano) + licznik_to_string(licznik) + "_" + model + "_" + typ + "_" + kolor;
    }
    else {
        nr_seryjny = string(11, '.') + "Puste Miejsce" + string(11, '.');
    }
}

void Rower::wyswietl_bagaznik(string nazwa, string info) {
    string widok[3];
    widok[0] = nazwa;
    widok[1] = info;
    if (bagaznik.empty()) {
        widok[2] = "...........Puste Miejsce...........";
    }
    else {
        widok[2] = bagaznik;
    }
    wyswietl_info(3, widok);
}

void Rower::zaladuj() {
    string przedmiot;
    if (bagaznik.empty() || bagaznik == "...........Puste Miejsce...........") {
        wyswietl_info("Wpisz przedmiot, który chcesz włożyć na bagażnik roweru");
        IG;
        getline(cin, przedmiot);
        string info = "W bagażniku umieszczono przedmiot " + przedmiot;
        czy_ok(info);
        bagaznik = move(przedmiot);
    }
    else {
        czy_ok("W bagażniku znajduje się już jakiś przedmiot, wpierw opróżnij bagażnik");
    }
}

void Rower::rozladuj(string* koszyk) {
    if (bagaznik.empty()) {
        czy_ok("W bagażniku nie ma żadnych przedmiotów");
    }
    else {
        try {
            unsigned int a = 0;
            while(!koszyk[a].empty()) {
                a++;
            }
            koszyk[a] = bagaznik;
        }
        catch (const out_of_range& wielkosc) {
            string info[2];
            info[0] = "Twój koszyk jest za mały,";
            info[1] = "aby wybrać wszystkie przedmioty z bagażnika";
            wyswietl_info(2, info);
            return;
        }
        string info = "Zabrano z bagażnika przedmiot " + bagaznik;
        czy_ok(info);
        bagaznik = "";
    }
}

void Rower::zatankuj(double &saldo) { //NASMARUJ
    if (zuzycie == 0) czy_ok("Rower nie wymaga nasmarowania");
    else {
        if (smar == 0) {
            if(tak_czy_nie("Nie masz wystarczającej ilości smaru", "Czy chcesz kupić smar?")) {
                kup_smar(saldo);
            }
            else {
                return;
            }
        }
        if (smar > zuzycie) {
            smar -= zuzycie;
            zuzycie = 0;
        }
        else {
            zuzycie -= smar;
            smar = 0;
        }
        czy_ok("Rower został nasmarowany");
    }
}

void Rower::jazda(double &saldo) {
    if (zuzycie == 1) { //nie pozwoli na jazdę z tak zużytym łańcuchem
        czy_ok("Niestety, twój rower nie jest nasmarowany i nigdzie nie ma zamiaru jechać");
        return;
    }
    wyswietl_info("Ile chcesz przejechać dzisiaj kilometrów?");
    int km = 0;
    cin >> km;
    km = czy_dobra_wartosc(km, 150);
    wyswietl_info("No to jedziemy!"); //Jedziemy!
    while (km > 0) { //Rozpoczynamy jazdę
        if (zuzycie > 0.99) { //Zawsze wieziemy ze sobą smar
            if(tak_czy_nie("Wychodzi na to, że łańcuch się zaciął i rower nie chce jechać", "Czy chcesz nasmarować rower?")) {
                zatankuj(saldo);
            }
            else {
                return;
            }
        }
        else {
            km--; //odlicza kilometry do końca trasy
            zuzycie += 0.01;
            if (!(km%10)) {
                string info = "Do celu pozostało " + to_string(km) + "km";
                wyswietl_info(info);
            }
        }
    }
    czy_ok("Dojechaliśmy!");
}

double Rower::ustal_cene() {
    double cena = 0.0;
    for (int i = 0; i < 100; i++) {
        if (i < K && kolor == Kolory[i]) {
            cena += (i + 1) * 10;
        }
        if (i < T && typ == Typy[i]) {
            cena += (i + 1) * 500;
        }
        if (i < M && model == Marki[i]) {
            cena += (i + 1) * 100;
        }
    }
    cena /= 4;
    return cena;
}

void Rower::kup_smar(double &saldo) {
    float ile = 0.0;
    wyswietl_info("Ile ml smaru chcesz kupić?");
    cin >> ile;
    ile = czy_dobra_wartosc(ile, 1000);
    smar += ile / 10;
    if(przelew(saldo, ile * 0.05, "Nie masz wystarczających środków, aby kupić smar")) {
        string info = "Zakupiono " + prec(ile, 2) + "ml smaru za " + prec(ile * 0.05, 2) + "zł";
        czy_ok(info);
    }
    else return;
}