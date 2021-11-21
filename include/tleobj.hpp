/**
 * @file tleobj.hpp
 * @author Sunip K. Mukherjee (sunipkmukherjee@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef TLE_OBJ_HPP
#define TLE_OBJ_HPP

#include <vector>
#include <stdio.h>

typedef struct
{
    char l1[70];
    char l2[70];
} TLEObj;

/**
 * @brief Read TLEs from a text file. The TLE lines must not have any character other than a newline after them for this to work. There must not be no other lines of 70 character length.
 * 
 * @param fname TLE file name
 * @return std::vector<TLEObj> Array of TLE objects
 */
std::vector<TLEObj> read_tle(const char *fname);
#endif // TLE_OBJ_HPP