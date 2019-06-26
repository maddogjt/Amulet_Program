#include <Arduino.h>

// Order matters since we use this to prioritize. Higher index means higher priority
typedef enum {
    command_noop,
    command_peer,
    command_beacon,
    command_flashmob,
} amulet_command_t;

struct Scan {
    Scan() : _rssi(-128) {}
    Scan( Scan &s) : _rssi(s._rssi), _cmd(s._cmd), _p0(s._p0), _p1(s._p1), _p2(s._p2), _timestamp(s._timestamp) {}
    Scan( int rssi, amulet_command_t cmd, int p0, int p1, int p2, uint32_t ts) : _rssi(rssi), _cmd(cmd), _p0(p0), _p1(p1), _p2(p2), _timestamp(ts) {}
    int _rssi;
    amulet_command_t _cmd;
    int _p0;
    int _p1;
    int _p2;
    uint32_t _timestamp;
};

class Signal
{
    public:
        Signal() {}
        Signal( Scan scan) : _scan(scan), _strength(10) {}
        inline void clear() { _strength = 0; _scan._rssi=-128; _scan._cmd = command_noop; _scan._timestamp=0; }
    public:
        Scan _scan;
        int _strength;
};

void signal_loop( int step );
void add_scan_data( Scan &s );
void weaken_old_signals();
Signal* current_top_signal();

