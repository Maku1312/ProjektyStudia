//
// Created by Dawid Makowski on 29.12.2021.
//

#include "klasy_zaklady.h"

//Plik zawiera funkcje wyświetlające menu zarządzania danym zakładem lub pojazdem
//W każdej funkcji, znajdują się instrukcje switch/case, które automatycznie przekierowują użytkownika do wybranej opcji (funkcji)
//funkcja, zarządzaj_pojazdem działa na wskaźniku do każdego typu pojazdu, każda klasa pojazdu ma swoją statyczną tablicę menu_wyboru, z której ta funkcja korzysta
//Każda funkcja, musi też sprawdzić, czy istnieje obiekt danego typu, jeśli chce się wykonać na nim jakąś akcję, po to są przysyłane te bool'e

void Fabryka::zarzadzaj(bool &czy_pojazd, bool &czy_rower, double &saldo, vector<Komis> &Targ, vector<SklepRowerowy> &Market) { //kolejne menu wyboru, po wybraniu z menu głównego
    string nazwaF = "Zarządzanie fabryką " + nazwa;
    //Opcje zarządzania fabryką
    string zarzadzaj[6] = {"Wyświetl stan fabryki", "Wyprodukuj pojazdy", "Przekaż pojazd do komisu", "Przekaż rower do sklepu", "Eksportuj fabrykę", "Wyjdź z fabryki"};
    int wybierz = 0;
    wyswietl_ekran(6, zarzadzaj, nazwaF, "Wybierz akcję");
    while (wybierz != 6) {
        cin >> wybierz;
        wybierz = czy_dobra_wartosc(wybierz, 6, 1);
        switch(wybierz) {
            case 1: { //wyświetl stan fabryki
                wyswietl();

                int powrot = 0;
                cin >> powrot;
                string info = "Naciśnij " + to_string(pojemnosc + 1) + " aby wrócić do menu zarządzania";
                powrot = czy_dobra_wartosc(powrot, pojemnosc + 1, pojemnosc + 1, info);

                wyswietl_ekran(6, zarzadzaj, nazwaF, "Wybierz akcję");
                break;
            }
            case 2: { //Produkcja samochodów
                if (ile_pojazdow() >= pojemnosc) { //Sprawdza, czy w fabryce jest miejsce na nowe pojazdy
                    wyswietl_ekran(6, zarzadzaj, nazwaF, "W fabryce nie ma miejsca na nowe pojazdy, wybierz inną akcję");
                }
                else {
                    int auta = 0;
                    wyswietl_info("Ile pojazdów chcesz wyprodukować?");
                    cin >> auta;
                    auta = czy_dobra_wartosc(auta, pojemnosc - ile_pojazdow(), 0, "W fabryce nie ma miejsca na taką liczbę pojazdów!");

                    praca_start(auta, czy_pojazd, czy_rower, saldo); //Tu się konstruuje pojazdy

                    wyswietl_ekran(6, zarzadzaj, nazwaF, "Wybierz akcję");
                }
                break;
            }
            case 3: { //Przekaż pojazd do komisu
                if (czy_pojazd) {
                    unsigned int k = Targ.size(); //maksymalna liczba komisów
                    unsigned int l = 0; //licznik komisów

                    string tabk[k];
                    for (int i = 0; i < k; i++) {
                        if (Targ[i]._nazwa() != "Nieznany") { //Jeśli nazwa komisu wynosi "Nieznany", wówczas komis nie istnieje
                            tabk[i] = "Komis " + Targ[i]._nazwa();
                            l++;
                        }
                    }
                    if (l != 0) { //Jeśli istnieje jakikolwiek komis program przekierowuje nas do wyboru komisu
                        wyswietl_ekran(l, tabk, "Lista komisów", "Wybierz komis, do którego chcesz przenieść pojazd"); //Ekran wyboru komisów

                        int komis = 0;
                        cin >> komis;
                        komis = czy_dobra_wartosc(komis, l, 1);

                        przekaz(Targ[komis - 1]);
                        wyswietl_ekran(6, zarzadzaj, nazwaF, "Wybierz akcję");
                    }
                    else { //Jeśli żaden komis nie istnieje następuje powrót do menu zarządzania fabryką
                        wyswietl_ekran(6, zarzadzaj, nazwaF, "Żaden komis nie istnieje! Wybierz inną akcję");
                    }
                }
                else {
                    wyswietl_ekran(6, zarzadzaj, nazwaF, "Brak pojazdów do przesłania! Wybierz inną akcję");
                }
                break;
            }
            case 4: { //Przekaż rower do sklepu
                if (czy_rower) {
                    unsigned int s = Market.size(); //maksymalna liczba rowerów
                    unsigned int l = 0; //licznik sklepów

                    string tabs[s];
                    for (int i = 0; i < s; i++) {
                        if (Market[i]._nazwa() != "Nieznany") { //Jeśli nazwa komisu wynosi "Nieznany", wówczas komis nie istnieje
                            tabs[i] = "Sklep " + Market[i]._nazwa();
                            l++;
                        }
                    }
                    if (l != 0) { //Jeśli istnieje jakikolwiek komis program przekierowuje nas do wyboru komisu
                        wyswietl_ekran(l, tabs, "Lista sklepów", "Wybierz sklep, do którego chcesz przenieść pojazd"); //Ekran wyboru komisów

                        int sklep = 0;
                        cin >> sklep;
                        sklep = czy_dobra_wartosc(sklep, l, 1);

                        przekaz(Market[sklep - 1]);
                        wyswietl_ekran(6, zarzadzaj, nazwaF, "Wybierz akcję");
                    }
                    else { //Jeśli żaden sklep nie istnieje następuje powrót do menu zarządzania fabryką
                        wyswietl_ekran(6, zarzadzaj, nazwaF, "Żaden sklep nie istnieje! Wybierz inną akcję");
                    }
                }
                else {
                    wyswietl_ekran(6, zarzadzaj, nazwaF, "Brak rowerów do przesłania! Wybierz inną akcję");
                }
                break;
            }
            case 5: { //Eksport
                string plik;
                string eksport[2];
                eksport[0] = "Czy chcesz eksportować fabrykę do pliku o nazwie";
                eksport[1] = "fabryka_" + nazwa_bez_spacji(nazwa) + ".txt";
                if(tak_czy_nie(eksport[0], eksport[1])) plik = "fabryka_" + nazwa_bez_spacji(nazwa) + ".txt";
                else {
                    wyswietl_info("Podaj nazwę pliku wraz z rozszerzeniem");
                    IG;
                    getline(cin, plik);
                }
                eksportuj(plik); //eksportuj fabrykę

                wyswietl_ekran(6, zarzadzaj, nazwaF, "Wybierz akcję");
                break;
            }
            case 6: {
                break; //Wraca do menu głównego
            }
            default: {
                wyswietl_info("Błąd techniczny! Wracam do menu głównego");
                Sleep(5000);
                wybierz = 6;
                break;
            }
        }
    }
}

void Komis::zarzadzaj(bool &czy_pojazd, double &saldo, string* koszyk) { //kolejne menu wyboru, po wybraniu z menu głównego
    string nazwaK = "Zarządzanie komisem " + nazwa;
    //Opcje zarządzania komisem
    string zarzadzaj[6] = {"Wyświetl stan komisu", "Sprzedaj pojazd", "Kup pojazd", "Zarządzaj pojazdem", "Eksportuj komis", "Wyjdź z komisu"};
    int wybierz = 0;
    wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
    while (wybierz != 6) {
        cin >> wybierz;
        wybierz = czy_dobra_wartosc(wybierz, 6, 1);
        switch(wybierz) {
            case 1: {
                wyswietl(); //wyświetl stan komisu

                int powrot = 0;
                cin >> powrot;
                string info = "Naciśnij " + to_string(pojemnosc + 1) + " aby wrócić do menu zarządzania";
                powrot = czy_dobra_wartosc(powrot, pojemnosc + 1, pojemnosc + 1, info);

                wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
                break;
            }
            case 2: {
                sprzedaz(saldo); //sprzedaj pojazdy
                wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
                break;
            }
            case 3: { //Kup pojazd
                string wybor[2] = {"Samochód", "Motocykl"};
                wyswietl_ekran(2, wybor, "Jaki typ pojazdu chcesz zakupić?");
                int w = 0;
                cin >> w;
                w = czy_dobra_wartosc(w, 2, 1);
                unsigned int pusty = znajdz_puste();
                if (pusty == pojemnosc) {
                    czy_ok("W komisie nie ma miejsca na nowe pojazdy");
                    break;
                }
                if (w == 1) {
                    Samochody[pusty] = kup_auto(saldo, czy_pojazd);
                    Prototypy[pusty] = &Samochody[pusty];
                }
                else if (w == 2) {
                    Motocykle[pusty] = kup_motor(saldo, czy_pojazd);
                    Prototypy[pusty] = &Motocykle[pusty];
                }
                else {
                    czy_ok("Podano zły wybór");
                    wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
                    break;
                }
                wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
                break;
            }
            case 4: { //Zarządzaj pojazdem, po wybraniu otworzy się nowe menu
                if (czy_pojazd) { //sprawdza, czy istnieje pojazd
                    int k = 0; //który pojazd z kolei
                    auto *pojazdy = new string[pojemnosc];
                    for (int j = 0; j < pojemnosc; j++) {
                        pojazdy[j] = Prototypy[j]->seria(); //pobiera do tablicy nr_seryjny każdego pojazdu
                    }
                    wyswietl_ekran(pojemnosc, pojazdy, "Wybierz pojazd do zarządzania"); //Wybór samochodu z fabryki do zarządzania
                    cin >> k;
                    k = czy_dobra_wartosc(k, pojemnosc, 1);
                    if(pojazdy[k - 1].substr(3, 4) == "Moto") {
                        zarzadzaj_pojazdem(Prototypy[k - 1], saldo, Motocykl::menu(), koszyk);
                        wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
                    }
                    else if(pojazdy[k - 1].substr(3, 4) == "Auto") {
                        zarzadzaj_pojazdem(Prototypy[k - 1], saldo, Samochod::menu(), koszyk);
                        wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
                    }
                    else {
                        wyswietl_ekran(6, zarzadzaj, nazwaK, "Błędny numer seryjny pojazdu. Wybierz inną akcję");
                    }
                    delete[] pojazdy;
                }
                else {
                    wyswietl_ekran(6, zarzadzaj, "Błąd! Żaden pojazd jeszcze nie istnieje.", "Wybierz inną akcję");
                }
                break;
            }
            case 5: { //Eksport
                string plik;

                string eksport[2];
                eksport[0] = "Czy chcesz eksportować komis do pliku o nazwie";
                eksport[1] = "komis_" + nazwa_bez_spacji(nazwa) + ".txt";

                if(tak_czy_nie(eksport[0], eksport[1])) plik = "komis_" + nazwa_bez_spacji(nazwa) + ".txt";
                else {
                    wyswietl_info("Podaj nazwę pliku wraz z rozszerzeniem");
                    IG;
                    getline(cin, plik);
                }

                eksportuj(plik); //eksportuj komis

                wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
                break;
            }
            case 6: {
                break; //Wraca do menu głównego
            }
            default: {
                wyswietl_info("Błąd techniczny! Wracam do menu głównego");
                Sleep(5000);
                wybierz = 6;
                break;
            }
        }
    }
}

void zarzadzaj_pojazdem(Lokomocja* L, double &saldo, string menu[8], string* &koszyk) { //menu wyboru po wybraniu z menu głównego
    string menu1[7];
    for (int i = 0; i < 7; i++) {
        menu1[i] = menu[i];
    }
    int wybierz = 1;
    while (wybierz != 7) {
        wyswietl_ekran(7, menu1, menu[7], L->nr_seryjny);
        cin >> wybierz;
        wybierz = czy_dobra_wartosc(wybierz, 7, 1);
        switch(wybierz) {
            case 1: { //Jazda pojazdem
                L->jazda(saldo);
                break;
            }
            case 2: { //Tankowanie pojazdu lub smarowanie roweru
                L->zatankuj(saldo);
                break;
            }
            case 3: { //Sprawdzanie stanu bagażnika
                L->wyswietl_bagaznik(L->nr_seryjny, "Stan bagażnika");
                break;
            }
            case 4: { //Wkładanie przedmiotów do bagażnika
                L->zaladuj();
                break;
            }
            case 5: {
                L->rozladuj(koszyk);
                break;
            }
            case 6: { //Lakierowanie
                unsigned int staz = pobierz_los(1, 30);
                Lakiernik Spec {"Zbyszek", staz};
                Spec.przelakieruj(L, saldo);
                break;
            }
            case 7: { //Powrót do menu głównego
                break;
            }
            default: {
                wyswietl_info("Błąd techniczny! Wracam do menu głównego");
                Sleep(5000);
                wybierz = 7;
                break;
            }
        }
    }
}

void SklepRowerowy::zarzadzaj(bool &czy_rower, double &saldo, string* koszyk) { //kolejne menu wyboru, po wybraniu z menu głównego
    string nazwaK = "Zarządzanie sklepem rowerowym " + nazwa;
    //Opcje zarządzania sklepem
    string zarzadzaj[5] = {"Wyświetl stan sklepu", "Sprzedaj rower", "Zarządzaj rowerem", "Eksportuj sklep", "Wyjdź ze sklepu"};
    int wybierz = 0;
    wyswietl_ekran(5, zarzadzaj, nazwaK, "Wybierz akcję");
    while (wybierz != 5) {
        cin >> wybierz;
        wybierz = czy_dobra_wartosc(wybierz, 5, 1);
        switch(wybierz) {
            case 1: {
                wyswietl(); //wyświetl stan sklepu

                int powrot = 0;
                cin >> powrot;
                string info = "Naciśnij " + to_string(pojemnosc + 1) + " aby wrócić do menu zarządzania";
                powrot = czy_dobra_wartosc(powrot, pojemnosc + 1, pojemnosc + 1, info);

                wyswietl_ekran(5, zarzadzaj, nazwaK, "Wybierz akcję");
                break;
            }
            case 2: {
                sprzedaz(saldo); //sprzedaj rowery
                wyswietl_ekran(5, zarzadzaj, nazwaK, "Wybierz akcję");
                break;
            }
            case 3: { //Zarządzaj rowerem, po wybraniu otworzy się nowe menu
                if (czy_rower) { //sprawdza, czy istnieje rower
                    int k = 0; //który rower z kolei
                    auto *rowerki = new string[pojemnosc];
                    for (int j = 0; j < pojemnosc; j++) {
                        rowerki[j] = Rowery[j].seria(); //pobiera do tablicy nr_seryjny każdego pojazdu
                    }
                    wyswietl_ekran(pojemnosc, rowerki, "Wybierz rower do zarządzania");
                    cin >> k;
                    k = czy_dobra_wartosc(k, pojemnosc, 1);
                    if (rowerki[k - 1].substr(3, 5) == "Rower") {
                        zarzadzaj_pojazdem(Prototypy[k - 1], saldo, Rower::menu(), koszyk);
                        wyswietl_ekran(6, zarzadzaj, nazwaK, "Wybierz akcję");
                    }
                    else if (rowerki[k - 1].substr(3, 5) == ".....") {
                        wyswietl_ekran(5, zarzadzaj, nazwaK, "Podane miejsce jest puste. Wybierz inną akcję");
                    }
                    else {
                        wyswietl_ekran(5, zarzadzaj, nazwaK, "Błędny numer seryjny roweru. Wybierz inną akcję");
                    }
                    delete[] rowerki;
                }
                else {
                    wyswietl_ekran(5, zarzadzaj, "Błąd! Żaden rower jeszcze nie istnieje.", "Wybierz inną akcję");
                }
                break;
            }
            case 4: { //Eksport
                string plik;

                string eksport[2];
                eksport[0] = "Czy chcesz eksportować sklep do pliku o nazwie";
                eksport[1] = "sklep_" + nazwa_bez_spacji(nazwa) + ".txt";

                if(tak_czy_nie(eksport[0], eksport[1])) plik = "sklep_" + nazwa_bez_spacji(nazwa) + ".txt";
                else {
                    wyswietl_info("Podaj nazwę pliku wraz z rozszerzeniem");
                    IG;
                    getline(cin, plik);
                }

                eksportuj(plik); //eksportuj sklep

                wyswietl_ekran(5, zarzadzaj, nazwaK, "Wybierz akcję");
                break;
            }
            case 5: {
                break; //Wraca do menu głównego
            }
            default: {
                wyswietl_info("Błąd techniczny! Wracam do menu głównego");
                Sleep(5000);
                wybierz = 5;
                break;
            }
        }
    }
}