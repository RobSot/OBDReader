/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* Define system operating frequency */

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        140000000L
#define FCY             SYS_FREQ/2

/* Oscillator types defines */

#define NOSC_FRC    0
#define NOSC_FRCPLL 1
#define NOSC_PRI    2
#define NOSC_PRIPLL 3
#define NOSC_SOSC   4
#define NOSC_LPRC   5


/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */
void ConfigurePPS(void);        /* Configure PPS remap pins */
void ConfigureInt(void);        /* Configure Interrupt */