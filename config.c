#include "config.h"

const char BITMAP_PATH[] = "/tmp/pred/"; // Pfad zu Ordner für Bitmap-Erzeugung mit abschließendem /
const char BITMAP_FILENAME[] = "%d.bmp"; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

const int MAP_WIDTH = 200; // Breite der Karte
const int MAP_HEIGHT = 200; // Höhe der Karte
const int SCALE_FACTOR = 3; // Skalierungsfaktor für die Bildausgabe
const int SIMULATION_STEPS = 500; // Anzahl der Simulationsschritte -1 für unendlich

const int MATURITY_AGE = 4; // Gebähralter von Beute
const int ELDERLY_AGE = 18; // Sterbelter
const int MAX_STARVE_TIME = 2; // max. Jahre Hunger
