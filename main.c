#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "lista_ligada.h"

////////////////////////
///* CONFIGURATIONS *///
////////////////////////

#define DEBUG 1

#define CALLS_PER_HOUR 80
#define GP_CALL_OPERATORS 1
#define AS_CALL_OPERATORS 1
#define NUMBER_OF_SAMPLES 3



///////////////////
///* CONSTANTS *///
///////////////////

#define RAND_MAX 2147483647
#define ZERO 0.0

#define FALSE 0
#define TRUE 1

#define LAMBDA 60.0*60*CALLS_PER_HOUR
#define N_GP GP_CALL_OPERATORS
#define N_AS AS_CALL_OPERATORS

// Event Types
#define ARRIVAL 0
#define DEPARTURE 1

#define ARRIVAL_GP 2
#define ARRIVAL_AS 3
#define DEPARTURE_GP 4
#define DEPARTURE_AS 5
#define DEPARTURE_FIC 6

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
    
    int callType = ZERO;
    double callDuration = ZERO;
    int gp_busy = ZERO;
    int as_busy = ZERO;
    int lGP = ZERO;
    int lAS = ZERO;
    int sampleNum = NUMBER_OF_SAMPLES;
    
    initializeRandomSeed();


    /// PROCESSING ///
    
    callType = determineCallType();
    event_list = adicionar(event_list, callType, ZERO);
    
    while ( event_list || gp_queue || as_queue )
    {
        
        if ( (event_list->tipo == ARRIVAL_GP) || (event_list->tipo == ARRIVAL_AS) ) {
printf("1\n");        
            if ( (--sampleNum) > ZERO ) {
printf("2\n");            
                callType = determineCallType();
                callDuration = generateNextArrival();
                event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
            }
            
            if ( gp_queue ) {
printf("3\n");            
                callType = event_list->tipo;
                callDuration = event_list->tempo;
                gp_queue = adicionar(gp_queue, callType, callDuration);
            }
            
            else {
printf("4\n");            
                if ( event_list->tipo == ARRIVAL_GP ) {
printf("5\n");                
                    if ( !gp_busy ) {
printf("6\n");                    
                        callType = DEPARTURE_GP;
                        callDuration = generateCallDuration(GP_E);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        lGP++;
                    }
                    
                    else {
printf("7\n");                    
                        callType = event_list->tipo;
                        callDuration = event_list->tempo;
                        gp_queue = adicionar(gp_queue, callType, callDuration);
                    }
                }
                
                else {
printf("8\n");                
                    if ( !gp_busy ) {
printf("9\n");                    
                        callType = DEPARTURE_FIC;
                        callDuration = generateCallDuration(AS_G);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        lGP++;
                    }
                    
                    else {
printf("10\n");                    
                        callType = event_list->tipo;
                        callDuration = event_list->tempo;
                        gp_queue = adicionar(gp_queue, callType, callDuration);
                    }
                }
            }
        }
        
        else {
printf("11\n");        
            if ( event_list->tipo == DEPARTURE_GP ) {
printf("12\n");
                if ( gp_queue ) {
printf("13\n");                
                    if ( gp_queue->tipo == ARRIVAL_GP ) {
printf("14\n");                    
                        callType = DEPARTURE_GP;
                        callDuration = generateCallDuration(GP_E);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                        gp_queue = remover(gp_queue);
                    }
                    
                    else { lGP--; }
                }
                
                else { lGP--; }
            }
            
            else if ( event_list->tipo == DEPARTURE_AS ) {
printf("15\n");
                if ( as_queue ) {
printf("16\n");
                    callType = DEPARTURE_AS;
                    callDuration = generateCallDuration(AS_E);
                    event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                    as_queue = remover(as_queue);
                }
                
                else { lAS--; }
            }
            
            else {
printf("17\n");
                if ( gp_queue ) {
printf("18\n");                
                    if ( gp_queue->tipo == ARRIVAL_AS ) {
printf("19\n");                    
                        callType = DEPARTURE_FIC;
                        callDuration = generateCallDuration(AS_G);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                        gp_queue = remover(gp_queue);
                    }
                    
                    else { lGP--; }
                }
                
                else { lGP--; }
            }
            
            if ( gp_queue ) {
printf("20\n");
                if ( gp_queue->tipo == ARRIVAL_GP ) {
printf("21\n");
                    if ( !gp_busy ) {
printf("22\n");
                        callType = DEPARTURE_GP;
                        callDuration = generateCallDuration(GP_E);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        lGP++;
                        
                        gp_queue = remover(gp_queue);
                    }
                }
                
                else {
printf("23\n");
                    if ( !gp_busy ) {
printf("24\n");
                        callType = DEPARTURE_FIC;
                        callDuration = generateCallDuration(AS_G);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        lGP++;
                        
                        gp_queue = remover(gp_queue);
                    }
                }
            }
            
            if ( as_queue ) {
printf("25\n");
                if ( !as_busy ) {
printf("26\n");
                    callType = DEPARTURE_AS;
                    callDuration = generateCallDuration(AS_E);
                    event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                    lAS++;
                        
                    as_queue = remover(as_queue);
                }
            }
        }
        
        if ( !gp_busy && gp_queue ) {
printf("27\n");        
            if ( gp_queue->tipo == ARRIVAL_GP ) {
printf("28\n");                   
                callType = DEPARTURE_GP;
                callDuration = generateCallDuration(GP_E);
                event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                gp_queue = remover(gp_queue);
            }
            
            else {
printf("29\n");                   
                callType = DEPARTURE_FIC;
                callDuration = generateCallDuration(AS_G);
                event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                gp_queue = remover(gp_queue);
            }
        }
        
        if ( !as_busy && as_queue ) {
printf("30\n");        
            callType = DEPARTURE_AS;
            callDuration = generateCallDuration(AS_E);
            event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
            as_queue = remover(as_queue);
        }
        
        event_list = remover(event_list);
        
        
        if (lGP >= GP_CALL_OPERATORS) { gp_busy = TRUE; }
        else { gp_busy = FALSE; }
        
        if (lAS >= AS_CALL_OPERATORS) { as_busy = TRUE; }
        else { as_busy = FALSE; }
    
    
        if (DEBUG && TRUE) {
        
            printf("\n\nEVENT LIST:\n");
            imprimir (event_list);
            
            
            printf("\n\nGP QUEUE:\n");
            imprimir (gp_queue);
            
            
            printf("\n\nAS QUEUE:\n");
            imprimir (as_queue);
            
            printf("\n\n--------------------------------\n\n");
        }
    }
    
    if (DEBUG && FALSE) {
        
        printf("\n\nEVENT LIST:\n");
        imprimir (event_list);
        
        
        printf("\n\nGP QUEUE:\n");
        imprimir (gp_queue);
        
        
        printf("\n\nAS QUEUE:\n");
        imprimir (as_queue);
        
        printf("\n\n");
    }

    return 0;
}
// Falta filas finita e infinita
// Check busy variables before checking if busy 


///////////////////
///* FUNCTIONS *///
///////////////////

// Initializes seed for random()
void initializeRandomSeed() {
    
    seed = time(NULL);
    seedAdjust = ZERO;
    
    if (DEBUG && TRUE) { seed = 1669899191; }
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
    
    if (u >= AS_REQUEST_PERCENTAGE) { return ARRIVAL_AS; }
    else { return ARRIVAL_GP; }
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

