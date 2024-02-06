#### Funkcje ####

# 0. Przypisuję punktom gminę i powiat

zalicz_do_jednostki <- function(Dane_, Ksztalt_, CRS_ = NULL, X_ = NULL, Y_ = NULL) {
  
  Dane_Kopia <- Dane_
  
  if (!is.null(CRS_)) {
    if (is.null(X_) || is.null(Y_)) {return()}
    else {
      Dane_Kopia <- as.data.frame(
        spTransform(
          SpatialPointsDataFrame(
            coords = data.frame(X = X_, Y = Y_),
            data = Dane_, proj4string = CRS(paste("+init=epsg:", CRS_, sep = ""))), CRS("+proj=utm +zone=34 +datum=WGS84")))
    }
    
  }
  
  DaneShape <- Ksztalt_
  ile_jednostek <- length(DaneShape$gml_id)
  Jednostki <- data.frame(matrix(ncol = 2, nrow = 0))
  
  Kolumna <- NULL
  if (Ksztalt_[1,]$RODZAJ == "powiat") {Kolumna <- "Powiat"
  } else if (Ksztalt_[1,]$RODZAJ == "gmina") {Kolumna <- "Gmina"
  } else if (Ksztalt_[1,]$RODZAJ == "wojewodztwo") {Kolumna <- "Województwo"}
  
  colnames(Jednostki) <- c("id", Kolumna)
  
  for (i in 1:ile_jednostek) {
    
    # Dla danej jednostki administracyjnej (np. dla danej gminy)
    # wybieram tylko te punkty, które leżą w jej obrębie
    punkty <- ppp(x = Dane_Kopia$X, y = Dane_Kopia$Y, window = as.owin(DaneShape[i,]))
    
    # Dla każdego punktu położonego w obrębie danej jednostki szukam jego id
    # a następnie zapisuję obok nazwę danej jednostki
    if (punkty$n > 0) {
      for (j in 1:punkty$n) {
        Jednostki[nrow(Jednostki) + 1,] <- c(Dane_Kopia$id[Dane_Kopia$X == punkty$x[j]], DaneShape[i,]$JPT_NAZWA_)
      }
    }
    
  }
  
  nieprzypisane <- DanePomiarowe$Punkty$id[!(DanePomiarowe$Punkty$id %in% Jednostki$id)]
  
  for (i in nieprzypisane) {
    
    Jednostki[nrow(Jednostki) + 1,] <- c(i, NA)
    
  }
  
  return(merge(x = Dane_, y = Jednostki, by = "id"))
  
}

# 1. Przypisuje wartość do jednostki administracyjnej
# Dane_ - Punkty
# Ksztalt_ - Shape jednostek administracyjnych
# Statystyka_ - Wartości w kolumnie które chcemy zagregować
agreguj <- function(Dane_, Ksztalt_, Statystyka_ = NULL) {
  
  NoweDane <- Ksztalt_
  ile_jednostek <- length(NoweDane$gml_id)
  NoweDane$Wartosc <- rep(NA, ile_jednostek)
  NoweDane$LPunktow <- rep(NA, ile_jednostek)
  
  Kolumna <- NULL
  if (Ksztalt_[1,]$RODZAJ == "powiat") {Kolumna <- Dane_$Powiat
  } else if (Ksztalt_[1,]$RODZAJ == "gmina") {Kolumna <- Dane_$Gmina
  } else if (Ksztalt_[1,]$RODZAJ == "wojewodztwo") {Kolumna <- Dane_$Województwo}
  
  for (i in 1:ile_jednostek) {
    
    wartosci <- Statystyka_[Ksztalt_[i,]$JPT_NAZWA_ == Kolumna]
    
    if (is.null(Statystyka_)) {
      NoweDane[i,]$Wartosc <- length(wartosci)
    }
    else if (length(wartosci) == 0) {
      NoweDane[i,]$Wartosc <- NA
      NoweDane[i,]$LPunktow <- 0
    }
    else {
      NoweDane[i,]$Wartosc <- mean(wartosci, na.rm = TRUE)
      NoweDane[i,]$LPunktow <- length(wartosci)
    }
    
  }
  
  return(NoweDane)
  
}

# 2. Dostosowuje skalę liczb z odpowiednim zaokrągleniem

tworz_sekwencje <- function(min_, max_, dlugosc_, wlacznie_ = TRUE) {
  if (wlacznie_) {
    if (max_ - min_ < dlugosc_) { # Jeśli różnica jest mniejsza długość to stosuję mądre zaokrąglanie
      diff <- if(max_ - min_ >= 2) 1 else max_ - min_ # Test żeby nie wywaliło logarytmu
      zaokr <- log(diff/dlugosc_, 10) # Logarytm do obliczenia zaokrąglenia
      zaokr <- if (zaokr < 0) floor(zaokr) else ceiling(zaokr) # Zaokrąglanie informacji o zaokrągleniu
      min_lok <- floor(min_* 10 ^ -zaokr) / (10 ^ -zaokr) # Zaokrąglam minimum do danego miejsca po przecinku w dół
      max_lok <- ceiling(max_* 10 ^ -zaokr) / (10 ^ -zaokr) # Zaokrąglam maksimum do danego miejsca po przecinku w górę
      return(round(seq(min_lok, max_lok, length.out = dlugosc_), -zaokr))
    }
    
    else {
      return(round(seq(floor(min_), ceiling(max_), length.out = dlugosc_)))
    }
  }
  else {
    if (max_ - min_ < dlugosc_) {
      diff <- if(max_ - min_ >= 2) 1 else max_ - min_ # Test żeby nie wywaliło logarytmu
      zaokr <- log(diff/dlugosc_, 10) # Logarytm do obliczenia zaokrąglenia
      zaokr <- if (zaokr < 0) floor(zaokr) else ceiling(zaokr) # Zaokrąglanie informacji o zaokrągleniu
      min_lok <- ceiling(min_* 10 ^ -zaokr) / (10 ^ -zaokr) # Zaokrąglam minimum do danego miejsca po przecinku w dół
      max_lok <- floor(max_* 10 ^ -zaokr) / (10 ^ -zaokr) # Zaokrąglam maksimum do danego miejsca po przecinku w górę
      
      return(round(seq(min_lok, max_lok, length.out = dlugosc_), -zaokr))
    }
    
    else {
      return(round(seq(ceiling(min_), floor(max_), length.out = dlugosc_)))
    }
  }
  
}

# 3. Funkcja make_bins

make_bins <- function(q_, Dane_, ZwrocNA_ = TRUE) {
  
  bins <- NULL
  if (sum(!is.na(Dane_)) == 0 || max(Dane_, na.rm = TRUE) == min(Dane_, na.rm = TRUE)) {
    if (ZwrocNA_) return(rep(NA,q_))
    else bins <- seq(1,q_)
  }
  else bins <- tworz_sekwencje(min(Dane_, na.rm = TRUE), max(Dane_, na.rm = TRUE), q_)
  
  if (bins[1] > 0) bins[1] <- 0 # Pierwsza wartość będzie miała zero
  else {bins[1] <- floor(bins[1])} # Jeśli są wartości ujemne, to najniższą zaokrąglam w dół
  
  return(bins)
}

# 4. Funkcja do tworzenia krigingu

kriging <- function(Dane_, Ksztalt_, Model_ = "Sph", RozmiarPixela_ = 1000) {
  
  if (sum(!is.na(Dane_$Czynnik)) == 0) return(0)
  
  coordinates(Dane_) <- ~X+Y
  proj4string(Dane_) <- CRS("+proj=utm +zone=34 +datum=WGS84")
  
  ras <- raster(extent(Ksztalt_))
  
  res(ras) <- c(RozmiarPixela_, RozmiarPixela_)
  ras[] <- 0
  projection(ras) <- CRS("+proj=utm +zone=34 +datum=WGS84")
  
  st_grid <- rasterToPoints(ras, spatial = TRUE)
  gridded(st_grid) <- TRUE
  st_grid <- as(st_grid, "SpatialPixels")
  
  auto <- autoKrige(Dane_$Czynnik[!is.na(Dane_$Czynnik)] ~ 1, input_data = Dane_[!is.na(Dane_$Czynnik),], new_data = st_grid, model = Model_)
  
  krop <- crop(raster(auto$krige_output[1]), extent(Ksztalt_))
  maska <- mask(krop, Ksztalt_)
  
  return(maska)
  
}

# 5. Funkcja obliczająca punkt na elipsie

oblicz_elipse <- function(a, b, alfa, d = 1, c = 111/72) {
  
  x <- sqrt(d^2/(1/(c^2) + (1/tan(alfa*pi/180))^2))
  y <- sqrt(d^2/(1 + 1/((c * 1/tan(alfa*pi/180))^2)))
  
  if (alfa > 90 && alfa < 180) {
    y <- b - y
    x <- x + a
  } else if (alfa > 180 && alfa < 270) {
    y <- b - y
    x <- a - x
  } else if (alfa < 360 && alfa > 270) {
    y <- y + b
    x <- a - x
  } else {
    x <- x + a
    y <- y + b
  }
  
  return(c(x,y))
  
}

# 6. Wykreślanie sieci neuronowej w pakiecie plotly

plotly_nn <- function(nn, czynnik) {
  
  pkt <- data.frame(x = rep(0,0), y = rep(0,0)) # Punkty na neurony
  wejscie <- colnames(nn$data[2:ncol(nn$data)]) # Nazwy zmiennych wejściowych
  lp <- length(nn$model.list$variables) # Liczba zmiennych wejściowych
  
  for (i in 1:lp) { # Punkty zmiennych wejściowych
    pkt[nrow(pkt) + 1,] <- c(10, 5 + 10 *(i-1))
  }
  
  # Ilość warstw
  nLay <- NULL
  tekst <- NULL
  for (i in 1:(length(nn$weights[[1]]) - 1)) nLay <- c(nLay, ncol(nn$weights[[1]][[i]]))
  
  if (length(nLay) == 1) { # Jeśli jedna warstwa ukryta
    
    pkt$x <- pkt$x + 20
    d <- 10 * lp / nLay
    s <- 1 / nLay * lp * 5
    for (i in 1:nLay) pkt[nrow(pkt) + 1,] <- c(50, s + d * (i-1))
    
    pkt[nrow(pkt) + 1,] <- c(70, lp * 5)
    tekst <- nLay
    
  }
  if (length(nLay) == 2) { # Jeśli dwie warstwy ukryte
    
    d <- 10 * lp / nLay[1]
    s <- 1 / nLay[1] * lp * 5
    for (i in 1:nLay[1]) pkt[nrow(pkt) + 1,] <- c(30, s + d * (i-1))
    
    d <- 10*lp/nLay[2]
    s <- 1 / nLay[2] * lp * 5
    for (i in 1:nLay[2]) pkt[nrow(pkt) + 1,] <- c(50, s + d * (i-1))
    
    pkt[nrow(pkt) + 1,] <- c(70, lp * 5)
    tekst <- paste0("c(", nLay[1], ",", nLay[2], ")")
    
  }
  if (length(nLay) == 3) { # Jeśli trzy warstwy ukryte
    
    d <- 10 * lp / nLay[1]
    s <- 1 / nLay[1] * lp * 5
    for (i in 1:nLay[1]) pkt[nrow(pkt) + 1,] <- c(30, s + d * (i-1))
    
    d <- 10*lp/nLay[2]
    s <- 1 / nLay[2] * lp * 5
    for (i in 1:nLay[2]) pkt[nrow(pkt) + 1,] <- c(50, s + d * (i-1))
    
    d <- 10*lp/nLay[3]
    s <- 1 / nLay[3] * lp * 5
    for (i in 1:nLay[3]) pkt[nrow(pkt) + 1,] <- c(70, s + d * (i-1))
    
    pkt[nrow(pkt) + 1,] <- c(90, lp * 5)
    tekst <- paste0("c(", nLay[1], ",", nLay[2], ",", nLay[3], ")")
    
  }
  
  bialy <- 'rgb(255,255,255)'
  czarny <- 'rgb(0,0,0)'
  szary <- 'rgb(200,200,200)'
  niebieski <- 'rgb(0,0,255)'
  
  fig <- plot_ly(pkt) %>%
    layout(title = paste('Sieć neuronowa', tekst),
           xaxis = list(showticklabels = F, zeroline = F, showgrid = F),
           yaxis = list(showticklabels = F, zeroline = F, showgrid = F)) %>%
    style(hoverinfo = "none")
  
  for (i in 1:(length(nLay)+1)) {
    
    if (i == 1) {
      
      for (j in 1:lp) {
        for (k in 1:nLay[1]) {
          fig <- fig %>%
            add_trace(x = pkt$x[c(j,lp+k)], y = pkt$y[c(j,lp+k)],
                      mode = 'lines+markers', line = list(color = szary),
                      marker = list(color = bialy, size = 30, line = list(color = czarny, width = 5)),
                      showlegend = F) %>%
            style(hoverinfo = "none")
          if (j == 1) {
            fig <- fig %>%
              add_annotations(x = pkt$x[lp+k], y = pkt$y[lp+k], showlegend = F,
                              text = round(nn$weights[[1]][[1]][1,nLay[1]-k+1],4),
                              ax = -40, ay = 0, font = list(color = niebieski, size = 10))
          }
          
        }
        fig <- fig %>%
          add_annotations(x = pkt$x[j], y = pkt$y[j],
                          text = wejscie[length(wejscie)-j+1], ax = -50, ay = 0,
                          font = list(size = 9))
      }
    }
    
    else if (i == length(nLay)+1) {
      
      for (j in 1:nLay[length(nLay)]) {
        fig <- fig %>%
          add_trace(x = pkt$x[c(nrow(pkt)-j,nrow(pkt))], y = pkt$y[c(nrow(pkt)-j,nrow(pkt))],
                    mode = 'lines+markers', line = list(color = szary),
                    marker = list(color = bialy, size = 30, line = list(color = czarny, width = 5)),
                    showlegend = F) %>%
          style(hoverinfo = "none")
      }
      
      fig <- fig %>%
        add_annotations(x = pkt$x[nrow(pkt)], y = pkt$y[nrow(pkt)], showlegend = F,
                        text = round(nn$weights[[1]][[i]][1],4),
                        ax = -40, ay = 0, font = list(color = niebieski, size = 10)) %>%
        add_annotations(x = pkt$x[nrow(pkt)], y = pkt$y[nrow(pkt)], showlegend = F,
                        text = czynnik,
                        ax = 60, ay = 0)
      
    }
    
    else {
      
      for (j in 1:nLay[i-1]) {
        jw <- 0
        if (i == 2) jw <- lp + j
        if (i > 2) jw <- lp + j + sum(nLay[1:(i-2)])
        for (k in 1:nLay[i]) {
          kw <- lp + sum(nLay[1:(i-1)]) + k
          fig <- fig %>%
            add_trace(x = pkt$x[c(jw,kw)], y = pkt$y[c(jw,kw)],
                      mode = 'lines+markers', line = list(color = szary),
                      marker = list(color = bialy, size = 30, line = list(color = czarny, width = 5)),
                      showlegend = F) %>%
            style(hoverinfo = "none")
          
          if (j == 1) {
            fig <- fig %>%
              add_annotations(x = pkt$x[kw], y = pkt$y[kw], showlegend = F,
                              text = round(nn$weights[[1]][[i]][1,nLay[i]-k+1],4),
                              ax = -40, ay = 0, font = list(color = niebieski, size = 10))
          }
          
        }
      }
      
    }
    
    
  }
  
  return(fig)
  
}

# 7. Funkcja uzupełniająca wartości puste (NA) uśredniając z sąsiednich wartości

usrednij_braki <- function(dane) {
  
  puste <- which(is.na(dane))
  
  if (length(puste) == 0) return(dane)
  
  for (i in 1:length(puste)) {
    col <- ceiling(puste[i]/nrow(dane))
    row <- puste[i] %% nrow(dane)
    
    if (i < length(puste)) { # Tylko jeśli nieostatni element pusty
      if (puste[i+1] - puste[i] == 1) { # Jeśli kolejny element też jest pusty
        k <- i # Zaczynam sprawdzać od tego elementu do kiedy pod rząd są puste
        while (k < length(puste) && puste[k+1] - puste[k] == 1) k <- k + 1
        k <- k - i + 1
        d <- dane[row+k,col] - dane[row-1,col]
        dane[row,col] <- d/(k+1) + dane[row-1,col]
        
      } else dane[row,col] <- mean(c(dane[row+1,col],dane[row-1,col]))
    } else dane[row,col] <- mean(c(dane[row+1,col],dane[row-1,col]))
    
  }
  
  return(dane)
  
}

utnij_braki <- function(dane) {
  
  puste <- which(is.na(dane))
  if (length(puste) == 0) return(dane)
  
  if (sum(puste %% nrow(dane) == 1) > 0) { # Jeśli wartości puste na początku to odcinamy
    odc1 <- 0
    for (c in 1:ncol(dane)) {
      r <- 1
      while (is.na(dane[r,c])) r <- r + 1
      if (r > odc1) odc1 <- r
    }
    dane <- dane[odc1:nrow(dane),]
  }
  
  puste <- which(is.na(dane))
  
  if (sum(puste %% nrow(dane) == 0) > 0) { # Jeśli wartości puste na końcu to odcinamy
    odc2 <- 0
    for (c in 1:ncol(dane)) {
      r <- nrow(dane)
      while (is.na(dane[r,c])) r <- r - 1
      if (nrow(dane) - r > odc2) odc2 <- nrow(dane) - r
    }
    dane <- dane[1:(nrow(dane)-odc2),]
  }
  
  return(dane)
  
}

#### Biblioteki ####

library(devtools)
library(leaflet)
library(RColorBrewer)
library(scales)
library(lattice)
library(dplyr)
library(shiny)
library(spatstat)
library(sf)
library(sp)
library(raster)
library(automap)
library(bslib)
library(plotly)
devtools::install_github("rstudio/gridlayout")
library(gridlayout)
library(randomForest)
library(neuralnet)
library(lubridate)
library(Metrics)


#### Get Dane ####

setwd("E:/AGH/Projekt_Inz/Wizualizacja/Aplikacja/Dane")
Dane <- NULL
pliki <- c("11.01.2024_10godz.Rdata", "12.01.2024_9godz.Rdata",
           "13.01.2024_19godz.Rdata", "14.01.2024_12godz.Rdata",
           "15.01.2024_10godz.Rdata", "16.01.2024_9godz.Rdata",
           "17.01.2024_11godz.Rdata")

# Łączenie plików z kilku dni pomiarowych do jednej ramki danych

for (i in pliki) {
  
  load(i)
  DaneHist <- DanePomiarowe$Dane[DanePomiarowe$Dane$DATE %in% unique(DanePomiarowe$Dane$DATE)[1:24],]
  
  if (i == pliki[1]) {Dane <- list(Punkty = DanePomiarowe$Punkty, Dane = DaneHist)
  } else {
    
    d <- DanePomiarowe$Dane$DATE[1] - Dane$Dane$DATE[nrow(Dane$Dane)]
    
    if (d < 1) { # Jeśli następnego dnia dane zostały pobrane wcześniej
      
      DaneHist <- DaneHist[DaneHist$DATE %in% unique(DaneHist$DATE)[(2-d):24],]
      
    } else if (d > 1) { # Jeśli następnego dnia dane zostały pobrane później
      
      DaneHist <- rbind(DaneProg[DaneProg$DATE %in% unique(DaneProg$DATE)[2:d],], DaneHist)
      
    }
    
    Dane$Dane <- rbind(Dane$Dane, DaneHist)
    
  }
  
  DaneProg <- DanePomiarowe$Dane[DanePomiarowe$Dane$DATE %in% unique(DanePomiarowe$Dane$DATE)[24:48],]
  
}

DanePomiarowe <- Dane

setwd("E:/AGH/Projekt_Inz/Wizualizacja/Aplikacja")

# Uzupełnianie braków w danych (jeśli brakuje temperatury lub wilgotności)

for (i in DanePomiarowe$Punkty$id) {
  
  if (sum(!is.na(DanePomiarowe$Dane$TEMPERATURE[DanePomiarowe$Dane$ID == i])) < 0.95 * length(DanePomiarowe$Dane$TEMPERATURE[DanePomiarowe$Dane$ID == i]))
    DanePomiarowe$Dane$TEMPERATURE[DanePomiarowe$Dane$ID == i] <- DanePomiarowe$Dane$TEMPERATURE_2M[DanePomiarowe$Dane$ID == i]
  if (sum(!is.na(DanePomiarowe$Dane$HUMIDITY[DanePomiarowe$Dane$ID == i])) < 0.95 * length(DanePomiarowe$Dane$HUMIDITY[DanePomiarowe$Dane$ID == i]))
    DanePomiarowe$Dane$HUMIDITY[DanePomiarowe$Dane$ID == i] <- DanePomiarowe$Dane$RELATIVEHUMIDITY_2M[DanePomiarowe$Dane$ID == i]
  if (sum(!is.na(DanePomiarowe$Dane$PRESSURE[DanePomiarowe$Dane$ID == i])) < 0.95 * length(DanePomiarowe$Dane$PRESSURE[DanePomiarowe$Dane$ID == i]))
    DanePomiarowe$Dane$PRESSURE[DanePomiarowe$Dane$ID == i] <- DanePomiarowe$Dane$PRESSURE_SEA[DanePomiarowe$Dane$ID == i]
}

# Sprawdzam, które czujniki zawierają co najmniej 75% obserwacji niepustych dla danego zanieczyszczenia

pkt_niepuste <- data.frame(matrix(TRUE, nrow = length(DanePomiarowe$Punkty$id), ncol = 4))

for (pkt in 1:length(DanePomiarowe$Punkty$id)) {
  
  DaneT <- DanePomiarowe$Dane[DanePomiarowe$Dane$ID == DanePomiarowe$Punkty$id[pkt],]
  pkt_niepuste[pkt,1] <- DanePomiarowe$Punkty$id[pkt]
  kolumny <- c(3,4,9)
  for (i in 1:length(kolumny)) if (sum(!is.na(DaneT[,kolumny[i]])) < nrow(DaneT)/4) pkt_niepuste[pkt,i+1] <- FALSE
  
}

colnames(pkt_niepuste) <- c("ID", "PM25", "PM10", "PM1")

# Przeliczanie kątów i odległości między punktami

Katy <- NULL
Odleglosci <- NULL

for (i in 1:nrow(DanePomiarowe$Punkty)) {
  
  dx <- DanePomiarowe$Punkty$location$longitude - DanePomiarowe$Punkty$location$longitude[i]
  dy <- DanePomiarowe$Punkty$location$latitude - DanePomiarowe$Punkty$location$latitude[i]
  kat <- atan(dx/dy)*180/pi
  kat[dx < 0 & dy > 0] <- 360 + kat[dx < 0 & dy > 0]
  kat[dy < 0] <- 180 + kat[dy < 0]
  kat[is.na(kat)] <- 0
  kat <- c(DanePomiarowe$Punkty$id[i],kat)
  
  dxm <- DanePomiarowe$Punkty$X - DanePomiarowe$Punkty$X[i]
  dym <- DanePomiarowe$Punkty$Y - DanePomiarowe$Punkty$Y[i]
  odl <- sqrt(dxm^2+dym^2)
  odl <- c(DanePomiarowe$Punkty$id[i],odl)
  
  Katy <- rbind(Katy,kat)
  Odleglosci <- rbind(Odleglosci,odl)
  
}

#### Import kształtów ####

WojMalopolskie <- st_read(dsn = "PodzAdm/WojMalopolskie.shp")
WojMalopolskie <- st_transform(WojMalopolskie, CRS("+proj=utm +zone=34 +datum=WGS84"))
WojShape <- as_Spatial(WojMalopolskie)
DanePomiarowe$Punkty <- zalicz_do_jednostki(DanePomiarowe$Punkty, WojMalopolskie)
WojMalopolskie <- st_transform(WojMalopolskie, crs = 4326)

PowiatyMalopolskie <- st_read(dsn = "PodzAdm/PowiatyMalopolskie.shp")
PowiatyMalopolskie <- st_transform(PowiatyMalopolskie, CRS("+proj=utm +zone=34 +datum=WGS84"))
DanePomiarowe$Punkty <- zalicz_do_jednostki(DanePomiarowe$Punkty, PowiatyMalopolskie)
PowiatyMalopolskie <- st_transform(PowiatyMalopolskie, crs = 4326)

GminyMalopolskie <- st_read(dsn = "PodzAdm/GminyMalopolskie.shp")
GminyMalopolskie <- st_transform(GminyMalopolskie, CRS("+proj=utm +zone=34 +datum=WGS84"))
DanePomiarowe$Punkty <- zalicz_do_jednostki(DanePomiarowe$Punkty, GminyMalopolskie)
GminyMalopolskie <- st_transform(GminyMalopolskie, crs = 4326)

#### Deklaracje zmiennych globalnych ####

DanePomiarowe$Punkty$adres <- paste(DanePomiarowe$Punkty$address$city, ", ", DanePomiarowe$Punkty$address$street, sep = "")
for (i in 1:length(DanePomiarowe$Punkty[,1])) {
  if (is.na(DanePomiarowe$Punkty$address$street[i])) {
    DanePomiarowe$Punkty$adres[i] <- DanePomiarowe$Punkty$address$city[i]
  }
}

# Normy

normy <- data.frame(
  PM2.5 = c(0,5,10,15,25,37.5,50,65,80,1000),
  PM10 = c(0,7.5,15,30,50,75,100,150,300,1000),
  NO2 = c(0,7.5,15,30,50,75,100,150,300,1000),
  SO2 = c(0,5,10,15,25,37.5,50,65,80,1000),
  CO = c(0,7.5,15,30,50,75,100,150,300,1000),
  O3 = c(0,7.5,15,30,50,75,100,150,300,1000),
  PM1 = c(0,7.5,15,30,50,75,100,150,300,1000),
  Ciśnienie = c(900,990,995,1000,1005,1010,1015,1020,1030,1100),
  Wilgotność = c(0,20,35,50,65,75,80,85,95,100),
  Temperatura = c(-100,-10,-5,0,7,15,22,30,38,100),
  Nic1 = rep(NA,10), # Potrzeba, żeby zgadzało się z kolumnami w danych
  Nic2 = rep(NA,10), # Potrzeba, żeby zgadzało się z kolumnami w danych
  PunktRosy_2m = c(0,20,35,50,65,75,80,85,95,100),
  Deszcz = c(0,1,2,3,7,10,20,30,100,1000),
  PredkoscWiatru = c(0,2,5,8,10,13,15,20,50,250),
  Nic2 = rep(NA,10), # Potrzeba, żeby zgadzało się z kolumnami w danych
  TemperaturaGruntu = c(-100,-10,-5,0,7,15,22,30,38,100),
  WilgotnoscGleby = c(0,0.10,0.15,0.25,0.35,0.50,0.75,1,1.5,100)
)

tag.map.title <- tags$style(HTML(".leaflet-control.map-title {
   position: fixed !important;
   right: 0%;
   top: 50px;
   width: 20%;
   text-align: center;
   padding-left: 10px;
   padding-right: 10px;
   background: rgba(255,255,255,0.75);
   font-weight: bold;
   font-size: 20px;
   box-shadow: 3px 3px 5px rgba(0,0,0,0.25);
   border-radius: 5px;
   font-family: Josefin Sans;
}"))

data <- paste(substr(as.character(min(DanePomiarowe$Dane$DATE)),9,10),"/",
              substr(as.character(max(DanePomiarowe$Dane$DATE)),9,10),".",
              substr(as.character(max(DanePomiarowe$Dane$DATE)),6,7),".",
              substr(as.character(min(DanePomiarowe$Dane$DATE)),1,4),sep=""
)

poprzednie_wyswietlanie <- "Punkty"
id_warstwy <- 1
poczatek_danych_okresowych <- 0
koniec_danych_okresowych <- 1
indeks_zanieczyszczenia <- 0
Paleta_Glob <- "RdYlGn"
Jednostka_Glob <- "µg/m³"
Nazwa_Wskaznika_Glob <- "Poziom zanieczyszczenia"
Zmiana_Godziny_Glob <- 0
Ile_Godzin <- as.numeric(difftime(max(DanePomiarowe$Dane$DATE), min(DanePomiarowe$Dane$DATE), units = "hours")) + 1

czc1 <- list(family = "Josefin Sans", size = 18)
czc <- list(family = "Josefin Sans")

# Wybory
zanieczyszczeniaAirly <- c("PM2.5", "PM10", "NO2", "SO2", "CO", "O3", "PM1",
                           "Ciśnienie", "Wilgotność", "Temperatura",
                           "Temperatura gruntu", "Punkt rosy 2m", "Deszcz",
                           "Prędkość wiatru", "Wilgotność gleby")
agregacje <- c("Punkty", "Województwo", "Powiaty", "Gminy", "Ciągła")

#### UI ####

ui <- navbarPage(data, id = "nav", theme = bslib::bs_theme(),
                 
                 #### Interaktywana mapa ####
                 
                 tabPanel("Interaktywna mapa",
                          div(class = "outer",
                              tags$head(
                                includeCSS("styles.css"),
                                includeScript("gomap.js")
                              ),
                              leafletOutput("map", width = "100%", height = "100%"),
                              tags$div(id="cite",
                                       'Na podstawie danych ', tags$em('Airly i OpenMeteo'), ' Dawid Makowski (AGH, 2023).'
                              ),
                              absolutePanel(
                                id = "controls", class = "panel panel-default", fixed = TRUE,
                                top = 60, left = 0, bottom = 0,
                                width = "20%", height = "100%", h4("Opcje mapy"),
                                
                                selectInput("zanieczyszczenieAirly", "Zanieczyszczenie powietrza", zanieczyszczeniaAirly[1:length(zanieczyszczeniaAirly)]),
                                
                                selectInput("agregacja", "Rodzaj agregacji", agregacje),
                                
                                conditionalPanel(
                                  condition = "input.agregacja == 'Ciągła'",
                                  selectInput("modelKriging", "Model krigingu", c("Sph", "Exp", "Gau", "Ste"), selected = "Sph")
                                ),
                                
                                radioButtons(inputId = "skalowanie", label = "Skalowanie kolorów",
                                             choices = c("Lokalnie", "Globalnie"), selected = "Lokalnie", inline = TRUE),
                                
                                uiOutput("hourUI"),
                                
                                plotOutput("wykresCzynnika", height = 250)
                                
                              )
                          )
                 ),
                 
                 #### Eksplorator danych ####
                 
                 tabPanel("Eksplorator danych",
                          fluidRow(
                            column(3,
                                   selectInput("powiaty", "Powiaty", c("Wszystkie powiaty" = "", PowiatyMalopolskie$JPT_NAZWA_), multiple=TRUE)
                            ),
                            column(1,
                                   numericInput("minScore", "Wartość minimalna", min = -100, max = 1099, value = 0)
                            ),
                            column(1,
                                   numericInput("maxScore", "Wartość maksymalna", min = -99, max = 1100, value = 100)
                            )
                          ),
                          hr(),
                          DT::dataTableOutput("tabela")
                 ),
                 
                 #### Prognoza zanieczyszczenia ####
                 
                 tabPanel("Prognoza zanieczyszczenia",
                          div(class = "outer", tags$head(includeCSS("styles.css"), includeScript("gomap.js"))),
                          grid_page(
                            layout = c(
                              "sidebar PlotMain PlotMain",
                              "sidebar Plot1    mapa   ",
                              "sidebar Plot2    mapa   "
                            ),
                            row_sizes = c("1fr", "1fr", "1fr"),
                            col_sizes = c("0.5fr", "0.7fr", "1.3fr"),
                            gap_size = "0.5rem",
                            
                            #### Sidebar ####
                            
                            grid_card(
                              area = "sidebar",
                              card_header(h5("Ustawienia prognozy")),
                              card_body(
                                min_height = "150px",
                                selectInput(
                                  inputId = "Algorithm",
                                  label = "Algorytm",
                                  choices = list("Las losowy" = "RF", "Sieć neuronowa" = "NN")
                                ),
                                conditionalPanel(
                                  condition = "input.Algorithm == 'RF'",
                                  numericInput("nTree", "Liczba drzew", 100, min = 1, max = 1000)
                                ),
                                conditionalPanel(
                                  condition = "input.Algorithm == 'NN'",
                                  numericInput("nLayer", "Liczba warstw ukrytych", 1, min = 1, max = 3),
                                  conditionalPanel(
                                    condition = "input.nLayer == 1",
                                    fluidRow(
                                      column(5,numericInput("nLayer1", "Warstwa 1", 5, min = 1, max = 100))
                                    )
                                  ),
                                  conditionalPanel(
                                    condition = "input.nLayer == 2",
                                    fluidRow(
                                      column(5,numericInput("nLayer1", "Warstwa 1", 5, min = 1, max = 100)),
                                      column(5,numericInput("nLayer2", "Warstwa 2", 3, min = 1, max = 50))
                                    )
                                  ),
                                  conditionalPanel(
                                    condition = "input.nLayer == 3",
                                    fluidRow(
                                      column(4, numericInput("nLayer1", "Warstwa 1", 5, min = 1, max = 100)),
                                      column(4, numericInput("nLayer2", "Warstwa 2", 3, min = 1, max = 50)),
                                      column(4, numericInput("nLayer3", "Warstwa 3", 1, min = 1, max = 30))
                                    )
                                  )
                                ),
                                selectInput(
                                  inputId = "Pollution",
                                  label = "Zanieczyszczenie",
                                  choices = list("PM2.5" = "PM25", "PM10" = "PM10", "PM1" = "PM1")
                                ),
                                selectInput(
                                  inputId = "VariablesPred",
                                  label = "Zmienne do prognozy",
                                  multiple = TRUE,
                                  choices = list(
                                    "Temperatura" = "Temperatura",
                                    "Ciśnienie" = "Cisnienie",
                                    "Wilgotność" = "Wilgotnosc",
                                    "Godzina dnia" = "Godzina",
                                    "Dzień tygodnia" = "Dzien",
                                    "Punkt rosy 2m" = "PunktRosy",
                                    "Intensywność opadów" = "Deszcz",
                                    "Prędkość wiaru" = "PredkoscWiatr",
                                    "Kierunek wiatru" = "KierunekWiatr",
                                    "Temperatura gleby" = "TempGleb",
                                    "Wilgotność gleby" = "WilgGleb"
                                  )
                                ),
                                # Filtracja punktów, do tych które zawierają co najmniej 75% niepustych danych
                                conditionalPanel(
                                  condition = "input.Pollution == 'PM25'",
                                  selectInput(
                                    inputId = "PointChoice",
                                    label = "Punkt do prognozy",
                                    choices = as.list(setNames(DanePomiarowe$Punkty$id[pkt_niepuste[,2]], DanePomiarowe$Punkty$adres[pkt_niepuste[,2]]))
                                  )
                                ),
                                conditionalPanel(
                                  condition = "input.Pollution == 'PM10'",
                                  selectInput(
                                    inputId = "PointChoice1",
                                    label = "Punkt do prognozy",
                                    choices = as.list(setNames(DanePomiarowe$Punkty$id[pkt_niepuste[,3]], DanePomiarowe$Punkty$adres[pkt_niepuste[,3]]))
                                  )
                                ),
                                conditionalPanel(
                                  condition = "input.Pollution == 'PM1'",
                                  selectInput(
                                    inputId = "PointChoice2",
                                    label = "Punkt do prognozy",
                                    choices = as.list(setNames(DanePomiarowe$Punkty$id[pkt_niepuste[,4]], DanePomiarowe$Punkty$adres[pkt_niepuste[,4]]))
                                  )
                                ),
                                actionButton("Start", "Uruchom prognozę")
                              )
                            ),
                            
                            #### Wykresy ####
                            
                            grid_card(
                              area = "Plot1",
                              card_body(
                                plotlyOutput(
                                  outputId = "Plot1",
                                  width = "100%",
                                  height = "100%"
                                )
                              )
                            ),
                            grid_card(
                              area = "mapa",
                              card_body(
                                gap = "5px",
                                min_height = "100px",
                                leafletOutput("map1")
                              )
                            ),
                            grid_card(
                              area = "Plot2",
                              card_body(plotlyOutput(outputId = "Plot2"))
                            ),
                            grid_card(
                              area = "PlotMain",
                              card_body(plotlyOutput(outputId = "PlotMain"))
                            )
                          )
                 ),
                 
                 #### Crosshair ####
                 
                 conditionalPanel("false", icon("crosshair"))
                 
                 #### Koniec ####
)

#### SERVER ####

server <- function(input, output, session) {
  
  v <- reactiveValues(data = NULL)
  
  # 1. Slider
  observe({
    
    interval <- 400
    if (input$agregacja == "Gminy") {
      interval <- 8000
    } else if (input$agregacja == "Powiaty") {
      interval <- 2000
    } else if (input$agregacja == "Województwo") {
      interval <- 750
    } else if (input$agregacja == "Ciągła") {
      interval <- 1500
    }
    
    output$hourUI <- renderUI({ #Slider przechowuje dane o zaznaczeniu godziny w inpucie
      sliderInput("hourSel", "Godziny",
                  min = min(DanePomiarowe$Dane$DATE),
                  max = max(DanePomiarowe$Dane$DATE),
                  value = min(DanePomiarowe$Dane$DATE),
                  step = as.numeric(max(DanePomiarowe$Dane$DATE) - min(DanePomiarowe$Dane$DATE)) * 24 * 3600 / 47,
                  timeFormat = "%H:%M",
                  animate = animationOptions(interval = interval, loop = FALSE)
      )
    })
    
  })
  
  # 2. Filtrowanie danych wg informacji z suwaka (godzina)
  filteredData <- reactive({
    
    req(input$hourSel) # Wymaga danych z suwaka
    DaneWybor <- NULL
    
    wybor <- input$zanieczyszczenieAirly
    if (wybor == "Brak") wybor <- "PM2.5"
    updateSelectInput(session, "zanieczyszczenieAirly", choices = zanieczyszczeniaAirly[1:length(zanieczyszczeniaAirly)-1], selected = wybor)
    
    # Na podstawie wyboru pomiaru dobierane są kolumny z ramki danych
    if (input$zanieczyszczenieAirly == "PM2.5") {
      indeks_zanieczyszczenia <<- 1
      Paleta_Glob <<- "RdYlGn"
      Jednostka_Glob <<- "µg/m³"
      Nazwa_Wskaznika_Glob <<- "Poziom PM2.5"
    }
    else if (input$zanieczyszczenieAirly == "PM10") {
      indeks_zanieczyszczenia <<- 2
      Paleta_Glob <<- "RdYlGn"
      Jednostka_Glob <<- "µg/m³"
      Nazwa_Wskaznika_Glob <<- "Poziom PM10"
    }
    else if (input$zanieczyszczenieAirly == "NO2") {
      indeks_zanieczyszczenia <<- 3
      Paleta_Glob <<- "RdYlGn"
      Jednostka_Glob <<- "µg/m³"
      Nazwa_Wskaznika_Glob <<- "Poziom NO2"
    }
    else if (input$zanieczyszczenieAirly == "SO2") {
      indeks_zanieczyszczenia <<- 4
      Paleta_Glob <<- "RdYlGn"
      Jednostka_Glob <<- "µg/m³"
      Nazwa_Wskaznika_Glob <<- "Poziom SO2"
    }
    else if (input$zanieczyszczenieAirly == "CO") {
      indeks_zanieczyszczenia <<- 5
      Paleta_Glob <<- "RdYlGn"
      Jednostka_Glob <<- "µg/m³"
      Nazwa_Wskaznika_Glob <<- "Poziom CO"
    }
    else if (input$zanieczyszczenieAirly == "O3") {
      indeks_zanieczyszczenia <<- 6
      Paleta_Glob <<- "RdYlGn"
      Jednostka_Glob <<- "µg/m³"
      Nazwa_Wskaznika_Glob <<- "Poziom O3"
    }
    else if (input$zanieczyszczenieAirly == "PM1") {
      indeks_zanieczyszczenia <<- 7
      Paleta_Glob <<- "RdYlGn"
      Jednostka_Glob <<- "µg/m³"
      Nazwa_Wskaznika_Glob <<- "Poziom PM1"
    }
    else if (input$zanieczyszczenieAirly == "Ciśnienie") {
      indeks_zanieczyszczenia <<- 8
      Paleta_Glob <<- "YlGnBu"
      Jednostka_Glob <<- "hPa"
      Nazwa_Wskaznika_Glob <<- "Ciśnienie"
    }
    else if (input$zanieczyszczenieAirly == "Wilgotność") {
      indeks_zanieczyszczenia <<- 9
      Paleta_Glob <<- "YlGn"
      Jednostka_Glob <<- "%"
      Nazwa_Wskaznika_Glob <<- "Wilgotność"
    }
    else if (input$zanieczyszczenieAirly == "Temperatura") {
      indeks_zanieczyszczenia <<- 10
      Paleta_Glob <<- "RdBu"
      Jednostka_Glob <<- "°C"
      Nazwa_Wskaznika_Glob <<- "Temperatura"
    }
    else if (input$zanieczyszczenieAirly == "Temperatura gruntu") {
      indeks_zanieczyszczenia <<- 17
      Paleta_Glob <<- "RdBu"
      Jednostka_Glob <<- "°C"
      Nazwa_Wskaznika_Glob <<- "Temperatura gruntu"
    }
    else if (input$zanieczyszczenieAirly == "Punkt rosy 2m") {
      indeks_zanieczyszczenia <<- 13
      Paleta_Glob <<- "RdBu"
      Jednostka_Glob <<- "°C"
      Nazwa_Wskaznika_Glob <<- "Punkt rosy 2m"
    }
    else if (input$zanieczyszczenieAirly == "Deszcz") {
      indeks_zanieczyszczenia <<- 14
      Paleta_Glob <<- "Blues"
      Jednostka_Glob <<- "mm"
      Nazwa_Wskaznika_Glob <<- "Deszcz"
    }
    else if (input$zanieczyszczenieAirly == "Prędkość wiatru") {
      indeks_zanieczyszczenia <<- 15
      Paleta_Glob <<- "YlOrRd"
      Jednostka_Glob <<- "km/h"
      Nazwa_Wskaznika_Glob <<- "Prędkość wiatru"
    }
    else if (input$zanieczyszczenieAirly == "Wilgotność gleby") {
      indeks_zanieczyszczenia <<- 18
      Paleta_Glob <<- "YlGn"
      Jednostka_Glob <<- "m³/m³"
      Nazwa_Wskaznika_Glob <<- "Wilgotność gleby"
    }
    
    godzina_ <- input$hourSel + 3600
    godzina1 <- format(round(godzina_, units="hours"), format="%H:%M:%S")
    godzina_ <- paste(substr(as.character(godzina_), 1,10), as.character(godzina1), "CET")
    
    godzina2 <- as.numeric(substr(as.character(godzina_), 12,13))
    godzina1 <- as.numeric(substr(as.character(DanePomiarowe$Dane$DATE[1]), 12,13))
    hourDiff <- godzina2 - godzina1
    
    dzien2 <- as.numeric(substr(as.character(godzina_), 9,10))
    dzien1 <- as.numeric(substr(as.character(DanePomiarowe$Dane$DATE[1]), 9,10))
    dayDiff <- 0
    
    if (dzien2 < dzien1) dzien2 <- dzien2 + dzien1 # W przypadku gdy przeskakuje na następny miesiąc, następny dzień będzie miał mniejszą wartość niż poprzedni
    
    dayDiff <- dzien2 - dzien1
    hourDiff <- hourDiff + 24 * dayDiff
    Zmiana_Godziny_Glob <<- hourDiff
    
    CzynnikCalyOkres <- NULL
    for (i in 1:Ile_Godzin) CzynnikCalyOkres <- cbind(CzynnikCalyOkres, DanePomiarowe$Dane[DanePomiarowe$Dane$DATE == DanePomiarowe$Dane$DATE[i], 2 + indeks_zanieczyszczenia])
    
    DaneWybor <- data.frame( # Tworzę nowego dataframe'a, który zawiera tylko dane z danej godziny
      id = DanePomiarowe$Punkty$id,
      lat = DanePomiarowe$Punkty$location$latitude,
      lon = DanePomiarowe$Punkty$location$longitude,
      subset(DanePomiarowe$Punkty, select = seq(4,13)),
      Czynnik = DanePomiarowe$Dane[DanePomiarowe$Dane$DATE == unique(DanePomiarowe$Dane$DATE)[hourDiff+1], 2 + indeks_zanieczyszczenia], # Wektor danych dla danej godziny z nową nazwą kolumny
      CzynnikCalyOkres = CzynnikCalyOkres,
      Kierunek = DanePomiarowe$Dane[DanePomiarowe$Dane$DATE == unique(DanePomiarowe$Dane$DATE)[hourDiff+1], 18])
    
    poczatek_danych_okresowych <<- 15
    koniec_danych_okresowych <<- length(DaneWybor) - 1
    
    DaneWybor
    
    
  })
  
  # 3. Podstawowa mapa leaflet (tylko mapa bez obiektów)
  output$map <- renderLeaflet({
    
    # Oznaczenie granic województwa i powiatów na legendzie
    kolory <- c(
      "white; width:30px; height:9px; border:3px solid black; border-radius:0%",
      "white; width:30px; height:9px; border:2px solid #5e5e5e; border-radius:0%"
    )
    
    tabele <- c(
      "<div style='display: inline-block;'>Województwo</div>",
      "<div style='display: inline-block;'>Powiat</div>"  
    )
    
    # Funkcja zawierająca elementy powtarzalne dla każdego typu pomiaru tworząca mapę bez punktów
    # Wywoływana w zależności od typu pomiaru
    mapuj_podstawe <- function(tytul_pam, paleta, legenda_tytul, Jednostka_ = "µg/m³", OdwrocPalete_ = TRUE) {
      
      ### Powtórzony kod, ale potrzebny, żeby ten blok z funkcją mapuj podstawę nie był wywoływany
      ### za każdym razem gdy zmieniania jest godzina, co jest czasochłonne
      DaneCalyOkres <- NULL
      i <- NULL
      
      if (input$zanieczyszczenieAirly == "PM2.5") {i <- 1}
      else if (input$zanieczyszczenieAirly == "PM10") {i <- 2}
      else if (input$zanieczyszczenieAirly == "NO2") {i <- 3}
      else if (input$zanieczyszczenieAirly == "SO2") {i <- 4}
      else if (input$zanieczyszczenieAirly == "CO") {i <- 5}
      else if (input$zanieczyszczenieAirly == "O3") {i <- 6}
      else if (input$zanieczyszczenieAirly == "PM1") {i <- 7}
      else if (input$zanieczyszczenieAirly == "Ciśnienie") {i <- 8}
      else if (input$zanieczyszczenieAirly == "Wilgotność") {i <- 9}
      else if (input$zanieczyszczenieAirly == "Temperatura") {i <- 10}
      else if (input$zanieczyszczenieAirly == "Temperatura gruntu") {i <- 17}
      else if (input$zanieczyszczenieAirly == "Punkt rosy 2m") {i <- 13}
      else if (input$zanieczyszczenieAirly == "Deszcz") {i <- 14}
      else if (input$zanieczyszczenieAirly == "Prędkość wiatru") {i <- 15}
      else if (input$zanieczyszczenieAirly == "Wilgotność gleby") {i <- 18}
      
      DaneCalyOkres <- NULL
      for (j in 1:Ile_Godzin) DaneCalyOkres <- cbind(DaneCalyOkres, DanePomiarowe$Dane[DanePomiarowe$Dane$DATE == DanePomiarowe$Dane$DATE[j], 2 + i])
      ###
      
      # Łączenie html'owego tytułu mapy z wcześniejszym stylem css
      tytul <- tags$div(
        tag.map.title, HTML("Interaktywna mapa zanieczyszczeń\n w województwie małopolskim")
      )
      
      klasyfikacja <- NULL
      DaneDoPaletowania <- DaneCalyOkres
      if (input$skalowanie == "Globalnie") {klasyfikacja <- normy[,i]}
      else {klasyfikacja <- make_bins(10, DaneDoPaletowania, ZwrocNA_ = FALSE)}
      
      mypalette <- colorBin(palette = paleta, domain = DaneDoPaletowania,
                            na.color = "transparent",
                            bins = klasyfikacja,
                            reverse = OdwrocPalete_)
      
      koloryBin <- mypalette(klasyfikacja[1:length(klasyfikacja)-1])
      
      dodajLegende <- function(map) {
        if ((sum(!is.na(DaneCalyOkres)) == 0)) {
          return (
            leaflet::addLegend(map, title = legenda_tytul, opacity = 0.8, position = "bottomright",
                               labels = "<div>Brak danych</div>",
                               colors = "white; width:5px; height:5px; border:1px solid black; border-radius:0%")
          )
        }
        else {
          napisy <- paste(" <", klasyfikacja[2], Jednostka_)
          for (i in 2:(length(klasyfikacja)-2)) {
            napisy <- c(napisy, paste(klasyfikacja[i], "-", klasyfikacja[i+1], Jednostka_))
          }
          napisy <- c(napisy, paste(" >", klasyfikacja[length(klasyfikacja)-1], Jednostka_))
          return(
            leaflet::addLegend(map, labels = napisy, colors = koloryBin, values = klasyfikacja, opacity = 0.8,
                               title = legenda_tytul, position = "bottomright")
          )
        }
      }
      
      # Funkcja leaflet tworząca mapę
      leaflet(options = leafletOptions(zoomControl = FALSE)) %>%
        htmlwidgets::onRender("function(el, x) {
               L.control.zoom({ position: 'bottomleft' }).addTo(this)
            }") %>%
        addTiles(group = "OSM (default)")  %>% 
        setView(lat = 49.775, lng = 20, zoom = 8) %>%
        clearShapes() %>%
        addPolygons(data = WojMalopolskie, color = "black", smoothFactor = 0.3,
                    fillOpacity = 0, opacity = 1, weight = 1.5) %>%
        addPolygons(data = PowiatyMalopolskie, color = "#5e5e5e", smoothFactor = 0.3,
                    fillOpacity = 0, opacity = 1, weight = 0.8) %>%
        leaflet::addControl(tytul, position = "topright", className = "map-title") %>%
        leaflet::addLegend(title = "Podział administracyjny", colors = kolory,
                           labels = tabele, position = "bottomright") %>%
        dodajLegende()
      
    }
    
    # Instrukcje warunkowego dla każdego typu pomiaru
    if (input$zanieczyszczenieAirly == "PM2.5")
      mapuj_podstawe('Poziom PM2.5', "RdYlGn", "PM2.5")
    else if (input$zanieczyszczenieAirly == "PM10")
      mapuj_podstawe('Poziom PM10', "RdYlGn", "PM10")
    else if (input$zanieczyszczenieAirly == "NO2")
      mapuj_podstawe('Poziom dwutlenku azotu', "RdYlGn", "NO2")
    else if (input$zanieczyszczenieAirly == "SO2")
      mapuj_podstawe('Poziom dwutlenku siarki', "RdYlGn", "SO2")
    else if (input$zanieczyszczenieAirly == "CO")
      mapuj_podstawe('Poziom zanieczyszczenia czadem', "RdYlGn", "CO")
    else if (input$zanieczyszczenieAirly == "O3")
      mapuj_podstawe('Stężenie ozonu w atmosferze', "RdYlGn", "O3")
    else if (input$zanieczyszczenieAirly == "PM1")
      mapuj_podstawe('Poziom PM1', "RdYlGn", "PM1")
    else if (input$zanieczyszczenieAirly == "Ciśnienie")
      mapuj_podstawe('Wysokość ciśnienia', "YlGnBu", "Ciśnienie", Jednostka_ = "hPa")
    else if (input$zanieczyszczenieAirly == "Wilgotność")
      mapuj_podstawe('Wilgotność powietrza', "YlGn", "Wilgotność", Jednostka_ = "%", OdwrocPalete_ = FALSE)
    else if (input$zanieczyszczenieAirly == "Temperatura")
      mapuj_podstawe('Temperatura', "RdBu", "Temperatura", Jednostka_ = "°C")
    else if (input$zanieczyszczenieAirly == "Temperatura gruntu")
      mapuj_podstawe('Temperatura gruntu', "RdBu", "Temperatura gruntu", Jednostka_ = "°C")
    else if (input$zanieczyszczenieAirly == "Punkt rosy 2m")
      mapuj_podstawe("Punkt rosy 2m", "RdBu", "Punkt rosy 2m", Jednostka_ = "°C")
    else if (input$zanieczyszczenieAirly == "Deszcz")
      mapuj_podstawe("Deszcz", "Blues", "Deszcz", Jednostka_ = "mm", OdwrocPalete_ = FALSE)
    else if (input$zanieczyszczenieAirly == "Prędkość wiatru")
      mapuj_podstawe("Prędkość wiatru", "YlOrRd", "Prędkość wiatru", Jednostka_ = "km/h", OdwrocPalete_ = FALSE)
    else if (input$zanieczyszczenieAirly == "Wilgotność gleby")
      mapuj_podstawe("Wilgotność gleby", "YlGn", "Wilgotność gleby", Jednostka_ = "m³/m³", OdwrocPalete_ = FALSE)
    
  })
  
  # 4.Dodawanie obiektów na mapę wg zaznaczonej godziny
  observe({
    
    DaneFiltrowane <- filteredData() # Pobieram przefiltrowane przez suwak dane
    DaneCalyOkres <- DaneFiltrowane[poczatek_danych_okresowych:koniec_danych_okresowych]
    
    mapuj_jednostki <- function(Dane_, Paleta_Glob) { # Funkcja do kolorowania gmin/powiatów
      
      DaneAgreg <- agreguj(DaneFiltrowane, Dane_, DaneFiltrowane$Czynnik)
      
      id_warstwy <- id_warstwy + 1
      
      czyszczenie_shapeow <- function(map) {
        if(poprzednie_wyswietlanie == DaneAgreg$RODZAJ[1]) {
          return(removeShape(map, layerId = as.character((id_warstwy - 1) * as.numeric(unique(na.omit(DaneAgreg$JPT_KOD_JE))))))
        } else {
          poprzednie_wyswietlanie <<- DaneAgreg$RODZAJ[1]
          return(clearShapes(map) %>% clearImages())
        }
      }
      
      ids <- as.character(id_warstwy*as.numeric(unique(na.omit(DaneAgreg$JPT_KOD_JE))))
      godzina <- substr(as.character(input$hourSel), 12,16)
      
      info <- paste(
        "<head><style>.div1 {text-align: center;}</style></head>",
        "<div class = \"div1\"><b>", DaneAgreg$JPT_NAZWA_, godzina, "</b></div>",
        "ID: <b>", DaneAgreg$JPT_KOD_JE, "</b><br/>",
        "Liczba pomiarów: <b>", DaneAgreg$LPunktow, "</b><br/>",
        Nazwa_Wskaznika_Glob, ": <b>", round(DaneAgreg$Wartosc,2), Jednostka_Glob, "</b><br/>") %>%
        lapply(htmltools::HTML)
      
      
      leafletProxy("map", data = DaneFiltrowane) %>%
        clearMarkers()  %>%
        czyszczenie_shapeow() %>%
        addPolygons(data = DaneAgreg, color = "#5e5e5e", smoothFactor = 0.3,
                    fillOpacity = 0.7, opacity = 1, weight = 0.8,
                    fillColor =  mypalette(DaneAgreg$Wartosc),
                    label = info,
                    labelOptions = labelOptions(
                      style = list(
                        "font-weight" = "normal",
                        "box-shadow" = "3px 3px rgba(0,0,0,0.25)",
                        "border-color" = "rgba(0,0,0,0.5)",
                        "font-family" = "Josefin Sans",
                        "background-color" = "#ededf7"
                      ),
                      textsize = "13px", direction = "auto"),
                    layerId = ids
        )
    }
    
    ## CZĘŚĆ WŁAŚCIWA FUNKCJI MAPUJ()
    
    OdwrocPalete_ <- if (input$zanieczyszczenieAirly %in% c("Wilgotność", "Deszcz", "Wilgotność gleby", "Wilgotność względna OpenMeteo 2m", "Prędkość wiatru")) {FALSE} else {TRUE}
    
    klasyfikacja <- NULL
    DaneDoPaletowania <- DaneCalyOkres
    if (input$skalowanie == "Globalnie") {klasyfikacja <- normy[,indeks_zanieczyszczenia]}
    else {klasyfikacja <- make_bins(10, DaneDoPaletowania)}
    
    mypalette <- colorBin(palette = Paleta_Glob, domain = DaneDoPaletowania,
                          na.color = "transparent",
                          bins = klasyfikacja,
                          reverse = OdwrocPalete_)
    
    raster_paleta <- colorBin(palette = colorRampPalette(RColorBrewer::brewer.pal(9, name = Paleta_Glob))(100), domain = DaneDoPaletowania,
                              na.color = "transparent",
                              bins = make_bins(100, klasyfikacja),
                              reverse = OdwrocPalete_)
    
    godzina <- substr(as.character(input$hourSel), 12,16)
    
    info <- paste( # Okno podręczne dla każdego czynnika
      "<head><style>.div1 {text-align: center;}</style></head>",
      "<div class = \"div1\"><b>", DaneFiltrowane$adres, godzina, "</b></div>",
      "ID: <b>", DaneFiltrowane$id, "</b><br/>",
      "Wysokość: <b>", DaneFiltrowane$elevation, "m</b><br/>",
      Nazwa_Wskaznika_Glob, ": <b>", DaneFiltrowane$Czynnik, Jednostka_Glob, "</b><br/>")
    
    leafletProxy("map") %>% clearMarkers()
    
    if (input$zanieczyszczenieAirly == "Prędkość wiatru") {
      
      info <- paste(info, "Kierunek wiatru: <b>", DaneFiltrowane$Kierunek, "°</b>")
      
      for (i in 1:length(DaneFiltrowane$Kierunek)) {
        ikona <- makeIcon(iconUrl = paste0("wind", DaneFiltrowane$Kierunek[i], ".png"))
        leafletProxy("map") %>% addMarkers(DaneFiltrowane$lon[i], DaneFiltrowane$lat[i], icon = ikona)
      }
      
    }
    
    info <- info %>% lapply(htmltools::HTML)
    
    if (input$agregacja == "Punkty") {
      
      
      czyszczenie_reszty <- function(map) {
        if(poprzednie_wyswietlanie != "Punkty") {
          poprzednie_wyswietlanie <<- "Punkty"
          if(poprzednie_wyswietlanie == "Raster") return(clearImages(map))
          else return(clearShapes(map))
        } else {
          return(addTiles(map))
        }
      }
      
      leafletProxy("map", data = DaneFiltrowane) %>%
        addCircleMarkers(~lon, ~lat,
                         fillColor = ~mypalette(Czynnik), fillOpacity = 1,
                         color="black", weight = 0.5, radius = 6, opacity = 0.5,
                         label = info,
                         labelOptions = labelOptions(
                           style = list(
                             "font-weight" = "normal",
                             "box-shadow" = "3px 3px rgba(0,0,0,0.25)",
                             "border-color" = "rgba(0,0,0,0.5)",
                             "font-family" = "Josefin Sans",
                             "background-color" = "#ededf7"
                           ),
                           textsize = "13px", direction = "auto")
        ) %>%
        czyszczenie_reszty()
      
    }
    
    else if (input$agregacja == "Gminy") {mapuj_jednostki(GminyMalopolskie, Paleta_Glob)}
    else if (input$agregacja == "Powiaty") {mapuj_jednostki(PowiatyMalopolskie, Paleta_Glob)}
    else if (input$agregacja == "Województwo") {mapuj_jednostki(WojMalopolskie, Paleta_Glob)}
    
    else if (input$agregacja == "Ciągła") {
      
      if (sum(!is.na(DaneFiltrowane$Czynnik)) > 0) {
        
        raster <- kriging(DaneFiltrowane, WojShape, input$modelKriging, 2000)
        
        czyszczenie_rastrow <- function(map) {
          if(poprzednie_wyswietlanie != "Punkty" && poprzednie_wyswietlanie != "Raster") {
            poprzednie_wyswietlanie <<- "Raster"
            return(clearShapes(map))
          } else if (poprzednie_wyswietlanie == "Punkty") {
            poprzednie_wyswietlanie <<- "Raster"
            return(clearMarkers(map))
          } else return(clearImages(map))
        }
        
        leafletProxy("map", data = DaneFiltrowane) %>%
          czyszczenie_rastrow() %>%
          addRasterImage(raster, colors = raster_paleta, opacity = 0.75) %>%
          addCircleMarkers(~lon, ~lat,
                           fillColor = ~mypalette(Czynnik), fillOpacity = 0.2,
                           color="black", weight = 0.5, radius = 6, opacity = 0.5,
                           label = info,
                           labelOptions = labelOptions(
                             style = list(
                               "font-weight" = "normal",
                               "box-shadow" = "3px 3px rgba(0,0,0,0.25)",
                               "border-color" = "rgba(0,0,0,0.5)",
                               "font-family" = "Josefin Sans",
                               "background-color" = "#ededf7"
                             ),
                             textsize = "13px", direction = "auto")
          )
        
      }
      
    }
    
  })
  
  # 5. Przybliżanie do punktu z eksploratora na mapę
  observe({
    if (is.null(input$goto))
      return()
    isolate({
      map <- leafletProxy("map")
      map %>% clearPopups()
      dist <- 0.05
      id <- input$goto$id
      lat <- input$goto$lat
      lng <- input$goto$lng
      map %>% fitBounds(lng - dist, lat - dist, lng + dist, lat + dist)
    })
  })
  
  # 6. Eksplorator danych
  output$tabela <- DT::renderDataTable({
    
    df <- data.frame(ID = filteredData()$id, "Adres czujnika" = filteredData()$adres,
                     Województwo = filteredData()$Województwo,
                     Powiat = filteredData()$Powiat,
                     Gmina = filteredData()$Gmina,
                     Wysokość = filteredData()$elevation,
                     Czynnik = filteredData()$Czynnik,
                     Lat = filteredData()$lat,
                     Long = filteredData()$lon,
                     check.names = FALSE) %>%
      filter(
        Czynnik >= input$minScore,
        Czynnik <= input$maxScore,
        is.null(input$powiaty) | Powiat %in% input$powiaty
      ) %>%
      mutate(Action = paste('<a class="go-map" href="" data-lat="', Lat, '" data-long="', Long, '" data-id="', ID, '"><i class="fa fa-crosshairs"></i></a>', sep=""))
    names(df)[names(df) == "Czynnik"] <- if(!is.null(input$zanieczyszczenieAirly)) {input$zanieczyszczenieAirly} else input$zanieczyszczenieOW
    names(df)[names(df) == "Lat"] <- "Szerokość geograficzna"
    names(df)[names(df) == "Long"] <- "Długość geograficzna"
    names(df)[names(df) == "Action"] <- "Przybliż"
    action <- DT::dataTableAjax(session, df, outputId = "tabela")
    
    DT::datatable(df, options = list(ajax = list(url = action)), escape = FALSE)
  })
  
  # 7. Wykres
  output$wykresCzynnika <- renderImage({
    
    DaneFiltrowane <- filteredData()
    
    wykreslaj <- function(Jednostka_ = "µg/m³", Paleta_ = "RdYlGn", OdwrocPalete_ = TRUE, LiniaMeteo_ = NULL) {
      
      przedzial <- seq(poczatek_danych_okresowych, koniec_danych_okresowych)
      
      if (sum(!is.na(DaneFiltrowane[przedzial])) == 0) {
        return(plot(0, main = "Brak danych", xlab = "", ylab = "", xaxt = "none", yaxt = "none", pch = 20, cex = 0))
      }
      
      klasyfikacja <- NULL
      DaneDoPaletowania <- DaneFiltrowane[przedzial]
      if (input$skalowanie == "Globalnie") {klasyfikacja <- normy[,indeks_zanieczyszczenia]}
      else {klasyfikacja <- make_bins(10, DaneDoPaletowania)}
      
      mypalette <- colorBin(palette = Paleta_, domain = DaneDoPaletowania,
                            na.color = "transparent",
                            bins = klasyfikacja,
                            reverse = OdwrocPalete_)
      
      dane <- rep(NA,length(przedzial))
      it <- 0
      for (j in przedzial) {
        it <- it + 1
        dane[it] <- mean(unlist(DaneFiltrowane[j]), na.rm = TRUE)
      }
      godziny <- round(seq(1, length(dane[!is.na(dane)]), length.out = 4))
      maks <- max(dane,na.rm = TRUE)
      mini <- min(dane,na.rm = TRUE)
      windowsFonts(font = windowsFont("Josefin Sans"))
      
      Godziny_Etykiety <- NULL
      Gdzie_Zmiana_Doby <- NULL
      Daty <- NULL
      
      Godziny_Etykiety <- c(substr(as.character(DanePomiarowe$Dane$DATE[godziny]),12,16))
      Gdzie_Zmiana_Doby <- which(nchar(as.character(unique(DanePomiarowe$Dane$DATE))) < 11)
      Daty <- paste(substr(as.character(unique(DanePomiarowe$Dane$DATE)[Gdzie_Zmiana_Doby]),9,10), ".", substr(as.character(unique(DanePomiarowe$Dane$DATE)[Gdzie_Zmiana_Doby]),6,7), sep = "")
      
      barplot(dane[!is.na(dane)],
              main = input$zanieczyszczenieAirly,
              xlab = "Godzina", ylab = paste("Wartość [", Jednostka_, "]", sep = ""),
              xaxt = "none", yaxt = "none", xpd = FALSE, col = mypalette(dane),
              space = 0, ylim = c(mini - (maks - mini) / 10, maks), cex.sub = 0.75,
              family = "font", border = "#00000033", sub = "Niebieska linia - Historia / Prognoza")
      axis(1, at = godziny, labels = Godziny_Etykiety, cex.axis = 0.85)
      axis(2, at = tworz_sekwencje(mini, maks, 4, wlacznie_ = FALSE), cex.axis = 0.8)
      abline(v = Gdzie_Zmiana_Doby, lwd = 3, col = "brown")
      abline(v = Ile_Godzin - 24, lwd = 3, lty = 2, col = "#0000ff80")
      for (i in 1:length(Gdzie_Zmiana_Doby)) {mtext(Daty[i], side = 3, line = 0, at = Gdzie_Zmiana_Doby[i], cex = 0.8)}
      abline(v = Zmiana_Godziny_Glob + 0.5, lwd = 2, col = "black")
      
      if (!is.null(LiniaMeteo_)) {
        lines(1:length(dane[!is.na(dane)]), LiniaMeteo_[1:length(dane[!is.na(dane)])])
        legend(cex = 0.7, x = "topleft", inset = c(-0.25, -0.3), legend = "OpenMeteo", lty = 1, col = "black", xpd = TRUE)
      }
      
    }
    
    # A temp file to save the output.
    # This file will be removed later by renderImage
    outfile <- tempfile(fileext = '.png')
    
    # Generate the PNG
    png(outfile, width = 300, height = 250)
    
    if (sum(input$zanieczyszczenieAirly == c("PM2.5", "PM10", "NO2", "SO2", "CO", "O3", "PM1")) > 0) {
      wykreslaj()
    }
    else if (input$zanieczyszczenieAirly == "Ciśnienie") wykreslaj(Paleta_ = "YlGnBu", Jednostka_ = "hPa")
    else if (input$zanieczyszczenieAirly == "Wilgotność") {
      Linia <- NULL
      for (i in 1:Ile_Godzin) Linia <- cbind(Linia, mean(DanePomiarowe$Dane[DanePomiarowe$Dane$DATE == DanePomiarowe$Dane$DATE[i], 2 + 12], na.rm = TRUE))
      wykreslaj(Paleta_ = "YlGn", Jednostka_ = "%", OdwrocPalete_ =  FALSE, LiniaMeteo_ = Linia)
    }
    else if (input$zanieczyszczenieAirly == "Temperatura") {
      Linia <- NULL
      for (i in 1:Ile_Godzin) Linia <- cbind(Linia, mean(DanePomiarowe$Dane[DanePomiarowe$Dane$DATE == DanePomiarowe$Dane$DATE[i], 2 + 11], na.rm = TRUE))
      wykreslaj(Paleta_ = "RdBu", Jednostka_ = "°C", LiniaMeteo_ = Linia)
    }
    else if (input$zanieczyszczenieAirly == "Temperatura gruntu") wykreslaj(Paleta_ = "RdBu", Jednostka_ = "°C")
    else if (input$zanieczyszczenieAirly == "Punkt rosy 2m") wykreslaj(Paleta_ = "RdBu", Jednostka_ = "°C")
    else if (input$zanieczyszczenieAirly == "Deszcz") wykreslaj(Paleta_ = "Blues", Jednostka_ = "mm", OdwrocPalete_ = FALSE)
    else if (input$zanieczyszczenieAirly == "Prędkość wiatru") wykreslaj(Paleta_ = "YlOrRd", Jednostka_ = "km/h", OdwrocPalete_ = FALSE)
    else if (input$zanieczyszczenieAirly == "Wilgotność gleby") wykreslaj(Paleta_ = "YlGn", Jednostka_ = "m³/m³", OdwrocPalete_ = FALSE)
    
    
    
    dev.off()
    
    list(src = outfile,
         contentType = 'image/png',
         width = 300,
         height = 250,
         alt = "This is alternate text")
  }, deleteFile = TRUE)
  
  # 8. Mapa w zakładce prognoza
  output$map1 <- renderLeaflet({
    
    # Oznaczenie granic województwa i powiatów na legendzie
    kolory <- c(
      "white; width:30px; height:9px; border:3px solid black; border-radius:0%",
      "white; width:30px; height:9px; border:2px solid #5e5e5e; border-radius:0%"
    )
    
    tabele <- c(
      "<div style='display: inline-block;'>Województwo</div>",
      "<div style='display: inline-block;'>Powiat</div>"  
    )
    
    pkt <- which(DanePomiarowe$Punkty$id == input$PointChoice)
    wiatr <- DanePomiarowe$Dane$WINDDIRECTION_10M[DanePomiarowe$Dane$DATE == DanePomiarowe$Dane$DATE[Ile_Godzin - 24] & DanePomiarowe$Dane$ID == DanePomiarowe$Punkty$id[pkt]]
    kat1 <- wiatr - 30
    kat2 <- wiatr + 30
    
    kat_pkt <- Katy[pkt,2:ncol(Katy)]
    odl_pkt <- Odleglosci[pkt,2:ncol(Odleglosci)]
    
    mypalette <- colorBin(palette = "RdYlGn", domain = odl_pkt,
                          na.color = "transparent",
                          bins = seq(0,max(odl_pkt, na.rm = TRUE), length.out = 10),
                          reverse = TRUE)
    
    # Tworzę okrąg zniekształcony do elipsy ze względu na transformację współrzędnych
    # geodezyjnych do geograficznych, aby linie były pozornie tej samej długości 
    
    p2 <- oblicz_elipse(DanePomiarowe$Punkty$location$longitude[pkt], DanePomiarowe$Punkty$location$latitude[pkt], kat1)
    p3 <- oblicz_elipse(DanePomiarowe$Punkty$location$longitude[pkt], DanePomiarowe$Punkty$location$latitude[pkt], kat2)
    
    p1 <- c(DanePomiarowe$Punkty$location$longitude[pkt], DanePomiarowe$Punkty$location$latitude[pkt])
    p1 <- c(p1, p2[1], p2[2])
    p1 <- c(p1, p3[1], p3[2])
    
    poly <- sp::Polygon(cbind(p1[c(1,3,1,5)], p1[c(2,4,2,6)]))
    poly <- sp::Polygons(list(poly), ID = "A")
    poly <- sp::SpatialPolygons(list(poly))
    
    info <- paste(
      "<head><style>.div1 {text-align: center;}</style></head>",
      "<div class = \"div1\">Punkt <b>", DanePomiarowe$Punkty$id, "</b></div>",
      "Odległość: <b>", round(odl_pkt/1000,2), "km</b><br/>",
      "Azymut: <b>", round(kat_pkt), "°</b>", sep = "") %>%
      lapply(htmltools::HTML)
    
    
    tag.map.title1 <- tags$style(HTML(".leaflet-control.map-title1 {
      right: 0%;
      top: 0px;
      width: 100%;
      padding-left: 10px;
      padding-right: 10px;
      background: rgba(255,255,255,0.75);
      font-size: 15px;
      box-shadow: 3px 3px 5px rgba(0,0,0,0.25);
      border-radius: 5px;
      font-family: Josefin Sans;
   }"))
    
    tytul1 <- tags$div(
      tag.map.title1, HTML("Punkt <b>", DanePomiarowe$Punkty$id[pkt], "</b><br>Wiatr: <b>", wiatr, "°</b><br>W zasięgu: <b>", paste0(sum(kat_pkt < kat2 & kat_pkt > kat1), " pkt</b>",  sep = ""))
    )
    
    napisy_legenda <- paste0(round(seq(0,max(odl_pkt, na.rm = TRUE), length.out = 10)[1:9]/1000), "km")
    
    leaflet(DanePomiarowe$Punkty) %>%
      addTiles(group = "OSM (default)")  %>% 
      setView(lat = 49.775, lng = 20, zoom = 8) %>%
      clearShapes() %>%
      addPolygons(data = WojMalopolskie, color = "black", smoothFactor = 0.3,
                  fillOpacity = 0, opacity = 1, weight = 1.5) %>%
      addPolygons(data = PowiatyMalopolskie, color = "#5e5e5e", smoothFactor = 0.3,
                  fillOpacity = 0, opacity = 1, weight = 0.8) %>%
      leaflet::addLegend(title = "Podział administracyjny", colors = kolory,
                         labels = tabele, position = "bottomright") %>%
      # Marker punktowy na punkt, dla którego liczymy prognozę
      addCircleMarkers(lng = ~location$longitude[pkt], lat = ~location$latitude[pkt],
                       fillOpacity = 1, color = "blue", weight = 0.5, radius = 6, opacity = 1) %>%
      # Markery punktowe dla punktów, które znajdują się na kącie skąd wieje wiatr
      addCircleMarkers(lng = ~location$longitude[which(kat_pkt < kat2 & kat_pkt > kat1)],
                       lat = ~location$latitude[which(kat_pkt < kat2 & kat_pkt > kat1)],
                       fillColor = mypalette(odl_pkt[which(kat_pkt < kat2 & kat_pkt > kat1)]),
                       weight = 2, radius = 6, opacity = 1, color = "black", fillOpacity = 1) %>%
      # Pozostałe markery (półprzezroczyste) - tak naprawdę nakładane na każdy punkt,
      # żeby pojawiło się okno podręczne
      addCircleMarkers(lng = ~location$longitude, lat = ~location$latitude, fillOpacity = 0.4, color = "black",
                       fillColor = ~mypalette(odl_pkt), weight = 0.5, radius = 6, opacity = 1,
                       label = info, labelOptions = labelOptions(
                         style = list(
                           "font-weight" = "normal",
                           "box-shadow" = "3px 3px rgba(0,0,0,0.25)",
                           "border-color" = "rgba(0,0,0,0.5)",
                           "font-family" = "Josefin Sans",
                           "background-color" = "#ededf7"
                         ),
                         textsize = "13px", direction = "auto")) %>%
      addPolygons(data = poly, color = "black", smoothFactor = 0.3,
                  fillOpacity = 0, opacity = 0.5, weight = 2) %>%
      leaflet::addLegend(colors = mypalette(seq(0,max(odl_pkt, na.rm = TRUE), length.out = 10)[1:9]), labels = napisy_legenda,
                         values = odl_pkt, opacity = 0.8, title = "Odległości",
                         position = "bottomright") %>%
      leaflet::addControl(tytul1, position = "topright", className = "map-title1") %>%
      addScaleBar(position = "bottomleft")
    
    
    
  })
  
  # 9. Wykresy prognozy
  
  output$PlotMain <- renderPlotly({
    
    if (!is.null(v$data)) {
      
      dane <- v$data
      kk <<- dane
      
      mse <- mean((dane$TEST[!is.na(dane$TEST)] - dane$PRED[!is.na(dane$PRED)])^2, na.rm = TRUE)
      mae <- mean(abs(dane$PRED[!is.na(dane$PRED)] - dane$TEST[!is.na(dane$TEST)]), na.rm = TRUE)
      
      maks <- as.numeric(dane$INFO[3])
      mini <- as.numeric(dane$INFO[4])
      dane$TRAIN <- dane$TRAIN * (maks - mini) + mini
      dane$TEST <- dane$TEST * (maks - mini) + mini
      dane$PRED <- dane$PRED * (maks - mini) + mini
      
      info <- list(yref = 'paper', xref = "paper",
                   xanchor = "middle", yanchor = "top",
                   y = 1, x = 0.5, showarrow = FALSE,
                   text = paste0("MSE: ", round(mse*100,2), "%  MAE: ", round(mae*100,2), "%"), 
                   font = list(size = 15))
      
      zmiana_dnia <- which(hour(dane$DATE) == 0)
      ktore_dni <- paste0(day(dane$DATE[hour(dane$DATE) == 0]), ".0", month(dane$DATE[hour(dane$DATE) == 0]))
      
      p <- plot_ly(data = dane) %>%
        layout(
          title = list(text = paste0("<b>Prognoza ", dane$INFO[1], " z użyciem ", dane$INFO[2], " na 24h</b>"), font = czc1),
          xaxis = list(title = list(text = "Dzień", font = czc),
                       ticktext = ktore_dni,
                       tickvals = zmiana_dnia
                       ),
          yaxis = list(title = list(text = paste0("Poziom ", dane$INFO[1], " [µg/m³]"), font = czc)),
          annotations = info,
          legend = list(title = list(text = "<b>Legenda</b>"))
        ) %>%
        add_trace(x = 1:nrow(dane), y = ~TRAIN, mode = 'lines', name = "Dane treningowe",
                  line = list(size = 2, color = 'rgba(0, 0, 0, .9)'),
                  hovertemplate = '%{y:.2f}') %>%
        add_trace(x = 1:nrow(dane), y = ~TEST, mode = 'lines+markers', name = "Dane testowe",
                  line = list(size = 2, color = 'rgba(100, 100, 100, .9)'),
                  marker = list(size = 5, color = 'rgba(100, 100, 100, .9)'),
                  hovertemplate = '%{y:.2f}') %>%
        add_trace(x = 1:nrow(dane), y = ~PRED, mode = 'lines', name = "Prognoza",
                  line = list(size = 2, color = 'rgba(0, 255, 0, .9)'),
                  hovertemplate = '%{y:.2f}')
      
      p
      
    }
    else {
      plot_ly() %>%
        layout(
          title = list(text = "<b>Prognoza zanieczyszczenia powietrza</b>", font = czc1),
          xaxis = list(title = list(text = "Dzień", font = czc)),
          yaxis = list(title = list(text = "Poziom zanieczyszczenia [µg/m³]", font = czc))
        )
    }
    
    
  })
  
  output$Plot1 <- renderPlotly({
    
    if(!is.null(v$data)) {
      
      dane <- v$data
      
      pearson <- cor(dane$TEST[!is.na(dane$TEST)][-1], dane$PRED[!is.na(dane$PRED)][-1])
      
      info <- list(yref = 'paper', xref = "paper",
                   xanchor = "middle", yanchor = "top",
                   y = 1, x = 0.5, showarrow = FALSE,
                   text = paste0("Współczynnik korelacji: ", round(pearson,4)), 
                   font = list(size = 10))
      
      p <- plot_ly(data = dane, type = "scatter") %>%
        add_trace(x = dane$TEST, y = dane$PRED, mode = "markers", name = "",
                  hovertemplate = paste('<i>Prog</i>: %{y:.2f}', '<br><i>Test</i>: %{x:.2f}<br>'),
                  showlegend = F) %>%
        add_trace(data = dane, x = c(0,1), y=c(0,1) , mode="lines", name = "",
                  line = list(color = 'rgb(205, 12, 24)', width = 1, dash = 'dash'),
                  type = "scatter", showlegend = F) %>% 
        style(hoverinfo = "none") %>%
        layout(
          title = list(text = "Weryfikacja prognozy", font = czc1),
          xaxis = list(title = list(text = "Wartości testowe", font = czc)),
          yaxis = list(title = list(text = "Wartości prognozowane", font = czc)),
          annotations = info
        )
      
      p
      
    } else {
      
      plot_ly() %>%
        layout(
          title = list(text = "Weryfikacja prognozy", font = czc1),
          xaxis = list(title = list(text = "Wartości testowe", font = czc)),
          yaxis = list(title = list(text = "Wartości prognozowane", font = czc))
        )
      
    }
    
  })
  
  output$Plot2 <- renderPlotly({
    
    temp <- NULL
    
    if (is.null(v$data)) {
      
      temp <- data.frame(x=rep(NA,24),y=rep(NA,24))
      plot_ly(data = temp, x = 1:24, y = ~y) %>%
        layout(
          title = list(text = "Mierniki jakości modelu", font = czc1),
          xaxis = list(title = list(text = "Czas [h]", font = czc)),
          yaxis = list(title = list(text = "Poziom PM10 [µg/m³]", font = czc))
        )
      
    } else if (input$Algorithm == "RF") {
      
      temp <- data.frame(Drzewa = 1:length(v$model$mse), MSE = v$model$mse)
      
      plot_ly(data = temp, x = ~Drzewa, y = ~MSE) %>%
        add_trace(x = temp$Drzewa, y = temp$MSE, name = "",
                  hovertemplate = paste('<i>MSE</i>: %{y:.2%}', '<br><i>L.drzew</i>: %{x:.d}<br>'),
                  showlegend = F) %>%
        layout(
          title = list(text = "Błąd średniokwadratowy", font = czc1),
          xaxis = list(title = list(text = "Ilość drzew", font = czc)),
          yaxis = list(title = list(text = "MSE", font = czc))
        )
    } else if (input$Algorithm == "NN") plotly_nn(v$model, input$Pollution)
    
    
  })
  
  # 10. Prognoza
  
  observeEvent(input$Start, {
    
    idnt <- NULL
    if (input$Pollution == "PM25") idnt <- input$PointChoice
    else if (input$Pollution == "PM10") idnt <- input$PointChoice1
    else if (input$Pollution == "PM1") idnt <- input$PointChoice2
    
    temp <- DanePomiarowe$Dane[DanePomiarowe$Dane$ID == idnt,]
    wybor <- data.frame(temp$ID, temp$DATE)
    
    if (input$Pollution == "PM25") wybor <- cbind(wybor, temp$PM25)
    else if (input$Pollution == "PM10") wybor <- cbind(wybor, temp$PM10)
    else if (input$Pollution == "PM1") wybor <- cbind(wybor, temp$PM1)
    
    colnames(wybor) <- c("ID", "DATE", "Czynnik")
    
    if ("Temperatura" %in% input$VariablesPred) wybor$TEMPERATURE <- temp$TEMPERATURE
    if ("Cisnienie" %in% input$VariablesPred) wybor$PRESSURE <- temp$PRESSURE
    if ("Wilgotnosc" %in% input$VariablesPred) wybor$HUMIDITY <- temp$HUMIDITY
    if ("Godzina" %in% input$VariablesPred) wybor$HOUR <- temp$HOUR
    if ("Dzien" %in% input$VariablesPred) wybor$DAY <- temp$DAY
    if ("PunktRosy" %in% input$VariablesPred) wybor$DEWPOINT_2M <- temp$DEWPOINT_2M
    if ("Deszcz" %in% input$VariablesPred) wybor$RAIN <- temp$RAIN
    if ("PredkoscWiatr" %in% input$VariablesPred) wybor$WINDSPEED_10M <- temp$WINDSPEED_10M
    if ("KierunekWiatr" %in% input$VariablesPred) wybor$WINDDIRECTION_10M <- temp$WINDDIRECTION_10M
    if ("TempGleb" %in% input$VariablesPred) wybor$SOIL_TEMPERATURE_0CM <- temp$SOIL_TEMPERATURE_0CM
    if ("WilgGleb" %in% input$VariablesPred) wybor$SOIL_MOISTURE_0_TO_1CM <- temp$SOIL_MOISTURE_0_TO_1CM
    
    if (ncol(wybor) < 4) {
      v$data <- NULL
      v$model <- NULL
      return(0)
    }
    
    wybor <- utnij_braki(wybor)
    wybor[,3:ncol(wybor)] <- usrednij_braki(wybor[,3:ncol(wybor)])
    
    DaneLiczbowe <- apply(wybor, 2, function(x) as.numeric(as.character(x)))
    
    maxs <- apply(DaneLiczbowe[,3:ncol(wybor)], 2, max) # Szukanie maksimum dla każdej kolumny
    mins <- apply(DaneLiczbowe[,3:ncol(wybor)], 2, min) # Szukanie minimum dla każdej kolumny
    maxs[maxs == 0] <- 1
    
    sc_data <- data.frame(scale(DaneLiczbowe[,3:ncol(wybor)], center = mins, scale = maxs - mins)) #Skalowanie danych
    train_data <- sc_data[1:(nrow(sc_data)-24),]
    test_data <- sc_data[(nrow(sc_data)-23):nrow(sc_data),]
    
    pred <- NULL
    model <- NULL # Nazwy sieci neuronowej w dopełniaczu używane do nazwy wykresu
    
    if (input$Algorithm == "RF") {
      rf <- randomForest(Czynnik ~ ., data = train_data, ntree = input$nTree)
      pred <- predict(rf, test_data)
      model <- "lasu losowego"
      v$model <- rf
        
    }
    else if (input$Algorithm == "NN") {
      nn <- NULL
      liniowy <- TRUE
      if (!is.null(wybor$HOUR)) liniowy <- FALSE
      if (input$nLayer == 1) {
        nn <- neuralnet(Czynnik ~ ., data = train_data, stepmax = 1e8,
                        hidden = input$nLayer1, linear.output = liniowy)
        pred <- predict(nn, test_data)
        zmienna_glob <<- nn
      } else if (input$nLayer == 2) {
        nn <- neuralnet(Czynnik ~ ., data = train_data, stepmax = 1e8, linear.output = liniowy,
                        hidden = c(input$nLayer1, input$nLayer2))
        pred <- predict(nn, test_data)
      } else if (input$nLayer == 3) {
        nn <- neuralnet(Czynnik ~ ., data = train_data, stepmax = 1e8, linear.output = liniowy,
                        hidden = c(input$nLayer1, input$nLayer2, input$nLayer3))
        pred <- predict(nn, test_data)
      }
      v$model <- nn
      model <- "sieci neuronowej"
      
    }
    
    v$data <- data.frame(
      wybor$DATE,
      c(train_data$Czynnik, rep(NA, nrow(test_data))),
      c(rep(NA,nrow(train_data)-1), train_data$Czynnik[nrow(train_data)], test_data$Czynnik),
      c(rep(NA,nrow(train_data)-1), train_data$Czynnik[nrow(train_data)], pred),
      c(input$Pollution, model, maxs[1], mins[1], rep(NA,nrow(sc_data)-4))
    )
    
    colnames(v$data) <- c("DATE", "TRAIN", "TEST", "PRED", "INFO")
    
    
    
  })
  
}

#### APLIKACJA ####

shinyApp(ui, server)
