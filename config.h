#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

extern const int PRINTING_DISABLED;

extern const char BITMAP_PATH[]; // Pfad zu Ordner für Bitmap-Erzeugung
extern const char BITMAP_FILENAME[]; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

extern const int MAP_WIDTH; // Breite der Karte
extern const int MAP_HEIGHT; // Höhe der Karte
extern const int SCALE_FACTOR; // Skalierungsfaktor für die Bildausgabe
extern const int SIMULATION_STEPS; // Anzahl der Simulationsschritte -1 für unendlich

extern const int ELDERLY_AGE[]; // Sterbelter
extern const int PREDATOR_MAX_STARVE_TIME; // max. Jahre Hunger für Predator

extern const float BIRTH_RATE[]; // Geburtenrate
extern const float DYING_RATE[]; // Sterbewahrscheinlichkeit: 1/x

extern const int MAX_PLANT_AGE; // max. Alter einer Pflanze

#endif
