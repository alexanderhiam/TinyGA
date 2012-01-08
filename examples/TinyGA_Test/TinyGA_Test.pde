/*
TinyGA_Test.pde - Alexander Hiam - 1/2012
Uses the TinyGA genetic algorithm library to guess numbers.
*/

#include <TinyGA.h>

// Create TinyGA instance, giving it a fitness function:
TinyGA ga = TinyGA(fitness);

uint8_t number_to_guess, char_num; 

void setup() {
  Serial.begin(9600);
  // Must seed random number generator before initializing TinyGA:
  randomSeed(analogRead(A4));
  start();
}

void start() {
  uint8_t i;
  char buffer[10]; // Buffer for serial receiver
  // Initialize with 10 individuals:
  ga.init(10);
  char_num = 0;
  Serial.println("TinyGA Test\nEnter a number between 0-255 and TinyGA will try to guess it");
  Serial.flush();
  while (!Serial.available()); // Wait for input
  i = 0;
  while(Serial.available()) {
    buffer[i++] = Serial.read();
    delay(10); // Give plenty of time for next character to arrive
  }
  buffer[i] = '\0'; // End array
  number_to_guess = atoi(buffer); // Convert to int
  Serial.print("Looking for number: ");
  Serial.println(number_to_guess, DEC); 
}

uint8_t fitness(uint8_t individual) {
  /* The fitenss function; takes a guess and returns a score that is larger 
     the closer to the correct number it is, and 255 if correct. */
  return 255 - abs(number_to_guess-individual);
}

void loop() {
  uint8_t result;
  if (char_num++ >= 60) {
    // Just keep track of where the status character is and move to a new line if >=60
    Serial.println();
    char_num = 1;
  }
  Serial.print("."); 
  // Run for 100 generations:
  result = ga.run(100); 
  if(result) {
    // Run returns 0 unless it got the correct answer, so we know we have it here
    Serial.println();
    Serial.print(result, DEC);
    Serial.println(" - Got it!");
    ga.print(); // Have TinyGA print its current population
    Serial.println();
    start(); // Back to the beginning
  }
}
