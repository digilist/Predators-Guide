#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED


/**
 * Repräsentiert das Spielfeld
 *
 */
struct Map {
	int width;
	int height;

	struct Field *fields; // Belegung der einzelnen Felder des Spielfelds
	int numberOfPredators; // Anzahl Räuber aktuell
	int numberOfPrey; // Anzahl Beute aktuell
};

/**
 * die möglichen Feldbelegungen: leeres Feld, Räuber & Beute
 *
 */
enum PopulationType {
	EMPTY, PREDATOR, PREY
};

/**
 * die Belegung eines Feldes
 *
 */
struct Field {
	enum PopulationType populationType;

	// wenn populationType == EMPTY muss keines dieser Werte gesetzt werden
	int age; // erreicht das Lebewesen ein bestimmtes Alter, bringt es ein neues Lebewesen auf die Welt

	int starveTime; // Anzahl der Zeiteinheiten, die der Räuber hungern muss (nur für Räuber)
					// nach x ZE stirbt der Räuber

};

void initField(int width, int height);

void* getField(struct Map *map, int x, int y);

void resetField(struct Field *field);

void dumpPopulation(struct Map *map);

#endif
