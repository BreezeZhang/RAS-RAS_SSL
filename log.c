#include "log.h"
#include <time.h>
#include <sys/timeb.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void logV(const char *msg, ...)
{
    char mss[4] = "";
    char datastr[16] = "";
    char timestr[16] = "";
    char logstr[1024] = "";
    struct tm *now;
    struct timeb    tb;
 
    FILE    *fp;
 
    ftime(&tb);
    now = localtime(&tb.time);
 
    sprintf(datastr, "%04d-%02d-%02d", now->tm_year+1900, now->tm_mon, now->tm_mday);
    sprintf(timestr, "%02d:%02d:%02d", now->tm_hour, now->tm_min, now->tm_sec);
    sprintf(mss, "%03d", tb.millitm);
    va_list argp;
    va_start(argp, msg);
    vsnprintf(logstr, 1024, msg, argp);
    va_end(argp);
 
    if((fp = fopen(LOGFILENAME, "a")) == NULL)
    {
        return;
    }
    fprintf(fp, "%s %s.%s : %s\n", datastr, timestr, mss, logstr);
    if(ftell(fp) > 1024*1024)
    {
        fclose(fp);
        char oldlog[1024]  = "";
        sprintf(oldlog, "%s_%02d_%02d_%02d.log", datastr, now->tm_hour, now->tm_min, now->tm_sec);
        rename(LOGFILENAME, oldlog);
    }
    else
    {
        fclose(fp);
    }
}
