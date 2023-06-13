//
// Created by Dawid Makowski on 30.11.2021.
//

#ifndef KLASY_POJAZDY_H
#define KLASY_POJAZDY_H
//definiuje zwłokę dla windowsa lub dla innych systemów
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#define sleep(n) Sleep(1000*(n))
#else
#include <thread>
    #define sleep(n) std::this_thread::sleep_for(std::chrono::seconds(n));
#endif

#include "funkcje_inne.h"
#include <limits> //dla define IG
//define IG-zapobiega mieszaniu się getline(cin,) oraz cin >> (stawiam przed getline)
#define IG cin.ignore(numeric_limits<streamsize>::max(), '\n')

using namespace std;
class Lokomocja;
class Fabryka;
//Klasa lakiernik-oddzielna klasa służąca do zmiany koloru pojazdów
class Lakiernik {
    //zamiana koloru pojazdu
    string imie;
    unsigned int staz;

public:

    explicit Lakiernik(string a = "Zbyszek", unsigned int b = 10) : imie(move(a)), staz(b) {}

    void info() {
        string inf = "Zawód: Lakiernik\nImię: " + imie + "\nStaż: " + to_string(staz) + " lat";
    }

    void przelakieruj(Lokomocja *L, double &saldo) const;
};
//Klasa podstawowa "Lokomocja" - dziedziczą po niej wszystkie pojazdy
class Lokomocja {

protected:

    static unsigned int r; //Liczba dodatkowych kolorów
    static constexpr int K = 7; //domyślna liczba dostępnych kolorów
    static string *Kolory; //Kolory domyślne + kolory dodane przez użytkownika

    string wyprodukowano;
    string nr_seryjny;
    double cena_det;
    string model;
    string kolor;

    static unsigned int dodaj_kolor(); //dodawanie kolorów przez użytkownika
    static string dolacz_kolor(unsigned int&);

public:

    friend class Lakiernik;
    friend ostream &operator<<(ostream &, const Lokomocja&); //przeładowanie cout klasy Lokomocja

    Lokomocja();
    Lokomocja(string a, string b, double c, string d);

    friend void zarzadzaj_pojazdem(Lokomocja*, double &saldo, string menu[7], string*&);

    virtual void wyswietl_bagaznik(string nazwa, string info) = 0;
    virtual void zatankuj(double &saldo) = 0; //metoda do tankowania
    virtual void jazda(double &saldo) = 0;
    virtual void zaladuj() = 0;
    virtual void rozladuj(string*) = 0;
    virtual double ustal_cene() = 0;

    string seria() const { //pobiera kopie numeru seryjnego auta
        return nr_seryjny; //prywatny składnik klasy
    }

    string _kolor() const {
        return kolor;
    }

    double cena() const { //zwraca kopię ceny
        return cena_det;
    }
};
//Klasa pojazd dziedzicząca po lokomocji-dziedziczy po niej motocykl i samochód
class Pojazd : public Lokomocja {

protected:
    //atrybuty pojazdu
    unsigned int poj_bag = 1;
    string *Bagaznik = nullptr;

    static constexpr int S = 6; //domyślna liczba dostępnych silników
    static string Silniki[S]; //Wybór silników

    string silnik;
    double moc;
    double stan = 0.0; //początkowa ilość paliwa w zbiorniku
    string nadwozie;
    unsigned long przebieg = 0; //początkowy przebieg w kilometrach
    static constexpr double MNOZ = 1.0; //mnożnik dla cen paliw
    double POJ = 0.0; //ile paliwa pomieści zbiornik
    //Metody prywatne
    static double rodzaj_silnika_to_mnoznik(const string &silnik); //określa które paliwo jest proporcjonalnie droższe
    static string dolacz_silnik(unsigned int&);
    static void okresl_moc(double&);

public:
    //Deklaracje przyjaźni
    friend class Komis;
    //Konstruktory
    Pojazd();
    Pojazd(string a, string b, string c, string d, double x = 100, double e = 50000.0, unsigned long p = 0, string F = "Kupiony"); //Konstruktor
    //Metody
    void zatankuj(double &saldo) override; //metoda do tankowania
    void jazda(double &saldo) override;//metoda do jazdy

    void wyswietl_bagaznik(string nazwa, string info) override;
    void zaladuj() override;
    void rozladuj(string*) override;
    //Metody inline
    unsigned long _przebieg() const {
        return przebieg;
    }

    void aktualizuj(int km) { //każdy pojazd ma swój przebieg
        przebieg += km; //Gdy wykonywana jest funkcja jazda(), to ta funkcja jest tam wywoływana
    }
};

class Samochod : public Pojazd {

    static unsigned int p; //Liczba dodatkowych modeli
    static constexpr int M = 16; //domyślna liczba dostępnych modeli
    static constexpr int N = 7; //domyślna liczba dostępnych nadwozi

    static string *Modele; //Modele domyślne + modele dodane przez użytkownika
    static string Nadwozia[N]; //Wybór nadwozia

    static string menu_zarzadzania[8];
    static unsigned int dodaj_model_auta();
    static string dolacz_model(unsigned int&);
    static string dolacz_nadwozie(unsigned int&);

    static unsigned int licznik;

public:
    //Deklaracje przyjaźni
    friend class Komis;
    friend Fabryka importuj_fabryke(const string &plik);
    //Konstruktory
    Samochod() : Pojazd() {};
    Samochod(string a, string b, string c, string d, double e, double f, unsigned long g, string h = "Kupiony");
    Samochod(const Samochod&);
    Samochod(Samochod&&) noexcept;
    explicit Samochod(string nazwa);
    //Operatory przypisania
    Samochod& operator=(const Samochod&);
    Samochod& operator=(Samochod&&) noexcept;
    //Metody
    static Samochod konstrukcja(double&, const string&);
    static string* menu() {return menu_zarzadzania;}
    double ustal_cene() override;
};

class Motocykl : public Pojazd {
    //Tablica dla menu zarządzania
    static string menu_zarzadzania[8];
    //Atrybuty
    static unsigned int p; //Liczba dodatkowych modeli
    static constexpr int M = 7; //domyślna liczba dostępnych modeli
    static constexpr int T = 7; //domyślna liczba dostępnych nadwozi
    static unsigned int licznik;

    static string *Modele; //Modele domyślne + modele dodane przez użytkownika
    static string Typy[T]; //Wybór typu
    //Metody prywatne
    static unsigned int dodaj_model_motoru();
    static string dolacz_typ(unsigned int&);
    static string dolacz_model(unsigned int&);

public:
    //Deklaracje przyjaźni
    friend class Komis;
    //Konstruktory
    Motocykl() : Pojazd() {};
    Motocykl(string a, string b, string c, string d, double e, double f, unsigned long g, string h = "Kupiony");
    Motocykl(const Motocykl&);
    Motocykl(Motocykl &&S) noexcept;
    explicit Motocykl(string nr);
    //Operatory przypisania
    Motocykl& operator=(const Motocykl &M);
    Motocykl& operator=(Motocykl &&M) noexcept;
    //Metody
    static Motocykl konstrukcja(double&, const string&);
    static string* menu() {return menu_zarzadzania;}
    double ustal_cene() override;
};
//Klasa rower dziedziczy bezpośrednio po klasie "Lokomocja"
class Rower : virtual public Lokomocja {
    //Tablica dla menu zarządzania
    static string menu_zarzadzania[8];
    //Atrybuty
    string bagaznik;
    string typ;
    float zuzycie = 0;
    static float smar;

    static unsigned int p; //Liczba dodatkowych marek (dodanych przez użytkownika)
    static constexpr int T = 5;
    static constexpr int M = 5;
    static string Typy[T];
    static string* Marki;
    static unsigned int licznik;
    //Metody prywatne
    static unsigned int dodaj_marke(); //dodawanie nowej marki przez użytkownika
    static string dolacz_marke(unsigned int&); //dołączanie marki jako element nowego obiektu
    static string dolacz_typ(unsigned int&); //dołączanie typu jako element nowego obiektu

public:
    //Konstruktory
    Rower() : Lokomocja() {};
    Rower(string a, string b, string c, double d, string e);
    explicit Rower(string nr);
    //Metody
    void wyswietl_bagaznik(string nazwa, string info) override;
    void zaladuj() override;
    void rozladuj(string*) override;
    void zatankuj(double&) override;
    void jazda(double&) override;

    static Rower konstrukcja(double&, const string&);
    static string* menu() {return menu_zarzadzania;}

    double ustal_cene() override;

    static void kup_smar(double&);
};
#endif //KLASY_POJAZDY_H