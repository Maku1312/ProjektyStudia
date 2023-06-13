//
// Created by Dawid Makowski on 30.11.2021.
//

#include "klasy_zaklady.h"
#include <fstream>

//Plik zawiera funkcje importu/eksportu każdego typu zakładu
//Zawiera też funkcję wybierz_plik, która wyświetla wszystkie pliki.txt w danym katalogu

string wybierz_plik(string nazwa, const string &sciezka = "./", string info = "###") { //funkcja wyświetlająca nazwy plików w danej ścieżce
    int n = 0; //ile jest plików z rozszerzeniem .txt

    try {
        bool slash = false;
        for (int i = 0; i < sciezka.size(); i++) {
            if (sciezka[1] == '/') {
                slash = true;
            }
        }
        if (!slash) {
            throw invalid_argument("Podano niewłaściwą ścieżkę!");
        }
    }
    catch (const invalid_argument& error) {
        cerr << "Błąd przy sczytywaniu ścieżki: " << error.what() << endl << flush;
        return "";
    }

    const char* sciezka_char = sciezka.c_str();

    DIR *dir; //deklaruję ścieżkę
    struct dirent *czytaj;
    //Otwieram ścieżkę po raz pierwszy, żeby pozyskać liczbę plików spełniających warunki
    if ((dir = opendir(sciezka_char)) != nullptr) { //Jeśli ścieżka się otwiera, kod się wykonuje poprawnie, przy okazji otwieram ścieżkę
        while ((czytaj = readdir (dir)) != nullptr) { //Liczę, ile jest plików z rozszerzeniem .txt
            string s = czytaj->d_name; //nazwa pliku
            if (s.length() > 4) { //warunek 1: długość pliku większa niż 4
                if (s.substr(s.length() - 4) == ".txt" && s != "CMakeCache.txt") { //warunek 2: plik z rozszerzeniem .txt, pomijamy CMakeCache.txt
                    n++;
                }
            }
        }
        closedir(dir); //zamykam ścieżkę
    }
    else { //Jeśli ścieżka się nie otwiera
        wyswietl_info("Nie można otworzyć danej ścieżki!");
    }
    //Otwieram ścieżkę po raz drugi, żeby pozyskać nazwy plików
    auto* pliki = new string[n]; //sczytywanie nazw plików
    dir = opendir(sciezka_char); //otwieram ścieżkę i robię to samo
    int i = 0;
    while ((czytaj = readdir (dir)) != nullptr) {
        string s = czytaj->d_name;
        if (s.length() > 4) {
            if(s.substr(s.length() - 4) == ".txt" && s != "CMakeCache.txt") {
                pliki[i] = s; //przypisuję nazwy plików do tablicy stringów
                i++;
            }
        }
    }
    closedir(dir); //zamykam ścieżkę

    wyswietl_ekran(n, pliki, move(nazwa), move(info)); //wyświetlam przesłaną tablicę nazw plików

    int w = 0;
    cin >> w;
    w = czy_dobra_wartosc(w, n, 1); //dokonuję wyboru pliku

    string ret = pliki[w - 1];
    delete[] pliki;
    return ret;
}

Fabryka importuj_fabryke(const string &plik) { //importuje dane z pliku do klasy Fabryka
    string import[6] = {"Sprawdź czy plik ma dane zapisane w następujący sposób:",
                        "Linijka 1: Fabryka (nazwa)",
                        "Linijka 2: Pojemność: (liczba)",
                        "Linijka 3: 1.PL_PojazdFabrykaLicznik_Model_Nadwozie_Kolor_SilnikMoc Kolor Przebieg",
                        "Przykład:  2.PL_AutoRadom001_Audi_Kombi_Bi_Diesel120 Biały 150000",
                        "    ...    "};
    wyswietl_info(6, import);
    Fabryka F;
    if (!tak_czy_nie("Kontynuować?")) {
        return F;
    }

    fstream fabr; //Tu zaczyna się pobieranie tekstu linijka po linijce
    fabr.open(plik, ios::in);
    auto *dane = new string[2]; //tablica na dane
    getline(fabr,dane[0]);
    if (dane[0].substr(0, 7) != "Fabryka") {
        throw invalid_argument("Plik nie zawiera nazwy fabryki");
    }
    getline(fabr,dane[1]);
    if (!(stoi(dane[1].substr(11)) < 100 && stoi(dane[1].substr(11)) > 0)) {
        throw invalid_argument("Pojemność musi być liczbą całkowitą w przedziale od 0 do 100");
    }
    unsigned int Fpojemnosc = stoi(dane[1].substr(11));
    auto *pojazdy = new string[Fpojemnosc];
    int j = 0;
    while (j < Fpojemnosc) {
        if (j <= 10) {
            getline(fabr, pojazdy[j]);
            pojazdy[j] = pojazdy[j].substr(3);
        }
        else if (j > 10) {
            getline(fabr, pojazdy[j]);
            pojazdy[j] = pojazdy[j].substr(4);
        }
        j++;
    }
    fabr.close();
    string Fnazwa = dane[0].substr(8); //Odtąd tekst jest przetwarzany na klasę Pojazd
    Fabryka W(Fnazwa, Fpojemnosc);
    unsigned int ile = 0;
    for (int i = 0; i < j - 2; i++) {
        if (pojazdy[i].substr(3, 4) == "Auto") {
            Samochod S(pojazdy[i]);
            W.Samochody[i] = S;
            W.Prototypy[i] = &W.Samochody[i];
            ile++;
        }
        else if (pojazdy[i].substr(3, 8) == "Motocykl") {
            Motocykl M(pojazdy[i]);
            W.Motocykle[i] = M;
            W.Prototypy[i] = &W.Motocykle[i];
            ile++;
        }
        else if (pojazdy[i].substr(3, 5) == "Rower") {
            Rower R(pojazdy[i]);
            W.Rowery[i] = R;
            W.Prototypy[i] = &W.Rowery[i];
            ile++;
        }
        else if (pojazdy[i].substr(0, 3) == "...") {
            W.Prototypy[i] = &W.Samochody[i];
        }
        else {
            string info = "Nie udało się wczytać pojazdu " + pojazdy[i];
            wyswietl_info(info);
        }
    }
    delete[] dane;
    delete[] pojazdy;
    string s = "Pomyślnie zaimportowano fabrykę " + W.nazwa + " z " + to_string(ile) + " pojazdami.";
    czy_ok(s);
    return W;

}

Komis importuj_komis(const string &plik) {
    const char* plik1 = plik.c_str();
    string import[6] = {"Sprawdź czy plik ma dane zapisane w następujący sposób:",
                        "Linijka 1: Komis (nazwa)",
                        "Linijka 2: Pojemność: (liczba)",
                        "Linijka 3: 1.PL_PojazdKomisLicznik_Model_Nadwozie_Kolor_SilnikMoc Kolor Przebieg",
                        "Przykład:  2.PL_AutoRadom001_Audi_Kombi_Bi_Diesel120 Biały 150000",
                        "    ...    "};
    wyswietl_info(6, import);
    Komis K;
    if (!tak_czy_nie("Kontynuować?")) {
        return K;
    }

    fstream kom; //Tu zaczyna się pobieranie tekstu linijka po linijce
    kom.open(plik1, ios::in);
    auto *dane = new string[2]; //tablica na dane
    getline(kom,dane[0]);
    if (dane[0].substr(0, 5) != "Komis") {
        throw invalid_argument("Plik nie zawiera nazwy komisu");
    }
    getline(kom,dane[1]);
    if (!(stoi(dane[1].substr(11)) < 100 && stoi(dane[1].substr(11)) > 0)) {
        throw invalid_argument("Pojemność musi być liczbą całkowitą w przedziale od 0 do 100");
    }
    auto Kpojemnosc = stoi(dane[1].substr(11));
    auto *pojazdy = new string[Kpojemnosc];
    int j = 0;
    while (j < Kpojemnosc) {
        if (j <= 10) {
            getline(kom, pojazdy[j]);
            pojazdy[j] = pojazdy[j].substr(3);
        }
        else if (j > 10) {
            getline(kom, pojazdy[j]);
            pojazdy[j] = pojazdy[j].substr(4);
        }
        j++;
    }
    kom.close();
    string Knazwa = dane[0].substr(6); //Odtąd tekst jest przetwarzany na klasę Pojazd
    Komis W(Knazwa, Kpojemnosc);
    unsigned int ile = 0;
    for (int i = 0; i < j - 2; i++) {
        if (pojazdy[i].substr(3, 4) == "Auto") {
            Samochod S(pojazdy[i]);
            W.Samochody[i] = S;
            W.Prototypy[i] = &W.Samochody[i];
            ile++;
        }
        else if (pojazdy[i].substr(3, 8) == "Motocykl") {
            Motocykl M(pojazdy[i]);
            W.Motocykle[i] = M;
            W.Prototypy[i] = &W.Motocykle[i];
            ile++;
        }
        else if (pojazdy[i].substr(0, 3) == "...") {
            W.Prototypy[i] = &W.Samochody[i];
        }
        else {
            string info = "Nie udało się wczytać pojazdu " + pojazdy[i];
            wyswietl_info(info);
        }
    }
    delete[] dane;
    delete[] pojazdy;
    string s = "Pomyślnie zaimportowano komis " + W.nazwa + " z " + to_string(ile) + " pojazdami.";
    czy_ok(s);
    return W;

}

SklepRowerowy importuj_sklep(const string &plik) {
    const char* plik1 = plik.c_str();
    string import[6] = {"Sprawdź czy plik ma dane zapisane w następujący sposób:",
                        "Linijka 1: Sklep rowerowy (nazwa)",
                        "Linijka 2: Pojemność: (liczba)",
                        "Linijka 3: 1.PL_RowerFabrykaLicznik_Model_Typ_Kolor",
                        "Przykład:  2.PL_RowerRadom001_BMX_miejski_Czarny",
                        "    ...    "};
    wyswietl_info(6, import);
    SklepRowerowy S;
    if (!tak_czy_nie("Kontynuować?")) {
        return S;
    }

    fstream sklep; //Tu zaczyna się pobieranie tekstu linijka po linijce
    sklep.open(plik1, ios::in);
    auto *dane = new string[2]; //tablica na dane
    getline(sklep,dane[0]);
    if (dane[0].substr(0, 14) != "Sklep rowerowy") {
        throw invalid_argument("Plik nie zawiera nazwy sklepu");
    }
    getline(sklep,dane[1]);
    if (!(stoi(dane[1].substr(11)) < 100 && stoi(dane[1].substr(11)) > 0)) {
        throw invalid_argument("Pojemność musi być liczbą całkowitą w przedziale od 0 do 100");
    }
    auto Spojemnosc = stoi(dane[1].substr(11));
    auto *pojazdy = new string[Spojemnosc];
    int j = 0;
    while (j < Spojemnosc) {
        if (j <= 10) {
            getline(sklep, pojazdy[j]);
            pojazdy[j] = pojazdy[j].substr(3);
        }
        else if (j > 10) {
            getline(sklep, pojazdy[j]);
            pojazdy[j] = pojazdy[j].substr(4);
        }
        j++;
    }
    sklep.close();
    string Snazwa = dane[0].substr(15);
    SklepRowerowy R(Snazwa, Spojemnosc);
    unsigned int ile = 0;
    for (int i = 0; i < j - 2; i++) {
        if (pojazdy[i].substr(3, 5) == "Rower") {
            Rower B(pojazdy[i]);
            R.Rowery[i] = B;
            R.Prototypy[i] = &R.Rowery[i];
            ile++;
        }
        else if (pojazdy[i].substr(0, 3) == "...") {
            R.Rowery[i] = Rower();
            R.Prototypy[i] = &R.Rowery[i];
        }
        else {
            string info = "Nie udało się wczytać pojazdu " + pojazdy[i];
            wyswietl_info(info);
        }
    }
    delete[] dane;
    delete[] pojazdy;
    string s = "Pomyślnie zaimportowano sklep " + R.nazwa + " z " + to_string(ile) + " rowerami.";
    czy_ok(s);
    return R;

}

void Fabryka::eksportuj(const string &plik, bool info) { //eksportuje fabrykę do pliku w trybie dopisywania
    const char* plik1 = plik.c_str();
    fstream fabr;
    fabr.open(plik1, ios::app);
    fabr << "Fabryka " << nazwa_bez_spacji(nazwa) << endl;
    fabr << "Pojemnosc: " << pojemnosc << endl;
    for (int j = 0; j < pojemnosc; j++) {
        if (Prototypy[j]->seria().substr(3, 5) == "Rower") {
            fabr << j + 1 << ". " << Prototypy[j]->seria() << endl;
        }
        else if (Prototypy[j]->seria().substr(3, 4) == "Auto" || Prototypy[j]->seria().substr(3, 8) == "Motocykl") {
            auto* P = dynamic_cast<Pojazd*>(Prototypy[j]);
            fabr << j + 1 << ". " << P->seria() << " " << P->_kolor() << " " << P->_przebieg() << endl;
        }
        else if (Prototypy[j]->seria().substr(3, 4) == "....") {
            fabr << j + 1 << ". " << string(11, '.') + "Puste Miejsce" + string(11, '.') << endl;
        }
        else {
            string blad = "Nie można wyeksportować pojazdu " + Prototypy[j]->seria();
            wyswietl_info(blad);
            fabr << j + 1 << ". " << string(11, '.') + "Puste Miejsce" + string(11, '.') << endl;
        }
    }
    fabr.close();
    if (info) { //Wypisywana jest informacja o wyeksportowaniu pojedynczej fabryki, tylko gdy jest zapisywana oddzielnie
        string s[2];
        s[0] = "Pomyślnie wyeksportowano fabrykę " + nazwa + " z " + to_string(ile_pojazdow()) + " pojazdami.";
        s[1] = "do pliku " + plik;
        czy_ok(2, s);
    }
}

void Komis::eksportuj(const string  &plik, bool info) { //eksportuje fabrykę do pliku w trybie dopisywania
    const char* plik1 = plik.c_str();
    fstream fabr;
    fabr.open(plik1, ios::app);
    fabr << "Komis " << nazwa_bez_spacji(nazwa) << endl;
    fabr << "Pojemnosc: " << pojemnosc << '\n';
    for (int j = 0; j < pojemnosc; j++) {
        if (Prototypy[j]->seria().substr(3, 4) == "Auto" || Prototypy[j]->seria().substr(3, 8) == "Motocykl") {
            fabr << j + 1 << ". " << Prototypy[j]->seria() << " " << Prototypy[j]->_kolor() << " " << Prototypy[j]->_przebieg() << endl;
        }
        else if (Prototypy[j]->seria().substr(3, 4) == "....") {
            fabr << j + 1 << ". " << string(11, '.') + "Puste Miejsce" + string(11, '.') << endl;
        }
        else {
            string blad = "Nie można wyeksportować pojazdu " + Prototypy[j]->seria();
            wyswietl_info(blad);
            fabr << j + 1 << ". " << string(11, '.') + "Puste Miejsce" + string(11, '.') << endl;
        }
    }
    fabr.close();
    if (info) { //Wypisywana jest informacja o wyeksportowaniu pojedynczego komisu, tylko gdy jest zapisywany oddzielnie
        string s[2];
        s[0] = "Pomyślnie wyeksportowano komis " + nazwa + " z " + to_string(ile_pojazdow()) + " pojazdami.";
        s[1] = "do pliku " + plik;
        czy_ok(2, s);
    }
}

void SklepRowerowy::eksportuj(const string& plik, bool info) {
    const char* plik1 = plik.c_str();
    fstream fabr;
    fabr.open(plik1, ios::app);
    fabr << "Sklep rowerowy " << nazwa_bez_spacji(nazwa) << endl;
    fabr << "Pojemnosc: " << pojemnosc << '\n';
    for (int j = 0; j < pojemnosc; j++) {
        if (Rowery[j].seria().substr(3, 5) == "Rower") {
            fabr << j + 1 << ". " << Rowery[j].seria() << endl;
        }
        else if (Rowery[j].seria().substr(3, 4) == "....") {
            fabr << j + 1 << ". " << string(11, '.') + "Puste Miejsce" + string(11, '.') << endl;
        }
        else {
            string blad = "Nie można wyeksportować roweru " + Rowery[j].seria();
            wyswietl_info(blad);
            fabr << j + 1 << ". " << string(11, '.') + "Puste Miejsce" + string(11, '.') << endl;
        }
    }
    fabr.close();
    if (info) { //Wypisywana jest informacja o wyeksportowaniu pojedynczego komisu, tylko gdy jest zapisywany oddzielnie
        string s[2];
        s[0] = "Pomyślnie wyeksportowano sklep rowerowy " + nazwa + " z " + to_string(ile_rowerow()) + " rowerami.";
        s[1] = "do pliku " + plik;
        czy_ok(2, s);
    }
}