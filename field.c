#include <stdio.h>
#include "field.h"

struct field {
	int width;
	int height;
};

enum species {
	PREDATOR,
	PREY
};

/**
 * 	Initialisiert das Spielfeld mit der angebenen Breite und Höhe
 * 
 */
void initField(int width, int height)
{
	struct field field = {width, height};
	
	printf("%d\n", field.width);
}
