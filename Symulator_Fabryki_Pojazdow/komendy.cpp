//
// Created by Dawid Makowski on 04.01.2022.
//

#include "klasy_zaklady.h"
#include <fstream>

#define JESLI(a, b) if(slowa[a] == #b) //jeśli słowo komendy jest równe oczekiwanej wartości
#define ZADUZO cout << "Polecenie nie zostało wykonane!" << endl << "Podano za dużo argumentów" << endl
#define ZAMALO cout << "Polecenie nie zostało wykonane!" << endl << "Podano za mało argumentów" << endl
#define BLAD(a) cout << blad << endl << #a << endl //drukuje podany błąd
#define WROC1 if (a != 1000) //jeśli a == 1000, to przerywa wykonywanie komendy
#define WROC2 if (p.a != 1000 && p.b != 1000) //jeśli a i b == 1000, to przerywa wykonywanie komendy

struct Pozycja { //struktura potrzebna do zwrócenia dwóch wartości przez funkcję znajdź_pojazd (zwraca indeks zakładu i indeks pojazdu w zakładzie)
    unsigned int a = 1000;
    unsigned int b = 1000;
};

template<class X>
Pozycja znajdz_pojazd(vector<X> &FIRMA, const string& pojazd); //szuka pojazdu w całym programie (wada - jeśli są dwa takie same, to wybierze ten pierwszy)
template<class X>
unsigned int znajdz_zaklad(vector<X> &FIRMA, const string& nazwa); //szuka zakładu o nazwie
template<class X>
Pozycja znajdz_pojazd_w_zakladzie(vector<X> &FIRMA, const string& pojazd, const string& nazwa); //szuka pojazdu w danym zakładzie

void wykonaj(string* rozkazy, unsigned int ile) {

    vector<Fabryka> Wytwornia(20); //wektor fabryk
    vector<Komis> Targ(20); //wektor komisów
    vector<SklepRowerowy> Market(20); //wektor sklepów
    string Koszyk[20] {}; //koszyk na rozpakowane z bagażnika rzeczy

    double saldo = 1000000.0; //ile mamy środków
    unsigned int f = 0; //obecna liczba fabryk
    unsigned int k = 0; //obecna liczba komisów
    unsigned int s = 0; //obecna liczba sklepów

    bool czy[5]; //0.fabryka, 1.komis, 2.sklep, 3.pojazd, 4.rower (tablica bool'i informująca czy dany obiekt istnieje
    for (bool& j : czy) {
        j = false;
    }

    for (int l = 0; l < ile; l++) { //pętla przechodząca po każdej komendzie
        string rozkaz = rozkazy[l];
        cout << endl << endl << rozkaz << " : " << endl << endl;

        string blad = "Polecenie nie zostało wykonane!";
        if (rozkaz.substr(0, 1) != "/") { //Jeśli tekst nie zaczyna się od znaku "/" to nie zostanie wykonane
            BLAD(Brak znaku rozpoczynającego komendę);
        }

        unsigned int d = 0;
        for (char i : rozkaz) {
            if (i == ' ' && i + 1 != '\n') d++; //zmienna 'd' oznacz liczbę słów w danej komendzie
        }
        d++;
        auto* slowa = new string[d]; //tablica na słowa w komendzie
        unsigned int pozycja = 0;
        for (int i = 0; i < d; i++) { //wydzielanie z komendy pojedynczych słów
            if (i == d - 1) slowa[i] = rozkaz.substr(pozycja + 1);
            if (i == 0) slowa[i] = rozkaz.substr(pozycja + 1, rozkaz.find(' ', pozycja + 2) - pozycja - 1);
            else slowa[i] = rozkaz.substr(pozycja + 2, rozkaz.find(' ', pozycja + 2) - pozycja - 1);
            pozycja += rozkaz.find(' ', pozycja + 2) - pozycja - 1;
            if (slowa[i][slowa[i].size() - 1] == ' ') slowa[i].pop_back();
            //cout << "Slowo nr " << i + 1 << ": " << slowa[i] << endl; //cout techniczny, wypisuje każde słowo z komendy
        }

        //Kod odpowiedzialny za wykonywanie komend

        JESLI(0,tworz) {
            JESLI(1,komis) {
                if (d == 2) {
                    Komis KOM = tworz_komis();
                    Targ[k] = KOM;
                    czy[1] = true;
                    cout << "Utworzono komis " + Targ[k]._nazwa();
                    k++;
                } //Tworzy komis z programu
                else if (d == 3) {
                    Komis KOM = Komis(slowa[2]);
                    Targ[k] = KOM;
                    czy[1] = true;
                    cout << "Utworzono komis " + Targ[k]._nazwa();
                    k++;
                } //Tworzy komis z podaną nazwą
                else if (d == 4) {
                    Komis KOM = Komis(slowa[2], stoi(slowa[3]));
                    Targ[k] = KOM;
                    czy[1] = true;
                    cout << "Utworzono komis " + Targ[k]._nazwa();
                    k++;
                } //Tworzy komis z podaną nazwą i pojemnością
                else if (d == 5) {
                    Komis KOM = Komis(slowa[2], stoi(slowa[3]), stof(slowa[4]));
                    Targ[k] = KOM;
                    czy[1] = true;
                    cout << "Utworzono komis " + Targ[k]._nazwa();
                    k++;
                } //Tworzy komis z podaną nazwą, pojemnością i marżą
                else ZADUZO;
            } //Tworzy komis
            else JESLI(1,fabryka) { //Analogicznie w fabryce
                if (d == 2) {
                    Fabryka FAB = tworz_fabryke();
                    Wytwornia[f] = FAB;
                    czy[0] = true; //Można zarządzać fabryką
                    cout << "Utworzono fabrykę " + Wytwornia[f]._nazwa();
                    f++;
                }
                else if (d == 3) {
                    Fabryka FAB = Fabryka(slowa[2]);
                    Wytwornia[f] = FAB;
                    czy[0] = true; //Można zarządzać fabryką
                    cout << "Utworzono fabrykę " + Wytwornia[f]._nazwa();
                    f++;
                }
                else if (d == 4) {
                    Fabryka FAB = Fabryka(slowa[2], stoi(slowa[3]));
                    Wytwornia[f] = FAB;
                    czy[0] = true; //Można zarządzać fabryką
                    cout << "Utworzono fabrykę " + Wytwornia[f]._nazwa();
                    f++;
                }
                else ZADUZO;
            } //Analogicznie w fabryce
            else JESLI(1,sklep) {
                if (d == 2) {
                    SklepRowerowy SR = tworz_sklep();
                    Market[s] = SR;
                    czy[2] = true; //Można zarządzać sklepem
                    cout << "Utworzono sklep rowerowy " + Market[s]._nazwa();
                    s++;
                }
                else if (d == 3) {
                    SklepRowerowy SR = SklepRowerowy(slowa[2]);
                    Market[s] = SR;
                    czy[2] = true; //Można zarządzać sklepem
                    cout << "Utworzono sklep rowerowy " + Market[s]._nazwa();
                    s++;
                }
                else if (d == 4) {
                    SklepRowerowy SR = SklepRowerowy(slowa[2], stoi(slowa[3]));
                    Market[s] = SR;
                    czy[2] = true; //Można zarządzać sklepem
                    cout << "Utworzono sklep rowerowy " + Market[s]._nazwa();
                    s++;
                }
                else if (d == 5) {
                    SklepRowerowy SR = SklepRowerowy(slowa[2], stoi(slowa[3]), stof(slowa[4]));
                    Market[s] = SR;
                    czy[2] = true; //Można zarządzać sklepem
                    cout << "Utworzono sklep rowerowy " + Market[s]._nazwa();
                    s++;
                }
                else ZADUZO;
            } //Analogicznie w sklepie rowerowym
            else JESLI(1,samochod) {
                if (czy[0] && d == 3) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        unsigned int puste = Wytwornia[a].znajdz_puste();
                        if (puste >= Wytwornia[a].pojemnosc) BLAD(Fabryka jest pełna!);
                        else {
                            Wytwornia[a].Samochody[puste] = Samochod::konstrukcja(saldo, Wytwornia[a].nazwa);
                            czy[3] = true;
                            Wytwornia[a].Prototypy[puste] = &Wytwornia[a].Samochody[puste];
                            cout << "Wyprodukowano pojazd: " << *Wytwornia[a].Prototypy[puste] << endl;
                        }
                    }
                } //Konstruuje samochód funkcją z programu
                else if (d > 3 && d < 8) ZAMALO;
                else if (d == 8) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        unsigned int puste = Wytwornia[a].znajdz_puste();
                        if (puste >= Wytwornia[a].pojemnosc) BLAD(Fabryka jest pełna!);
                        else {
                            Wytwornia[a].Samochody[puste] = Samochod(slowa[3], slowa[4], slowa[5], slowa[6], stoi(slowa[7]), 15000, 0, nazwa_bez_spacji(slowa[2]));
                            czy[3] = true;
                            Wytwornia[a].Prototypy[puste] = &Wytwornia[a].Samochody[puste];
                            cout << "Wyprodukowano pojazd: " << *Wytwornia[a].Prototypy[puste] << endl;
                        }
                    }
                } //Konstruuje samochód ręcznie
                else ZADUZO;
            } //Tworzy samochód w danej fabryce
            else JESLI(1,motocykl) {
                if (czy[0] && d == 3) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        unsigned int puste = Wytwornia[a].znajdz_puste();
                        if (puste >= Wytwornia[a].pojemnosc) BLAD(Fabryka jest pełna!);
                        else {
                            Wytwornia[a].Motocykle[puste] = Motocykl::konstrukcja(saldo, Wytwornia[a].nazwa);
                            czy[3] = true;
                            Wytwornia[a].Prototypy[puste] = &Wytwornia[a].Motocykle[puste];
                            cout << "Wyprodukowano pojazd: " << *Wytwornia[a].Prototypy[puste] << endl;
                        }
                    }
                }
                else if (d > 3 && d < 8) ZAMALO;
                else if (d == 8) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        unsigned int puste = Wytwornia[a].znajdz_puste();
                        if (puste >= Wytwornia[a].pojemnosc) BLAD(Fabryka jest pełna!);
                        else {
                            Wytwornia[a].Motocykle[puste] = Motocykl(slowa[3], slowa[4], slowa[5], slowa[6], stoi(slowa[7]), 15000, 0, nazwa_bez_spacji(slowa[2]));
                            czy[3] = true;
                            Wytwornia[a].Prototypy[puste] = &Wytwornia[a].Motocykle[puste];
                            cout << "Wyprodukowano pojazd: " << *Wytwornia[a].Prototypy[puste] << endl;
                        }
                    }
                }
                else ZADUZO;
            } //Tworzy motocykl w danej fabryce
            else JESLI(1,rower) {
                if (czy[0] && d == 3) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        unsigned int puste = Wytwornia[a].znajdz_puste();
                        if (puste >= Wytwornia[a].pojemnosc) BLAD(Fabryka jest pełna!);
                        else {
                            Wytwornia[a].Rowery[puste] = Rower::konstrukcja(saldo, Wytwornia[a].nazwa);
                            czy[4] = true;
                            Wytwornia[a].Prototypy[puste] = &Wytwornia[a].Rowery[puste];
                            cout << "Wyprodukowano pojazd: " << *Wytwornia[a].Prototypy[puste] << endl;
                        }
                    }
                }
                else if (d > 3 && d < 6) ZAMALO;
                else if (d == 6) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        unsigned int puste = Wytwornia[a].znajdz_puste();
                        if (puste >= Wytwornia[a].pojemnosc) BLAD(Fabryka jest pełna!);
                        else {
                            Wytwornia[a].Rowery[puste] = Rower(slowa[3], slowa[4], slowa[5], 1000, nazwa_bez_spacji(slowa[2]));
                            czy[3] = true;
                            Wytwornia[a].Prototypy[puste] = &Wytwornia[a].Rowery[puste];
                            cout << "Wyprodukowano pojazd: " << *Wytwornia[a].Prototypy[puste] << endl;
                        }
                    }
                }
                else ZADUZO;
            } //Tworzy rower w danej fabryce
            else ZAMALO;
        }

        else JESLI(0,import) {
            JESLI(1,komis) {
                if (d == 2) BLAD(Polecenie wymaga nazwy pliku);
                else if (d == 3) {
                    try {
                        Targ[k] = importuj_komis(slowa[2]);
                    }
                    catch (const out_of_range& error) {
                        cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać komis" << endl;
                        cerr << "what(): " << error.what() << endl << flush;
                    }
                    catch (const invalid_argument &niepoprawny_arg) {
                        cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać komis" << endl;
                        cerr << "what():" <<  niepoprawny_arg.what() << endl << flush;
                    }
                    catch (...) {
                        cerr << "Wykryto niezidentyfikowany problem" << endl;
                        cerr << "Sprawdź ponownie strukturę swojego pliku!" << endl << flush;
                    }

                    czy[1] = true; //Można zarządzać komisem
                    czy[3] = true; //Można zarządzać pojazdami
                    k++;
                } //Import komisu
                else ZADUZO;
            }
            else JESLI(1,fabryka) {
                if (d == 2) BLAD(Polecenie wymaga nazwy pliku);
                else if (d == 3) {
                    try {
                        Wytwornia[f] = importuj_fabryke(slowa[2]);
                    }
                    catch (const out_of_range& error) {
                        cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać fabrykę" << endl;
                        cerr << "what(): " << error.what() << endl << flush;
                    }
                    catch (const invalid_argument &niepoprawny_arg) {
                        cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać fabrykę" << endl;
                        cerr << "what():" <<  niepoprawny_arg.what() << endl << flush;
                    }
                    catch (...) {
                        cerr << "Wykryto niezidentyfikowany problem" << endl;
                        cerr << "Sprawdź ponownie strukturę swojego pliku!" << endl << flush;
                    }

                    czy[0] = true; //Można zarządzać komisem
                    czy[3] = true; //Można zarządzać pojazdami
                    czy[4] = true; //Można zarządzać rowerami
                    f++;
                } //Import fabryki
                else ZADUZO;
            }
            else JESLI(1,sklep) {
                if (d == 2) BLAD(Polecenie wymaga nazwy pliku);
                else if (d == 3) {
                    try {
                        Market[s] = importuj_sklep(slowa[2]);
                    }
                    catch (const out_of_range& error) {
                        cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać sklep" << endl;
                        cerr << "what(): " << error.what() << endl << flush;
                    }
                    catch (const invalid_argument &niepoprawny_arg) {
                        cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać sklep" << endl;
                        cerr << "what():" <<  niepoprawny_arg.what() << endl << flush;
                    }
                    catch (...) {
                        cerr << "Wykryto niezidentyfikowany problem" << endl;
                        cerr << "Sprawdź ponownie strukturę swojego pliku!" << endl << flush;
                    }

                    czy[2] = true; //Można zarządzać komisem
                    czy[4] = true; //Można zarządzać rowerami
                    s++;
                } //Import sklepu
                else ZADUZO;
            }
            else ZAMALO;
        }

        else JESLI(0,eksport) {
            JESLI(1,komis) {
                if (czy[1] && d == 3) {
                    unsigned int a = znajdz_zaklad(Targ, slowa[2]);
                    WROC1 {
                        string plik = "komis_" + nazwa_bez_spacji(Targ[a]._nazwa()) + ".txt";
                        Targ[a].eksportuj(plik);
                    }
                }
                else if (!czy[1]) BLAD(Żaden komis jeszcze nie istnieje);
                else ZADUZO;
            }
            else JESLI(1,fabryka) {
                if (czy[0] && d == 3) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        string plik = "fabryka_" + nazwa_bez_spacji(Wytwornia[a]._nazwa()) + ".txt";
                        Wytwornia[a].eksportuj(plik);
                    }
                }
                else if (!czy[0]) BLAD(Żadna fabryka jeszcze nie istnieje);
                else ZADUZO;
            }
            else JESLI(1,sklep) {
                if (czy[2] && d == 3) {
                    unsigned int a = znajdz_zaklad(Market, slowa[2]);
                    WROC1 {
                        string plik = "sklep_" + nazwa_bez_spacji(Market[a]._nazwa()) + ".txt";
                        Market[a].eksportuj(plik);
                    }
                }
                else if (!czy[2]) BLAD(Żaden sklep jeszcze nie istnieje);
                else ZADUZO;
            }
            else JESLI(1,wszystko) {
                if ((czy[0] || czy[1] || czy[2]) && d == 2) {
                    for (int i = 0; i < f; i++) {
                        string plik = "fabryka_" + nazwa_bez_spacji(Wytwornia[i]._nazwa()) + ".txt";
                        const char* plik1 = plik.c_str();
                        Wytwornia[i].eksportuj(plik1, false);
                    }
                    for (int i = 0; i < k; i++) {
                        string plik = "komis_" + nazwa_bez_spacji(Targ[i]._nazwa()) + ".txt";
                        const char* plik1 = plik.c_str();
                        Targ[i].eksportuj(plik1, false);
                    }
                    for (int i = 0; i < s; i++) {
                        string plik = "sklep_" + nazwa_bez_spacji(Market[i]._nazwa()) + ".txt";
                        const char* plik1 = plik.c_str();
                        Market[i].eksportuj(plik1, false);
                    }
                    string info[4];
                    info[0] = "Pomyślnie zapisano projekt z ";
                    info[1] = to_string(f) + " fabrykami";
                    info[2] = to_string(k) + " komisami";
                    info[3] = to_string(s) + " sklepami";
                    wyswietl_info(4, info);
                }
                else if (!(czy[0] || czy[1] || czy[2])) BLAD(W programie nie ma żadnego obiektu do zapisania);
                else ZADUZO;
            }
            else ZAMALO;
        }

        else JESLI(0,przekaz) {
            JESLI(1,samochod) {
                if (d == 3 && czy[1]) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        auto *tabk = new string[Targ.size()];
                        for (int i = 0; i < Targ.size(); i++) {
                            if (Targ[i]._nazwa() != "Nieznany") { //Jeśli nazwa komisu wynosi "Nieznany", wówczas komis nie istnieje
                                tabk[i] = "Komis " + Targ[i]._nazwa();
                            }
                        }
                        if (k != 0) { //Jeśli istnieje jakikolwiek komis program przekierowuje nas do wyboru komisu
                            wyswietl_ekran(k, tabk, "Lista komisów", "Wybierz komis, do którego chcesz przenieść pojazd"); //Ekran wyboru komisów
                            int komis = 0;
                            cin >> komis;
                            komis = czy_dobra_wartosc(komis, k, 1);
                            Wytwornia[a].przekaz(Targ[komis - 1]);
                        }
                        delete[] tabk;
                    }
                }
                else if (d == 4 && czy[1]) {
                    Pozycja p = {znajdz_zaklad(Wytwornia, slowa[2]), znajdz_zaklad(Targ, slowa[3])};
                    WROC2 Wytwornia[p.a].przekaz(Targ[p.b]);
                }
                else if (!czy[1]) BLAD(Żadne komis jeszcze nie istnieje);
                else ZADUZO;
            }
            JESLI(1,motocykl) {
                if (d == 3 && czy[1]) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        auto* tabk = new string[Targ.size()];
                        for (int i = 0; i < Targ.size(); i++) {
                            if (Targ[i]._nazwa() != "Nieznany") { //Jeśli nazwa komisu wynosi "Nieznany", wówczas komis nie istnieje
                                tabk[i] = "Komis " + Targ[i]._nazwa();
                            }
                        }
                        if (k != 0) { //Jeśli istnieje jakikolwiek komis program przekierowuje nas do wyboru komisu
                            wyswietl_ekran(k, tabk, "Lista komisów", "Wybierz komis, do którego chcesz przenieść pojazd"); //Ekran wyboru komisów
                            int komis = 0;
                            cin >> komis;
                            komis = czy_dobra_wartosc(komis, k, 1);
                            Wytwornia[a].przekaz(Targ[komis - 1]);
                        }
                        delete[] tabk;
                    }
                }
                else if (d == 4 && czy[1]) {
                    Pozycja p = {znajdz_zaklad(Wytwornia, slowa[2]), znajdz_zaklad(Targ, slowa[3])};
                    WROC2 Wytwornia[p.a].przekaz(Targ[p.b]);
                }
                else if (!czy[1]) BLAD(Żadne komis jeszcze nie istnieje);
                else ZADUZO;
            }
            JESLI(1,rower) {
                if (d == 3 && czy[2]) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        auto* tabs = new string[Market.size()];
                        for (int i = 0; i < Market.size(); i++) {
                            if (Market[i]._nazwa() != "Nieznany") { //Jeśli nazwa sklepu wynosi "Nieznany", wówczas sklep nie istnieje
                                tabs[i] = "Sklep " + Market[i]._nazwa();
                            }
                        }
                        if (s != 0) { //Jeśli istnieje jakikolwiek komis program przekierowuje nas do wyboru komisu
                            wyswietl_ekran(s, tabs, "Lista sklepów", "Wybierz sklep, do którego chcesz przenieść rower"); //Ekran wyboru komisów
                            int sklep = 0;
                            cin >> sklep;
                            sklep = czy_dobra_wartosc(sklep, s, 1);
                            Wytwornia[a].przekaz(Market[sklep - 1]);
                        }
                        delete[] tabs;
                    }
                }
                else if (d == 4 && czy[2]) {
                    Pozycja p = {znajdz_zaklad(Wytwornia, slowa[2]), znajdz_zaklad(Market, slowa[3])};
                    WROC2 Wytwornia[p.a].przekaz(Market[p.b]);
                }
                else if (!czy[2]) BLAD(Żaden sklep jeszcze nie istnieje);
                else ZADUZO;
            }
            else ZAMALO;
        } //Przekazuje dany pojazd do sklepu lub komisu

        else JESLI(0,sprzedaj) {
            JESLI(1,samochod) {
                if (d == 2) ZAMALO;
                else if (czy[1] && czy[3] && d == 3) {
                    unsigned int a = znajdz_zaklad(Targ, slowa[2]);
                    WROC1 Targ[a].sprzedaz(saldo);
                }
                else if (!czy[1]) BLAD(Żaden komis jeszcze nie istnieje!);
                else if (!czy[3]) BLAD(Żadne pojazd jeszcze nie istnieje!);
                else ZADUZO;
            }
            else JESLI(1,motocykl) {
                if (d == 2) ZAMALO;
                else if (czy[1] && czy[3] && d == 3) {
                    unsigned int a = znajdz_zaklad(Targ, slowa[2]);
                    WROC1 Targ[a].sprzedaz(saldo);
                }
                else if (!czy[1]) BLAD(Żaden komis jeszcze nie istnieje!);
                else if (!czy[3]) BLAD(Żadne pojazd jeszcze nie istnieje!);
                else ZADUZO;
            }
            else JESLI(1,rower) {
                if (d == 2) ZAMALO;
                else if (czy[2] && czy[4] && d == 3) {
                    unsigned int a = znajdz_zaklad(Market, slowa[2]);
                    WROC1 Market[a].sprzedaz(saldo);
                }
                else if (!czy[2]) BLAD(Żaden sklep jeszcze nie istnieje!);
                else if (!czy[4]) BLAD(Żadne rower jeszcze nie istnieje!);
                else ZADUZO;
            }
            else ZAMALO;
        }

        else JESLI(0,kup) {
            JESLI(1,samochod) {
                if (d == 3 && czy[1]) {
                    unsigned int a = znajdz_zaklad(Targ, slowa[2]);
                    WROC1 {
                        unsigned int pusty = Targ[a].znajdz_puste();
                        if (pusty == Targ[a].pojemnosc) czy_ok("W komisie nie ma miejsca na nowe pojazdy");
                        Targ[a].Samochody[pusty] = Targ[a].kup_auto(saldo, czy[3]);
                        Targ[a].Prototypy[pusty] = &Targ[a].Samochody[pusty];
                    }
                }
                else if (!czy[1]) BLAD(Żaden komis jeszcze nie istnieje!);
                else ZADUZO;
            }
            else JESLI(1,motocykl) {
                if (d == 3 && czy[1]) {
                    unsigned int a = znajdz_zaklad(Targ, slowa[2]);
                    WROC1 {
                        unsigned int pusty = Targ[a].znajdz_puste();
                        if (pusty == Targ[a].pojemnosc) czy_ok("W komisie nie ma miejsca na nowe pojazdy");
                        Targ[a].Motocykle[pusty] = Targ[a].kup_motor(saldo, czy[3]);
                        Targ[a].Prototypy[pusty] = &Targ[a].Motocykle[pusty];
                    }
                }
                else if (!czy[1]) BLAD(Żaden komis jeszcze nie istnieje!);
                else ZADUZO;
            }
            else ZAMALO;
        }

        else JESLI(0,lakieruj) {
            unsigned int staz = pobierz_los(1,30);
            Lakiernik Spec {"Zbyszek", staz};
            JESLI(1,samochod) {
                if (d == 3 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd(Targ, slowa[2]);
                    WROC2 Spec.przelakieruj(Targ[p.a].Prototypy[p.b], saldo);
                }
                else if (d == 4 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[2], slowa[3]);
                    WROC2 Spec.przelakieruj(Targ[p.a].Prototypy[p.b], saldo);
                }
                else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                else ZADUZO;
            }
            else JESLI(1,motocykl) {
                if (d == 3 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd(Targ, slowa[2]);
                    WROC2 Spec.przelakieruj(Targ[p.a].Prototypy[p.b], saldo);
                }
                else if (d == 4 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[2], slowa[3]);
                    WROC2 Spec.przelakieruj(Targ[p.a].Prototypy[p.b], saldo);
                }
                else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                else ZADUZO;
            }
            else JESLI(1,rower) {
                if (d == 3 && czy[4] && czy[2]) {
                    Pozycja p = znajdz_pojazd(Market, slowa[2]);
                    WROC2 Spec.przelakieruj(Market[p.a].Prototypy[p.b], saldo);
                }
                else if (d == 4 && czy[4] && czy[2]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Market, slowa[2], slowa[3]);
                    WROC2 Spec.przelakieruj(Market[p.a].Prototypy[p.b], saldo);
                }
                else if (!czy[2]) BLAD(Żaden sklep nie istnieje!);
                else if (!czy[4]) BLAD(Żaden rower nie istnieje!);
                else ZADUZO;
            }
            else ZAMALO;
        }

        else JESLI(0,tankuj) {
            JESLI(1,samochod) {
                if (d == 3 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd(Targ, slowa[2]);
                    WROC2 Targ[p.a].Samochody[p.b].zatankuj(saldo);
                }
                else if (d == 4 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[2], slowa[3]);
                    WROC2 Targ[p.a].Samochody[p.b].zatankuj(saldo);
                }
                else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                else ZADUZO;
            }
            else JESLI(1,motocykl) {
                if (d == 3 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd(Targ, slowa[2]);
                    WROC2 Targ[p.a].Motocykle[p.b].zatankuj(saldo);
                }
                else if (d == 4 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[2], slowa[3]);
                    WROC2 Targ[p.a].Motocykle[p.b].zatankuj(saldo);
                }
                else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                else ZADUZO;
            }
            else ZAMALO;
        }

        else JESLI(0,nasmaruj) {
            JESLI(1,rower) {
                if (d == 3 && czy[4] && czy[2]) {
                    Pozycja p = znajdz_pojazd(Market, slowa[2]);
                    WROC2 Market[p.a].Rowery[p.b].zatankuj(saldo);
                }
                else if (d == 4 && czy[4] && czy[2]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Market, slowa[2], slowa[3]);
                    WROC2 Market[p.a].Rowery[p.b].zatankuj(saldo);
                }
                else if (!czy[2]) BLAD(Żaden sklep nie istnieje!);
                else if (!czy[4]) BLAD(Żaden rower nie istnieje!);
                else ZADUZO;
            }
            else ZAMALO;
        }

        else JESLI(0,jazda) {
            JESLI(1,samochod) {
                if (d == 3 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd(Targ, slowa[2]);
                    WROC2 Targ[p.a].Samochody[p.b].jazda(saldo);
                }
                else if (d == 4 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[2], slowa[3]);
                    WROC2 Targ[p.a].Samochody[p.b].jazda(saldo);
                }
                else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                else ZADUZO;
            }
            else JESLI(1,motocykl) {
                if (d == 3 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd(Targ, slowa[2]);
                    WROC2 Targ[p.a].Motocykle[p.b].jazda(saldo);
                }
                else if (d == 4 && czy[3] && czy[1]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[2], slowa[3]);
                    WROC2 Targ[p.a].Motocykle[p.b].jazda(saldo);
                }
                else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                else ZADUZO;
            }
            else JESLI(1,rower) {
                if (d == 3 && czy[4] && czy[2]) {
                    Pozycja p = znajdz_pojazd(Market, slowa[2]);
                    WROC2 Market[p.a].Rowery[p.b].jazda(saldo);
                }
                else if (d == 4 && czy[4] && czy[2]) {
                    Pozycja p = znajdz_pojazd_w_zakladzie(Market, slowa[2], slowa[3]);
                    WROC2 Market[p.a].Rowery[p.b].jazda(saldo);
                }
                else if (!czy[2]) BLAD(Żaden sklep nie istnieje!);
                else if (!czy[4]) BLAD(Żaden rower nie istnieje!);
                else ZADUZO;
            }
            else ZAMALO;
        }

        else JESLI(0,zaladuj) {
            JESLI(1,bagaznik) {
                JESLI(2,samochod) {
                    if (d == 4 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd(Targ, slowa[3]);
                        WROC2 Targ[p.a].Samochody[p.b].zaladuj();
                    }
                    else if (d == 5 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[3], slowa[4]);
                        WROC2 Targ[p.a].Samochody[p.b].zaladuj();
                    }
                    else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                    else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                    else ZADUZO;
                }
                else JESLI(2,motocykl) {
                    if (d == 3 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd(Targ, slowa[3]);
                        WROC2 Targ[p.a].Motocykle[p.b].zaladuj();
                    }
                    else if (d == 4 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[3], slowa[4]);
                        WROC2 Targ[p.a].Motocykle[p.b].zaladuj();
                    }
                    else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                    else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                    else ZADUZO;
                }
                else JESLI(2,rower) {
                    if (d == 4 && czy[4] && czy[2]) {
                        Pozycja p = znajdz_pojazd(Market, slowa[3]);
                        WROC2 Market[p.a].Rowery[p.b].zaladuj();
                    }
                    else if (d == 5 && czy[4] && czy[2]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Market, slowa[3], slowa[4]);
                        WROC2 Market[p.a].Rowery[p.b].zaladuj();
                    }
                    else if (!czy[2]) BLAD(Żaden sklep nie istnieje!);
                    else if (!czy[4]) BLAD(Żaden rower nie istnieje!);
                    else ZADUZO;
                }
                else ZAMALO;
            }
            else ZAMALO;
        }

        else JESLI(0,rozladuj) {
            JESLI(1,bagaznik) {
                JESLI(2,samochod) {
                    if (d == 4 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd(Targ, slowa[3]);
                        WROC2 Targ[p.a].Samochody[p.b].rozladuj(Koszyk);
                    }
                    else if (d == 5 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[3], slowa[4]);
                        WROC2 Targ[p.a].Samochody[p.b].rozladuj(Koszyk);
                    }
                    else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                    else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                    else ZADUZO;
                }
                else JESLI(2,motocykl) {
                    if (d == 3 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd(Targ, slowa[3]);
                        WROC2 Targ[p.a].Motocykle[p.b].rozladuj(Koszyk);
                    }
                    else if (d == 4 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[3], slowa[4]);
                        WROC2 Targ[p.a].Motocykle[p.b].rozladuj(Koszyk);
                    }
                    else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                    else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                    else ZADUZO;
                }
                else JESLI(2,rower) {
                    if (d == 4 && czy[4] && czy[2]) {
                        Pozycja p = znajdz_pojazd(Market, slowa[3]);
                        WROC2 Market[p.a].Rowery[p.b].rozladuj(Koszyk);
                    }
                    else if (d == 5 && czy[4] && czy[2]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Market, slowa[3], slowa[4]);
                        WROC2 Market[p.a].Rowery[p.b].rozladuj(Koszyk);
                    }
                    else if (!czy[2]) BLAD(Żaden sklep nie istnieje!);
                    else if (!czy[4]) BLAD(Żaden rower nie istnieje!);
                    else ZADUZO;
                }
                else ZAMALO;
            }
            else ZAMALO;
        }

        else JESLI(0,wyswietl) {
            JESLI(1,komis) {
                if (czy[1] && d == 3) {
                    unsigned int a = znajdz_zaklad(Targ, slowa[2]);
                    WROC1 {
                        Targ[a].wyswietl(); //wyświetl stan komisu
                        int powrot = 0;
                        cin >> powrot;
                        string info = "Naciśnij " + to_string(Targ[a].pojemnosc + 1) + " aby opuścić okienko";
                        powrot = czy_dobra_wartosc(powrot, Targ[a].pojemnosc + 1, Targ[a].pojemnosc + 1, info);
                    }
                }
                else if (!czy[1]) BLAD(Żaden komis nie istnieje);
                else ZADUZO;
            }
            else JESLI(1,fabryka) {
                if (czy[0] && d == 3) {
                    unsigned int a = znajdz_zaklad(Wytwornia, slowa[2]);
                    WROC1 {
                        Wytwornia[a].wyswietl();
                        int powrot = 0;
                        cin >> powrot;
                        string info = "Naciśnij " + to_string(Wytwornia[a].pojemnosc + 1) + " aby opuścić okienko";
                        powrot = czy_dobra_wartosc(powrot, Wytwornia[a].pojemnosc + 1, Wytwornia[a].pojemnosc + 1, info);
                    }
                }
                else if (!czy[0]) BLAD(Żadna fabryka nie istnieje);
                else ZADUZO;
            }
            else JESLI(1,sklep) {
                if (czy[2] && d == 3) {
                    unsigned int a = znajdz_zaklad(Market, slowa[2]);
                    WROC1 {
                        Market[a].wyswietl();
                        int powrot = 0;
                        cin >> powrot;
                        string info = "Naciśnij " + to_string(Market[a].pojemnosc + 1) + " aby opuścić okienko";
                        powrot = czy_dobra_wartosc(powrot, Market[a].pojemnosc + 1, Market[a].pojemnosc + 1, info);
                    }
                }
                else if (!czy[2]) BLAD(Żaden sklep nie istnieje);
                else ZADUZO;
            }
            else JESLI(1, bagaznik) {
                JESLI(2,samochod) {
                    if (d == 4 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd(Targ, slowa[3]);
                        WROC2 Targ[p.a].Prototypy[p.b]->wyswietl_bagaznik(Targ[p.a].Prototypy[p.b]->seria(), "Stan bagażnika");
                    }
                    else if (d == 5 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[3], slowa[4]);
                        WROC2 Targ[p.a].Prototypy[p.b]->wyswietl_bagaznik(Targ[p.a].Prototypy[p.b]->seria(), "Stan bagażnika");
                    }
                    else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                    else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                    else ZADUZO;
                }
                else JESLI(2,motocykl) {
                    if (d == 4 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd(Targ, slowa[3]);
                        WROC2 Targ[p.a].Prototypy[p.b]->wyswietl_bagaznik(Targ[p.a].Prototypy[p.b]->seria(), "Stan bagażnika");
                    }
                    else if (d == 5 && czy[3] && czy[1]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Targ, slowa[3], slowa[4]);
                        WROC2 Targ[p.a].Prototypy[p.b]->wyswietl_bagaznik(Targ[p.a].Prototypy[p.b]->seria(), "Stan bagażnika");
                    }
                    else if (!czy[1]) BLAD(Żaden komis nie istnieje!);
                    else if (!czy[3]) BLAD(Żaden pojazd nie istnieje!);
                    else ZADUZO;
                }
                else JESLI(2,rower) {
                    if (d == 4 && czy[4] && czy[2]) {
                        Pozycja p = znajdz_pojazd(Market, slowa[3]);
                        WROC2 Market[p.a].Rowery[p.b].wyswietl_bagaznik(Market[p.a].Rowery[p.b].seria(), "Stan bagażnika");
                    }
                    else if (d == 5 && czy[4] && czy[2]) {
                        Pozycja p = znajdz_pojazd_w_zakladzie(Market, slowa[3], slowa[4]);
                        WROC2 Market[p.a].Rowery[p.b].wyswietl_bagaznik(Market[p.a].Rowery[p.b].seria(), "Stan bagażnika");
                    }
                    else if (!czy[2]) BLAD(Żaden sklep nie istnieje!);
                    else if (!czy[4]) BLAD(Żaden rower nie istnieje!);
                    else ZADUZO;
                }
                else ZAMALO;
            }
            else ZAMALO;
        }

        else JESLI(0,pomoc) {
            cout << R""(Lista komend:

1./tworz zaklad_rodzaj "zaklad_nazwa" "pojemność" (marża)
2./tworz pojazd_rodzaj fabryka_nazwa "marka" (silnik) "typ" "kolor" (moc)
3./import zaklad_rodzaj plik.txt
4./eksport zaklad_rodzaj zaklad_nazwa
5/eksport wszystko
6./przekaz pojazd_rodzaj fabryka_nazwa "zaklad_nazwa"
7./kup pojazd_rodzaj komis_nazwa
8./sprzedaj pojazd_rodzaj zaklad_nazwa
9./lakieruj pojazd_rodzaj nr_seryjny "zaklad_nazwa"
10./tankuj pojazd_rodzaj nr_seryjny "zaklad nazwa"
11./nasmaruj rower nr_seryjny "sklep_nazwa"
12./jazda pojazd_rodzaj nr_seryjny "zaklad_nazwa"
13./zaladuj pojazd_rodzaj nr_seryjny "zaklad_nazwa"
14./rozladuj pojazd_rodzaj nr_seryjny "zaklad_nazwa"
15./wyswietl zaklad_rodzaj "zaklad_nazwa"
16./wyswietl bagaznik pojazd_rodzaj nr_seryjny "zaklad_nazwa"
17./zakoncz program
18./pomoc

Legenda:

1. / - znak rozpoczynający komendę
2. "" - opcjonalna część komendy
3. () - część komendy obowiązująca w określonych przypadkach

Jeśli nazwa własna zawiera spację, należy ją zastąpić znakiem '-' (minus))"";
            if (d > 1) ZADUZO;
        } //Wyświetla listę komend

        else JESLI(0,zakoncz) {
            JESLI(1,program) {
                if (d == 2) {
                    cout << "Program zakończony pomyślnie!" << endl;
                    exit(1);
                }
                else ZADUZO;
            }
            else ZAMALO;
        }

        else BLAD(Podano niewłaściwą komendę);

        delete[] slowa;

        sleep(1);
    }


}

void pobierz_i_wykonaj(const string& plik) {
    fstream polecenia;
    polecenia.open(plik, ios::in);
    string rozkaz;
    int n = 0;
    while(getline(polecenia, rozkaz)) if (!rozkaz.empty()) n++;
    polecenia.close();

    auto* rozkazy = new string[n];

    polecenia.open(plik, ios::in);
    int i = 0;
    while(getline(polecenia, rozkaz)) {
        if (!rozkaz.empty()) {
            rozkazy[i] = rozkaz;
            i++;
        }
    }
    polecenia.close();

    string info[5] = {"Uwaga!", "Program wykonywany z konsoli poleceń", "Wykonywane będą polecenia podane w pliku ", "Aby zobaczyć listę poleceń wpisz /pomoc", "Wybierz \"1\", aby kontynuować"};
    info[2] += plik;
    czy_ok(5,info);

    wykonaj(rozkazy, n);

    delete[] rozkazy;
}
//Funkcja sprawdza, w jakim trybie użytkownik chce operować i czy plik jest zgodny
bool plik_czy_cin(int ile, char** plik) {
    if (ile < 2) return false;
    string tekst = string(plik[1]);
    if (tekst.substr(tekst.size() - 4) != ".txt") {
        czy_ok("Plik nie posiada rozszerzenia .txt");
        return false;
    }
    pobierz_i_wykonaj(tekst);
    return true;
}
//Szuka pojazdu w danym zakładzie
template<class X>
Pozycja znajdz_pojazd_w_zakladzie(vector<X> &FIRMA, const string& pojazd, const string& nazwa) {
    unsigned int a = 0;
    unsigned int b = 0;
    Pozycja pos; //Domyślnie pos.a = 1000 i pos.b = 1000
    for (int i = 0; i < FIRMA.size(); i++) { //Wpierw szuka czy istnieje zakład o podanej nazwie
        if (nazwa_bez_spacji(FIRMA[i]._nazwa()) == nazwa) {
            a = i;
            break;
        }
        if (i == FIRMA.size() - 1) {
            return pos;
        }
    }
    for (int i = 0; i < FIRMA[a].pojemnosc; i++) { //Potem szuka w znalezionym zakładzie pojazdu
        if (FIRMA[a].Prototypy[i]->seria() == pojazd) {
            b = i;
            break;
        }
        if (i == FIRMA[a].pojemnosc - 1) {
            cout << "Polecenie nie zostało wykonane" << endl <<  "Nie ma takiego pojazdu!" << endl;
            return pos;
        }
    }
    pos = {a,b}; //zwraca a-pozycję zakładu oraz b-pozycję pojazdu
    return pos;
}
//Szuka zakładu o podanej nazwie
template<class X>
unsigned int znajdz_zaklad(vector<X> &FIRMA, const string& nazwa) {
    int a = 0;
    for (int i = 0; i < FIRMA.size(); i++) {
        if (nazwa_bez_spacji(FIRMA[i]._nazwa()) == nazwa) {
            a = i;
            break;
        }
        if (i == FIRMA.size() - 1) {
            cout << "Polecenie nie zostało wykonane" << endl << "Nie znaleziono takiego zakładu" << endl;
            return 1000;
        }
    }
    return a;
}
//Szuka pojazdu w każdym zakładzie danego typu
template<class X>
Pozycja znajdz_pojazd(vector<X> &FIRMA, const string& pojazd) {
    unsigned int a = 0;
    unsigned int b = 0;
    for (int i = 0; i < FIRMA.size(); i++) { //Przebiega po każdym komisie/sklepie
        for (int j = 0; j < FIRMA[i].pojemnosc; j++) { //Przebiega po każdym pojeździe
            if (FIRMA[i].Prototypy[j]->seria() == pojazd) {
                a = i;
                b = j;
                Pozycja pos{a,b};
                return pos;
            }
        }
    }
    cout << "Polecenie nie zostało wykonane" << endl << "Nie znaleziono takiego pojazdu" << endl;
    Pozycja pos;
    return pos;
}