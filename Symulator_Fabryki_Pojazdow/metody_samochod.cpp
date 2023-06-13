//
// Created by Dawid Makowski on 29.12.2021.
//

#include "klasy_zaklady.h"

unsigned int Samochod::p = 0;

string *Samochod::Modele = new string[30] {"Polonez", "Audi", "BMW", "Citroen", "Fiat", "Ford", "Kia", "Mazda", "Peugeot", "Seat", "Skoda", "Toyota", "Volkswagen", "Volvo", "Mercedes", "Porsche"};
string Samochod::Nadwozia[N] {"Sedan", "Hatchback", "Kombi", "Kabriolet", "Pickup", "Minivan", "Limuzyna"}; //Wybór nadwozia
string Samochod::menu_zarzadzania[8] {"Jedź samochodem", "Zatankuj samochód", "Wyświetl bagażnik", "Załaduj bagażnik", "Rozładuj bagażnik", "Zmień kolor samochodu", "Wysiądź z auta", "Zarządzanie samochodem"};

unsigned int Samochod::licznik = 0;

unsigned int Samochod::dodaj_model_auta() {//dodawanie modelu zażyczonego sobie przez użytkownika
    wyswietl_info("Ile chcesz dodać nowych modeli?");
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
        Modele[M+i+p] = marka; //nowe modele są dodawane do tablicy Modele
    }
    Modele[M+n+p] = "Dodaj modele";
    return n;
}

string Samochod::dolacz_model(unsigned int &k) {
    Modele[M + p] = "Dodaj modele";
    wyswietl_ekran(M + p + 1, Modele, "Wybierz model auta"); //Wybór modelu auta
    cin >> k; //Wartość sczytywana, jeśli 17 to dodaje się nowy, zdefiniowany przez użytkownika model
    k = czy_dobra_wartosc(k, M + p + 1, 1);

    while (k == M + p + 1) {
        p += dodaj_model_auta(); //Ile dodano nowych modeli
        wyswietl_ekran(M+ p + 1, Modele, "Wybierz model auta");
        cin >> k;
        k = czy_dobra_wartosc(k, M + p + 1, 1);
    }
    return Modele[k - 1];
}

string Samochod::dolacz_nadwozie(unsigned int &k) {
    wyswietl_ekran(N, Nadwozia, "Wybierz nadwozie"); //Wybór nadwozia
    cin >> k;
    k = czy_dobra_wartosc(k, N, 1);
    return Nadwozia[k - 1];
}

Samochod::Samochod(string a, string b, string c, string d, double e, double f, unsigned long g, string h) :
        Pojazd(a, b, c, d, e, f, g, h) {//Jeśli obiekt został zdefiniowany, ustalany jest jego numer seryjny
    string smoc;
    if (moc < 100) {
        smoc = '0' + to_string((int)moc);
    }
    else {
        smoc = to_string((int)moc);
    }
    nr_seryjny = "PL_Auto" + nazwa_bez_spacji(wyprodukowano) + licznik_to_string(licznik) + "_" + model + "_" + nadwozie + "_" + kolor[0] + kolor[1] + "_" + silnik + smoc;
    licznik++;
    POJ = 30;

    if (nadwozie == "Sedan") {poj_bag = 10;}
    else if (nadwozie == "Hatchback") {poj_bag = 12;}
    else if (nadwozie == "Kombi") {poj_bag = 14;}
    else if (nadwozie == "Kabriolet") {poj_bag = 6;}
    else if (nadwozie == "Pickup") {poj_bag = 18;}
    else if (nadwozie == "Minivan") {poj_bag = 10;}
    else if (nadwozie == "Limuzyna") {poj_bag = 14;}
    else {poj_bag = 0;}
    Bagaznik = new string[poj_bag];

}

Samochod::Samochod(const Samochod &S) : Samochod(S.model, S.silnik, S.nadwozie, S.kolor, S.moc, S.cena_det, S.przebieg, S.wyprodukowano) {
    poj_bag = S.poj_bag;
    nr_seryjny = S.nr_seryjny;
    POJ = S.POJ;
    poj_bag = S.poj_bag;
    Bagaznik = new string[poj_bag];
}

Samochod::Samochod(Samochod &&S) noexcept {
    model = S.model;
    silnik = S.silnik;
    nadwozie = S.nadwozie;
    kolor = S.kolor;
    moc = S.moc;
    poj_bag = S.poj_bag;
    cena_det = S.cena_det;
    przebieg = S.przebieg;
    wyprodukowano = S.wyprodukowano;
    poj_bag = S.poj_bag;
    nr_seryjny = S.nr_seryjny;
    Bagaznik = S.Bagaznik;
    POJ = S.POJ;
    S.Bagaznik = nullptr;
}

Samochod::Samochod(string nr) {
    if(nr[4] != '.') {
        wyprodukowano = nr.substr(7, nr.find('0') - 7);
        licznik = stoi(nr.substr(7+wyprodukowano.size(), 3));
        model = nr.substr(11+wyprodukowano.size(), nr.find('_', 12+wyprodukowano.size())-11-wyprodukowano.size());
        nadwozie = nr.substr(12+wyprodukowano.size()+model.size(), nr.find('_', 13+wyprodukowano.size()+model.size())-12-wyprodukowano.size()-model.size());
        silnik = nr.substr(16+wyprodukowano.size()+model.size()+nadwozie.size(), nr.find(' ')-(16+wyprodukowano.size()+model.size()+nadwozie.size())-3);
        moc = stoi(nr.substr(16+wyprodukowano.size()+model.size()+nadwozie.size()+silnik.size(), 3));
        kolor = nr.substr(nr.find(' ') + 1, nr.find(' ', 20+wyprodukowano.size()+model.size()+nadwozie.size()+silnik.size())-nr.find(' '));
        przebieg = stoi(nr.substr(nr.find(' ', 20+wyprodukowano.size()+model.size()+nadwozie.size()+silnik.size())));
        string smoc;
        if (moc < 100) {
            smoc = '0' + to_string((int)moc);
        }
        else {
            smoc = to_string((int)moc);
        }
        nr_seryjny = "PL_Auto" + nazwa_bez_spacji(wyprodukowano) + licznik_to_string(licznik) + "_" + model + "_" + nadwozie + "_" + kolor[0] + kolor[1] + "_" + silnik + smoc;
        POJ = 30.0;

        if (nadwozie == "Sedan") poj_bag = 10;
        else if (nadwozie == "Hatchback") poj_bag = 12;
        else if (nadwozie == "Kombi") poj_bag = 14;
        else if (nadwozie == "Kabriolet") poj_bag = 6;
        else if (nadwozie == "Pickup") poj_bag = 18;
        else if (nadwozie == "Minivan") poj_bag = 10;
        else if (nadwozie == "Limuzyna") poj_bag = 14;
        else poj_bag = 0;
        Bagaznik = new string[poj_bag];
    }
    else {
        nr_seryjny = string(11, '.') + "Puste Miejsce" + string(11, '.');
        Bagaznik = new string[0];
    }
}

Samochod& Samochod::operator=(const Samochod &S) {
    if(&S != this) {
        delete[] Bagaznik;
        model = S.model;
        silnik = S.silnik;
        nadwozie = S.nadwozie;
        kolor = S.kolor;
        moc = S.moc;
        cena_det = S.cena_det;
        przebieg = S.przebieg;
        wyprodukowano = S.wyprodukowano;
        poj_bag = S.poj_bag;
        nr_seryjny = S.nr_seryjny;
        POJ = S.POJ;
        poj_bag = S.poj_bag;
        Bagaznik = new string[poj_bag];
    }
    return *this;
}

Samochod& Samochod::operator=(Samochod &&S) noexcept {
    if(&S != this) {
        delete[] Bagaznik;
        model = S.model;
        silnik = S.silnik;
        nadwozie = S.nadwozie;
        kolor = S.kolor;
        moc = S.moc;
        poj_bag = S.poj_bag;
        cena_det = S.cena_det;
        przebieg = S.przebieg;
        wyprodukowano = S.wyprodukowano;
        poj_bag = S.poj_bag;
        nr_seryjny = S.nr_seryjny;
        Bagaznik = S.Bagaznik;
        POJ = S.POJ;
        S.Bagaznik = nullptr;
    }
    return *this;
}

double Samochod::ustal_cene() {
    double cena = 0.0;
    for (int i = 0; i < 100; i++) {
        if (i < K && kolor == Kolory[i]) {
            cena += (i + 1) * 10;
        }
        if (i < M && model == Modele[i]) {
            cena += (i + 1) * 10000;
        }
        if (i < N && nadwozie == Nadwozia[i]) {
            cena += (i + 1) * 100;
        }
        if (i < S && silnik == Silniki[i]) {
            cena += (i + 1) * 1000;
        }
    }
    cena += 100.0 * pow(5.0, (double)moc / 50.0); //zależność ceny od mocy silnika
    cena /= 4;
    cena *= (1 - (double)przebieg / 600000);
    if (przebieg > 600000) return 1000.0;
    return cena;
}