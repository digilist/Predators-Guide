#include "config.h"
#include "map.h"
#include "parallel.h"

int DEBUG = 0;
const int PRINTING_ENABLED = 0;

const char SAVE_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 100; // Breite der Karte
const int MAP_HEIGHT = 100; // Höhe der Karte
const int SCALE_FACTOR = 1; // Skalierungsfaktor für die Bildausgabe
const int MAX_SIMULATION_STEPS = 3; // Anzahl der Simulationsschritte -1 für unendlich

const float MAP_FILL_RATE = 0.3;
const float PREDATOR_RATE = 0.2; // an der MAP_FILL_RATE
const float PREY_RATE = 0.4; // an der MAP_FILL_RATE
const float PLANT_RATE = 0.1; // an der Gesamtmap

//const float MAP_FILL_RATE = 1;
//const float PREDATOR_RATE = 0.5; // an der MAP_FILL_RATE
//const float PREY_RATE = 0.5; // an der MAP_FILL_RATE
//const float PLANT_RATE = 0.5; // an der Gesamtmap

const int ELDERLY_AGE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	30, // PREY
	50 // PREDATOR
};

const int MAX_ENERGY = 5;

const float DYING_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.01, // PREY
	0.03 // PREDATOR
};

const float BIRTH_RATE[NUMBER_OF_POPULATION_TYPES] = {
	0, // EMPTY
	0.3, // PREY
	0.15 // PREDATOR
};
