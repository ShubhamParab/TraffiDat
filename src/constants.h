#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>

// message type
const static qint64 INFO = 0;
const static qint64 VERBOSE = 1;
const static qint64 WARNING = 2;
const static qint64 ERROR = 3;

// treeView levels
const static qint64 ROOT_ITEM = -1;
const static qint64 JUNCTION_ITEM = 0;
const static qint64 TRAFFIC_LIGHT_ITEM = 1;
const static qint64 HEAD_UPSTREAM_ROADS_ITEM = 2;
const static qint64 UPSTREAM_ROAD_ITEM = 3;
const static qint64 HEAD_DOWNSTREAM_ROAD_ITEM = 4;
const static qint64 DOWNSTREAM_ROAD_ITEM = 5;

// traffic signals
const static qint64 RED_SIGNAL = 0;
const static qint64 YELLOW_SIGNAL = 1;
const static qint64 GREEN_SIGNAL = 2;
const static qint64 NO_SIGNAL = 3;

const static qint64 TIMER_YELLOW_SIGNAL = 2;


// map types
const static qint64 ROAD_MAP_TYPE = 1;
const static qint64 JUNCTION_MAP_TYPE = 2;

// graphics
const static qint64 LANE_SIZE = 2;


// play types
const static qint64 START_PLAY = 0;
const static qint64 PAUSE_PLAY = 1;
const static qint64 STOP_PLAY = 2;

// road
const static qint64 UPSTREAM = 1;
const static qint64 DOWNSTREAM = 2;

#endif // CONSTANTS_H
