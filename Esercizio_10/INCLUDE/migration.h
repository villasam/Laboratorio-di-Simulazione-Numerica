#ifndef __MIGRATION_H__
#define __MIGRATION_H__

#include "population.h"
#include "map.h"
#include "random.h"

void MigrateBestIndividuals(Population& pop, const Map& map, Random& rnd, int rank, int size);

#endif