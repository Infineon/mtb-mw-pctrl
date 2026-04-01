/**
 * @file PCTRL log routines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "cy_result.h"
#include "cy_pctrl_log.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************
*                      Macros
******************************************************/

/******************************************************
*                    Constants
******************************************************/

/** Size of the logging buffer */
#ifndef CY_PCTRL_LOGBUF_SIZE
#define CY_PCTRL_LOGBUF_SIZE (1024)
#endif

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

typedef struct
{
    bool                    init;
//    cy_mutex_t              mutex;
    CY_PCTRL_LOG_LEVEL_T    loglevel[CYLF_PCTRL_MAX];
//    uint32_t                start_time;
    char                    logbuf[CY_PCTRL_LOGBUF_SIZE];
    uint16_t                seq_num;
    pctrl_log_output        platform_log;
    platform_get_time       platform_time;
//    void                    *worker_thread;
} cy_pctrl_log_data_t;

/******************************************************
*               Function declarations
******************************************************/

/******************************************************
*               Variables definitions
******************************************************/

static cy_pctrl_log_data_t cy_pctrl_log;

/******************************************************
*               Function definitions
******************************************************/

/*
 * Default implementation to output the log messages.
 * This function is called if the user doesn't define a output logging function.
 */
static int cy_pctrl_log_output(CY_PCTRL_LOG_FACILITY_T facility, CY_PCTRL_LOG_LEVEL_T level,
                               char* logmsg)
{
    printf("[F%d] : [L%d] : %s", facility, level, logmsg);

    return 1;
}


/*
 * Default implementation to get the time.
 * This function is called if the user doesn't provide a callback function to get time.
 */
static cy_rslt_t cy_pctrl_log_get_time(uint32_t* time)
{
//    cy_rtos_get_time(time);
    *time = 0; // avoid compiler error on unused variable
    return CY_RSLT_SUCCESS;
}


cy_rslt_t cy_pctrl_log_init(CY_PCTRL_LOG_LEVEL_T level, pctrl_log_output platform_output,
                            platform_get_time platform_time)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    int i;

    if (cy_pctrl_log.init)
    {
        return CY_RSLT_SUCCESS;
    }

    memset(&cy_pctrl_log, 0x00, sizeof(cy_pctrl_log));
//    cy_rtos_get_time(&cy_pctrl_log.start_time);

    /*
     * Create our mutex.
     */
//    result = cy_rtos_init_mutex(&cy_pctrl_log.mutex);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        return result;
//    }

    /*
     * Set the starting log level.
     */

    if (level >= CY_PCTRL_LOG_MAX)
    {
        level = (CY_PCTRL_LOG_LEVEL_T)(CY_PCTRL_LOG_MAX - 1);
    }

    /* For all facilities */

    for (i = 0; i < CYLF_PCTRL_MAX; i++)
    {
        cy_pctrl_log.loglevel[i] = level;
    }

    /*
     * Set the platform output and time routines.
     */

    if (platform_output != NULL)
    {
        cy_pctrl_log.platform_log  = platform_output;
    }
    else
    {
        cy_pctrl_log.platform_log = cy_pctrl_log_output;
    }

    if (platform_time != NULL)
    {
        cy_pctrl_log.platform_time = platform_time;
    }
    else
    {
        cy_pctrl_log.platform_time = cy_pctrl_log_get_time;
    }

    /*
     * All done.
     */

    cy_pctrl_log.init = true;

    return result;
}


cy_rslt_t cy_pctrl_log_shutdown(void)
{
    if (!cy_pctrl_log.init)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    cy_pctrl_log.init = false;

//    cy_rtos_deinit_mutex(&cy_log.mutex);

    return CY_RSLT_SUCCESS;
}


cy_rslt_t cy_pctrl_log_set_platform_output(pctrl_log_output platform_output_func)
{
    if (!cy_pctrl_log.init)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    cy_pctrl_log.platform_log = platform_output_func;

    return CY_RSLT_SUCCESS;
}


cy_rslt_t cy_pctrl_log_set_platform_time(platform_get_time platform_time)
{
    if (!cy_pctrl_log.init)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    cy_pctrl_log.platform_time = platform_time;

    return CY_RSLT_SUCCESS;
}


cy_rslt_t cy_pctrl_log_set_facility_level(CY_PCTRL_LOG_FACILITY_T facility,
                                          CY_PCTRL_LOG_LEVEL_T level)
{
    if (!cy_pctrl_log.init)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    if (facility >= CYLF_PCTRL_MAX)
    {
        facility = CYLF_PCTRL_DEFAULT;
    }

    if (level >= CY_PCTRL_LOG_MAX)
    {
        level = (CY_PCTRL_LOG_LEVEL_T)(CY_PCTRL_LOG_MAX - 1);
    }
    cy_pctrl_log.loglevel[facility] = level;

    return CY_RSLT_SUCCESS;
}


cy_rslt_t cy_pctrl_log_set_all_levels(CY_PCTRL_LOG_LEVEL_T level)
{
    int i;

    if (!cy_pctrl_log.init)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    if (level >= CY_PCTRL_LOG_MAX)
    {
        level = (CY_PCTRL_LOG_LEVEL_T)(CY_PCTRL_LOG_MAX - 1);
    }

    for (i = 0; i < CYLF_PCTRL_MAX; i++)
    {
        cy_pctrl_log.loglevel[i] = level;
    }

    return CY_RSLT_SUCCESS;
}


CY_PCTRL_LOG_LEVEL_T cy_pctrl_log_get_facility_level(CY_PCTRL_LOG_FACILITY_T facility)
{
    CY_PCTRL_LOG_LEVEL_T local_loglevel = CY_PCTRL_LOG_OFF;

    if (!cy_pctrl_log.init)
    {
        return local_loglevel;
    }

    if (facility >= CYLF_PCTRL_MAX)
    {
        facility = CYLF_PCTRL_DEFAULT;
    }

    local_loglevel = cy_pctrl_log.loglevel[facility];

    return local_loglevel;
}


cy_rslt_t cy_pctrl_log_msg_(CY_PCTRL_LOG_FACILITY_T facility, CY_PCTRL_LOG_LEVEL_T level,
                            const char* fmt, ...)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
//    uint32_t time_from_start;
//    uint32_t cur_time;
//    int hrs, mins, secs, ms;
    va_list args;
    int len;

    if (!cy_pctrl_log.init)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    /* Is logging enabled for the requested level of the requested facility? */
    if (facility >= CYLF_PCTRL_MAX)
    {
        facility = CYLF_PCTRL_DEFAULT;
    }
    if ((cy_pctrl_log.platform_log == NULL) ||
        (cy_pctrl_log.loglevel[facility] == CY_PCTRL_LOG_OFF) ||
        (level > cy_pctrl_log.loglevel[facility]))
    {
        return CY_RSLT_SUCCESS;
    }

    /*
     * Create the time stamp.
     */
//    if (cy_pctrl_log.platform_time != NULL)
//    {
//        result = cy_pctrl_log.platform_time(&time_from_start);
//        if (result != CY_RSLT_SUCCESS)
//        {
//            return result;
//        }
//    }
//    else
//    {
//        cy_rtos_get_time(&cur_time);
//        time_from_start = cur_time - cy_pctrl_log.start_time;
//    }
//
//    ms   = time_from_start % 1000;
//    time_from_start /= 1000;
//
//    secs = time_from_start % 60;
//    time_from_start /= 60;
//
//    mins = time_from_start % 60;
//    hrs  = (time_from_start / 60) % 24;

    /*
     * We use a common buffer for composing the log messages so we need to protect against
     * multiple threads calling us simultaneously.
     */
//    result = cy_rtos_get_mutex(&cy_pctrl_log.mutex, CY_RTOS_NEVER_TIMEOUT);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        return CY_RSLT_TYPE_ERROR;
//    }

    len = snprintf(cy_pctrl_log.logbuf, CY_PCTRL_LOGBUF_SIZE, "%04d " /*%02d:%02d:%02d.%03d */ "", \
                   cy_pctrl_log.seq_num /*, hrs, mins, secs, ms*/);

    va_start(args, fmt);
    len = vsnprintf(&cy_pctrl_log.logbuf[len], CY_PCTRL_LOGBUF_SIZE - len, fmt, args);
    if ((len == -1) || (len >= CY_PCTRL_LOGBUF_SIZE))
    {
        /* The vsnprintf() output was truncated. */
        cy_pctrl_log.logbuf[CY_PCTRL_LOGBUF_SIZE - 1] = '\0';
    }
    va_end(args);

    cy_pctrl_log.platform_log(facility, level, cy_pctrl_log.logbuf);

    /* Increment sequence number for next line*/
    cy_pctrl_log.seq_num++;

//    cy_rtos_set_mutex(&cy_pctrl_log.mutex);

    return result;
}


cy_rslt_t cy_pctrl_log_printf(const char* fmt, ...)
{
    cy_rslt_t result;
    va_list args;

    va_start(args, fmt);
    result = cy_pctrl_log_vprintf(fmt, args);
    va_end(args);

    return result;
}


cy_rslt_t cy_pctrl_log_vprintf(const char* fmt, va_list varg)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    int len;

    if (!cy_pctrl_log.init)
    {
        return CY_RSLT_TYPE_ERROR;
    }

    /*
     * We use a common buffer for composing the log messages so we need to protect against
     * multiple threads calling us simultaneously.
     */
//    Result = cy_rtos_get_mutex(&cy_pctrl_log.mutex, CY_RTOS_NEVER_TIMEOUT);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        return CY_RSLT_TYPE_ERROR;
//    }

    len = vsnprintf(cy_pctrl_log.logbuf, CY_PCTRL_LOGBUF_SIZE, fmt, varg);
    if ((len == -1) || (len >= CY_PCTRL_LOGBUF_SIZE))
    {
        /* The vsnprintf() output was truncated. */
        cy_pctrl_log.logbuf[CY_PCTRL_LOGBUF_SIZE - 1] = '\0';
    }

    cy_pctrl_log.platform_log(CYLF_PCTRL_DEFAULT, CY_PCTRL_LOG_PRINT, cy_pctrl_log.logbuf);

//    cy_rtos_set_mutex(&cy_pctrl_log.mutex);

    return result;
}


#ifdef __cplusplus
}
#endif
