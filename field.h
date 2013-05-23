#ifndef FIELD_H_INCLUDED
#define FIELD_H_INCLUDED


/**
 * Repräsentiert das Spielfeld
 *
 */
struct PlayingField {
	int width;
	int height;

	struct FieldContent **population; // Belegung der einzelnen Felder des Spielfelds
	int numberOfPredators; // Anzahl Räuber aktuell
	int numberOfPreys; // Anzahl Beute aktuell
};

/**
 * die Belegung eines Feldes
 *
 */
struct FieldContent {
	enum FieldPopulationType *populationType;

	// wenn populationType == EMPTY muss keines dieser Werte gesetzt werden
	int age; // erreicht das Lebewesen ein bestimmtes Alter, bringt es ein neues Lebewesen auf die Welt

	int starveTime; // Anzahl der Zeiteinheiten, die der Räuber hungern muss (nur für Räuber)
					// nach x ZE stirbt der Räuber

};

/**
 * die möglichen Feldbelegungen: leeres Feld, Räuber & Beute
 *
 */
enum FieldPopulationType {
	EMPTY, PREDATOR, PREY
};

/**
 * Richtungen in die sich ein Tier bewegen kann
 *
 */
enum Direction {
	TOP, BOTTOM, LEFT, RIGHT
};

void initField(int width, int height);

void* getFieldContent(struct PlayingField *field, int x, int y);

void dumpPopulation(struct PlayingField *field);

#endif
