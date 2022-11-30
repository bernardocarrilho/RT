#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "lista_ligada.h"


// DEBUG=1 for debug mode
#define DEBUG 1

#define RAND_MAX 2147483647
#define ZERO 0

// Event Types
#define CHEGADA 0
#define PARTIDA 1

// Call Types
#define GP 0
#define AS 1

// GP and AS Request Division
#define AS_REQUEST_PERCENTAGE 0.7

// Call Duration Types
#define GP_E 1
#define AS_E 2
#define AS_G 3

// Call Duration Values
#define GP_E_MIN 60*1
#define GP_E_MAX 60*5
#define GP_E_AVG 60*2

#define AS_G_MIN 60*(1/2.0)
#define AS_G_MAX 60*2
#define AS_G_AVG 60*1
#define AS_G_DEV 60*(1/3.0)

#define AS_E_MIN 60*1
#define AS_E_MAX 999 // No Limit? ///////////////////////////////////
#define AS_E_AVG 60*2.5


// Auxiliary Variables
time_t seed;
int seedAdjust;


// Auxiliary Functions
void initializeRandomSeed();
void generateRandomSeed();
int determineCallType();
double generateCallDuration(int callType);



int main() {

    initializeRandomSeed();

    return 0;
}


// Initializes seed for random()
void initializeRandomSeed() {
    
    seed = time(NULL);
    seedAdjust = ZERO;
    
    if (DEBUG) { printf("DEBUG: seed=%ld\n", seed); }
    
    return;
}

// Generates a new seed for random()
void generateRandomSeed() {

    srandom(seed + (seedAdjust++));
    
    if (DEBUG) { printf("DEBUG: newSeed=%ld\n", seed + seedAdjust-1); }
    
    return;
}


// Determines if a call needs only GP or also AS
int determineCallType() {

    generateRandomSeed();
    double u = (double) ( random()+1 ) / RAND_MAX;
    
    if (DEBUG) { printf("DEBUG: callType=%f\n", u); }
    
    if (u >= AS_REQUEST_PERCENTAGE) { return AS; }
    else { return GP; }
}


// Generates durations for calls
double generateCallDuration(int callType) {
    
    double d, u1, u2, theta, r;
    
    if (callType == GP_E) {
        do {
            generateRandomSeed();
            u1 = (double) ( random()+1 ) / RAND_MAX;
            if (DEBUG) { printf("DEBUG: u1=%f\n", u1); }
            
            d = -GP_E_AVG * log(u1);
            if (DEBUG) { printf("DEBUG: d=%f\n", d); }
            
        } while ( (d < GP_E_MIN) || (d > GP_E_MAX) );
        
        return d;
    }
    
    else if (callType == AS_E) {
        do {
            generateRandomSeed();
            u1 = (double) ( random()+1 ) / RAND_MAX;
            if (DEBUG) { printf("DEBUG: u1=%f\n", u1); }
            
            d = -AS_E_AVG * log(u1);
            if (DEBUG) { printf("DEBUG: d=%f\n", d); }
            
        } while ( (d < AS_E_MIN) || (d > AS_E_MAX) );
        
        return d;
    }
    
    else {
        do {
            generateRandomSeed();
            u1 = (double) ( random()+1 ) / RAND_MAX;
            if (DEBUG) { printf("DEBUG: u1=%f\n", u1); }
            
            generateRandomSeed();
            u2 = (double) ( random()+1 ) / RAND_MAX;
            if (DEBUG) { printf("DEBUG: u2=%f\n", u2); }
            
            theta = 2*M_PI*u1;
            r = sqrt(-2*log(u2));
            
            d = r*cos(theta) * AS_G_DEV + AS_G_AVG;
            if (DEBUG) { printf("DEBUG: d=%f\n", d); }
            
        } while ( (d < AS_G_MIN) || (d > AS_G_MAX) );
        
        return d;
    }
}

