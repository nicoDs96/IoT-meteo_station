# IoT-meteo_stat

## Links
System Demo (Youtube): https://youtu.be/tRe6Aw1Ow0k
Tutorial and Description (Linkedin Blog): https://www.linkedin.com/pulse/setting-up-complete-iot-system-aws-core-apache-python-nicola-di-santo


## About this repository

I have realized a Simulated IoT system using Python, AWS IoT Core and Apache Superset. In detail, following the attached tutorial, We will emulate sensors from an environmental station. The generated data are sent to AWS IoT Core using MQTT protocol. We will explore two possible storage options: one is cloud based with AWS S3, Athena and IoT Core rules, the other is local storage with a Python MQTT consumer and SQLite database but can easily generalize with whatever DB. We will then quickly setup a web-based dashboard using Apache Superset.

SQLite has been chosen so that we can also upload the populated DB into the GitHub repository (in the db folder) and, if your goal is only to test Apache Superset, you do not need to interact with AWS nor run the script to generate data.

NB: keys used to work with AWS IoT core are actually removed from the directory for security reasons and only the Authority root certificate is left, since it is accessible by everyone. 

## Acknowledgments

This reposiotry is part of an assignment from IoT course held at Sapienza University - Rome (IT) during the academic year 2019-2020. For details on the assignment visit the [course page](http://ichatz.me/Site/InternetOfThings2020).
