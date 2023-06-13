//
// Created by Dawid Makowski on 02.12.2021.
//

#include "klasy_zaklady.h"

Samochod Samochod::konstrukcja(double &saldo, const string &nazwaF) { //Tu się odbywa konstrukcja samochodów

    double cena = 0.0; //Końcowy koszt produkcji samochodu
    string model_user;
    string silnik_user;
    string nadwozie_user;
    string kolor_user;
    double moc = 100.0;
    unsigned int w = 0; //Wybór

    //Wybór modelu

    model_user = dolacz_model(w); //Przypisanie modelu pojazdu do zmiennej lokalnej
    cena += w * 10000; //Określenie kosztu produkcji

    //Wybór silnika

    silnik_user = dolacz_silnik(w); //Przypisanie silnika do zmiennej lokalnej
    cena += w * 1000;

    //Wybór nadwozia

    nadwozie_user = dolacz_nadwozie(w); //Przypisanie rodzaju nadwozia do zmiennej lokalnej
    cena += w * 100;

    //Wybór koloru

    kolor_user = dolacz_kolor(w);
    cena += w * 10;

    //Określenie mocy silnika

    okresl_moc(moc);
    cena += 200.0 * pow(5.0, (double)moc / 50.0); //Zależność mocy i kosztów

    if (saldo - cena < 0) {
        string tab[3];
        tab[0] = "Niestety, nie stać nas na wyprodukowanie takiego samochodu, wybierz tańsze części.";
        tab[1] = "Koszt wyprodukowania tego pojazdu: " + prec(cena, 2) + "zł";
        tab[2] = "Twoje saldo: " + prec(saldo, 2) + "zł";
        wyswietl_info(3,tab);
        Samochod brak;
        return brak;
    }

    cena /= 4; //Jako że w fabryce odbywa się masowa produkcja wszystko jest tańsze

    saldo -= cena;
    Samochod nowy(model_user, silnik_user, nadwozie_user, kolor_user, moc, cena, 0, nazwaF); //Finalna konstrukcja auta
    return nowy;
}

Motocykl Motocykl::konstrukcja(double &saldo, const string &nazwaF) {

    double cena = 0.0; //Końcowy koszt produkcji motocyklu
    string model_user;
    string silnik_user;
    string typ_user;
    string kolor_user;
    double moc = 100.0;
    unsigned int w = 0; //Wybór

    //Wybór modelu

    model_user = dolacz_model(w); //Przypisanie modelu pojazdu do zmiennej lokalnej
    cena += w * 5000; //Określenie kosztu produkcji

    //Wybór silnika

    silnik_user = dolacz_silnik(w); //Przypisanie silnika do zmiennej lokalnej
    cena += w * 1000;

    //Wybór typu

    typ_user = dolacz_typ(w); //Przypisanie rodzaju nadwozia do zmiennej lokalnej
    cena += w * 5000;

    //Wybór koloru

    kolor_user = dolacz_kolor(w);
    cena += w * 10;

    //Określenie mocy silnika

    okresl_moc(moc);
    cena += 200.0 * pow(5.0, (double)moc / 50.0); //Zależność mocy i kosztów

    if (saldo - cena < 0) {
        string tab[3];
        tab[0] = "Niestety, nie stać nas na wyprodukowanie takiego samochodu, wybierz tańsze części.";
        tab[1] = "Koszt wyprodukowania tego pojazdu: " + prec(cena, 2) + "zł";
        tab[2] = "Twoje saldo: " + prec(saldo, 2) + "zł";
        wyswietl_info(3,tab);
        Motocykl brak;
        return brak;
    }

    cena /= 4; //Jako że w fabryce odbywa się masowa produkcja wszystko jest tańsze

    saldo -= cena;
    Motocykl nowy(model_user, silnik_user, typ_user, kolor_user, moc, cena, 0, nazwaF); //Finalna konstrukcja motocyklu
    return nowy;

}

Rower Rower::konstrukcja(double &saldo, const string &nazwaF) { //Tu się odbywa konstrukcja samochodów

    double cena = 0.0; //Końcowy koszt produkcji samochodu
    string model_user;
    string typ_user;
    string kolor_user;
    unsigned int w = 0; //Wybór

    //Wybór modelu

    model_user = dolacz_marke(w); //Przypisanie modelu pojazdu do zmiennej lokalnej
    cena += w * 1000; //Określenie kosztu produkcji

    //Wybór typu

    typ_user = dolacz_typ(w); //Przypisanie rodzaju nadwozia do zmiennej lokalnej
    cena += w * 100;

    //Wybór koloru

    kolor_user = dolacz_kolor(w);
    cena += w * 10;

    if (saldo - cena < 0) {
        string tab[3];
        tab[0] = "Niestety, nie stać nas na wyprodukowanie takiego roweru, wybierz tańsze części.";
        tab[1] = "Koszt wyprodukowania roweru: " + prec(cena, 2) + "zł";
        tab[2] = "Twoje saldo: " + prec(saldo, 2) + "zł";
        wyswietl_info(3,tab);
        Rower brak;
        return brak;
    }

    cena /= 4; //Jako że w fabryce odbywa się masowa produkcja wszystko jest tańsze

    saldo -= cena;
    Rower nowy(model_user, typ_user, kolor_user, cena, nazwaF); //Finalna konstrukcja roweru
    return nowy;
}