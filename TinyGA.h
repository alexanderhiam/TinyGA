/*
 TinyGA - TinyGA.h - v0.1.5
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

#ifndef _TINY_GA_H
#define _TINY_GA_H

#include <inttypes.h>

#define MAX_POPULATION 100 // < EEPROM_SIZE-8
                           // Used for population and fitness buffer sizes

#define P_MUTATION 10 
// Probability of mutation of each offspring is 1/P_MUTATION

#ifndef EEPROM_OFFSET
#define EEPROM_OFFSET 0
#endif

#define TINYGA_KEY 0x42
#define EEPROM_SIZE 1024 // For ATmega328
//#define EEPROM_SIZE 512 // For ATmega168


class TinyGA {
 public:
  TinyGA(uint8_t (*fitness_function)(uint8_t));
  void init(uint8_t population_size);
  uint8_t load(void);
  uint8_t save(void);
  void print(void);
  void reset(void);
  void reset(uint8_t new_population_size);
  uint8_t run(uint8_t n_generations);
  int generation;
  uint8_t pop_size;

 private:
  void evolve(void);
  uint8_t crossover(uint8_t parent1, uint8_t parent2);
  uint8_t eread(int address);
  void ewrite(int address, uint8_t value);
  uint8_t (*fitness)(uint8_t);
  uint8_t population[MAX_POPULATION];
  uint8_t fitnesses[MAX_POPULATION];
};

#endif
