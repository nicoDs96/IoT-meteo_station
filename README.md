# Internet of Things Environmental station
Project Directories structure:
```
├── db                              #SQLite database with real data (.tables to see all available tables)
│   
├── python_simulated_station        # code from the first assignment simulating env. station with python
│   
├── RIOT_simulated_station          # code to simulate env. station in RIOT OS
│   │   
│   ├── lib                         # all the scripts needed to run the gateway (from mosquitto /rsmb/src/clients)                        
│   │   
│   ├── simulated_station           # simulated station #1
│   │   └── bin
│   │   
│   └── simulated_station_2         # simulated station #2 
│       └── bin
│      
└── RIOT_testlab_sensors            # Code to read M3 temperature and pressure sensors 


```

## RIOT OS Station - Assignment 2
A IoT system using RIOT OS, A Python Transparent Gateway, AWS IoT Core, Apache Superset and one between  {AWS S3 and AWS Athena} or SQLite database. <br><br>
Cloud and analytics reuse the same code and configuration of Assignemt 1 (see below). Tutorial on how to build a RIOT OS application and how to build a Gateway to allow comunication between board using MQTT-SN and AWS IoT Core broker is provided below.
### Links
System Demo (Youtube): https://youtu.be/MY6dfM6pYLs <br><br>

Tutorial and Description (Linkedin Blog): https://www.linkedin.com/pulse/designing-environmental-station-based-riot-os-aws-iot-nicola-di-santo <br>

## Python Simulated Station - Assignment 1 
A Simulated IoT system using Python, AWS IoT Core, Apache Superset and one between  {AWS S3 and AWS Athena} or SQLite database. (directory: python_simulated_station)
### Links
System Demo (Youtube): https://youtu.be/tRe6Aw1Ow0k <br><br>
Cutted video (YouTube): https://youtu.be/aOt-4mvsAAo <br><br>
Tutorial and Description (Linkedin Blog): https://www.linkedin.com/pulse/setting-up-complete-iot-system-aws-core-apache-python-nicola-di-santo


## About this repository

I have first realized a Simulated IoT system using Python, AWS IoT Core and Apache Superset. Then also a simulated version of environmental station running on RIOT OS has been implemented toghether with a transparent gateway to forward messages to the cloud. <br>In detail, following the attached tutorials, We will emulate sensors from an environmental station. The generated data are sent to AWS IoT Core using MQTT protocol. We will explore two possible storage options: one is cloud based with AWS S3, Athena and IoT Core rules, the other is local storage with a Python MQTT consumer and SQLite database but can easily generalize with whatever DB. We will then quickly setup a web-based dashboard using Apache Superset. <br> The same functionalities running within a real board with real environmental sensors is going to work quickly (it is possible to read sensor but still not possible to send those values to the cloud).

SQLite has been chosen so that we can also upload the populated DB into the GitHub repository (in the db folder) and, if your goal is only to test Apache Superset, you do not need to interact with AWS nor run the script to generate data.

NB: keys used to work with AWS IoT core are actually removed from the directory for security reasons and only the Authority root certificate is left, since it is accessible by everyone. 

## Acknowledgments

This reposiotry is part of an assignment from IoT course held at Sapienza University - Rome (IT) during the academic year 2019-2020. For details on the assignment visit the [course page](http://ichatz.me/Site/InternetOfThings2020).
