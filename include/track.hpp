/**
 * @file track.hpp
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief 
 * @version See Git tags for version information.
 * @date 2021.08.12
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef TRACK_HPP
#define TRACK_HPP

#include "CoordTopocentric.h"
#include "SGP4.h"
#include "Observer.h"

#define SEC *1000000 // nanoseconds to seconds
#define DEG *(180/3.1415926) // radians to degrees
#define GS_LAT 42.655583
#define GS_LON -71.325433
#define ELEV 0.061 // Lowell ASL + Olney Height; Kilometers for some reason.
#define MIN_ELEV 10.0 // degrees
#define ELEV_ADJ 0 // degrees adjustment +-
#define AZIM_ADJ -34 // degrees adjustment +-

/**
 * @brief Two line element of the ISS.
 * 
 * TLE Updated: 	8/13/2021 3:28:02 AM UTC
 * TLE Epoch: 	8/13/2021 4:25:00 AM UTC
 * https://live.ariss.org/iss.txt
 *
 */

// REAL TLE
//const char TLE[2][70] = {"1 49277U 98067SW  21291.58382988  .00019053  00000-0  34470-3 0  9994",
//                         "2 49277  51.6436  92.5376 0006211 144.3626 215.7631 15.49679319  1015"};

// TLE Object SX [https://www.n2yo.com/satellite/?s=49278]
const char TLE[2][70] = {"1 49278U 98067SX  21323.34441057  .00027466  00000-0  44695-3 0  9995",
                          "2 49278  51.6405 294.9097 0002575 252.7789 107.2919 15.52448182  5961"};

/**
 * @brief Opens a serial connection.
 * 
 * @param devname Name of the device.
 * @return int File descriptor on success, negative on failure.
 */
int open_connection(char *devname);

/**
 * @brief 
 * 
 * @param connection 
 * @param azimuth 
 * @return int 
 */
int aim_azimuth(int connection, double azimuth);

/**
 * @brief 
 * 
 * @param connection 
 * @param elevation 
 * @return int 
 */
int aim_elevation(int connection, double elevation);

/**
 * @brief Finds the topocentric coordinates of the next targetrise.
 * 
 * @param model 
 * @param dish 
 * @return CoordTopocentric 
 */
CoordTopocentric find_next_targetrise(SGP4 *model, Observer *dish);

#endif // TRACK_HPP
