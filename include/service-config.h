/*
* CamFlow userspace provenance tool
*
* Author: Thomas Pasquier <tfjmp@seas.harvard.edu>
*
* Copyright (C) 2017-2018 University of Cambridge, Harvard University
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2, as
* published by the Free Software Foundation.
*
*/
#ifndef __SERVICE_CONFIG_H
#define __SERVICE_CONFIG_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ini.h"

#define CONFIG_PATH "/etc/camflowd.ini"
#define MAX_TOPIC_LENGTH 256
#define MAX_MQTT_CLIENT_ID_LENGTH 23 // see https://www.eclipse.org/paho/files/mqttdoc/Cclient/_m_q_t_t_client_8h.html#a5cb44bc0e06bcc95a314d51320a0cd1b
#define MAX_OUTPUT_LENGTH 256
#define MAX_FORMAT_LENGTH 256

typedef struct{
  int qos;
  char address[PATH_MAX]; // assuming we could use unix socket
  char username[1024];
  char password[1024];
	char log[PATH_MAX];
  char output[MAX_OUTPUT_LENGTH];
  char format[MAX_FORMAT_LENGTH];
  char provenance_topic[MAX_TOPIC_LENGTH];
  char machine_topic[MAX_TOPIC_LENGTH];
  char client_id[MAX_MQTT_CLIENT_ID_LENGTH];
} configuration;

static configuration __service_config;

#define MATCH(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)
/* call back for configuation */
static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;
		if(MATCH("general", "log")){
			strncpy(pconfig->log, value, PATH_MAX);
		}else if(MATCH("general", "output")){
			strncpy(pconfig->output, value, MAX_OUTPUT_LENGTH);
		}else if(MATCH("general", "format")){
			strncpy(pconfig->format, value, MAX_OUTPUT_LENGTH);
		}else if(MATCH("mqtt", "qos")) {
      pconfig->qos = atoi(value);
    }else if (MATCH("mqtt", "address")) {
      strncpy(pconfig->address, value, PATH_MAX);
    }else if(MATCH("mqtt", "username")){
      strncpy(pconfig->username, value, 1024);
    }else if(MATCH("mqtt", "password")){
      strncpy(pconfig->password, value, 1024);
    } else{
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

static inline void read_config(void){
  memset(&__service_config, 0, sizeof(configuration));
  if (ini_parse(CONFIG_PATH, handler, &__service_config) < 0) {
      printf("Can't load configuration: %s\n", CONFIG_PATH);
      exit(-1);
  }
}

#define IS_CONFIG_MQTT()        (strcmp(__service_config.output, "mqtt") == 0)
#define IS_CONFIG_LOG()         (strcmp(__service_config.output, "log") == 0)
#define IS_CONFIG_NULL()        (strcmp(__service_config.output, "null") == 0)

#define IS_FORMAT_W3C()         (strcmp(__service_config.format, "w3c") == 0)
#define IS_FORMAT_SPADE_JSON()  (strcmp(__service_config.format, "spade_json") == 0)
#endif
