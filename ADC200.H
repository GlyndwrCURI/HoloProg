/****************************************************************************
 *
 * Filename:    adc200.h
 * Copyright:   Pico Technology Limited 1995-2007
 * Author:      Mike Green
 * Description:
 *
 * This header defines the interface to driver routines for the PICO ADC-200
 * range of products.
 *
 * History:
 *  29Nov95 MKG Updated for Windows
 *
 * Revision Info: "file %n date %f revision %v"
 *                ""
 *
 ****************************************************************************/

#ifdef __cplusplus
#define PREF1 extern "C"
#else
#define PREF1
#endif


#if defined(WIN32)
#define PREF2 __declspec(dllexport) __stdcall   // BC5, MS
#define HUGE
#ifndef FAR
#define FAR
#endif

#elif (defined(WINDOWS) || defined(PM))
#define PREF2 _export pascal
#define HUGE    huge
#ifndef FAR
#define FAR     far
#endif

#elif defined (DOS)
#define PREF2 far pascal
#define FAR far
// DOS can't do HUGE, because it's compiler-dependent
#define HUGE

#elif defined (LABWINDOWS)
#define PREF2 far
#define FAR far
#define HUGE

#elif defined (LINUX)
#define PREF2
#define FAR
#define HUGE

#else
#error Operating system not defined
#endif

/* Use 2-byte values for ENUMs
 */
#define ENUM(x) unsigned short

/* Range of port numbers for LPT files
 */
#define A200_FIRST_LPT  1
#define A200_LAST_LPT   3

/* Range of port numbers for USB devices
 */
#define A200_FIRST_USB  101
#define A200_LAST_USB   104

/* Maximum number of ADC200 units that can be opened at once
 */
#define A200_MAX_UNITS (A200_LAST_LPT - A200_FIRST_LPT + A200_LAST_USB - A200_FIRST_USB + 2)


/* Although the ADC200 uses an 8-bit ADC, it is usually possible to
 * oversample (collect multiple readings at each time) by up to 16.
 * the results are therefore ALWAYS scaled up to 12-bits, even if
 * oversampling is not used.
 *
 * The maximum and minimum values returned are therefore as follows:
 */

#define A200_MAX_OVERSAMPLE 16

#define A200_MAX_VALUE  2048
#define A200_MIN_VALUE -2047

#define A212_MAX_VALUE  2048
#define A212_MIN_VALUE -2047

#define A216_MAX_VALUE  32767
#define A216_MIN_VALUE -32767



typedef enum
  {
  A200_10MV,      // Not available on all units
  A200_20MV,      // Not available on all units
  A200_50MV,
  A200_100MV,
  A200_200MV,
  A200_500MV,
  A200_1V,
  A200_2V,
  A200_5V,
  A200_10V,
  A200_20V,
  A200_MAX_GAINS
  } A200_GAIN;

typedef enum
  {
  A200_CHANNEL_A,
  A200_CHANNEL_B,
  A200_CHANNEL_C,     // Not available on all units
  A200_CHANNEL_D,     // Not available on all units
  A200_MAX_CHANNELS
  } A200_CHANNEL;

typedef enum
  {
  A200_CHANNEL_A_ONLY,
  A200_CHANNEL_B_ONLY,
  A200_BOTH_CHANNELS,
  A200_MAX_MODES
  } A200_MODE;



typedef unsigned short A200_TIME;
#define A200_MAX_TIMEBASES 19
#define A212_50_MAX_TIMEBASES 16
#define A216_MAX_TIMEBASES 15

typedef enum {
        A200_FS,
        A200_PS,
        A200_NS,
        A200_US,
        A200_MS,
        A200_S,
        TU_MAX
        } A200_TIME_UNITS;

typedef enum
  {
  A200_OK,
  A200_INVALID_PORT,
  A200_INVALID_HW_VERSION,
  A200_INVALID_SW_VERSION,
  A200_CONFIG_FAILED,
  A200_ADDR_READ_FAILED,
  A200_NVR_FAIL,
  A200_UNIT_NOT_FOUND,
  A200_INVALID_LENGTH,
  A200_DRIVER_NOT_FOUND,
  A200_OLD_DRIVER_VERSION,
  A200_USB_ADAPTER_NOT_FOUND,
  A200_USB_ADAPTER_NOT_CONFIGURED,
  A200_ETS_CALIB_FAIL
  } A200_ERROR;

typedef enum
                {
                A200_RISING,
                A200_FALLING,
                A200_MAX_DIRS
                }       A200_TDIR;

typedef enum
                {
                A200_SOURCE_A,
                A200_SOURCE_B,
                A200_SOURCE_EXT,
                A200_MAX_SOURCES
                } A200_TSOURCE;

typedef enum {
        ETS_OFF,                        // ETS disabled
        ETS_FAST,                       // Say ready as soon as once requested no of interleaves is available
        ETS_SLOW} A200_ETS;             // Say ready every time you collect a new set of no_of_cycles

PREF1 short PREF2 adc200_get_status (void);

PREF1 short PREF2 adc200_get_product (void);

PREF1 short PREF2 adc200_get_hw_version (void);

PREF1 short PREF2 adc200_get_unit_info (
         char * str,
         short str_lth,
         short line,
         short port);

PREF1 unsigned short PREF2 adc200_set_range (
                ENUM (A200_CHANNEL) channel,
                ENUM (A200_GAIN) gain);

PREF1 unsigned short PREF2 adc200_set_dc (
        ENUM (A200_CHANNEL) channel,
         unsigned short dc);

PREF1 unsigned short PREF2 adc200_get_max_ets (void);

PREF1 unsigned long PREF2 adc200_get_ets_time (void);

PREF1 unsigned short PREF2 adc200_set_ets (
        unsigned short max_interleave,
        unsigned short max_cycles,
        ENUM (A200_ETS) mode);

PREF1 unsigned short PREF2 adc200_has_relays (void);

PREF1 unsigned short PREF2 adc200_set_channels (
        ENUM (A200_MODE) mode);

PREF1 short PREF2 adc200_set_timebase (
        unsigned long FAR * ns,
        unsigned char FAR * is_slow,
        A200_TIME timebase);

PREF1 short PREF2 adc200_set_oversample (
         unsigned short oversample);

PREF1 unsigned long PREF2 adc200_max_samples (void);

PREF1 void PREF2 adc200_set_trigger (
                  unsigned short  enabled,
        ENUM(A200_TSOURCE)    source,
        ENUM(A200_TDIR)       direction,
        short             delay_percent,
        short             threshold);

PREF1 void PREF2 adc200_set_rapid (
                  unsigned short  enabled);

PREF1 long PREF2 adc200_set_frequency (
                        long frequency);

PREF1 void PREF2 adc200_apply_fix (
                        short fix_no,
                        short value);

PREF1 short PREF2 adc200_open_unit (
         short port);

PREF1 short PREF2 adc200_set_unit (
         short port);

PREF1 void PREF2 adc200_close_unit (
         short port);

PREF1 short PREF2 adc200_run (
         unsigned long no_of_values);

PREF1 short PREF2 adc200_ready (void);

PREF1 void PREF2 adc200_stop (void);

PREF1 void PREF2 adc200_get_values (
                        short HUGE *     buffer_a,
                        short HUGE *     buffer_b,
                        unsigned long  no_of_values);

PREF1 unsigned short PREF2 adc200_set_time_units (A200_TIME_UNITS units);

PREF1 unsigned long PREF2 adc200_get_times_and_values (
                        long  HUGE *     times,
                        short HUGE *     buffer_a,
                        short HUGE *     buffer_b,
                        unsigned long  no_of_values);

PREF1 void PREF2 adc200_get_four_values (
                        short HUGE *     buffer_a,
                        short HUGE *     buffer_b,
                        short HUGE *     buffer_c,
                        short HUGE *     buffer_d,
                        unsigned long  no_of_values);

PREF1 void PREF2 adc200_get_single (
                        short FAR *     buffer);

PREF1 short PREF2 adc200_get_overflow (
                        ENUM(A200_CHANNEL) channel);

PREF1 unsigned short PREF2 adc200_get_driver_version (void);
PREF1 short PREF2 adc200_get_product (void);



