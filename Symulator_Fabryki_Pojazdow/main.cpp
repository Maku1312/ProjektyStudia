//
// Created by Dawid Makowski on 30.11.2021.
//

#include "klasy_pojazdy.h"
#include "klasy_zaklady.h"

using namespace std;

string wybierz_plik(string nazwa, const string& sciezka = "./", string info = "###");

int main(int argc, char *argv[]) //W funkcji main głównym elementem jest instrukcja switch/case, która stanowi podstawę menu głównego
{

    los(); //tworzy tablicę 100 losowych liczb

    if(plik_czy_cin(argc, argv)) return 0; //Uruchamianie z podaniem w konsoli ścieżki do pliku lub domyślnie

    vector<Fabryka> Wytwornia(20); //wektor fabryk
    vector<Komis> Targ(20); //wektor komisów
    vector<SklepRowerowy> Market(20); //wektor sklepów
    string koszyk[20] {};

    double saldo = 1000000.0; //ile mamy środków
    int f = 0; //obecna liczba fabryk
    int k = 0; //obecna liczba komisów
    int s = 0; //obecna liczba sklepów

    bool czy_fabryka = false; //Czy została stworzona jakakolwiek fabryka?
    bool czy_komis = false; //Czy został stworzony jakikolwiek komis?
    bool czy_sklep = false;
    bool czy_pojazd = false; //Czy zostało wyprodukowane jakiekolwiek pojazd?
    bool czy_rower = false; //Czy został wyprodukowany jakikolwiek rower?
    bool czy_zapytac = false; //Czy zapytać użytkownika o to, czy na pewno chce opuścić program (zależne od tego, czy wykonał jakąś akcję)

    int wybierz = 0; //Wybór
    wyswietl_menu();
    while (wybierz != 11) {
        cin >> wybierz;
        wybierz = czy_dobra_wartosc(wybierz, 11, 1);
        switch(wybierz) {
            case 1: { //Tworzenie komisu
                Komis KOM = tworz_komis();
                Targ[k] = KOM;

                czy_komis = true; //Można zarządzać komisem
                czy_zapytac = true; //Wprowadzone zostały zmiany

                string info = "Utworzono komis " + Targ[k]._nazwa();
                czy_ok(info);

                wyswietl_menu();
                k++;
                break;
            }
            case 2: { //Tworzenie fabryki
                Fabryka FAB = tworz_fabryke();
                Wytwornia[f] = FAB;

                czy_fabryka = true; //Można zarządzać fabryką
                czy_zapytac = true; //Wprowadzone zostały zmiany

                string info = "Utworzono fabrykę " + Wytwornia[f]._nazwa();
                czy_ok(info);

                wyswietl_menu();
                f++;
                break;
            }
            case 3: { //Twórz sklep rowerowy
                SklepRowerowy SR = tworz_sklep();
                Market[s] = SR;

                czy_sklep = true; //Można zarządzać sklepem
                czy_zapytac = true; //Wprowadzone zostały zmiany

                string info = "Utworzono sklep rowerowy " + Market[s]._nazwa();
                czy_ok(info);

                wyswietl_menu();
                s++;
                break;
            }
            case 4: { //Importuj komis z pliku
                string plik;

                if(!tak_czy_nie("Czy chcesz zaimportować plik z domyślnego folderu?")) {
                    wyswietl_info("Podaj ścieżkę do folderu, w którym znajdują się pliki do zaimportowania");
                    string sciezka_str;
                    IG;
                    getline(cin,sciezka_str);
                    plik = wybierz_plik("Wybierz plik, z którego chcesz zaimportować komis", sciezka_str);
                }
                else {
                    plik = wybierz_plik("Wybierz plik, z którego chcesz zaimportować komis");
                }

                if (plik.empty()) break;

                try {
                    Targ[k] = importuj_komis(plik);
                }
                catch (const out_of_range& error) {
                    cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać komis" << endl;
                    cerr << "what(): " << error.what() << endl << flush;
                    wyswietl_menu();
                    break;
                }
                catch (const invalid_argument &niepoprawny_arg) {
                    cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać komis" << endl;
                    cerr << "what():" <<  niepoprawny_arg.what() << endl << flush;
                    wyswietl_menu();
                    break;
                }
                catch (...) {
                    cerr << "Wykryto niezidentyfikowany problem" << endl;
                    cerr << "Sprawdź ponownie strukturę swojego pliku!" << endl << flush;
                    wyswietl_menu();
                    break;
                }

                czy_komis = true; //Można zarządzać komisem
                czy_pojazd = true; //Można zarządzać pojazdami
                czy_zapytac = true; //Wprowadzone zostały zmiany

                wyswietl_menu();
                k++;
                break;
            }
            case 5: { //Importuj fabrykę z pliku
                string plik;

                if(!tak_czy_nie("Czy chcesz zaimportować plik z domyślnego folderu?")) {
                    wyswietl_info("Podaj ścieżkę do folderu, w którym znajdują się pliki do zaimportowania");
                    string sciezka_str;
                    IG;
                    getline(cin, sciezka_str);
                    plik = string(wybierz_plik("Wybierz plik, z którego chcesz zaimportować fabrykę", sciezka_str));
                }
                else {
                    plik = string(wybierz_plik("Wybierz plik, z którego chcesz zaimportować fabrykę"));
                }

                if (plik.empty()) break;

                try {
                    Wytwornia[f] = importuj_fabryke(plik);
                }
                catch (const out_of_range& error) {
                    cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać fabrykę" << endl;
                    cerr << "what(): " << error.what() << endl << flush;
                    wyswietl_menu();
                    break;
                }
                catch (const invalid_argument &niepoprawny_arg) {
                    cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać fabrykę" << endl;
                    cerr << "what():" <<  niepoprawny_arg.what() << endl << flush;
                    wyswietl_menu();
                    break;
                }
                catch (...) {
                    cerr << "Wykryto niezidentyfikowany problem" << endl;
                    cerr << "Sprawdź ponownie strukturę swojego pliku!" << endl << flush;
                    wyswietl_menu();
                    break;
                }

                czy_fabryka = true; //Można zarządzać fabryką
                czy_pojazd = true; //Można zarządzać pojazdami
                czy_rower = true; //Można zarządzać rowerami
                czy_zapytac = true; //Wprowadzone zostały zmiany

                wyswietl_menu();
                f++;
                break;
            }
            case 6: { //Importuj sklep rowerowy
                string plik;

                if(!tak_czy_nie("Czy chcesz zaimportować plik z domyślnego folderu?")) {
                    wyswietl_info("Podaj ścieżkę do folderu, w którym znajdują się pliki do zaimportowania");
                    string sciezka_str;
                    IG;
                    getline(cin, sciezka_str);
                    plik = string(wybierz_plik("Wybierz plik, z którego chcesz zaimportować sklep", sciezka_str));
                }
                else {
                    plik = string(wybierz_plik("Wybierz plik, z którego chcesz zaimportować sklep"));
                }

                if (plik.empty()) break;

                try {
                    Market[s] = importuj_sklep(plik);
                }
                catch (const out_of_range& error) {
                    cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać sklep" << endl;
                    cerr << "what(): " << error.what() << endl << flush;
                    wyswietl_menu();
                    break;
                }
                catch (const invalid_argument &niepoprawny_arg) {
                    cerr << "Błąd wczytywania pliku. Upewnij się, że chcesz wczytać sklep" << endl;
                    cerr << "what():" << niepoprawny_arg.what() << endl << flush;
                    wyswietl_menu();
                    break;
                }
                catch (...) {
                    cerr << "Wykryto niezidentyfikowany problem" << endl;
                    cerr << "Sprawdź ponownie strukturę swojego pliku!" << endl << flush;
                    wyswietl_menu();
                    break;
                }

                czy_sklep = true; //Można zarządzać sklepem
                czy_rower = true; //Można zarządzać rowerami
                czy_zapytac = true; //Wprowadzone zostały zmiany

                wyswietl_menu();
                s++;
                break;
            }
            case 7: { //Zarządzaj komisem, po wybraniu otworzy się nowe menu
                //sprawdza, czy istnieje komis
                if (czy_komis) {
                    if (k == 1) { //Jeśli jest jeden komis to nie wyświetla ekranu wyboru komisów
                        Targ[0].zarzadzaj(czy_pojazd, saldo, koszyk);
                    } else { //Jeśli jest więcej komisów, to wyświetla ekran wyboru komisów
                        string tabk[k];
                        for (int i = 0; i < k; i++) {
                            tabk[i] = "Komis " + Targ[i]._nazwa();
                        }
                        wyswietl_ekran(k, tabk, "Lista komisów", "Wybierz komis do zarządzania"); //Ekran wyboru komisów
                        int w = 0;
                        cin >> w;
                        w = czy_dobra_wartosc(w, k, 1);
                        Targ[w - 1].zarzadzaj(czy_pojazd, saldo, koszyk); //Zarządzanie komisem
                    }
                    wyswietl_menu();
                    czy_zapytac = true; //Wprowadzone zostały zmiany
                }
                else {
                    wyswietl_menu("Żaden komis jeszcze nie istnieje, wybierz inną akcję");
                }

                break;
            }
            case 8: { //Zarządzaj fabryką, po wybraniu otworzy się nowe menu
                if (czy_fabryka) { //sprawdza, czy istnieje fabryka
                    if (f == 1) { //Jeśli jest jedna fabryka to nie wyświetla ekranu wyboru fabryk
                        Wytwornia[0].zarzadzaj(czy_pojazd, czy_rower, saldo, Targ, Market);
                    }
                    else { //Jeśli jest więcej fabryk, to wyświetla ekran wyboru fabryk
                        string tabf[f];
                        for (int i = 0; i < f; i++) {
                            tabf[i] = "Fabryka " + Wytwornia[i]._nazwa();
                        }
                        wyswietl_ekran(f, tabf, "Lista fabryk", "Wybierz fabrykę do zarządzania");
                        int w = 0;
                        cin >> w;
                        w = czy_dobra_wartosc(w, f, 1);
                        Wytwornia[w - 1].zarzadzaj(czy_pojazd, czy_rower, saldo, Targ, Market); //Zarządzanie fabryką
                    }
                    wyswietl_menu();
                    czy_zapytac = true; //Wprowadzone zostały zmiany
                }
                else {
                    wyswietl_menu("Żadna fabryka jeszcze nie istnieje, wybierz inną akcję");
                }
                break;
            }
            case 9: { //Zarządzaj sklepem rowerowym
                if (czy_sklep) {
                    if (s == 1) { //Jeśli jest jeden sklep to nie wyświetla ekranu wyboru sklepów
                        Market[0].zarzadzaj(czy_rower, saldo, koszyk);
                    } else { //Jeśli jest więcej sklepów, to wyświetla ekran wyboru sklepów
                        string tabs[s];
                        for (int i = 0; i < s; i++) {
                            tabs[i] = "Sklep " + Market[i]._nazwa();
                        }
                        wyswietl_ekran(k, tabs, "Lista sklepów", "Wybierz sklep do zarządzania"); //Ekran wyboru sklepów
                        int w = 0;
                        cin >> w;
                        w = czy_dobra_wartosc(w, s, 1);
                        Market[w - 1].zarzadzaj(czy_rower, saldo, koszyk); //Zarządzanie sklepem
                    }
                    wyswietl_menu();
                    czy_zapytac = true; //Wprowadzone zostały zmiany
                }
                else {
                    wyswietl_menu("Żaden sklep jeszcze nie istnieje, wybierz inną akcję");
                }

                break;
            }
            case 10: { //Zapisz wszystko
                if (czy_fabryka || czy_komis || czy_sklep) {
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
                    wyswietl_menu();
                    czy_zapytac = false; //Zmiany zostały zapisane
                }
                else {
                    wyswietl_menu("W programie nie ma żadnego obiektu do zapisania, wybierz inną akcję");
                }
                break;
            }
            case 11: { //Koniec programu
                if (czy_zapytac) { //Jeśli istnieją jakieś niezapisane zmiany, to musimy poinformować użytkownika, że utraci wszystkie zmiany
                    if (tak_czy_nie("W programie są niezapisane zmiany", "Czy chcesz je zapisać?")) {
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
                }
                break;
            }
            default: {
                wyswietl_info("Błąd techniczny! Koniec programu.");
                return 0;
            }
        }
    }
    czy_ok("Program zakończony pomyślnie!");
    return 0;
}