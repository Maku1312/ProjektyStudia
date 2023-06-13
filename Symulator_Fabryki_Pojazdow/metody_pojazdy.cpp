//
// Created by Dawid Makowski on 03.12.2021.
//

#include "klasy_zaklady.h"

string Pojazd::Silniki[S] {"Diesel", "Benzyna", "CNG", "LPG", "Hybryda", "Elektryk"}; //Wybór silników

double Pojazd::rodzaj_silnika_to_mnoznik(const string &silnik) { //określa proporcjonalne ceny paliw z przymrużeniem oka
    double j = 0.0;
    if (silnik == "Benzyna") {
        j = 6.30;
    }
    else if (silnik == "Diesel") {
        j = 6;
    }
    else if (silnik == "CNG") {
        j = 4.5;
    }
    else if (silnik == "LPG") {
        j = 3.5;
    }
    else if (silnik == "Hybryda") {
        j = 3.2;
    }
    else if (silnik == "Elektryk") {
        j = 3;
    }
    return j;
}

string Pojazd::dolacz_silnik(unsigned int &k) {
    wyswietl_ekran(S, Silniki, "Wybierz silnik");
    cin >> k;
    k = czy_dobra_wartosc(k, S, 1);
    return Silniki[k - 1];
}

void Pojazd::okresl_moc(double &moc) {
    wyswietl_info("Wprowadź moc silnika, większa moc spowoduje naliczenie dodatkowych kosztów."); //Określenie mocy silnika
    cin >> moc;
    moc = czy_dobra_wartosc(moc, 400, 50, "Błąd! Nie stać nas na takie silniki!");
}

Pojazd::Pojazd(string a, string b, string c, string d, double x, double e, unsigned long p, string F) :
        Lokomocja(a, d, e, F), silnik(b), nadwozie(c), moc(x), przebieg(p) {}

Pojazd::Pojazd() :
        Lokomocja(), silnik(), nadwozie(), moc(100), przebieg(0) {
    Bagaznik = new string[0];

    nr_seryjny = "...........Puste Miejsce...........";
}

void Pojazd::zatankuj(double &saldo) { //Funkcja tankujące wybrane auto

    string tank[2];
    string przelej;
    string brak;
    double ile;
    double max;
    string info[2];

    if (silnik == "Hybryda" || silnik == "Elektryk") {
        tank[0] = "Ile procent baterii naładować?";
        tank[1] = "Aktualny stan baterii: " + prec(((stan / POJ) * 100), 2) + "%";
        przelej = "Błąd! Za mało środków, aby naładować tyle procent baterii";
        brak = "Błąd! Nie możesz naładować tyle procent baterii!";
        max = ((POJ - stan) / POJ ) * 100;
    }
    else {
        tank[0] = "Ile litrów zatankować?";
        tank[1] = "Możesz zatankować maksymalnie " + prec(POJ, 2) + " litrów.";
        przelej = "Błąd! Za mało środków, aby zatankować taką ilość paliwa";
        brak = "Błąd! W baku nie ma tyle miejsca na taką ilość paliwa.";
        max = POJ - stan;
    }

    wyswietl_info(2, tank); //okienko, nie możemy zatankować więcej niż określona ilość

    cin >> ile;
    ile = czy_dobra_wartosc(ile, max, 0, brak);
    if (silnik == "Hybryda" || silnik == "Elektryk") ile = (ile / 100) * POJ;

    double cena = rodzaj_silnika_to_mnoznik(silnik); //obliczanie ceny paliwa / prądu
    cena *= ile * MNOZ;

    if (przelew(saldo, cena, przelej)) {
        stan += ile; //tankowanie paliwa
        if (silnik == "Hybryda" || silnik == "Elektryk") info[0] = "Pozostało " + prec(((stan / POJ) * 100), 2) + "% baterii";
        else info[0] = "W baku znajduje się " + prec(stan, 2) + " litrów paliwa";
        info[1] = "Saldo po wykonaniu transakcji wynosi " + prec(saldo, 2) + " zł";
        czy_ok(2, info);
    }
}

void Pojazd::jazda(double &saldo) { //funkcja do jazdy wybranego przez użytkownika auta

    string brak[2];
    string koniec[2];
    string info[2];

    if (silnik == "Hybryda" || silnik == "Elektryk") {
        brak[0] = "Niestety, bateria w pojeździe się rozładowała";
        brak[1] = "Musisz ją najpierw naładować";
        koniec[0] = "Niestety, bateria w pojeździe się rozładowała";
        koniec[1] = "Czy chcesz naładować pojazd?";
    }
    else {
        brak[0] = "Niestety nie masz wystarczającej ilości paliwa, aby gdzieś pojechać.";
        brak[1] = "Wpierw musisz zatankować pojazd.";
        koniec[0] = "Niestety nie masz wystarczającej ilości paliwa, aby pojechać dalej.";
        koniec[1] = "Czy chcesz zatankować pojazd?";
    }

    if (stan < 0.5) { //nie pozwoli na jazdę z tak małą ilością paliwa / prądu
        czy_ok(2, brak);
        return;
    }
    wyswietl_info("Ile chcesz przejechać dzisiaj kilometrów?");
    int km = 0;
    int licz = 0;
    cin >> km;
    km = czy_dobra_wartosc(km, 1000);
    wyswietl_info("No to jedziemy!"); //Jedziemy!
    while (km > 0) { //Rozpoczynamy jazdę, jeśli skończy nam się paliwo, zostaniemy o tym niezwłocznie poinformowani
        if (stan < 0.5) { //Na szczęście wszędzie jest stacja benzynowa
            if(tak_czy_nie(koniec[0], koniec[1])) {
                zatankuj(saldo);
            }
            else {
                aktualizuj(licz);
                return;
            }
        }
        else {
            licz++;
            km--; //odlicza kilometry do końca trasy
            stan -= 0.001*moc; //odejmuje paliwo, im większa moc silnika, tym szybciej schodzi
            if (!(km%10)) {
                string infokm = "Do celu pozostało " + to_string(km) + "km";
                wyswietl_info(infokm);
            }
        }
    }
    info[0] = "Dojechaliśmy!";
    if (silnik == "Hybryda" || silnik == "Elektryk") info[1] = "Pozostało " + prec(((stan / POJ) * 100), 2) + "% baterii";
    else info[1] = "W baku pozostało " + prec(stan, 2) + " litrów paliwa.";
    czy_ok(2, info);
    aktualizuj(licz);
}

void Pojazd::wyswietl_bagaznik(string nazwa, string info) {
    auto *widok = new string[poj_bag];
    for (int i = 0; i < poj_bag; i++) {
        if (Bagaznik[i].empty()) {
            widok[i] = "...........Puste Miejsce...........";
        }
        else {
            widok[i] = Bagaznik[i];
        }
    }
    wyswietl_ekran(poj_bag, widok, nazwa, info);
    delete[] widok;
}

void Pojazd::zaladuj() {
    int i = 0;
    while (!(Bagaznik[i].empty() || Bagaznik[i] == "...........Puste Miejsce...........")) {
        i++;
        if (i == poj_bag) {
            czy_ok("Bagażnik pełny! Nie można dodawać więcej przedmiotów");
            return;
        }
    }
    do {
        wyswietl_bagaznik("Stan bagażnika", "Podaj nazwę przedmiotu, który chcesz włożyć do bagażnika");
        string przedmiot;
        IG;
        getline(cin, przedmiot);
        int w = 0;
        while (!(Bagaznik[i].empty() || Bagaznik[i] == "...........Puste Miejsce...........")) {
            w++;
            if (w == poj_bag) {
                czy_ok("Bagażnik pełny! Nie można dodawać więcej przedmiotów");
                return;
            }
        }
        Bagaznik[i] = move(przedmiot);
        string info = "Dodano " + Bagaznik[i] + " na " + to_string(i + 1) + " miejsce w bagażniku";
        i++;
        if (i == poj_bag) {
            string koniec[2];
            koniec[0] = info;
            koniec[1] = "Załadunek zakończony, brak miejsca";
            czy_ok(2, koniec);
            return;
        }
        else czy_ok(info);
    } while (tak_czy_nie("Czy chciałbyś jeszcze dodać jakieś przedmioty?"));
}

void Pojazd::rozladuj(string* koszyk) {
    unsigned ile = 0; //Ile przedmiotów użytkownik chce wziąć?
    unsigned int ile_npst = 0; //Ile przedmiotów w bagażniku?
    for (int i = 0; i < poj_bag; i++) {
        if (!Bagaznik[i].empty()) {
            ile_npst++;
        }
    }

    if (!ile_npst) { //Jeśli bagażnik jest pusty
        czy_ok("Nie możesz rozładować pustego bagażnika!");
        return;
    }

    wyswietl_bagaznik(nr_seryjny, "Ile przedmiotów chcesz zabrać?");
    cin >> ile;
    ile = czy_dobra_wartosc(ile, ile_npst, 0);
    for (int i = 0; i < ile; i++) {
        wyswietl_bagaznik(nr_seryjny, "Stan bagażnika");
        int w = 0;
        do {
            wyswietl_info("Wybierz przedmiot, który chcesz wyjąć z bagażnika");
            cin >> w;
            w = czy_dobra_wartosc(w, poj_bag, 1);
            if (Bagaznik[w - 1].empty()) czy_ok("To miejsce jest puste!");
        } while (Bagaznik[w - 1].empty());
        try {
            unsigned int a = 0;
            while(!koszyk[a].empty()) {
                a++;
            }
            koszyk[a] = Bagaznik[w - 1];
        }
        catch (const out_of_range& wielkosc) {
            string info[2];
            info[0] = "Twój koszyk jest za mały,";
            info[1] = "aby wybrać wszystkie przedmioty z bagażnika";
            wyswietl_info(2, info);
            return;
        }
        string ok = "Zabrano z bagażnika przedmiot " + Bagaznik[w - 1];
        czy_ok(ok);
        Bagaznik[w - 1] = "";
    }
}