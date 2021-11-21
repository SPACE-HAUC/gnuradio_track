#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "meb_debug.h"
#include "tleobj.hpp"
#include <iostream>

std::vector<TLEObj> read_tle(const char *fname)
{
    int m_obj = 1;
    int n_obj = 0;

    FILE *fp = fopen(fname, "r"); 
    if (fp == NULL)
    {
        printf(FATAL "Error opening TLE file %s, exiting\n", fname);
        throw std::invalid_argument("Could not access file");
    }

    std::vector<TLEObj> objs(m_obj);
    // start reading TLE file
    char *res;
    do
    {
        char buf[128];
        memset(buf, 0x0, sizeof(buf));
        // 1. Read one line
        res = fgets(buf, sizeof(buf), fp);
        dbprintf(RED_FG "%s", buf);
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) != 69) // L1?
            continue;
        if (buf[0] != '1') // not L1
        {
            dbprintlf(FATAL "Expecting Line 1, %c, read %s", buf[0], buf);
            continue;
        }
        if (m_obj <= n_obj)
        {
            m_obj++;
            objs.resize(m_obj);
        }
        strncpy(objs[n_obj].l1, buf, 69); // L1 is found, read it in
        dbprintlf(BLUE_FG "Obj %d, L1: %s", n_obj, objs[n_obj].l1);
        // 2. Read line 2
        res = fgets(buf, sizeof(buf), fp);
        dbprintf(RED_FG "%s", buf);
        buf[strcspn(buf, "\n")] = '\0';
        if (strlen(buf) != 69) // L2?
            continue;
        if (buf[0] != '2') // not L2
        {
            dbprintlf(FATAL "Expecting Line 2, read %s", buf);
            continue;
        }
        strncpy(objs[n_obj].l2, buf, 69); // L2 is found, read it in
        objs[n_obj].l2[69] = '\0';
        dbprintlf(BLUE_FG "Obj %d, L2: %s", n_obj, objs[n_obj].l2);
        n_obj++;
    } while (res != NULL);
    fclose(fp);
    return objs;
}

#ifdef READ_TLE_UNIT_TEST
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invocation: %s <File containing TLE Set>\n\n", argv[0]);
        return 0;
    }

    FILE *fp = fopen(argv[1], "r"); 
    if (fp == NULL)
    {
        printf(FATAL "Error opening TLE file %s, exiting\n", argv[1]);
        return 0;
    }

    std::vector<TLEObj> tles = read_tle(argv[1]);
    printf("Read: %d TLEs\n", tles.size());
    for (int i = 0; i < tles.size(); i++)
    {
        std::cout << "TLE " << i << " of " << tles.size() << ":" << std::endl;
        std::cout << tles[i].l1 << std::endl;
        std::cout << tles[i].l2 << std::endl;
        std::cout << std::endl;
    }
    return 0;
}
#endif