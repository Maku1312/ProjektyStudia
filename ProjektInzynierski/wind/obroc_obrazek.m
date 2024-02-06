cd 'C:/Users/gmako/AGH/Projekt_Inz/Wizualizacja/Aplikacja/wind'

[obraz, ~, alfa] = imread("wind00.png");

for i = 1:360
    plik = strcat("wind", int2str(i), ".png");
    imwrite(imrotate(obraz,360-i), plik, 'Alpha', imrotate(alfa,360-i));
end