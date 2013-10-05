#include "config.h"
#include "map.h"
#include "parallel.h"

int DEBUG = 0;
const int PRINTING_ENABLED = 0;

const char SAVE_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 300; // Breite der Karte
const int MAP_HEIGHT = 300; // Höhe der Karte
const int SCALE_FACTOR = 1; // Skalierungsfaktor für die Bildausgabe
const int MAX_SIMULATION_STEPS = 100; // Anzahl der Simulationsschritte

const float MAP_FILL_RATE = 0.3;
const float PREDATOR_RATE = 0.5; // an der MAP_FILL_RATE
const float PREY_RATE = 0.5; // an der MAP_FILL_RATE
const float PLANT_RATE = 0.1; // an der Gesamtmap

const int ELDERLY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	30, // PREY
	30 // PREDATOR
};

const int MAX_ENERGY = 10;

const float DYING_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.02, // PREY
	0.03 // PREDATOR
};

const float BIRTH_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.5, // PREY
	0.2 // PREDATOR
};
