#********************************************************
# MAKEFILE for BUILDING EXAMPLES FOR HIPO4 LIBRARY
# AUTHOR: GAVALIAN DATE: 10/24/2018
#
# Version modifiée par Felix Touchte Codjo pour son 
# usage personnel. Merci GAVALIAN ! 
# Date : 12 avril 2024
#********************************************************

# ATTENTION

# Les "paths" sont en chemin relatifs (rq: les ".."). Prière de localiser le dossier *hipo/hipo4...
# Quelques options de compilations
# 	-I<path> : Ajoute le répertoire <path> au chemin de recherche des fichiers d'en-tête.
# 	-L<path> : Ajoute le répertoire <path> au chemin de recherche des bibliothèques.

#PATH2HIPO := /homeijclab/touchte-codjo/hipo
#PATH2HIPO := /home/ftouchte/hipo
PATH2HIPO := /home/touchte-codjo/framework/hipo

HIPOCFLAGS  := -I$(PATH2HIPO)/hipo4 -I$(PATH2HIPO)/hipo4/chart   
HIPOLIBS    := -L$(PATH2HIPO)/lib -lhipo4 
									
LZ4LIBS     := -L$(PATH2HIPO)/lz4/lib -llz4
LZ4INCLUDES := -I$(PATH2HIPO)/lz4/lib


# ROOT libraries 
ROOTLIBS = $(shell root-config --libs)
# ROOT include flags
ROOTCFLAGS = $(shell root-config --cflags)

GTKLIBS = $(shell pkg-config --libs gtkmm-4.0)
GTKFLAGS = $(shell pkg-config --cflags gtkmm-4.0)

CAIROLIBS = $(shell pkg-config --libs cairomm-1.16)
CAIROFLAGS = $(shell pkg-config --cflags cairomm-1.16)


CXX       := g++
CXXFLAGS  += -Wall -fPIC -std=c++17
LD        := g++
LDFLAGS   :=


#all:  showFile histo plot benchmark simu
all: hits hist1d shape noise_count rms hv_scan first_channel view3D 

view3D: view3D.o fAxis.o fCanvas.o
	$(CXX) -o view3D.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS) $(CAIROLIBS)  $(GTKLIBS)

hits: hits.o
	$(CXX) -o hits.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

hist1d: hist1d.o
	$(CXX) -o hist1d.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

first_channel: first_channel.o
	$(CXX) -o first_channel.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS)

hv_scan: hv_scan.o fAxis.o fCanvas.o
	$(CXX) -o hv_scan.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS) $(CAIROLIBS)  $(GTKLIBS)

shape: shape.o
	$(CXX) -o shape.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS) 

noise_count: noise_count.o
	$(CXX) -o noise_count.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS) 

rms: rms.o
	$(CXX) -o rms.exe $^ $(HIPOLIBS) $(LZ4LIBS) $(ROOTLIBS) 

# $< représente la première de la cible, i.e histo.o
# $^ représente la liste complète des dépendances

clean:
	@echo 'Removing all build files'
	@rm -rf *.o *~ *.exe example*hipo *.pdf

%.o: %.cpp
	$(CXX) -c $< -O2 $(CXXFLAGS) $(HIPOCFLAGS) $(LZ4INCLUDES) $(ROOTCFLAGS) $(GTKFLAGS) $(CAIROFLAGS) 
	

