cmake_minimum_required(VERSION 3.17)
project(MakowskiDawid-etap2)

set(CMAKE_CXX_STANDARD 14)

add_executable(MakowskiDawid-etap2 komendy.cpp generator_liczb_losowych.cpp zarzadzanie.cpp metody_lokomocja.cpp metody_pojazdy.cpp metody_samochod.cpp metody_motocykl.cpp metody_rower.cpp metody_zaklady.cpp metody_fabryka.cpp metody_komis.cpp metody_sklep.cpp operacje_na_plikach.cpp wyswietlacz.cpp konstrukcja.cpp funkcje_main.cpp main.cpp)