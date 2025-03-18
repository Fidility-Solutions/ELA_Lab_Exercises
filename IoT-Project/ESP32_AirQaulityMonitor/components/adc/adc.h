/* Include Header files */
#include "main.h"
#include <math.h>

/* Macros */
#define RL_VALUE 10.0  /* Load resistance in kilo-ohms */
#define R0 30.0        /* Sensor resistance in clean air (calibrated) */
#define A 116.6020682  /* CO₂ curve constant */
#define B -2.769034857 /* CO₂ curve slope */

/* Function prototypes */
void initialize_mq135_adc(void);
float calculate_mq135_ppm(void);
int read_mq135(void);

