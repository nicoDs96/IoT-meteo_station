
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
   
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x00, 0x3D, 0x18, 0x58, \
                                                    0xF6, 0xE0, 0xEB, 0xD7 };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x70, 0xB3, 0xD5, 0x7E, \
                                                    0xD0, 0x02, 0xD5, 0x8A };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x46, 0xA4, 0x2B, 0x09, \
                                                    0x73, 0xA9, 0x2E, 0x66, \
                                                    0xCA, 0xA1, 0x22, 0x87, \
                                                    0x04, 0x3A, 0xCF, 0x5A };

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
		    /*
		    ===================================================
		    Those case are commented since not recognized on Iot-Lab Testbed at compiletime
		    ===================================================
		    case SEMTECH_LORAMAC_RX_DATA: 
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
            /* start receiving thread */
            thread_create(_recv_stack, sizeof(_recv_stack), THREAD_PRIORITY_MAIN - 1, 0, _recv, NULL, "recv thread");
        }
    }
    //station simulation
    uint32_t  	seed = 1;
    random_init(seed);
    int i = 0;
    while(1){
        uint8_t aggregate_count = 0;
        uint8_t aggregation_window_size = 60; //sec
        /*
        collect data each 1sec, compute the avg over a time window of size
        aggregation_window_size and then send one message containing the
        aggregate measure.
        */
        uint8_t temp_agg = 0; 
        uint8_t hum_agg =  0;
        uint8_t wind_i_agg = 0;
        uint8_t wind_d_agg = 0;
        uint8_t rain_h_agg = 0;
        
        while(aggregate_count < aggregation_window_size){ 
            aggregate_count += 1;
            
            //Simulate Sensors values
            uint8_t temp = (uint8_t)random_uint32_range(TEMP_MIN, TEMP_MAX);
            uint8_t hum = (uint8_t)random_uint32_range(HUM_MIN, HUM_MAX );
            uint8_t wind_i = (uint8_t)random_uint32_range(WIND_I_MIN, WIND_I_MAX);
            uint8_t wind_d = (uint8_t)random_uint32_range(WIND_D_MIN, WIND_D_MAX);
            uint8_t rain_h = (uint8_t)random_uint32_range(RAIN_H_MIN, RAIN_H_MAX);
            
            //compute the avg value (each measurement will contribute with weight 1/aggregation_window_size
            temp_agg += temp/aggregation_window_size; 
            hum_agg +=  hum/aggregation_window_size;
            wind_i_agg += wind_i/aggregation_window_size;
            wind_d_agg += wind_d/aggregation_window_size;
            rain_h_agg += rain_h/aggregation_window_size;
            
            /* wait 1 sec to measure again */
            xtimer_sleep(1); //sec
        
        }
        /*
        since LoRa requires low weight payload we try to be as minimal as possible 
        to be fair with other users of the network
        */
        char message[30];
        sprintf(message, "%d,%d,%d,%d,%d", temp_agg,hum_agg,wind_i_agg,wind_d_agg,rain_h_agg);
        
        //Print only the first message to give a feedback about data.
        if(i<1){
            puts("--------------------------------\n");
            printf("Sending Message:\n%s\n",message);
            puts("\n\nFuture Messages won't be printed.\n--------------------------------\n");
            i++;
        }
           
        /*
        
        */ 
           
        //Publish to TTN 
        int retry = 0;
        while(retry<10){
                
            retry++;
            
            if( pub_msg(message) == 1){
                printf("Tentative %d of 10 failed.\nI will retry to send in 1 minute.\n",retry);
                xtimer_sleep(60); //sec
            }
            else{
                puts("\nSuccess\n");
                retry=10;
            }
        } 
        
        xtimer_sleep(60); //sleep 1 minutes and send another message
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
