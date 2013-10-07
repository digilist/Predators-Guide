#include "config.h"
#include "map.h"
#include "parallel.h"

int DEBUG = 0;
const int PRINTING_ENABLED = 1;

const char SAVE_PATH[] = "/tmp/pred/"; // path to the folder, which will contain the images
const char BITMAP_FILENAME[] = "%d.bmp"; // filename for a single bitmap, must contain a %d for the number of the simulation step

const int WEAK_SCALING = 0; // weak scaling for the parallelization?
const int MAP_WIDTH = 500; // width of the map
const int MAP_HEIGHT = 500; // height of the map
const int SCALE_FACTOR = 1; // to scale the bitmap
const int MAX_SIMULATION_STEPS = 200; // maximum number of simulation steps

const float MAP_FILL_RATE = 0.3; // for Predators and Prey
const float PREDATOR_RATE = 0.5; // of MAP_FILL_RATE
const float PREY_RATE = 0.5; // of MAP_FILL_RATE
const float PLANT_RATE = 0.1; // spawn rate, unrelated to MAP_FILL_RATE

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
