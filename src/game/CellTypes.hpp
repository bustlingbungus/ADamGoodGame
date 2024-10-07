#pragma once

// define macros to represent cell types

/* BYTE DECOMPOSITION:

byte 1:
    1-8 = cell info (wall type, etc)

byte 2:
    9-12 = building hp (0-15)

    13: water by default?
    14: barrier?
    15: occupied?
    16: indestructible by player? 

byte 3:
    17: is dried?
    18-21: cell max health
    22-24:...

byte 4:
    25: can be destroyed by enemies
    26: has timer?
    27-32: timer (0-63)
*/
#define EMPTY           0b00000000000000000000000000000000
#define WATER           0b00000000000000000001000000000000
#define BARRIER         0b00000000000000000010000000000000
#define OCCUPIED        0b00000000000000000100000000000000
#define INDESTRUBTIBLE  0b00000000000000001000000000000000
#define IS_DRIED        0b00000000000000010000000000000000
#define CAN_DIE         0b00000001000000000000000000000000
#define HAS_TIMER       0b00000010000000000000000000000000

// macros to easily access certain regions of data
#define CELL_ID         0b00000000000000000000000011111111
#define TIMER           0b11111100000000000000000000000000
#define HEALTH          0b00000000000000000000111100000000
#define MAX_HEALTH      0b00000000000111100000000000000000

#define LOG             0b00000001000100000110100000000001 // 8  health
#define DAM             0b00000001000100000100100000000010 // 8  health
#define TREE            0b00000001000110000110110000000011 // 12 health
#define STUMP           0b00000001000100000110100000000100 // 8  health
#define SAPLING         0b00000011000001000110001000000101 // 2  health
#define BORDER          0b00000000000000001110000000000110 // not destructible
#define CLOSED_DOOR     0b00000001000011000110011000000111 // 6  health
#define OPEN_DOOR       0b00000001000011000100011000001000 // 6  health
#define BOULDER         0b00000001000111100110111100001001 // 15 health
#define BERRY_BUSH      0b00000000000000000110000000001010 // not destructible
#define EMPTY_BUSH      0b00000010000000001110000000001011 // not destructible