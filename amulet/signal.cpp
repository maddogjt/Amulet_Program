#include "signal.h"
#include <Arduino.h>

#define SIGNALS_MAX_LEN 200
int signals_len=0;
Signal signals[SIGNALS_MAX_LEN];

bool scans_have_same_command( Scan &s1, Scan &s2) {
    return  
    s1._cmd == s2._cmd &&
    s1._p0 == s2._p0 &&
    s1._p1 == s2._p1 &&
    s1._p2 == s2._p2;
}

// Signals that are closer add more strength to the signal
int strength_score_from_rssi( int rssi ) {
    if( rssi < -90 ) {
        return 1;
    } else if( rssi < -80 ) {
        return 2;
    } else if( rssi < -70 ) {
        return 4;
    } else if( rssi < -55 ) {
        return 8;
    } else {
        return 20;
    }
}   

void insert_new_scan( Scan &s ) {
    bool inserted = false;
    for( int i=0; i < signals_len; i++) {
        if( signals[i]._strength == 0 ) {
            signals[i] = Signal(s);
            signals[i]._strength = strength_score_from_rssi(s._rssi);
            inserted = true;
            break;
        }
    }
    if( !inserted && signals_len + 1 < SIGNALS_MAX_LEN) {
        signals[signals_len] = Signal(s);
        signals_len++;
    }
}

void add_scan_data( Scan &s )
{
    int strengthen_delta = strength_score_from_rssi(s._rssi);
    bool found = false;
    for( int i=0; i< signals_len; i++) {
        if( scans_have_same_command(signals[i]._scan, s ) )
        {
            signals[i]._strength = max(200, signals[i]._strength + strengthen_delta);
            signals[i]._scan._timestamp = max(signals[i]._scan._timestamp, s._timestamp);
            found = true;
            break;
        }
    }
    if( !found ) {
        insert_new_scan(s);
    }
}

void signal_loop( int step )
{
    weaken_old_signals();
}

void weaken_old_signals( )
{
    static uint32_t too_old = 2000; // if things are older than 2 seconds
    static int weaken_delta = -5;    // then weaken by this amount
    uint32_t t = millis();

    // Iterate the array backwards (so the end marker can easily be decremented)
    for( int i=signals_len-1; i >=0 ; i--) {
        // If the signal is old, weaken it.
        if( t - signals[i]._scan._timestamp > too_old ) {
            signals[i]._strength += weaken_delta;

            // If the signal is too weak, cull it.
            if( signals[i]._strength <= 0 ) {
                signals[i].clear();

                // If this is the last element we are tracking, decrement the end marker
                if( i == signals_len - 1) {
                    signals_len--;
                }
            }
        }
    }
}

Signal* current_top_signal() {
    int current_strength = 0;
    int current_cmd = 0;
    int current_index = -1;
    for( int i=0; i< signals_len; i++) {
        if( ((int)signals[i]._scan._cmd > current_cmd ) ||
            ( signals[i]._strength > current_strength && (int)signals[i]._scan._cmd == current_cmd ))
         {
            current_cmd = (int)signals[i]._scan._cmd;
            current_strength = signals[i]._strength;
            current_index = i;
        }
    }

    if( current_index >= 0 ) {
        return &(signals[current_index]);
    } else {
        return 0;
    }
}