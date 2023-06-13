###  1. WCZYTANIE PACZEK  ###

#install.packages("automap")
library(automap)

#install.packages("httr")
library(httr)

#install.packages("jsonlite")
library(jsonlite)

#install.packages("maptools")
library(maptools)

#install.packages("spatstat")
library(spatstat)

#install.packages("sp")
library(sp)

#install.packages("sf")
library(sf)

#install.packages("plotrix")
library(plotrix)

#install.packages("tmaptools")
library(tmaptools)

setwd() # Ustaw własną ścieżkę

load(file = "10.01/10.01_21.Rdata")
load(file = "krk_point_23km.Rdata")

# krk_tab <- list_inst2

dzielnice <- st_read("DzielniceKrakowa/dzielnice_Krakowa.shp") #układ odniesienia(CRS) to ETRS89 (Poland CS92)
dzielniceWGS84 <- st_transform(dzielnice, crs = 4326) #Konwertujemy do WGS84 (4326)
krk_WGS84 <- st_union(dzielniceWGS84) #Zostawiamy tylko kontur miasta 
krk_UTM <- st_transform(krk_WGS84, CRS("+proj=utm +zone=34 +datum=WGS84")) #Przekształcamy na UTM

### 3. WIZUALIZACJA ###

## 3.1. PRZYGOTOWANIE WIZUALIZACJI ##

krk_centrum <- c(423941.75, 5546031.89) #Współrzędne centrum mapy

coord <- as.data.frame(krk_point_UTM)

left_down <- c(min(coord$lon), min(coord$lat)) #Współrzędne lewego dolnego rogu mapy
right_up <- c(max(coord$lon), max(coord$lat)) #Współrzędne prawego górnego rogu mapy

okno = owin(xrange = c(left_down[1], right_up[1]), yrange = c(left_down[2], right_up[2]))

krk_point_ppp_id <- ppp(
   x = krk_point_UTM$lon,
   y = krk_point_UTM$lat,
   marks=data.frame(elev = krk_point_UTM$elev, id = krk_point_UTM$id),
   window = okno
)

krk_n_id <- length(krk_point_ppp_id$marks$id) #Liczba czujników
krk_id <- krk_point_ppp_id$marks$id #Id czujników

id_PM25 <- matrix(NA, krk_n_id, 24)
id_temp <- matrix(NA, krk_n_id, 24)
id_hum <- matrix(NA, krk_n_id, 24)
id_press <- matrix(NA, krk_n_id, 24)

#KONWERSJA WŁAŚCIWYCH WARTOŚCI CURRENT DLA PYŁÓW, TEMPERATURY, WILGOTNOŚCI I CIŚNIENIA
for (i in seq(1, krk_n_id)) {
   
   for (j in 1:24) {
      
      #ZMIENNE LOGICZNE DO WYSZUKANIA PÓL O DANEJ NAZWIE
      logic_PM25 <- krk_tab[[i]]$history$values[[j]]$name == "PM25"
      logic_temp <- krk_tab[[i]]$history$values[[j]]$name == "TEMPERATURE"
      logic_hum <- krk_tab[[i]]$history$values[[j]]$name == "HUMIDITY"
      logic_press <- krk_tab[[i]]$history$values[[j]]$name == "PRESSURE"
      
      #Testujemy, czy istnieje jedno i tylko jedno takie pole (zdarzają się błędne odczyty - tych nie chcemy zapisać)
      if (sum(logic_PM25) == 1) id_PM25[i,j] <- krk_tab[[i]]$history$values[[j]][logic_PM25, 2]
      if (sum(logic_temp == 1)) id_temp[i,j] <- krk_tab[[i]]$history$values[[j]][logic_temp, 2]
      if (sum(logic_hum == 1)) id_hum[i,j] <- krk_tab[[i]]$history$values[[j]][logic_hum, 2]
      if (sum(logic_press == 1)) id_press[i,j] <- krk_tab[[i]]$history$values[[j]][logic_press, 2]
      
   }
   
}

krk_point_spdf <- as.SpatialPointsDataFrame.ppp(krk_point_ppp_id)
coordinates(krk_point_spdf)

krk_point_spdf$id_PM25 <- id_PM25
krk_point_spdf$id_temp <- id_temp
krk_point_spdf$id_hum <- id_hum
krk_point_spdf$id_press <- id_press

miss_PM25 <- is.na(krk_point_spdf$id_PM25)
miss_temp <- is.na(krk_point_spdf$id_temp)
miss_hum <- is.na(krk_point_spdf$id_hum)
miss_press <- is.na(krk_point_spdf$id_press)

#ZCZYTUJĘ WARTOŚCI MAKSYMALNE I MINIMALNE DLA POMIARÓW ŻEBY POTEM WYSKALOWAĆ KOLORY NA MAPIE
maks_PM25 <- max(krk_point_spdf$id_PM25, na.rm = TRUE)
maks_temp <- max(krk_point_spdf$id_temp, na.rm = TRUE)
maks_hum <- max(krk_point_spdf$id_hum, na.rm = TRUE)
maks_press <- max(krk_point_spdf$id_press, na.rm = TRUE)

min_PM25 <- min(krk_point_spdf$id_PM25, na.rm = TRUE)
min_temp <- min(krk_point_spdf$id_temp, na.rm = TRUE)
min_hum <- min(krk_point_spdf$id_hum, na.rm = TRUE)
min_press <- min(krk_point_spdf$id_press, na.rm = TRUE)

bound <- st_as_sf(krk_UTM)

size <- c(100,100)

points <- (right_up - left_down)/size
num_points <- ceiling(points)

grid <- GridTopology(left_down, size, num_points)

gridpoints <- SpatialPoints(grid, proj4string = CRS("+proj=utm +zone=34 +datum=WGS84"))

spgrid <- SpatialPixels(gridpoints)

prog_bar <- txtProgressBar(min = 0, max = 4*24, style = 3)

### 3.2. KRIGING (INTERPOLACJA) DLA KAŻDEJ ZMIENNEJ ###

for (j in 1:24) {
   
   
   
   
   #PALETA KOLORÓW DLA PUNKTÓW
   #PM25
   Pal <- colorRampPalette(c('#1a9850','#66bd63','#a6d96a','#d9ef8b','#ffffbf','#fee08b','#fdae61','#f46d43','#d73027'))
   krk_point_spdf$Col_PM <- Pal(10)[as.numeric(cut(krk_point_spdf$id_PM25[,j], breaks = 10))]
   #Temperatura
   Pal <- colorRampPalette(c('blue', 'white', 'red'))
   krk_point_spdf$Col_temp <- Pal(10)[as.numeric(cut(krk_point_spdf$id_temp[,j], breaks = 10))]
   #Ciśnienie
   Pal <- colorRampPalette(c("#1F28A2", "#0097A8", "#9CDABA", "#CDDF90", "#BC8A29", "#7D0112"))
   krk_point_spdf$Col_press <- Pal(10)[as.numeric(cut(krk_point_spdf$id_press[,j], breaks = 10))]
   #Wilgotność
   Pal <- colorRampPalette(c("#F9FFAF", "#AFE2A3", "#68C19D", "#259E98", "#09778D", "#2E4F79"))
   krk_point_spdf$Col_hum <- Pal(10)[as.numeric(cut(krk_point_spdf$id_hum[,j], breaks = 10))]
   
   
   
   modele <- c("Sph", "Exp", "Gau", "Ste")
   
   #INFORMACJA O DACIE I GODZINIE ODCZYTU
   data_godzina <- krk_tab[[1]]$history$fromDateTime[j] #Pobieram dane o czasie z dowolnego czujnika
   data <- substring(data_godzina, 1, 10) #Biorę substring tylko dla daty
   godzina <- as.integer(substring(data_godzina, 12, 13)) + 1 #Biorę substring tylko dla godziny
   print(paste("Mapa z godziny ", godzina, ":00. Iteracja: ", j, sep = ""))
   
   
   
   for (i in 1:4) {
      
      
      setTxtProgressBar(prog_bar, 4*(j-1)+i)
      
      if (i == 2) {
         elev_auto_PM25 <- autoKrige(id_PM25[,j] ~ 1, input_data = krk_point_spdf[!miss_PM25[,j],], new_data = spgrid, model = modele[i])
         elev_auto_temp <- autoKrige(id_temp[,j] ~ 1, input_data = krk_point_spdf[!miss_temp[,j],], new_data = spgrid, model = modele[i])
         
         
         
         ## 3.2.1. PM25 ##
         
         op <- par(cex = 0.5) #Manipulacja wielkością czcionki (tutaj aby zwiększyć rozmiar mapy)
         
         df <- data.frame( #Wyciągam wartości z autoKrige'u do dataframe'a aby wywołać go w funkcji image()
            x = elev_auto_PM25$krige_output[1]@coords[,1],
            y = elev_auto_PM25$krige_output[1]@coords[,2],
            z = elev_auto_PM25$krige_output[1]$var1.pred
         )
         df$z[1] <- min_PM25
         df$z[2] <- maks_PM25
         
         plik = paste("mapa_pm", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width = 559, height = 531)
         
         #KOLOROWA MAPA
         image(df, col = hcl.colors(40, "RdYlGn", rev = TRUE), xlab = "", ylab = "", axes = FALSE)
         
         #TYTUŁ MAPY
         op <- par(cex = 0.8) #Manipulacja wielkością czcionki (tutaj dla tytułu mapy)
         tytul <- paste("Mapa zanieczyszczeń PM2.5 na obszarze Krakowa\n w dniu ", data, " o godzinie ", godzina, ":00", sep = "")
         podtytul <- paste("Kriging: model \"", modele[i], "\"", sep = "")
         title(main = tytul, sub = podtytul, line = 0.2)
         
         #DODAWANIE INNYCH OBIEKTÓW NA MAPĘ
         op <- par(cex = 0.5) #Rozmiar czcionki
         plot(krk_UTM, add = TRUE) #Dodaję kontur miasta
         op <- par(cex = 1) #Rozmiar czcionki
         points(krk_point_spdf, pch = 19, col = krk_point_spdf$Col_PM) #Dodaję kolorowe punkty z pomiarami
         
         #LEGENDA
         
         op <- par(cex = 0.7, font = 2) #Pogrubiona czcionka
         
         leg_bok = 0.795 #Położenie poziome legendy
         leg_posx = (right_up[1] - left_down[1]) * leg_bok + left_down[1]
         leg_posx1 = (right_up[1] - left_down[1]) * (leg_bok + 0.006) + left_down[1]
         
         leg_wys = 0.15 #Wysokość na jakiej znajduje się legenda
         leg_posy = (right_up[2] - left_down[2]) * leg_wys + left_down[2]
         leg_posy1 = (right_up[2] - left_down[2]) * (leg_wys - 0.075) + left_down[2]
         
         legend( #Legenda górna
            leg_posx, leg_posy,
            legend = "Granice Krakowa",
            col = "black",
            lty = 1,
            bg = "#DBEA92",
            title = "Legenda",
            xpd = TRUE
         )
         
         #Wartości na legendzie dolnej
         maks = paste("max = ", as.character(round(max(df$z), 2)), "µg/m3")
         sred = paste("avg = ", as.character(round(mean(df$z), 2)), "µg/m3")
         mini = paste("min = ", as.character(round(min(df$z), 2)), "µg/m3")
         
         legend( #Legenda dolna
            leg_posx1, leg_posy1,
            legend = c(maks, sred, mini),
            fill = hcl.colors(3, "RdYlGn"),
            bg = "#DBEA92",
            title = "Stan zanieczyszczeń",
            xpd = TRUE
         )
         
         on.exit(par(opar))
         dev.off()
         
         ## 3.2.2. TEMPERATURA ##
         
         op <- par(cex = 0.5) #Manipulacja wielkością czcionki (tutaj aby zwiększyć rozmiar mapy)
         
         df <- data.frame( #Wyciągam wartości z autoKrige'u do dataframe'a aby wywołać go w funkcji image()
            x = elev_auto_temp$krige_output[1]@coords[,1],
            y = elev_auto_temp$krige_output[1]@coords[,2],
            z = elev_auto_temp$krige_output[1]$var1.pred
         )
         
         df$z[1] <- min_temp
         df$z[2] <- maks_temp
         
         plik = paste("mapa_temp", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width = 559, height = 531)
         
         #KOLOROWA MAPA
         image(df, col = hcl.colors(40, "RdBu", rev = TRUE), xlab = "", ylab = "", axes = FALSE)
         
         #TYTUŁ MAPY
         op <- par(cex = 0.8)
         tytul <- paste("Mapa temperatur na obszarze Krakowa\n w dniu ", data, " o godzinie ", godzina, ":00", sep = "")
         podtytul <- paste("Kriging: model \"", modele[i], "\"", sep = "")
         title(main = tytul, sub = podtytul, line = 0.2)
         
         #DODAWANIE INNYCH OBIEKTÓW NA MAPĘ
         op <- par(cex = 0.5) #Rozmiar czcionki
         plot(krk_UTM, add = TRUE) #Dodaję kontur miasta
         op <- par(cex = 1) #Rozmiar czcionki
         points(krk_point_spdf, pch = 19, col = krk_point_spdf$Col_temp) #Dodaję kolorowe punkty z pomiarami
         
         #LEGENDA
         
         op <- par(cex = 0.7, font = 2) #Pogrubiona czcionka
         
         leg_bok = 0.795 #Położenie poziome legendy
         leg_posx = (right_up[1] - left_down[1]) * leg_bok + left_down[1]
         leg_posx1 = (right_up[1] - left_down[1]) * (leg_bok + 0.048) + left_down[1]
         
         leg_wys = 0.15 #Wysokość na jakiej znajduje się legenda
         leg_posy = (right_up[2] - left_down[2]) * leg_wys + left_down[2]
         leg_posy1 = (right_up[2] - left_down[2]) * (leg_wys - 0.075) + left_down[2]
         
         legend( #Legenda górna
            leg_posx, leg_posy,
            legend = "Granice Krakowa",
            col = "black",
            lty = 1,
            bg = "white",
            title = "Legenda",
            xpd = TRUE
         )
         
         #Wartości na legendzie dolnej
         maks = paste("max = ", as.character(round(max(df$z), 2)), "°C")
         sred = paste("avg = ", as.character(round(mean(df$z), 2)), "°C")
         mini = paste("min = ", as.character(round(min(df$z), 2)), "°C")
         
         legend( #Legenda dolna
            leg_posx1, leg_posy1,
            legend = c(maks, sred, mini),
            fill = hcl.colors(3, "RdBu"),
            bg = "white",
            title = "Temperatura",
            xpd = TRUE
         )
         
         on.exit(par(opar))
         dev.off()
         
         
         
         
         
      }
      
      if (godzina == 9 || godzina == 15) {
         
         elev_auto_PM25 <- autoKrige(id_PM25[,j] ~ 1, input_data = krk_point_spdf[!miss_PM25[,j],], new_data = spgrid, model = modele[i])
         elev_auto_temp <- autoKrige(id_temp[,j] ~ 1, input_data = krk_point_spdf[!miss_temp[,j],], new_data = spgrid, model = modele[i])
         elev_auto_hum <- autoKrige(id_hum[,j] ~ 1, input_data = krk_point_spdf[!miss_hum[,j],], new_data = spgrid, model = modele[i])
         elev_auto_press <- autoKrige(id_press[,j] ~ 1, input_data = krk_point_spdf[!miss_press[,j],], new_data = spgrid, model = modele[i])
         
         ## 3.2.1. PM25 ##
         
         plik = paste("Kriging_pm", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width=711, height=400)
         plot(elev_auto_PM25)
         dev.off()
         
         op <- par(cex = 0.5) #Manipulacja wielkością czcionki (tutaj aby zwiększyć rozmiar mapy)
         
         df <- data.frame( #Wyciągam wartości z autoKrige'u do dataframe'a aby wywołać go w funkcji image()
            x = elev_auto_PM25$krige_output[1]@coords[,1],
            y = elev_auto_PM25$krige_output[1]@coords[,2],
            z = elev_auto_PM25$krige_output[1]$var1.pred
         )
         
         plik = paste("mapa_pm", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width = 559, height = 531)
         
         #KOLOROWA MAPA
         image(df, col = hcl.colors(40, "RdYlGn", rev = TRUE), xlab = "", ylab = "", axes = FALSE)
         
         #TYTUŁ MAPY
         op <- par(cex = 0.8) #Manipulacja wielkością czcionki (tutaj dla tytułu mapy)
         tytul <- paste("Mapa zanieczyszczeń PM2.5 na obszarze Krakowa\n w dniu ", data, " o godzinie ", godzina, ":00", sep = "")
         podtytul <- paste("Kriging: model \"", modele[i], "\"", sep = "")
         title(main = tytul, sub = podtytul, line = 0.2)
         
         #DODAWANIE INNYCH OBIEKTÓW NA MAPĘ
         op <- par(cex = 0.5) #Rozmiar czcionki
         plot(krk_UTM, add = TRUE) #Dodaję kontur miasta
         op <- par(cex = 1) #Rozmiar czcionki
         points(krk_point_spdf, pch = 19, col = krk_point_spdf$Col_PM) #Dodaję kolorowe punkty z pomiarami
         
         #LEGENDA
         
         op <- par(cex = 0.7, font = 2) #Pogrubiona czcionka
         
         leg_bok = 0.795 #Położenie poziome legendy
         leg_posx = (right_up[1] - left_down[1]) * leg_bok + left_down[1]
         leg_posx1 = (right_up[1] - left_down[1]) * (leg_bok + 0.006) + left_down[1]
         
         leg_wys = 0.15 #Wysokość na jakiej znajduje się legenda
         leg_posy = (right_up[2] - left_down[2]) * leg_wys + left_down[2]
         leg_posy1 = (right_up[2] - left_down[2]) * (leg_wys - 0.075) + left_down[2]
         
         legend( #Legenda górna
            leg_posx, leg_posy,
            legend = "Granice Krakowa",
            col = "black",
            lty = 1,
            bg = "#DBEA92",
            title = "Legenda",
            xpd = TRUE
         )
         
         #Wartości na legendzie dolnej
         maks = paste("max = ", as.character(round(max(df$z), 2)), "µg/m3")
         sred = paste("avg = ", as.character(round(mean(df$z), 2)), "µg/m3")
         mini = paste("min = ", as.character(round(min(df$z), 2)), "µg/m3")
         
         legend( #Legenda dolna
            leg_posx1, leg_posy1,
            legend = c(maks, sred, mini),
            fill = hcl.colors(3, "RdYlGn"),
            bg = "#DBEA92",
            title = "Stan zanieczyszczeń",
            xpd = TRUE
         )
         
         on.exit(par(opar))
         dev.off()
         
         ## 3.2.2. TEMPERATURA ##
         
         plik = paste("Kriging_temp", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width=711, height=400)
         plot(elev_auto_temp)
         dev.off()
         
         op <- par(cex = 0.5) #Manipulacja wielkością czcionki (tutaj aby zwiększyć rozmiar mapy)
         
         df <- data.frame( #Wyciągam wartości z autoKrige'u do dataframe'a aby wywołać go w funkcji image()
            x = elev_auto_temp$krige_output[1]@coords[,1],
            y = elev_auto_temp$krige_output[1]@coords[,2],
            z = elev_auto_temp$krige_output[1]$var1.pred
         )
         
         
         plik = paste("mapa_temp", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width = 559, height = 531)
         
         #KOLOROWA MAPA
         image(df, col = hcl.colors(40, "RdBu", rev = TRUE), xlab = "", ylab = "", axes = FALSE)
         
         #TYTUŁ MAPY
         op <- par(cex = 0.8)
         tytul <- paste("Mapa temperatur na obszarze Krakowa\n w dniu ", data, " o godzinie ", godzina, ":00", sep = "")
         podtytul <- paste("Kriging: model \"", modele[i], "\"", sep = "")
         title(main = tytul, sub = podtytul, line = 0.2)
         
         #DODAWANIE INNYCH OBIEKTÓW NA MAPĘ
         op <- par(cex = 0.5) #Rozmiar czcionki
         plot(krk_UTM, add = TRUE) #Dodaję kontur miasta
         op <- par(cex = 1) #Rozmiar czcionki
         points(krk_point_spdf, pch = 19, col = krk_point_spdf$Col_temp) #Dodaję kolorowe punkty z pomiarami
         
         #LEGENDA
         
         op <- par(cex = 0.7, font = 2) #Pogrubiona czcionka
         
         leg_bok = 0.795 #Położenie poziome legendy
         leg_posx = (right_up[1] - left_down[1]) * leg_bok + left_down[1]
         leg_posx1 = (right_up[1] - left_down[1]) * (leg_bok + 0.048) + left_down[1]
         
         leg_wys = 0.15 #Wysokość na jakiej znajduje się legenda
         leg_posy = (right_up[2] - left_down[2]) * leg_wys + left_down[2]
         leg_posy1 = (right_up[2] - left_down[2]) * (leg_wys - 0.075) + left_down[2]
         
         legend( #Legenda górna
            leg_posx, leg_posy,
            legend = "Granice Krakowa",
            col = "black",
            lty = 1,
            bg = "white",
            title = "Legenda",
            xpd = TRUE
         )
         
         #Wartości na legendzie dolnej
         maks = paste("max = ", as.character(round(max(df$z), 2)), "°C")
         sred = paste("avg = ", as.character(round(mean(df$z), 2)), "°C")
         mini = paste("min = ", as.character(round(min(df$z), 2)), "°C")
         
         legend( #Legenda dolna
            leg_posx1, leg_posy1,
            legend = c(maks, sred, mini),
            fill = hcl.colors(3, "RdBu"),
            bg = "white",
            title = "Temperatura",
            xpd = TRUE
         )
         
         on.exit(par(opar))
         dev.off()
         
         
         
         ## 3.2.3. CIŚNIENIE ##
         
         plik = paste("Kriging_press", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width = 711, height = 400)
         plot(elev_auto_press)
         dev.off()
         
         op <- par(cex = 0.5) #Manipulacja wielkością czcionki (tutaj aby zwiększyć rozmiar mapy)
         
         df <- data.frame( #Wyciągam wartości z autoKrige'u do dataframe'a aby wywołać go w funkcji image()
            x = elev_auto_press$krige_output[1]@coords[,1],
            y = elev_auto_press$krige_output[1]@coords[,2],
            z = elev_auto_press$krige_output[1]$var1.pred
         )
         
         plik = paste("mapa_press", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width = 559, height = 531)
         
         #KOLOROWA MAPA
         image(df, col = hcl.colors(40, "Roma", rev = TRUE), xlab = "", ylab = "", axes = FALSE)
         
         #TYTUŁ MAPY
         op <- par(cex = 0.8) #Manipulacja wielkością czcionki (tutaj dla tytułu mapy)
         tytul <- paste("Mapa poziomu ciśnienia na obszarze Krakowa\n w dniu ", data, " o godzinie ", godzina, ":00", sep = "")
         podtytul <- paste("Kriging: model \"", modele[i], "\"", sep = "")
         title(main = tytul, sub = podtytul, line = 0.2)
         
         #DODAWANIE INNYCH OBIEKTÓW NA MAPĘ
         op <- par(cex = 0.5) #Rozmiar czcionki
         plot(krk_UTM, add = TRUE) #Dodaję kontur miasta
         op <- par(cex = 1) #Rozmiar czcionki
         points(krk_point_spdf, pch = 19, col = krk_point_spdf$Col_press) #Dodaję kolorowe punkty z pomiarami
         
         #LEGENDA
         
         op <- par(cex = 0.7, font = 2) #Pogrubiona czcionka
         
         leg_bok = 0.795 #Położenie poziome legendy
         leg_posx = (right_up[1] - left_down[1]) * leg_bok + left_down[1]
         leg_posx1 = (right_up[1] - left_down[1]) * (leg_bok + 0.006) + left_down[1]
         
         leg_wys = 0.15 #Wysokość na jakiej znajduje się legenda
         leg_posy = (right_up[2] - left_down[2]) * leg_wys + left_down[2]
         leg_posy1 = (right_up[2] - left_down[2]) * (leg_wys - 0.075) + left_down[2]
         
         legend( #Legenda górna
            leg_posx, leg_posy,
            legend = "Granice Krakowa",
            col = "black",
            lty = 1,
            bg = "#00A3AA",
            title = "Legenda",
            xpd = TRUE
         )
         
         #Wartości na legendzie dolnej
         maks = paste("max = ", as.character(round(max(df$z), 2)), "hPa")
         sred = paste("avg = ", as.character(round(mean(df$z), 2)), "hPa")
         mini = paste("min = ", as.character(round(min(df$z), 2)), "hPa")
         
         legend( #Legenda dolna
            leg_posx1, leg_posy1,
            legend = c(maks, sred, mini),
            fill = hcl.colors(3, "Roma"),
            bg = "#00A3AA",
            title = "Ciśnienie",
            xpd = TRUE
         )
         
         on.exit(par(opar))
         dev.off()
         
         
         
         
         ## 3.2.4. WILGOTNOŚĆ ##
         
         plik = paste("Kriging_hum", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width=711, height=400)
         plot(elev_auto_hum)
         dev.off()
         
         op <- par(cex = 0.5) #Manipulacja wielkością czcionki (tutaj aby zwiększyć rozmiar mapy)
         
         df <- data.frame( #Wyciągam wartości z autoKrige'u do dataframe'a aby wywołać go w funkcji image()
            x = elev_auto_hum$krige_output[1]@coords[,1],
            y = elev_auto_hum$krige_output[1]@coords[,2],
            z = elev_auto_hum$krige_output[1]$var1.pred
         )
         
         plik = paste("mapa_hum", "_", data, "_", godzina, modele[i], ".png", sep = "")
         png(file = plik, width = 559, height = 531)
         
         #KOLOROWA MAPA
         image(df, col = hcl.colors(40, "BluYl", rev = TRUE), xlab = "", ylab = "", axes = FALSE)
         
         #TYTUŁ MAPY
         op <- par(cex = 0.8) #Manipulacja wielkością czcionki (tutaj dla tytułu mapy)
         tytul <- paste("Mapa poziomu wilgotności na obszarze Krakowa\n w dniu ", data, " o godzinie ", godzina, ":00", sep = "")
         podtytul <- paste("Kriging: model \"", modele[i], "\"", sep = "")
         title(main = tytul, sub = podtytul, line = 0.2)
         
         #DODAWANIE INNYCH OBIEKTÓW NA MAPĘ
         op <- par(cex = 0.5) #Rozmiar czcionki
         plot(krk_UTM, add = TRUE) #Dodaję kontur miasta
         op <- par(cex = 1) #Rozmiar czcionki
         points(krk_point_spdf, pch = 19, col = krk_point_spdf$Col_hum) #Dodaję kolorowe punkty z pomiarami
         
         #LEGENDA
         
         op <- par(cex = 0.7, font = 2) #Pogrubiona czcionka
         
         leg_bok = 0.795 #Położenie poziome legendy
         leg_posx = (right_up[1] - left_down[1]) * leg_bok + left_down[1]
         leg_posx1 = (right_up[1] - left_down[1]) * (leg_bok + 0.037) + left_down[1]
         
         leg_wys = 0.15 #Wysokość na jakiej znajduje się legenda
         leg_posy = (right_up[2] - left_down[2]) * leg_wys + left_down[2]
         leg_posy1 = (right_up[2] - left_down[2]) * (leg_wys - 0.075) + left_down[2]
         
         legend( #Legenda górna
            leg_posx, leg_posy,
            legend = "Granice Krakowa",
            col = "black",
            lty = 1,
            bg = "#00A3AA",
            title = "Legenda",
            xpd = TRUE
         )
         
         #Wartości na legendzie dolnej
         maks = paste("max = ", as.character(round(max(df$z), 2)), "%")
         sred = paste("avg = ", as.character(round(mean(df$z), 2)), "%")
         mini = paste("min = ", as.character(round(min(df$z), 2)), "%")
         
         legend( #Legenda dolna
            leg_posx1, leg_posy1,
            legend = c(maks, sred, mini),
            fill = hcl.colors(3, "BluYl"),
            bg = "#00A3AA",
            title = "Wilgotność",
            xpd = TRUE
         )
         
         on.exit(par(opar))
         dev.off()
         
         
         
         
      }
      
   
      
   } #Koniec pętli do wizualizacji krigingu

   
}

### 4. Eksport istotnych danych ###

for (i in 1:24) {
   dg <- krk_tab[[1]]$history$fromDateTime[i]
   g <- as.integer(substring(dg, 12, 13)) + 1
   print(g)
   if (g == 9 || g == 15) {
      df <- data.frame(
         ID = krk_point_spdf$marks.id,
         PM25 = id_PM25[,i],
         Temperatura = id_temp[,i],
         Wilgotnosc = id_hum[,i],
         Cisnienie = id_press[,i]
      )
      
      plik <- paste("Dane_", data, "_0", g, ".csv", sep = "")
      write.csv(df, plik, row.names=FALSE)
      
   }
}