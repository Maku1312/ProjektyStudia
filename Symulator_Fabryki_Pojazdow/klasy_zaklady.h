//
// Created by Dawid Makowski on 15.12.2021.
//

#ifndef KLASY_ZAKLADY_H
#define KLASY_ZAKLADY_H

#include "klasy_pojazdy.h"
#include "funkcje_inne.h"
#include <dirent.h>

using namespace std;

class Komis;
class Fabryka;
class SklepRowerowy;
struct Pozycja;

class Zaklad {

protected:

    Lokomocja **Prototypy; //Tablica wskaźników do pojazdów (na danym miejscu w zakładzie może być dowolny pojazd)

    string nazwa; //Nazwa zakładu
    unsigned int pojemnosc; //Pojemnosc zakładu

public:
    //deklaracje przyjaźni
    friend void Lakiernik::przelakieruj(Lokomocja *L, double &saldo) const; //funkcja zmieniająca kolor pojazdu
    //deklaracje przyjaźni funkcji do wykonywania w trybie czytania z pliku
    template<class X>
    friend Pozycja znajdz_pojazd(vector<X> &FIRMA, const string& pojazd); //znajduje pojazd, szukając po każdym zakładzie
    template<class X>
    friend Pozycja znajdz_pojazd_w_zakladzie(vector<X> &FIRMA, const string& pojazd, const string& nazwa); //znajduje pojazd w danym zakładzie
    friend void wykonaj(string* rozkazy, unsigned int ile);
    //Konstruktory
    explicit Zaklad(string a = "Nieznany", unsigned int b = 20); //konstruktor
    Zaklad(const Zaklad &Z); //konstruktor kopiujący
    //Metody
    string _nazwa() { //funkcja zwraca kopię nazwy zakładu
        return nazwa;
    };
    //Metody wirtualne
    virtual string wyswietl() const {return "";} //wyświetla stan zakładu
    virtual void zarzadzaj(bool&, double&) {} //zarządzanie zakładem
    virtual void eksportuj(const char*) {} //eksportuje zakład do pliku
    //Destruktor
    ~Zaklad() {delete[] Prototypy;}
};

class Fabryka : public Zaklad {

    Rower *Rowery; //tablica rowerów o wielkości pojemności fabryki
    Motocykl *Motocykle; //tablica motocykli
    Samochod *Samochody; //tablica samochodów

public:
    //Deklaracje przyjaźni
    friend void wykonaj(string* rozkazy, unsigned int ile); //funkcja wykonujące komendy z pliku "stdin....txt"
    friend ostream& operator<<(ostream& os, const Fabryka& F); //przeładowany cout dla fabryki
    friend Fabryka importuj_fabryke(const string&);
    //Konstruktory
    explicit Fabryka(string nazwa = "Nieznana", unsigned int pojemnosc = 20); //konstruktor fabryki
    Fabryka(const Fabryka&); //konstruktor kopiujący fabryki
    Fabryka(Fabryka&&) noexcept; //konstruktor przenoszący fabryki
    //Operatory przypisania
    Fabryka& operator=(const Fabryka &F); //operator przypisania kopiujący
    Fabryka& operator=(Fabryka &&F) noexcept; //operator przypisania przenoszący
    //Metody (same deklaracje)
    string wyswietl(string info = "Wyprodukowane pojazdy", bool druk = false) const; //wyświetlanie wyprodukowanych pojazdów wraz z ich modelami
    unsigned int znajdz_puste(); //znajduje pierwsze puste miejsce w zakładzie
    void przekaz(Komis&); //przekazuje pojazd z fabryki do komisu
    void przekaz(SklepRowerowy&); //przekazuje rower z fabryki do sklepu
    void zarzadzaj(bool& czy_pojazd, bool& czy_rower, double& saldo, vector<Komis>&, vector<SklepRowerowy>&); //zarządzanie zakładem (zarzadzanie.cpp)
    void praca_start(int ile, bool &czy_pojazd, bool &czy_rower, double &saldo); //funkcja konstruująca pojazdy podaną ilość razy (funkcje_main.cpp)
    void eksportuj(const string& plik, bool wyswietl = true); //eksport podanego zakładu (operacje_na_plikach.cpp)
    //Metody inline
    void usun_pojazd(int i) { //usuwa pojazd z danego miejsca w fabryce
        if (Prototypy[i]->seria().substr(3, 4) == "Auto") {
            Samochody[i] = Samochod();
        }
        else if (Prototypy[i]->seria().substr(3, 8) == "Motocykl") {
            Motocykle[i] = Motocykl();
        }
        else if (Prototypy[i]->seria().substr(3, 5) == "Rower") {
            Rowery[i] = Rower();
        }
        else czy_ok("Podane miejsce w fabryce jest puste!");
    }

    int ile_pojazdow() { //sprawdza ile jest pojazdów w dane fabryce
        int a = 0;
        for (int i = 0; i < pojemnosc; i++) {
            if (Prototypy[i]->seria() != "...........Puste Miejsce...........") {
                a++;
            }
        }
        return a;
    }
    //Destrutkor
    ~Fabryka() {
        delete[] Rowery;
        delete[] Motocykle;
        delete[] Samochody;
    }
};

class Komis : public Zaklad {

    Pojazd** Prototypy; //W komisie przechowywane są tylko samochody i motocykle, więc deklaruję tablicę wskaźników do pojazdu
    Motocykl *Motocykle; //Tablica motocyklów
    Samochod *Samochody; //Tablica samochodów
    float margin = 1.0; //Marża komisu

public:
    //Deklaracje przyjaźni
    friend ostream& operator<<(ostream& os, const Komis& K); //Operator wypisywania na ekran
    friend Komis importuj_komis(const string&); //Import komisu
    friend class Fabryka; //Przyjaźń z klasą fabryka dla przekazywania pojazdów z fabryki do komisu
    //Deklaracje przyjaźni funkcji do wykonywania w trybie czytania z pliku (te same co w Zakład)
    template<class X>
    friend Pozycja znajdz_pojazd(vector<X> &FIRMA, const string& pojazd);
    template<class X>
    friend Pozycja znajdz_pojazd_w_zakladzie(vector<X> &FIRMA, const string& pojazd, const string& nazwa);
    friend void wykonaj(string* rozkazy, unsigned int ile);
    //Konstruktory
    explicit Komis(string nazwa = "Nieznany", unsigned int pojemnosc = 20, float m = 1.0); //Konstruktor
    Komis(const Komis&); //Konstruktor kopiujący
    Komis(Komis&&) noexcept; //Konstruktor przenoszący
    //Operatory przypisania
    Komis& operator=(const Komis&); //Operator kopiujący
    Komis& operator=(Komis&&) noexcept; //Operator przenoszący
    //Metody
    string wyswietl(string info = "Posiadane pojazdy", bool druk = false) const; //wyświetlanie posiadanych pojazdów
    unsigned int znajdz_puste();
    void cofnij_licznik(unsigned int); //istnieje 50% prawdopodobieństwa, że podzielimy przebieg pojazdu przez 2
    void sprzedaz(double &saldo); //sprzedaż pojazdów
    static Samochod kup_auto(double &saldo, bool &czy); //kupno samochodów
    static Motocykl kup_motor(double &saldo, bool &czy); //kupno motocykli
    void zarzadzaj(bool&, double&, string*); //definicja w zarzadzanie.cpp
    void eksportuj(const string& plik, bool wyswietl = true); //definicja w operacje_na_plikach.cpp
    //Metody inline
    int ile_pojazdow() {
        int a = 0;
        for (int i = 0; i < pojemnosc; i++) {
            if (Prototypy[i]->seria() != "...........Puste Miejsce...........") {
                a++;
            }
        }
        return a;
    }
    //Destruktor
    ~Komis() {
        delete[] Prototypy;
        delete[] Motocykle;
        delete[] Samochody;
    }
};

class SklepRowerowy : public Zaklad {

public:

    Lokomocja** Prototypy;
    Rower* Rowery;
    float margin = 1.0; //Marża

public:
    //Deklaracje przyjaźni
    friend class Fabryka; //Przyjaźń z klasą fabryka dla przekazywania pojazdów z fabryki do sklepu
    friend ostream& operator<<(ostream&, const SklepRowerowy&);
    friend SklepRowerowy importuj_sklep(const string&);
    //Konstruktory
    explicit SklepRowerowy(string nazwa = "Nieznany", unsigned int pojemnosc = 20, float m = 1.0);
    SklepRowerowy(const SklepRowerowy&);
    SklepRowerowy(SklepRowerowy&&) noexcept;
    //Operatory przypisania
    SklepRowerowy& operator=(const SklepRowerowy&);
    SklepRowerowy& operator=(SklepRowerowy&&) noexcept;
    //Metody
    string wyswietl(string info = "Posiadane rowery", bool druk = false) const;
    unsigned int znajdz_puste();
    void sprzedaz(double &saldo);
    void zarzadzaj(bool&, double&, string*); //definicja w zarzadzanie.cpp
    void eksportuj(const string& plik, bool druk = true); //definicja w operacje_na_plikach.cpp
    //Metody inline
    int ile_rowerow() {
        int a = 0;
        for (int i = 0; i < pojemnosc; i++) {
            if (Rowery[i].seria() != "...........Puste Miejsce...........") {
                a++;
            }
        }
        return a;
    }
    //Destruktor
    ~SklepRowerowy() {delete[] Rowery;}
};
//definicje w funkcje_main.cpp
Fabryka tworz_fabryke();
Komis tworz_komis();
SklepRowerowy tworz_sklep();
//definicje w operacje_na_plikach.cpp
Fabryka importuj_fabryke(const string&);
Komis importuj_komis(const string&);
SklepRowerowy importuj_sklep(const string&);

#endif //KLASY_ZAKLADY_H