/* Include Header files */
#include <math.h>

/* Define Macros */
#define MQ135_LOAD_RESISTANCE_KOHM     		10.0    	/* Load resistance (RL) in kilo-ohms */
#define MQ135_CLEAN_AIR_RESISTANCE_KOHM 	30.0   		/* Sensor resistance (R0) in clean air (calibrated) */
#define MQ135_CO2_CURVE_CONSTANT       		116.6020682  	/* Constant 'A' in CO₂ logarithmic equation */
#define MQ135_CO2_CURVE_EXPONENT       		-2.769034857 	/* Exponent 'B' (slope) in CO₂ logarithmic equation */



/* Function prototypes */
void initialize_mq135_adc(void);
float calculate_mq135_ppm(uint16_t u16rawVal);
int read_mq135(void);

