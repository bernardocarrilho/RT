#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "lista_ligada.h"

////////////////////////
///* CONFIGURATIONS *///
////////////////////////

#define DEBUG 0

#define CALLS_PER_HOUR 80
#define GP_CALL_OPERATORS 12
#define AS_CALL_OPERATORS 6
#define GP_QUEUE_LENGTH 1
#define NUMBER_OF_SAMPLES 10000



///////////////////
///* CONSTANTS *///
///////////////////

#define RAND_MAX 2147483647
#define ZERO 0.0

#define FALSE 0
#define TRUE 1

#define LAMBDA CALLS_PER_HOUR/60.0/60
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
#define AS_E_MAX 999//60*5 // No Limit? ///////////////////////////////////
#define AS_E_AVG 60*2.5



///////////////////
///* VARIABLES *///
///////////////////

time_t seed;
int seedAdjust;

double avg;
float avg_count;
double avgDelay;

int gp_busy;
int as_busy;
int lGP;
int lAS;
int gp_filled;
int nGP;



///////////////////
///* FUNCTIONS *///
///////////////////

void initializeRandomSeed();
void generateRandomSeed();
int determineCallType();
double generateCallDuration(int callType);
double generateNextArrival();
void updateAverage(double timeDiff);
void updateVariables();



//////////////
///* MAIN *///
//////////////

int main() {

    /// INITIALIZATION ///
    
    lista * event_list = NULL;
    lista * gp_queue = NULL;
    lista * as_queue = NULL;
    lista * avg_aux = NULL;
    
    int callType = ZERO;
    double callDuration = ZERO;
    gp_busy = ZERO;
    as_busy = ZERO;
    lGP = ZERO;
    lAS = ZERO;
    gp_filled = ZERO;
    nGP = ZERO;
    int sampleNum = NUMBER_OF_SAMPLES;
    avg = ZERO;
    avg_count = ZERO;
    
    double probDelay = ZERO;
    double probLost = ZERO;
    avgDelay = ZERO;
    double asAvg = ZERO;
    int asSampleNum = ZERO;
    
    initializeRandomSeed();


    /// PROCESSING ///
    
    if (DEBUG) { printf("1\n"); }
    callType = determineCallType();
    event_list = adicionar(event_list, callType, ZERO);
    
    if ( callType == ARRIVAL_AS ) { avg_aux = adicionar(avg_aux, callType, ZERO); asSampleNum++; }
    
    while ( event_list || gp_queue || as_queue )
    {
    
        updateVariables();
        
        if ( (event_list->tipo == ARRIVAL_GP) || (event_list->tipo == ARRIVAL_AS) ) {
            if (DEBUG) { printf("2\n"); }

            if ( (--sampleNum) > ZERO ) {
                if (DEBUG) { printf("2.1\n"); }

                callType = determineCallType();
                callDuration = generateNextArrival();
                event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                
                if ( callType == ARRIVAL_AS ) { avg_aux = adicionar(avg_aux, callType, event_list->tempo + callDuration); asSampleNum++; }
            }
            
            if ( gp_queue ) {
                if (DEBUG) { printf("2.2\n"); }

                if ( !gp_filled ) {  
                    if (DEBUG) { printf("2.2.1\n"); }
                
                    callType = event_list->tipo;
                    callDuration = event_list->tempo;
                    gp_queue = adicionar(gp_queue, callType, callDuration);
                    
                    probDelay++;
                    nGP++;
                }
                
                else {
                
                    probLost++;
                    if ( event_list->tipo == ARRIVAL_AS ) { avg_aux = remover(avg_aux); asSampleNum--; }
                }
            }
            
            else {
                if (DEBUG) { printf("2.3\n"); }

                if ( event_list->tipo == ARRIVAL_GP ) {
                    if (DEBUG) { printf("2.3.1\n"); }

                    if ( !gp_busy ) {
                        if (DEBUG) { printf("2.3.1.1\n"); }

                        callType = DEPARTURE_GP;
                        callDuration = generateCallDuration(GP_E);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                        lGP++;
                        updateAverage(ZERO);
                        
                    }
                    
                    else {
                        if (DEBUG) { printf("2.3.1.2\n"); }

                        if ( !gp_filled ) {    
                          
                            callType = event_list->tipo;
                            callDuration = event_list->tempo;
                            gp_queue = adicionar(gp_queue, callType, callDuration);
                            
                            probDelay++;
                            nGP++;
                        }
                        
                        else {
                
                            probLost++;
                            if ( event_list->tipo == ARRIVAL_AS ) { avg_aux = remover(avg_aux); asSampleNum--; }
                        }
                    }
                }
                
                else {
                    if (DEBUG) { printf("2.3.2\n"); }

                    if ( !gp_busy ) {
                        if (DEBUG) { printf("2.3.2.1\n"); }

                        callType = DEPARTURE_FIC;
                        callDuration = generateCallDuration(AS_G);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                        lGP++;
                        updateAverage(ZERO);
                    }
                    
                    else {
                        if (DEBUG) { printf("2.3.2.2\n"); }

                        if ( !gp_filled ) { 
                        
                            callType = event_list->tipo;
                            callDuration = event_list->tempo;
                            gp_queue = adicionar(gp_queue, callType, callDuration);
                            
                            probDelay++;
                            nGP++;
                        }
                        
                        else {
                
                            probLost++;
                            if ( event_list->tipo == ARRIVAL_AS ) { avg_aux = remover(avg_aux); asSampleNum--; }
                        }
                    }
                }
            }
        }
        
        else {
            updateVariables();
            
            if (DEBUG) { printf("3\n"); }

            if ( event_list->tipo == DEPARTURE_GP ) {
                if (DEBUG) { printf("3.1\n"); }

                if ( gp_queue ) {
                    if (DEBUG) { printf("3.1.1\n"); }

                    if ( gp_queue->tipo == ARRIVAL_GP ) {
                        if (DEBUG) { printf("3.1.1.1\n"); }

                        callType = DEPARTURE_GP;
                        callDuration = generateCallDuration(GP_E);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                        updateAverage(event_list->tempo - gp_queue->tempo);
                        
                        gp_queue = remover(gp_queue);
                        nGP--;
                    }
                    
                    else { lGP--; }
                }
                
                else { lGP--; }
            }
            
            else if ( event_list->tipo == DEPARTURE_AS ) {
                if (DEBUG) { printf("3.2\n"); }

                if ( as_queue ) {
                    if (DEBUG) { printf("3.2.1\n"); }

                    callType = DEPARTURE_AS;
                    callDuration = generateCallDuration(AS_E);
                    event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                    
                    asAvg += event_list->tempo - avg_aux->tempo;
                    avg_aux = remover(avg_aux);
                        
                    as_queue = remover(as_queue);
                }
                
                else { lAS--; }
            }
            
            else {
                if (DEBUG) { printf("3.3\n"); }

                if ( gp_queue ) {
                    if (DEBUG) { printf("3.3.1\n"); }

                    if ( gp_queue->tipo == ARRIVAL_AS ) {
                        if (DEBUG) { printf("3.3.1.1\n"); }

                        callType = DEPARTURE_FIC;
                        callDuration = generateCallDuration(AS_G);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                        updateAverage(event_list->tempo - gp_queue->tempo);
                        
                        gp_queue = remover(gp_queue);
                        nGP--;
                    }
                    
                    else { lGP--; }
                }
                
                else { lGP--; }
                
                if ( as_queue ) {
                    if (DEBUG) { printf("3.3.2\n"); }

                    callType = ARRIVAL_AS;
                    callDuration = ZERO;
                    as_queue = adicionar(as_queue, callType, event_list->tempo + callDuration);
                }
                
                else {
                    if (DEBUG) { printf("3.3.3\n"); }
                
                    if ( !as_busy ) {
                        if (DEBUG) { printf("3.3.3.1\n"); }

                        callType = DEPARTURE_AS;
                        callDuration = generateCallDuration(AS_E);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                    
                        asAvg += event_list->tempo - avg_aux->tempo;
                        avg_aux = remover(avg_aux);
                    
                        lAS++;
                    }
                    
                    else {
                        if (DEBUG) { printf("3.3.3.2\n"); }

                        callType = ARRIVAL_AS;
                        callDuration = ZERO;
                        as_queue = adicionar(as_queue, callType, event_list->tempo + callDuration);
                    }
                }
            }
            
            updateVariables();
            
            if ( gp_queue ) {
                if (DEBUG) { printf("3.4.1\n"); }

                if ( gp_queue->tipo == ARRIVAL_GP ) {
                    if (DEBUG) { printf("3.4.1.1\n"); }

                    if ( !gp_busy ) {
                        if (DEBUG) { printf("3.4.1.1.1\n"); }

                        callType = DEPARTURE_GP;
                        callDuration = generateCallDuration(GP_E);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        lGP++;
                        
                        updateAverage(event_list->tempo - gp_queue->tempo);
                        
                        gp_queue = remover(gp_queue);
                        nGP--;
                    }
                }
                
                else {
                    if (DEBUG) { printf("3.4.1.2\n"); }

                    if ( !gp_busy ) {
                        if (DEBUG) { printf("3.4.1.2.1\n"); }

                        callType = DEPARTURE_FIC;
                        callDuration = generateCallDuration(AS_G);
                        event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        lGP++;
                        
                        updateAverage(event_list->tempo - gp_queue->tempo);
                        
                        gp_queue = remover(gp_queue);
                        nGP--;
                    }
                }
            }
            
            updateVariables();
            
            if ( as_queue ) {
                if (DEBUG) { printf("3.4.2\n"); }

                if ( !as_busy ) {
                    if (DEBUG) { printf("3.4.2.1\n"); }

                    callType = DEPARTURE_AS;
                    callDuration = generateCallDuration(AS_E);
                    event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                    lAS++;
                        
                    asAvg += event_list->tempo - avg_aux->tempo;
                    avg_aux = remover(avg_aux);
                        
                    as_queue = remover(as_queue);
                }
            }
        }
        
        
        updateVariables();
        
        
        if ( !gp_busy && gp_queue ) {
            if (DEBUG) { printf("4.1\n"); }

            if ( gp_queue->tipo == ARRIVAL_GP ) {
                if (DEBUG) { printf("4.1.1\n"); }

                callType = DEPARTURE_GP;
                callDuration = generateCallDuration(GP_E);
                event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                
                updateAverage(event_list->tempo - gp_queue->tempo);
                        
                gp_queue = remover(gp_queue);
                nGP--;
            }
            
            else {
                if (DEBUG) { printf("4.1.2\n"); }

                callType = DEPARTURE_FIC;
                callDuration = generateCallDuration(AS_G);
                event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
                updateAverage(event_list->tempo - gp_queue->tempo);
                        
                gp_queue = remover(gp_queue);
                nGP--;
            }
        }
        
        updateVariables();
        
        if ( !as_busy && as_queue ) {
            if (DEBUG) { printf("4.2\n"); }
       
            callType = DEPARTURE_AS;
            callDuration = generateCallDuration(AS_E);
            event_list = adicionar(event_list, callType, event_list->tempo + callDuration);
                        
            asAvg += event_list->tempo - avg_aux->tempo;
            avg_aux = remover(avg_aux);
                        
            as_queue = remover(as_queue);
        }
        
        event_list = remover(event_list);
        
        updateVariables();
    
    
        if (DEBUG && TRUE) {
        
            printf("\n\nEVENT LIST:\n");
            imprimir (event_list);
            
            
            printf("\n\nGP QUEUE:\n");
            imprimir (gp_queue);
            
            
            printf("\n\nAS QUEUE:\n");
            imprimir (as_queue);
            
            
            printf("\n\nAS AVG:\n");
            imprimir (avg_aux);
            
            
            printf("\n\nAVG=%f\n", avg);
            printf("AVG COUNT=%f\n\n", avg_count);
            
            printf("P(Delay) = %f\n", probDelay);
            printf("P(Lost) = %f\n", probLost);
            printf("Delay (Average) = %f\n", avgDelay);
            printf("AS Delay (Average) = %f\n", asAvg);
            
            
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
    
    
    /// STATISTCS ///
    
    probDelay /= NUMBER_OF_SAMPLES;
    probLost /= NUMBER_OF_SAMPLES;
    asAvg /= asSampleNum;
    
    
    /// PRINTING ///
    
    printf("---INPUT---\n\n");
    
    printf("CALLS_PER_HOUR=%d\n", CALLS_PER_HOUR);
    printf("GP_CALL_OPERATORS=%d\n", GP_CALL_OPERATORS);
    printf("AS_CALL_OPERATORS=%d\n", AS_CALL_OPERATORS);
    printf("GP_QUEUE_LENGTH=%d\n", GP_QUEUE_LENGTH);
    printf("NUMBER_OF_SAMPLES=%d\n", NUMBER_OF_SAMPLES);
    printf("\n\n");
    
    
    printf("---OUTPUT---\n\n");
    
    printf("P(Delay) = %f\n", probDelay);
    printf("P(Lost) = %f\n", probLost);
    printf("Delay (Average) = %f\n", avgDelay);
    printf("AS Delay (Average) = %f\n", asAvg);
    printf("\n\n");

    return 0;
}


///////////////////
///* FUNCTIONS *///
///////////////////

// Initializes seed for random()
void initializeRandomSeed() {
    
    seed = time(NULL);
    seedAdjust = ZERO;
    
    if (DEBUG && FALSE) { seed = 1669899191; }
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

    c = -( 1.0/(LAMBDA) ) * log(u);
    if (DEBUG) { printf("DEBUG: c=%f\n", c); }
    
    return c;
}

void updateAverage(double timeDiff) {

    avg_count++;
    avg = avg * ((avg_count-1)/avg_count) + (timeDiff) * (1/avg_count);
    
    if ( timeDiff > ZERO ) { avgDelay += timeDiff; }

    return;
}

void updateVariables() {
    if (lGP >= GP_CALL_OPERATORS) { gp_busy = TRUE; }
    else { gp_busy = FALSE; }

    if (lAS >= AS_CALL_OPERATORS) { as_busy = TRUE; }
    else { as_busy = FALSE; }

    if (nGP >= GP_QUEUE_LENGTH) { gp_filled = TRUE; }
    else { gp_filled = FALSE; }
}

