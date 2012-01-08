/*
 TinyGA - TinyGA.cpp - rev 0.1
 Created: 1/2012
 Author: Alexander Hiam - <ahiam@marlboro.edu> - http://www.alexanderhiam.com
 Website: https://github.com/alexanderhiam/TinyGA

 A lightweight genetic algorithm library for Arduino microcontrollers.

 Copyright (c) 2012 - Alexander Hiam 

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. 
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h" // Arduino IDE >= v1.0
#else
#include "WProgram.h"
#endif

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "TinyGA.h"


TinyGA::TinyGA(uint8_t (*fitness_function)(uint8_t)) {
  fitness = fitness_function;
}

void TinyGA::init(uint8_t population_size) {
  uint8_t i;
  pop_size = population_size;
  generation = 0;
  // Initialize random poulation:
  for (i=0; i<pop_size; i++) {
    population[i] = (uint8_t)random(256);
  }
}

uint8_t TinyGA::load(void) {
  uint8_t i;
  for (i=0; i<3; i++) {
    if (eread(EEPROM_OFFSET) != TINYGA_KEY) {
      return 0; // No population found.
    }
  }
  pop_size = eread(EEPROM_OFFSET+3);
  generation = (eread(EEPROM_OFFSET+4)<<8)|eread(EEPROM_OFFSET+5);
  for (i=0; i<pop_size; i++) {
    population[i] = eread(EEPROM_OFFSET+6+i);
  }
  return 1;
}

uint8_t TinyGA::save(void) {
  uint8_t i;
  if ((EEPROM_OFFSET+6+pop_size) > EEPROM_SIZE) {
    return 0; // Population to big for offset.
  }
  for (i=0; i<3; i++) {
    ewrite(EEPROM_OFFSET+i, TINYGA_KEY);
  }
  ewrite(EEPROM_OFFSET+3, pop_size);
  ewrite(EEPROM_OFFSET+4, generation>>8);
  ewrite(EEPROM_OFFSET+5, generation&0xff);
  for (i=0; i<pop_size; i++) {
    ewrite(EEPROM_OFFSET+6+i, population[i]);
  }
  return 1;
}

void TinyGA::print(void) {
  Serial.print("Generation ");
  Serial.print(generation, DEC);
  Serial.println(":");
  Serial.print("Population size: ");
  Serial.println(pop_size, DEC);
  for (int i=0; i<pop_size; i++) {
    Serial.print(population[i], DEC);
    Serial.print(", ");
  } 
  Serial.println();
}

uint8_t TinyGA::run(uint8_t n_generations) {
  uint8_t gen, ind;
  for (gen=0; gen<n_generations; gen++) {
    for (ind=0; ind<pop_size; ind++){
      // Calculate fitness:
      fitnesses[ind] = fitness(population[ind]);
      // If fitness=255 we've found correct answer, return it:
      if (fitnesses[ind] == 255) return population[ind];
    }
    evolve();
  }
  return 0;
}

void TinyGA::reset(void) {
  uint8_t i;
  init(pop_size); // Use current population size
  for (i=0; i<3; i++) {
    if (eread(EEPROM_OFFSET) != TINYGA_KEY) return;
  }
  ewrite(EEPROM_OFFSET, 0x00);
}

void TinyGA::reset(uint8_t new_population_size) {
  init(new_population_size);
  for (i=0; i<3; i++) {
    if (eread(EEPROM_OFFSET) != TINYGA_KEY) return;
  }
  ewrite(EEPROM_OFFSET, 0x00);
}

void TinyGA::evolve(void) {
  uint8_t i, j, max, parent1, parent2;
  uint8_t new_pop[pop_size];
  int avg;
  max = 0;
  avg = 0;
  // First calculate average and max fitness:
  for (i=0; i<pop_size; i++) {
    avg += fitnesses[i];
    if (fitnesses[i] > max) max = fitnesses[i];
  }
  avg /= pop_size;
  if (avg == max) {
    // All fitnesses are the same, halve average or we'll ger stuck:
    avg>>=1;
  } 
  j = 0;
  // Better half of population survives:
  for (i=0; i<pop_size; i++) {
    if (fitnesses[i] >= avg) {
      new_pop[j++] = population[i];
    }
  }
  if (j == pop_size) {
    // All individuals have been selected, only use half.
    j>>=1;
  }
  i=j;
  while (i < pop_size) {
    parent1 = parent2 = random(j);
    // Don't want the same index:
    while (parent2 == parent1) parent2 = random(j);
    new_pop[i++] = crossover(new_pop[parent1], new_pop[parent2]);
  }
  for (i=0; i<pop_size; i++) {
    population[i] = new_pop[i];
  }
  generation++;  
}

uint8_t TinyGA::crossover(uint8_t parent1, uint8_t parent2) {
  // Make random crossover mask: 
  uint8_t mask = (0xff<<random(1, 7)); 
  // Make child:
  uint8_t child = (parent1 & mask)|(parent2 & ~mask);
  if (!random(P_MUTATION)) {
    // This could be anywhere from no change to a full toggle:
    child ^= (random(256));
  }
  return child;
}

uint8_t TinyGA::eread(int address) {
  /* Same as Arduino's EEPROM.read() */
  return eeprom_read_byte((unsigned char *) address);
}

void TinyGA::ewrite(int address, uint8_t value) {
  /* Same as Arduino's EEPROM.write() */
  eeprom_write_byte((unsigned char *) address, value);
}
