#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

extern const char BITMAP_PATH[]; // Pfad zu Ordner für Bitmap-Erzeugung
extern const char BITMAP_FILENAME[]; // Dateiname für Bitmap (muss ein %d für die Nummer des Schrittes enthalten)

extern const int MAP_WIDTH; // Breite der Karte
extern const int MAP_HEIGHT; // Höhe der Karte
extern const int SCALE_FACTOR; // Skalierungsfaktor für die Bildausgabe
extern const int SIMULATION_STEPS; // Anzahl der Simulationsschritte -1 für unendlich

extern const int MATURITY_AGE; // Gebähralter von Beute
extern const int ELDERLY_AGE; // Sterbelter
extern const int MAX_STARVE_TIME; // max. Jahre Hunger

#endif