
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "shell.h"
#include "msg.h"
#include "thread.h"
#include "random.h"
#include "xtimer.h"
#include "net/loramac.h"
#include "semtech_loramac.h"


#define RECV_MSG_QUEUE                   (4U)
static msg_t _recv_queue[RECV_MSG_QUEUE];
static char _recv_stack[THREAD_STACKSIZE_DEFAULT];

//User defined macros
uint32_t TEMP_MIN = 0;
uint32_t TEMP_MAX = 50;
uint32_t HUM_MAX = 100;
uint32_t HUM_MIN = 0;
uint32_t WIND_D_MAX = 360;
uint32_t WIND_D_MIN = 0;
uint32_t WIND_I_MIN = 0;
uint32_t WIND_I_MAX = 100;
uint32_t RAIN_H_MIN = 0;
uint32_t RAIN_H_MAX = 50;
bool connected = false;
semtech_loramac_t loramac;  /* The loramac stack device descriptor */
/* define the required keys for OTAA, e.g over-the-air activation (the
   null arrays need to be updated with valid LoRa values) */
   
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x00, 0x8B, 0x51, 0x44, \
                                                    0x3D, 0x92, 0xFC, 0x6F };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x70, 0xB3, 0xD5, 0x7E, \
                                                    0xD0, 0x02, 0xD5, 0x8A };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0xD1, 0x4D, 0x14, 0xAC, \
                                                    0x89, 0xA4, 0xF8, 0x03, \
                                                    0x02, 0x31, 0x07, 0x50, \
                                                    0x89, 0x78, 0x8A, 0x84 };

//static char stack[THREAD_STACKSIZE_DEFAULT];

//static char id[] = "stat1";

static void *_recv(void *arg)
{
    msg_init_queue(_recv_queue, RECV_MSG_QUEUE);
    (void)arg;
    while (1) {
        /* blocks until some data is received */
        semtech_loramac_recv(&loramac);
        loramac.rx_data.payload[loramac.rx_data.payload_len] = 0;
        printf("Data received: %s, port: %d\n",
               (char *)loramac.rx_data.payload, loramac.rx_data.port);
    }
    
    return NULL;
}
/*Tryies to connect to the lora network */
int connect(void)
{
    /* 1. initialize the LoRaMAC MAC layer */
    semtech_loramac_init(&loramac);
    /* 2. set the keys identifying the device */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);
    /* 2.1 setting device rate */
    semtech_loramac_set_dr(&loramac, 6);
    /* 3. join the network */
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("\nJoin procedure succeeded, otaa net joined.\n");
    /* 4. send some data */
    return 0;
    
}

/*Publish a @message into @topic with @QoS.*/
static int pub_msg(char message[])
{
    
    uint8_t return_code = semtech_loramac_send(&loramac, (uint8_t *)message, strlen(message)); 
    if (return_code != SEMTECH_LORAMAC_TX_DONE) {
        puts("semtech_loramac_send error: ");
        switch (return_code){
		    case SEMTECH_LORAMAC_JOIN_SUCCEEDED: 
		    printf("SEMTECH_LORAMAC_JOIN_SUCCEEDED\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_JOIN_FAILED: 
		    printf("SEMTECH_LORAMAC_JOIN_FAILED\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_NOT_JOINED: 
		    printf("SEMTECH_LORAMAC_NOT_JOINED\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_ALREADY_JOINED: 
		    printf("SEMTECH_LORAMAC_ALREADY_JOINED\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_TX_OK: 
		    printf("SEMTECH_LORAMAC_TX_OK\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_TX_SCHEDULE: 
		    printf("SEMTECH_LORAMAC_TX_SCHEDULE\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_TX_DONE: 
		    printf("SEMTECH_LORAMAC_TX_DONE\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_TX_CNF_FAILED: 
		    printf("SEMTECH_LORAMAC_TX_CNF_FAILED\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_TX_ERROR: 
		    printf("SEMTECH_LORAMAC_TX_ERROR\n");
		    break; //causa l'uscita immediata dallo switch
		    /*case SEMTECH_LORAMAC_RX_DATA: 
		    printf("SEMTECH_LORAMAC_RX_DATA");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_RX_LINK_CHECK: 
		    printf("SEMTECH_LORAMAC_RX_LINK_CHECK");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_RX_CONFIRMED: 
		    printf("SEMTECH_LORAMAC_RX_CONFIRMED");
		    break; //causa l'uscita immediata dallo switch*/
		    case SEMTECH_LORAMAC_BUSY: 
		    printf("SEMTECH_LORAMAC_BUSY\n");
		    break; //causa l'uscita immediata dallo switch
		    case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED: 
		    printf("SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED\n");
		    break; //causa l'uscita immediata dallo switch
		    default:
		    printf("ma che cazz sta succededendo.pd\n");
		    
		    
        }
        return 1;
    }
    
    return 0;
}

/* 
 * Simulating Environmental Station
 * And sending values to mqtts to Broker.
 *
 */

static int start_station(void)
{
       
    
    
    if(!connected){ //we need to connect only once
        //connect
        if( connect() == 1){
            puts("Conncetion error. Exiting.");
            return 1;
        } else{
            connected = true; //do not connect again
            thread_create(_recv_stack, sizeof(_recv_stack), THREAD_PRIORITY_MAIN - 1, 0, _recv, NULL, "recv thread");
        }
    }
    //station simulation
    uint32_t  	seed = 1;
    random_init(seed);
    int i = 0;
    while(1){
        //Simulate Sensors values
        uint8_t temp = (uint8_t)random_uint32_range(TEMP_MIN, 2*TEMP_MAX);
        uint8_t hum = (uint8_t)random_uint32_range(HUM_MIN, HUM_MAX );
        uint8_t wind_i = (uint8_t)random_uint32_range(WIND_I_MIN, WIND_I_MAX);
        uint8_t wind_d = (uint8_t)random_uint32_range(WIND_D_MIN, WIND_D_MAX);
        uint8_t rain_h = (uint8_t)random_uint32_range(RAIN_H_MIN, RAIN_H_MAX);

        //Get the current time and print it into the string date_time
        /*char date_time[30];
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(date_time,"%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        //define the message as a string and print values in the message string
        char message[200];
        sprintf(message, "{\"station_id\":\"%s\",\"timestamp\":\"%s\",\"temperature\":%u,\"humidity\":%u,\"wind_direction\":%u,\"wind_intensity\":%u,\"rain_height\":%u}",
                 id,date_time,temp,hum,wind_i,wind_d,rain_h);*/
        char message[30];
        sprintf(message, "%u,%u,%u,%u,%u", temp,hum,wind_i,wind_d,rain_h);
        
        //Print only the first message to give a feedback about data.
        if(i<1){
            puts("--------------------------------\n");
            printf("Sending Message:\n%s\n",message);
            puts("\n\nFuture Messages won't be printed.\n--------------------------------\n");
            i++;
        }
           //Publish to TTN 
        int retry = 0;
        while(retry<10){
                
            retry++;
            
            if( pub_msg(message) == 1){
                printf("Failed sending %d/10 times.\nI will retry in 1 minute.\n",retry);
                xtimer_sleep(60); //sec
            }
            else{
                puts("\nSuccess\n");
                retry=10;
            }
        } 
        
        xtimer_sleep(60*5); //sleep 5 minutes and send another message
    }



    return 0;
}

 static int cmd_start(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    start_station();
    return 0;
}


static const shell_command_t shell_commands[] = {
        {"start", "Start Environmental Station Simulation.",cmd_start},
        { NULL, NULL, NULL }
};

int main(void)
{
    puts("MQTT-SN Environment Station Simulation \n");
    puts("Type 'help' to get started. Have a look at the README.md for more"
         "information.");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
