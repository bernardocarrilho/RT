#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "lista_ligada.h"

////////////////////////
///* CONFIGURATIONS *///
////////////////////////

// DEBUG=1 for debug mode
#define DEBUG 1

#define CALLS_PER_HOUR 80
#define GP_CALL_OPERATORS 1
#define AS_CALL_OPERATORS 1



///////////////////
///* CONSTANTS *///
///////////////////

#define RAND_MAX 2147483647
#define ZERO 0.0

#define LAMBDA 60.0*60*CALLS_PER_HOUR
#define N_GP GP_CALL_OPERATORS
#define N_AS AS_CALL_OPERATORS

// Event Types
#define ARRIVAL 0
#define DEPARTURE 1

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



///////////////////
///* VARIABLES *///
///////////////////

time_t seed;
int seedAdjust;



///////////////////
///* FUNCTIONS *///
///////////////////

void initializeRandomSeed();
void generateRandomSeed();
int determineCallType();
double generateCallDuration(int callType);
double generateNextArrival();



//////////////
///* MAIN *///
//////////////

int main() {

    /// INITIALIZATION ///
    
    lista * event_list = NULL;
    lista * gp_queue = NULL;
    lista * as_queue = NULL;
    
    double callDuration = ZERO;
    int gp_busy = ZERO;
    int as_busy = ZERO;
    //int lGP = ZERO;
    //int lAS = ZERO;
    
    initializeRandomSeed();


    /// PROCESSING ///
    
    event_list = adicionar(event_list, ARRIVAL, ZERO);
    
    if (event_list->tipo == ARRIVAL) {
    
        callDuration = generateNextArrival();
        event_list = adicionar(event_list, ARRIVAL, event_list->tempo + callDuration);
    
        if (gp_busy) {
            gp_queue = adicionar(gp_queue, ARRIVAL, event_list->tempo);
            event_list = remover(event_list);
        }
        
        else {
        
            if (determineCallType() == GP) {
            
                callDuration = generateCallDuration(GP_E);
                event_list = adicionar(event_list, DEPARTURE, event_list->tempo + callDuration);
                
                event_list = remover(event_list);
            }
            
            else {
                
                if (as_busy) {
                    callDuration = generateCallDuration(AS_G);
                    as_queue = adicionar(as_queue, ARRIVAL, event_list->tempo + callDuration);
                    
                    event_list = remover(event_list);
                }
                
                else {
                    callDuration = generateCallDuration(AS_G);
                    callDuration += generateCallDuration(AS_E);
                    event_list = adicionar(event_list, DEPARTURE, event_list->tempo + callDuration);
                }
            }
        }
    }
    
    // Ponto 3.)

    return 0;
}



///////////////////
///* FUNCTIONS *///
///////////////////

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


// Generates Time Until Next Arrival
double generateNextArrival() {

    double u,c;

    generateRandomSeed();
    u = (double) ( random()+1 ) / RAND_MAX;
    if (DEBUG) { printf("DEBUG: u=%f\n", u); }
    
    c = -( 1/LAMBDA ) * log(u);
    if (DEBUG) { printf("DEBUG: c=%f\n", c); }
    
    return c;
}

