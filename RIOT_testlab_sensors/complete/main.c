#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "shell.h"
#include "msg.h"
#include "net/emcute.h" // https://doc.riot-os.org/emcute_8h.html#
#include "net/ipv6/addr.h"
#include "thread.h"
#include "xtimer.h" 
#include "lps331ap.h" //sensors driver 
#include "lps331ap_params.h" //sensors driver 


#define EMCUTE_PORT         (1883U)
#define EMCUTE_ID           ("stat_1")
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)

bool connected = false;

static char stack[THREAD_STACKSIZE_DEFAULT];
static msg_t queue[8];

static emcute_sub_t subscriptions[NUMOFSUBS];

static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}


static unsigned get_qos(const char *str)
{
    int qos = atoi(str);
    switch (qos) {
        case 1:     return EMCUTE_QOS_1;
        case 2:     return EMCUTE_QOS_2;
        default:    return EMCUTE_QOS_0;
    }
}



/*Tryies to connect to the broker with @address and @port*/
int connect(char address[], char port[])
{
    (void) port;
    sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT };
    char *topic = NULL;
    char *message = "";
    size_t len = 0;

    /* parse address */

    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, address) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }
    gw.port = atoi(port);
    printf("Port set to %i\n",gw.port);

    puts("2 ");
    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", address, (int)gw.port);
        return 1;
    }
    printf("Successfully connected to gateway at [%s]:%i\n", address, (int)gw.port);

    return 0;
}

/*Publish a @message into @topic with @QoS.*/
static int pub_msg(char topic[], char message[], char QoS[])
{
    emcute_topic_t t;
    unsigned flags = EMCUTE_QOS_0;

    /* parse QoS level */
    flags |= get_qos(QoS);
    //printf("pub with topic:\t%s\nmsg:\t%s\nflags:\t0x%02x\n", topic, message, (int)flags);

    /* step 1: get topic id */
    t.name = topic;
    if (emcute_reg(&t) != EMCUTE_OK) {
        puts("error: unable to obtain topic ID");
        return 1;
    }

    /* step 2: publish data */
    if (emcute_pub(&t, message, strlen(message), flags) != EMCUTE_OK) {
        printf("error: unable to publish data to topic '%s [id: %i]'\n", t.name, (int)t.id);
        return 1;
    }

    printf("Published %i bytes to topic '%s [id: %i]'\n",
           (int)strlen(message), t.name, t.id);

    return 0;
}


int read_sensors(int *pres, int *temp)
{
   
    static lps331ap_t dev;\
    
    printf("Reading values from %s pressure sensor\n\n", LPS331AP_SAUL_NAME);
    printf("Initializing %s sensor\n", LPS331AP_SAUL_NAME);
    if (lps331ap_init(&dev, &lps331ap_params[0]) != 0) {
        puts("Initialization failed");
        return 1;
    }

    
    lps331ap_enable(&dev);
    xtimer_sleep(1); /* wait a bit for the measurements to complete */

    &temp = lps331ap_read_temp(&dev); //temp and pres are already pointers
    &pres = lps331ap_read_pres(&dev);
    lps331ap_disable(&dev); //do not waste energy, disable sensors after measurements

    int temp_abs = &temp / 100;
    &temp -= temp_abs * 100;

    printf("Pressure value: %ihPa - Temperature: %2i.%02i°C\n",
           &pres, temp_abs, &temp);
           
    return 0;
}
 
 

static int start_station(char id[])
{
       
    printf("Setting Station_id and Topic to: %s",id);
    
    if(!connected){ //we need to connect only once
        //connect
        if( connect("fec0:affe::1","1885") == 1){
            puts("Conncetion error. Exiting.");
            return 1;
        } else{
            connected = true; //do not connect again
        }
    }

    //uint32_t  	seed = 1;
    //random_init(seed);
    int i = 0;
    while(1){
        //Simulate Sensors values
        int temp;
        int pres;
        if( read_sensors(&pres, &temp) == 1){ //error reading
            puts("Unable to read sensors at this round. Not sending anything.\n");
            
        }else{ //read sensor ok then send data
            //Get the current time and print it into the string date_time
            char date_time[30];
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            sprintf(date_time,"%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

            //define the message as a string and print values in the message string
            char message[200];
            sprintf(message, "{\"station_id\":\"%s\",\"timestamp\":\"%s\",\"temperature\":\"%d\",\"pres\":\"%d\"}",
                     id,date_time,temp,pres);
            
            //Print only the first message to give a feedback about data.
            if(i<1){
                puts("--------------------------------\n");
                printf("Sending Message:\n%s\n",message);
                puts("\n\nFuture Messages won't be printed.\n--------------------------------\n");
                i++;
            }
               //Publish to topic
            pub_msg(id, message, "0"); //params: @topic_id, @message, @QoS <- NOTE topic name is the same of id
            i += 1;
        }
        

        
        xtimer_sleep(10);
    }



    return 0;
}

 static int cmd_start(int argc, char **argv)
{
    if( argc <= 1 ){
        printf("Usage: %s station_id\n", argv[0]);
        return 1;
    }
    start_station(argv[1]);
    return 0;
}


static const shell_command_t shell_commands[] = {
        {"start", "Start Environmental Station Telemetry.",cmd_start},
        { NULL, NULL, NULL }
};

int main(void)
{
    puts("MQTT-SN Environment Station Simulation \n");
    puts("Type 'help' to get started. Have a look at the README.md for more"
         "information.");

    /* the main thread needs a msg queue to be able to run `ping6`*/
    msg_init_queue(queue, (sizeof(queue) / sizeof(msg_t)));

    /* initialize our subscription buffers */
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
