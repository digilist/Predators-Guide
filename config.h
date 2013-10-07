#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "map.h"

extern int DEBUG;
extern const int PRINTING_ENABLED;

extern const char SAVE_PATH[];
extern const char BITMAP_FILENAME[];

extern const int WEAK_SCALING;
extern const int MAP_WIDTH;
extern const int MAP_HEIGHT;
extern const int SCALE_FACTOR;
extern const int MAX_SIMULATION_STEPS;

const float MAP_FILL_RATE;
const float PREDATOR_RATE;
const float PREY_RATE;
const float PLANT_RATE;

extern const int ELDERLY_AGE[];
extern const int MAX_ENERGY;

extern const float BIRTH_RATE[];
extern const float DYING_RATE[];

extern const int MAX_PLANT_AGE;

#endif
