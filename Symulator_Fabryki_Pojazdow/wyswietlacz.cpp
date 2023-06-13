//
// Created by Dawid Makowski on 30.11.2021.
//

#include "funkcje_inne.h"

using namespace std;

//plik zawiera, funkcje wyświetlające alfanumeryczny ekran na różne sposoby
//wyświetl_menu() wyświetla główne menu w trzech kolumnach (11 opcji)
//wyświetl_ekran(), wyświetla jakieś menu wyboru w dwóch kolumnach
//wyświetl_info(), wyświetla informację w jednej linijce lub wielu
//wyświetl_ok(), działa podobnie jak wyświetl_info(), ale wyświetla pod spodem tekst 'ok'

void wyswietl_menu(string info) {

    string nazwa = "Program Fabryki & Pojazdy";
    char znak = '#';
    vector<string> opcje = {"Twórz nowy komis", "Twórz nową fabrykę", "Twórz nowy sklep rowerowy", "Importuj komis z pliku", "Importuj fabrykę z pliku", "Importuj sklep z pliku", "Zarządzaj komisem", "Zarządzaj fabryką", "Zarządzaj sklepem", "Zapisz cały projekt", "Zakończ program", "############################"};
    string odstep = "|\n|" + string(32, znak) + "|" + string(32, znak) + "|" + string(32, znak) + "|\n";
    unsigned int odstepy[] {6,5,4,3,1,1, 3,3,2,1,3,3, 5,5,5,4,5,5, 4,4,6,6,0,0};
    if (info.size() > 90) {
        info = info.substr(0,86);
        for (int i = 0; i < 4; i++) {
            info[86 - i] = '.';
        }
    }
    unsigned int infoparz = 0;
    if (info.size()%2) infoparz= 1;
    unsigned int spacje = 45 - (info.size()) / 2;

    cout << "\n " << string(98, '_') << endl;
    cout << "/" << string(32, znak) << "|" << string(32, znak) << "|" << string(32, znak) << "\\\n";
    cout << "|" + string(4, znak) + string(32, ' ') + nazwa + string(33, ' ') + string(4, znak) + "|\n";
    cout << "|" + string(4, znak) + string(spacje, ' ') + info + string(spacje - infoparz, ' ') + string(4, znak);

    cout << odstep;

    int k = 0;
    for (int i = 0; i < 4; i++) {
        cout << "|";
        for (int j = 3 * i; j < 3 * (i + 1); j++) {
            cout << string(2, znak) << string(odstepy[2 * j], ' ') << opcje[j] << string(odstepy[2 * j + 1], ' ') << string(2, znak) << "|";
        }
        cout << endl;
        for (int j = k; j < k + 3; j++) {
            cout << "|" << string(14, znak) << "|" << to_string(int((j+1)/10)) << to_string(int((j+1)%10)) << "|" << string(14, znak);
        }
        k += 3;
        cout << odstep;
    }
    cout << "\\" << string(32, znak) << "|" << string(32, znak) << "|" << string(32, znak) << "/" << endl;
    cout << " " << string(98, '-') << endl;
}

void wyswietl_ekran(vector<string> opcje, string nazwa, string info, char znak) { //Funkcja tworzy okno wyborowe dla wybranej ilości opcji

    try { //Obsługa wyjątku, jeśli funkcja wyświetlająca nie zadziała

        unsigned int ile = opcje.size(); //wielkość wektora stringów

        int inf_parz; //czy info ma parzystą liczbę znaków
        int naz_parz; //czy nazwa ma parzystą liczbę znaków

        if (nazwa.size() > 105) nazwa = nazwa.substr(0, 105) + "..."; //Jeśli nazwa za długa, to jest skracana
        if (info.size() > 105) info = info.substr(0, 105) + "..."; //I tu tak samo

        if (nazwa.find('\n') <
            nazwa.size()) { //Jeśli zdarzy się, że w tekście będzie '\n' (co popsuje estetykę okienka), to tekst taki nie jest wypisywany
            nazwa = "Błędny zapis tekstu!";
        }

        if (info.find('\n') < info.size()) {
            info = "Błędny zapis tekstu!";
        }

        if (info.size() % 2) { inf_parz = 0; }
        else { inf_parz = 1; }

        if (nazwa.size() % 2) { naz_parz = 0; }
        else { naz_parz = 1; }

        unsigned int rozmiar; //przechowuje rozmiar największego stringa przesłanego do funkcji, dzięki temu każdy tekst się zmieści
        unsigned int m = opcje[0].size();
        if (nazwa.size() > info.size()) { rozmiar = nazwa.size(); }
        else { rozmiar = info.size(); }
        for (int i = 0; i < ile; i++) {
            if (opcje[i].size() > 49) {
                opcje[i] = opcje[i].substr(0, 49) + "..."; //Jeśli tekst za długi to skracamy
            }
            if (rozmiar < opcje[i].size()) rozmiar = opcje[i].size();
            if (opcje[i].size() > m) m = opcje[i].size();
        }

        if (rozmiar > 105) rozmiar = 105; //Gdyby rozmiar był większy to ekran by się rozwarstwił

        if (rozmiar < 2 * m && rozmiar > m && m < 49) {
            rozmiar = m;
        }

        if (rozmiar >= 2 * m) {
            rozmiar = 1 + rozmiar / 2;
        }

        if (rozmiar % 2) { rozmiar++; }

        string nazwa1;
        string info1;
        unsigned int spacje_nazwa = rozmiar + 2 - (nazwa.size()) / 2;
        unsigned int spacje_info = rozmiar + 2 - (info.size()) / 2;
        //Tu tworzone są Tytuł i podtytuł
        nazwa1 = "|" + string(4, znak) + string(spacje_nazwa, ' ') + nazwa + string(spacje_nazwa + naz_parz, ' ') +
                 string(4, znak) + "|\n";
        info1 = "|" + string(4, znak) + string(spacje_info, ' ') + info + string(spacje_info + inf_parz, ' ') +
                string(4, znak) + "|\n";
        //Tu zaczyna się drukowanie okienka
        cout << "\n " << string(2 * rozmiar + 13, '_') << endl;
        cout << "/" << string(rozmiar + 6, znak) << "|" << string(rozmiar + 6, znak) << "\\\n";
        cout << nazwa1; //Wydruk tytułu
        cout << info1; //Wydruk podtytułu

        for (int i = 0; i < ile; i += 2) { //Pętla drukująca wszystkie opcje wyboru

            if (opcje[i].find('\n') <
                opcje[i].size()) { //Jeśli zdarzy się, że w tekście będzie '\n' (co popsuje estetykę okienka), to tekst taki nie jest wypisywany
                opcje[i] = "Błędny zapis tekstu!";
            }
            if (i + 1 < ile) {
                if (opcje[i + 1].find('\n') < opcje[i + 1].size()) {
                    opcje[i + 1] = "Błędny zapis tekstu!";
                }
            }
            unsigned int spacje1; //Ilość spacji dla opcji z prawej strony dla dobrego wyjustowania
            int nparz1 = 0;
            int nparz2 = 0;
            string opcje1; //tekst opcji z prawej wraz z odstępami justującymi
            string opcje2; //tekst opcji z lewej wraz z odstępami justującymi

            int dz2 = (i + 1) / 10; //Tworzę cyfrę dziesiątek indeksu wyboru dla opcji z lewej
            int jd2 = (i + 1) % 10; //Tworzę cyfrę jedności indeksu wyboru dla opcji z lewej
            int dz1 = (i + 2) / 10; //Tworzę cyfrę dziesiątek indeksu wyboru dla opcji z prawej
            int jd1 = (i + 2) % 10; //Tworzę cyfrę jedności indeksu wyboru dla opcji z prawej

            //Tworzę opcje1 - tekst opcji z prawej strony
            if (i + 1 >=
                ile) { //Jeśli opcja z prawej strony ma indeks większy niż liczba wyborów, wówczas zapycha się ten indeks domyślnym znakiem
                opcje1 = string(rozmiar, znak);
            } else {
                spacje1 = (rozmiar - opcje[i + 1].size()) / 2;
                if (opcje[i + 1].size() %
                    2) { nparz1 = 1; } //Jeśli opcja z prawej jest nieparzysta przypisuję zmiennej nparz1 jedynkę
                opcje1 = string(spacje1 + nparz1, ' ') + opcje[i + 1] +
                         string(spacje1, ' '); //Tworzę tekst opcji wraz ze spacjami
            }

            //Tworzę opcje0 - tekst opcji z lewej strony
            unsigned int spacje2 =
                    (rozmiar - opcje[i].size()) / 2; //Ilość spacji dla opcji z lewej strony dla dobrego wyjustowania
            if (opcje[i].size() %
                2) { nparz2 = 1; } //Jeśli opcja z lewej jest nieparzysta przypisuję zmiennej nparz2 jedynkę
            opcje2 = string(spacje2 + nparz2, ' ') + opcje[i] +
                     string(spacje2, ' '); //Tworzę tekst opcji wraz ze spacjami

            //Wydruk opcji "i" oraz opcji "i+1" obok siebie
            cout << "|" << string(rozmiar + 6, znak) << "|" << string(rozmiar + 6, znak) << "|" << endl;
            cout << "|" << string(2, znak) << " " << opcje2 << " " << string(2, znak) << "|" << string(2, znak) << " "
                 << opcje1 << " " << string(2, znak) << "|" << endl;
            cout << "|" << string(rozmiar / 2 + 1, znak) << "|" << dz2 << jd2 << "|" << string(rozmiar / 2 + 1, znak)
                 << "|" << string(rozmiar / 2 + 1, znak) << "|" << dz1 << jd1 << "|" << string(rozmiar / 2 + 1, znak)
                 << "|" << endl;
        }
        cout << "\\" << string(rozmiar + 6, znak) << "|" << string(rozmiar + 6, znak) << "/" << endl;
        cout << " " << string(2 * rozmiar + 13, '-') << endl;
        //Koniec wydruku
    }
    catch (const out_of_range& error) {
        cerr << "Wygląda na to, że funkcja odpowiedzialna" << endl;
        cerr << "za wyświetlanie ekranu przestała działać" << endl;
        cerr << "Jeśli zechcesz możesz zgłosić problem na mojego maila" << endl;
        cerr << "what(): " << error.what() << endl << flush;
    }
    catch (...) {
        cerr << "Wykryto niezidentyfikowany problem" << endl;
        cerr << "Wygląda na to, że funkcja odpowiedzialna" << endl;
        cerr << "za wyświetlanie ekranu przestała działać" << endl;
        cerr << "Jeśli zechcesz możesz zgłosić problem na mojego maila" << endl << flush;
    }
}

void wyswietl_ekran(unsigned int ile, string* opcje, string nazwa, string info, char znak) { //Funkcja tworzy okno wyborowe dla wybranej ilości opcji

    try { //Obsługa wyjątku, jeśli funkcja wyświetlająca nie zadziała
        int inf_parz; //czy info ma parzystą liczbę znaków
        int naz_parz; //czy nazwa ma parzystą liczbę znaków

        if (nazwa.size() > 105) nazwa = nazwa.substr(0, 105) + "...";
        if (info.size() > 105) info = info.substr(0, 105) + "...";

        if (nazwa.find('\n') < nazwa.size()) { //Jeśli zdarzy się, że w tekście będzie '\n' (co popsuje estetykę okienka), to tekst taki nie jest wypisywany
            nazwa = "Błędny zapis tekstu!";
        }

        if (info.find('\n') < info.size()) {
            info = "Błędny zapis tekstu!";
        }

        if (info.size()%2) {inf_parz = 0;}
        else {inf_parz = 1;}

        if (nazwa.size()%2) {naz_parz = 0;}
        else {naz_parz = 1;}

        unsigned int rozmiar; //przechowuje rozmiar największego stringa przesłanego do funkcji, dzięki temu każdy tekst się zmieści
        unsigned int m = opcje[0].size();
        if (nazwa.size() > info.size()) {rozmiar = nazwa.size();}
        else {rozmiar = info.size();}
        for (int i = 0; i < ile; i++) {
            if (opcje[i].size() > 49) {
                opcje[i] = opcje[i].substr(0, 49) + "...";
            }
            if (rozmiar < opcje[i].size()) rozmiar = opcje[i].size();
            if (opcje[i].size() > m) m = opcje[i].size();
        }

        if (rozmiar > 105) rozmiar = 105;

        if (rozmiar < 2 * m && rozmiar > m && m < 49) {
            rozmiar = m;
        }

        if (rozmiar >= 2 * m) {
            rozmiar = 1 + rozmiar / 2;
        }

        if (rozmiar%2) {rozmiar++;}

        string nazwa1;
        string info1;
        unsigned int spacje_nazwa = rozmiar + 2 - (nazwa.size()) / 2;
        unsigned int spacje_info = rozmiar + 2 - (info.size()) / 2;
        //Tu tworzone są Tytuł i podtytuł
        nazwa1 = "|" + string(4, znak) + string(spacje_nazwa, ' ') + nazwa + string(spacje_nazwa + naz_parz, ' ') + string(4, znak) + "|\n";
        info1 = "|" + string(4, znak) + string(spacje_info, ' ') + info + string(spacje_info + inf_parz, ' ') + string(4, znak) + "|\n";
        //Tu zaczyna się drukowanie okienka
        cout << "\n " << string(2*rozmiar + 13, '_') << endl;
        cout << "/" << string(rozmiar + 6, znak) << "|" << string(rozmiar + 6, znak) << "\\\n";
        cout << nazwa1; //Wydruk tytułu
        cout << info1; //Wydruk podtytułu

        for(int i = 0; i < ile; i += 2) { //Pętla drukująca wszystkie opcje wyboru

            if (opcje[i].find('\n') < opcje[i].size()) { //Jeśli zdarzy się, że w tekście będzie '\n' (co popsuje estetykę okienka), to tekst taki nie jest wypisywany
                opcje[i] = "Błędny zapis tekstu!";
            }
            if (i + 1 < ile) {
                if (opcje[i + 1].find('\n') < opcje[i + 1].size()) {
                    opcje[i + 1] = "Błędny zapis tekstu!";
                }
            }
            unsigned int spacje1; //Ilość spacji dla opcji z prawej strony dla dobrego wyjustowania
            int nparz1 = 0;
            int nparz2 = 0;
            string opcje1; //tekst opcji z prawej wraz z odstępami justującymi
            string opcje2; //tekst opcji z lewej wraz z odstępami justującymi

            int dz2 = (i+1)/10; //Tworzę cyfrę dziesiątek indeksu wyboru dla opcji z lewej
            int jd2 = (i+1)%10; //Tworzę cyfrę jedności indeksu wyboru dla opcji z lewej
            int dz1 = (i+2)/10; //Tworzę cyfrę dziesiątek indeksu wyboru dla opcji z prawej
            int jd1 = (i+2)%10; //Tworzę cyfrę jedności indeksu wyboru dla opcji z prawej

            //Tworzę opcje1 - tekst opcji z prawej strony
            if (i + 1 >= ile) { //Jeśli opcja z prawej strony ma indeks większy niż liczba wyborów, wówczas zapycha się ten indeks domyślnym znakiem
                opcje1 = string(rozmiar, znak);
            }

            else {
                spacje1 = (rozmiar - opcje[i+1].size())/2;
                if(opcje[i+1].size()%2) {nparz1 = 1;} //Jeśli opcja z prawej jest nieparzysta przypisuję zmiennej nparz1 jedynkę
                opcje1 = string(spacje1 + nparz1, ' ') + opcje[i+1] + string(spacje1, ' '); //Tworzę tekst opcji wraz ze spacjami
            }

            //Tworzę opcje0 - tekst opcji z lewej strony
            unsigned int spacje2 = (rozmiar - opcje[i].size())/2; //Ilość spacji dla opcji z lewej strony dla dobrego wyjustowania
            if(opcje[i].size()%2) {nparz2 = 1;} //Jeśli opcja z lewej jest nieparzysta przypisuję zmiennej nparz2 jedynkę
            opcje2 = string(spacje2 + nparz2, ' ') + opcje[i] + string(spacje2, ' '); //Tworzę tekst opcji wraz ze spacjami

            //Wydruk opcji "i" oraz opcji "i+1" obok siebie
            cout << "|" << string(rozmiar + 6, znak) << "|" << string(rozmiar + 6, znak) << "|" << endl;
            cout << "|" << string(2, znak) << " " << opcje2 << " " << string(2, znak) << "|" << string(2, znak) << " " << opcje1 << " " << string(2, znak) << "|" << endl;
            cout << "|" << string(rozmiar/2 + 1, znak) << "|" << dz2 << jd2 << "|" << string(rozmiar/2 + 1, znak) << "|" << string(rozmiar/2 + 1, znak) << "|" << dz1 << jd1 << "|" << string(rozmiar/2 + 1, znak) << "|" << endl;
        }
        cout << "\\" << string(rozmiar + 6, znak) << "|" << string(rozmiar + 6, znak) << "/" << endl;
        cout << " " << string(2*rozmiar + 13, '-') << endl;
        //Koniec wydruku
    }
    catch (const out_of_range& error) {
        cerr << "Wygląda na to, że funkcja odpowiedzialna" << endl;
        cerr << "za wyświetlanie ekranu przestała działać" << endl;
        cerr << "Jeśli zechcesz możesz zgłosić problem na mojego maila" << endl;
        cerr << "what(): " << error.what() << endl << flush;
    }
    catch (...) {
        cerr << "Wykryto niezidentyfikowany problem" << endl;
        cerr << "Wygląda na to, że funkcja odpowiedzialna" << endl;
        cerr << "za wyświetlanie ekranu przestała działać" << endl;
        cerr << "Jeśli zechcesz możesz zgłosić problem na mojego maila" << endl << flush;
    }
}

void wyswietl_ok(string tekst, char znak) {

    if (tekst.find('\n') < tekst.size()) { //Jeśli zdarzy się, że w tekście będzie '\n' (co popsuje estetykę okienka), to tekst taki nie jest wypisywany
        tekst = "Błędny zapis tekstu!";
    }

    if (tekst.size() > 108) tekst = tekst.substr(0, 108) + "..."; //Gdyby tekst był dłuższy rozwarstwiłby się ekran

    unsigned int rozmiar = tekst.size();

    if (rozmiar%2 && rozmiar != 111) {rozmiar++;}

    cout << "\n" << " " << string(rozmiar + 6, '_') << " " << endl;
    cout << "/" << string(rozmiar + 6, znak) << "\\" << endl;
    cout << "|" << string(2, znak) << " " << tekst << " " << string(2, znak) << "|" << endl;
    cout << "|" << string(6 + rozmiar, znak) << "|" << endl;
    cout << "|" << string(2, znak) << string(rozmiar / 2, ' ') << "OK" << string(rozmiar / 2, ' ') << string(2, znak) << "|" << endl;
    cout << "\\" << string(rozmiar / 2 + 1, znak) << "|01|" << string(rozmiar / 2 + 1, znak) << "/" << endl;
    cout << " " << string(rozmiar + 6, '-') << " " << endl;

}

void wyswietl_ok(unsigned int ile, string* info, char znak) {

    unsigned int rozmiar = 0; //Przechowuje rozmiar największego stringa
    for (int i = 0; i < ile; i++) {
        if (info[i].size() > 108) {
            info[i] = info[i].substr(0, 108) + "..."; //Gdyby tekst był dłuższy rozwarstwiłby się ekran
            rozmiar = 111;
            break;
        }
        if (rozmiar < info[i].size()) {
            rozmiar = info[i].size();
        }
    }

    cout << "\n" << " " << string(rozmiar + 6, '_') << " " << endl; //Tu zaczyna się wydruk
    cout << "/" << string(rozmiar + 6, znak) << "\\" << endl;

    for (int i = 0; i < ile; i++) { //Pętla drukująca każdą linijkę

        if (info[i].find('\n') < info[i].size()) { //Jeśli zdarzy się, że w tekście będzie '\n' (co popsuje estetykę okienka), to tekst taki nie jest wypisywany
            info[i] = "Błędny zapis tekstu!";
        }

        int nparz = 0; /*zmienna potrzebna do wyrównania liczby spacji, gdy długość danego tekstu jest nieparzysta,
        natomiast długość najdłuższego tekstu jest parzysta (i na odwrót) wówczas znak kończący ramkę pojawia się o jeden znak za wcześnie*/

        if ((info[i].size() + rozmiar)%2) { //sprawdzam, czy suma długości danego tekstu i najdłuższego jest nieparzysta (przypadek parz + nparz)
            nparz = 1;
        }

        cout << "|" << string(2, znak) << " " << string((rozmiar-info[i].size())/2, ' ') << info[i] << string((rozmiar-info[i].size())/2 + nparz, ' ') << " " << string(2, znak) << "|" << endl;
        if (i != ile - 1) {
            cout << "|" << string(rozmiar+6, '-') << "|" << endl; //Kreski oddzielające drukowane między tekstami
        }

    }

    cout << "|" << string(6 + rozmiar, znak) << "|" << endl;
    cout << "|" << string(2, znak) << string(rozmiar / 2, ' ') << "OK" << string(rozmiar / 2, ' ') << string(2, znak) << "|" << endl;
    cout << "\\" << string(rozmiar / 2 + 1, znak) << "|01|" << string(rozmiar / 2 + 1, znak) << "/" << endl;
    cout << " " << string(rozmiar + 6, '-') << " " << endl;
}

void wyswietl_info(int ile, string *info, char znak) { //Funkcja tworzy okno informacyjne dla listy o określonej długości linijek

    unsigned int rozmiar = 0; //Przechowuje rozmiar największego stringa
    for (int i = 0; i < ile; i++) {
        if (info[i].size() > 108) {
            info[i] = info[i].substr(0, 108) + "..."; //Gdyby tekst był dłuższy rozwarstwiłby się ekran
            rozmiar = 111;
            break;
        }
        if (rozmiar < info[i].size()) {
            rozmiar = info[i].size();
        }
    }

    cout << "\n" << " " << string(rozmiar + 6, '_') << " " << endl; //Tu zaczyna się wydruk
    cout << "/" << string(rozmiar + 6, znak) << "\\" << endl;

    for (int i = 0; i < ile; i++) { //Pętla drukująca każdą linijkę

        if (info[i].find('\n') < info[i].size()) { //Jeśli zdarzy się, że w tekście będzie '\n' (co popsuje estetykę okienka), to tekst taki nie jest wypisywany
            info[i] = "Błędny zapis tekstu!";
        }

        int nparz = 0; /*zmienna potrzebna do wyrównania liczby spacji, gdy długość danego tekstu jest nieparzysta,
        natomiast długość najdłuższego tekstu jest parzysta (i na odwrót) wówczas znak kończący ramkę pojawia się o jeden znak za wcześnie*/

        if ((info[i].size() + rozmiar)%2) { //sprawdzam, czy suma długości danego tekstu i najdłuższego jest nieparzysta (przypadek parz + nparz)
            nparz = 1;
        }

        cout << "|" << string(2, znak) << " " << string((rozmiar-info[i].size())/2, ' ') << info[i] << string((rozmiar-info[i].size())/2 + nparz, ' ') << " " << string(2, znak) << "|" << endl;
        if (i != ile - 1) {
            cout << "|" << string(rozmiar+6, '-') << "|" << endl; //Kreski oddzielające drukowane między tekstami
        }

    }
    cout << "\\" << string(rozmiar + 6, znak) << "/" << endl;
    cout << " " << string(rozmiar + 6, '-') << " " << endl;
    //Koniec wydruku
}

void wyswietl_info(string tekst, char znak) { //Uproszczona wersja powyższej funkcji dla krótkiego tekstu

    if (tekst.find('\n') < tekst.size()) { //Jeśli zdarzy się, że w tekście będzie '\n' (co popsuje estetykę okienka), to tekst taki nie jest wypisywany
        tekst = "Błędny zapis tekstu!";
    }

    if (tekst.size() > 108) tekst = tekst.substr(0, 108) + "..."; //Gdyby tekst był dłuższy rozwarstwiłby się ekran


    unsigned int rozmiar = tekst.size(); //przechowuje rozmiar stringa
    cout << "\n" << " " << string(rozmiar + 6, '_') << " " << endl; //Każdy cout wypisuje inną linijkę
    cout << "/" << string(rozmiar + 6, znak) << "\\" << endl;
    cout << "|" << string(2, znak) << " " << tekst << " " << string(2, znak) << "|" << endl; //Linijka wypisująca nasz string
    cout << "\\" << string(rozmiar + 6, znak) << "/" << endl;
    cout << " " << string(rozmiar + 6, '-') << " " << endl;
    //Koniec wydruku
}