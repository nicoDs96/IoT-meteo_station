


#include <stdio.h>
#include "shell.h"
#include "lpsxxx.h"
#include "lpsxxx_params.h"

int main(void)
{
    puts("Welcome to RIOT!\n");
    puts("Type `help` for help, type `saul` to see all SAUL devices\n");
    
    static Ipsxxx_t dev;
    
    printf("Test application for %s pressure sensor\n\n", LPSXXX_SAUL_NAME);
    printf("Initializing %s sensor\n", LPSXXX_SAUL_NAME);
    if (lpsxxx_init(&dev, &lpsxxx_params[0]) != LPSXXX_OK) {
        puts("Initialization failed");
        return 1;
    }

    uint16_t pres;
    int16_t temp;
    while (1) {
        lpsxxx_enable(&dev);
        xtimer_sleep(1); /* wait a bit for the measurements to complete */

        lpsxxx_read_temp(&dev, &temp);
        lpsxxx_read_pres(&dev, &pres);
        lpsxxx_disable(&dev); //do not waste energy

        int temp_abs = temp / 100;
        temp -= temp_abs * 100;

        printf("Pressure value: %ihPa - Temperature: %2i.%02i°C\n",
               pres, temp_abs, temp);
    }
    
   
   lpsxxx_t

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
