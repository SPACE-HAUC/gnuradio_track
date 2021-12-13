#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <queue>
#include <string>
#include "DateTime.hpp"
#include "meb_print.h"

#include "CoordTopocentric.hpp"
#include "SGP4.hpp"
#include "Observer.hpp"

#define SEC *1000000 // nanoseconds to seconds
#define DEG *(180/3.1415926) // radians to degrees
#define GS_LAT 42.655583
#define GS_LON -71.325433
#define ELEV 0.061 // Lowell ASL + Olney Height; Kilometers for some reason.
#define MIN_ELEV 10.0 // degrees
#define ELEV_ADJ 0 // degrees adjustment +-
#define AZIM_ADJ -34 // degrees adjustment +-

using namespace LSGP4;


volatile sig_atomic_t done = 0;
void sighandler(int sig)
{
    done = 1;
}

char cmdbuf[512];
char fname[128];

std::queue<std::string> files;
pthread_mutex_t queue = PTHREAD_MUTEX_INITIALIZER;

void *compress_cleanup(void *in)
{
    while (!done)
    {
        char cmd[512];
        std::string fn = "";
        pthread_mutex_lock(&queue);
        if (!files.empty())
            fn = files.front();
        pthread_mutex_unlock(&queue);
        if (fn != "")
        {
            snprintf(cmd, sizeof(cmd), "7za a -mm=BZip2 %s.bzip2 %s.bin", fn.c_str(), fn.c_str());
            system(cmd);
            snprintf(cmd, sizeof(cmd), "rm -f %s.bin", fn.c_str());
            system(cmd);
            pthread_mutex_lock(&queue);
            files.pop();
            pthread_mutex_unlock(&queue);
        }
        sleep(1);
    }
    return NULL;
}

void *datacollect(void *in)
{
    system(cmdbuf);
    pthread_mutex_lock(&queue);
    files.push(std::string(fname));
    pthread_mutex_unlock(&queue);
    return NULL;
}

static char *get_time_now_raw()
{
    static __thread char buf[128];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buf, sizeof(buf), "%02d%02d%02d",
             tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

static char *get_datetime_now_raw()
{
    static __thread char buf[128];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buf, sizeof(buf), "%04d%02d%2d_%02d%02d%02d",
             tm.tm_year + 1900, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Invocation: %s <File containing TLE Set>\n\n", argv[0]);
        return 0;
    }
    // read TLEs
    auto objs = ReadTleFromFile(argv[1]);
    bprintlf(BLUE_FG "Read %d TLEs", objs.size());
    if (objs.size() < 1)
    {
        dbprintlf(FATAL "Did not read any valid TLEs, exiting");
        return 0;
    }
    // set up signal handler
    signal(SIGINT, sighandler);
    // set up targets
    std::vector<SGP4> targets(objs.size(), objs[0]); // no default constructor exists, so create vector with all same TLEs
    for (int i = 0; i < objs.size(); i++)
    {
        targets[i].SetTle(Tle(objs[i])); // set individual TLEs
    }
    // set up observer
    Observer *dish = new Observer(GS_LAT, GS_LON, ELEV);
    // get into main loop
    bool pending_az = false;
    bool pending_el = false;
    bool pending_any = false;

    bool sat_viewable = false;

    double cmd_az = 0;
    double cmd_el = M_PI_2;

    int sleep_timer = 0;
    int sleep_timer_max = 0;

    int pass_length = 0;
    pthread_t jobthread = 0;
    pthread_t compressthread = 0;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&compressthread, NULL, &compress_cleanup, NULL))
    {
        errprintlf(FATAL "Could not create compress thread");
    }
    SGP4 *current_target = nullptr;

    // Set up for network updating TLEs.
    const char *url = "https://celestrak.com/NORAD/elements/stations.txt";
    bool initial_load = true;
    DateTime launch_time;
    launch_time = launch_time.Now();

    while (!done) // main loop
    {
        // Step 1: Execute command
        sleep(1);
        // should we be sleeping?
        if (sleep_timer)
        {
            if (sleep_timer > sleep_timer_max) // update max
                sleep_timer_max = sleep_timer;

            if (sleep_timer == 21 && current_target != nullptr) // 21 second mark, now we prepare command
            {
                snprintf(fname, sizeof(fname), "%s_%d_%d", get_datetime_now_raw(), pass_length, current_target->GetTle().NoradNumber());
                snprintf(cmdbuf, sizeof(cmdbuf), "/usr/bin/python3 rtl_sdr_iqcapture_250k.py %d %s.bin", pass_length + 40, fname);
            }
            if (sleep_timer == 20 && current_target != nullptr) // 20 seconds mark, now we command
            {
                if (jobthread != 0)
                    pthread_join(jobthread, NULL);
                if (pthread_create(&jobthread, &attr, &datacollect, NULL))
                    errprintlf("Error creating data collect thread");
            }
            sleep_timer--;
            tprintlf(BLUE_FG "Will be sleeping for %d more seconds...", sleep_timer);
            continue;
        }
        else
        {
            sleep_timer_max = 0;
        }
        // Step 1: Get time now
        DateTime tnow = DateTime::Now(true);
        // for each object figure out current position
        CoordTopocentric current_pos;
        for (int i = 0; i < targets.size(); i++)
        {
            SGP4 *target = &targets[i];
            Eci pos_now = target->FindPosition(tnow);
            current_pos = dish->GetLookAngle(pos_now);
            CoordGeodetic current_lla = pos_now.ToGeodetic();
            tprintlf("Obj %d: %.2f AZ, %.2f EL | %.2f LA, %.2f LN", target->GetTle().NoradNumber(), current_pos.azimuth DEG, current_pos.elevation DEG, current_lla.latitude DEG, current_lla.longitude DEG);
            if (current_pos.elevation DEG > MIN_ELEV && current_target == nullptr) // we are already in a pass, scramble!
            {
                current_target = target;
                break;
            }
        }
        // Step 2: Figure out which object we will see in the next 4 minutes if we are not already in a pass
        DateTime tnext = tnow;
#define LOOKAHEAD_MIN 1
#define LOOKAHEAD_MAX 2
        tnext = tnext.AddMinutes(LOOKAHEAD_MAX); // 4 minutes lookahead

        for (int j = 0; j < targets.size() && current_target == nullptr; j++)
        {
            SGP4 *target = &targets[j];
            if (launch_time.AddDays(1) < tnow || initial_load)
            {
                objs[j].UpdateFromNetwork(url);
                target->SetTle(objs[j]);
                launch_time = tnow;
            }
            for (int i = 0; i < (LOOKAHEAD_MAX - LOOKAHEAD_MIN) * 60; i++)
            {
                Eci eci_ahd = target->FindPosition(tnext);
                CoordTopocentric pos_ahd = dish->GetLookAngle(eci_ahd);
                if (i == 0)
                    tprintlf(GREEN_BG RED_FG "Lookahead %d: %.2f AZ %.2f EL", target->GetTle().NoradNumber(), pos_ahd.azimuth DEG, pos_ahd.elevation DEG);
                int ahd_el = pos_ahd.elevation DEG;
                if (ahd_el < (int)MIN_ELEV) // still not in view 4 minutes ahead, don't care
                {
                    break;
                }
                if (ahd_el > (int)MIN_ELEV) // already up, find where it is at proper elevation
                {
                    tnext = tnext.AddSeconds(-1);
                }
                else // right point
                {
                    current_target = target;
                    cmd_az = pos_ahd.azimuth DEG;
                    cmd_el = pos_ahd.elevation DEG;
                    pending_az = true;
                    pending_el = true;
                    sleep_timer = LOOKAHEAD_MAX * 60 - i; // lookahead left
                    // check when the pass is over
                    pass_length = 0;
                    for (int el = ahd_el; el >= MIN_ELEV;)
                    {
                        pass_length++;
                        tnext = tnext.AddSeconds(1);
                        eci_ahd = target->FindPosition(tnext);
                        pos_ahd = dish->GetLookAngle(eci_ahd);
                        el = pos_ahd.elevation DEG;
                    }
                    // here we know the pass length
                    break; // break inner for loop
                }
            }
            if (current_target != nullptr) // target has been assigned
                break;
        }
        if (initial_load)
            initial_load = false;

        // Step 3: Are we in a pass?
        if (current_target != nullptr)
        {
            Eci pos_now = current_target->FindPosition(tnow);
            current_pos = dish->GetLookAngle(pos_now);
        }
        else
        {
            continue;
        }
        if (current_pos.elevation DEG > MIN_ELEV)
        {
            if (!sat_viewable) // satellite just became visible
            {
            }
            sat_viewable = true;
            if (fabs(cmd_az DEG - current_pos.azimuth DEG) > 1) // azimuth has changed
            {
                cmd_az = current_pos.azimuth DEG;
                pending_az = true;
            }
            if (fabs(cmd_el DEG - current_pos.elevation DEG) > 1)
            {
                cmd_el = current_pos.elevation DEG;
                pending_el = true;
            }
            continue;
        }
        // Step 4: Were we in a pass?
        if (sat_viewable) // we are here, but sat_viewable is on. Meaning we just got out of a pass
        {
            sat_viewable = false;
            cmd_az = -AZIM_ADJ;
            cmd_el = 90;
            pending_az = true;
            pending_el = true;
            sleep_timer = 120; // 120 seconds
            current_target = nullptr;
        }
        sat_viewable = false;
    }
    // clean up
    pthread_attr_destroy(&attr);
    pthread_join(compressthread, NULL);
    delete dish;
    return 0;
}