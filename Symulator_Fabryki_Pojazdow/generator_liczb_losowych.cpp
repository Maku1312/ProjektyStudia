//
// Created by Dawid Makowski on 05.01.2022.
//

#include "funkcje_inne.h"

static unsigned int tab_los[100];

void los() { //funkcja generuje 100 liczb losowych
    random_device rd;
    mt19937::result_type ziarno = rd() ^ ((mt19937::result_type)
            chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count() + (mt19937::result_type)
            chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count());

    mt19937 gen(ziarno);
    uniform_int_distribution<unsigned> distrib(1, 1000);

    for(unsigned int& i : tab_los)
    {
        unsigned int a;
        a = distrib(gen);
        i = a;
    }
}

unsigned int pobierz_los(unsigned int min, unsigned int max) { //tworzy liczbę losową z przedziału, wykorzystując wcześniej wygenerowaną tablicę
    if (max < min) return 0; //Jeśli max mniejsze od min to funkcja się nie wykonuje
    int i = 0;
    for (i = 0; i < 100; i++) { //szuka pierwszej, niewykorzystanej liczby w tablicy
        if (tab_los[i] < 1000) {
            break;
        }
        if (i == 99 && tab_los[i] > 1000) { //jeśli wszystkie liczby zostałyby wykorzystane, tablica jest resetowana
            los();
            i = 0;
            break;
        }
    }
    double polozenie = (double)tab_los[i] / 1000; //liczby w tablicy są od 0 do 1000, tu następuje konwersja na od min do max
    tab_los[i] = 1000;
    double l = polozenie * (max - min) + min;
    unsigned int y;
    if (int(l * 10) % 10 > 4) y = int(l + 1.0); //zaokrąglanie, jeśli pierwsza liczba po przecinku jest większa od 4, to zaokrąglam w górę
    else y = (int)l; //jeśli nie, to po prostu obcinam część po przecinku
    return y;
}