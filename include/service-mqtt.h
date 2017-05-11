/*
* CamFlow userspace provenance tool
*
* Author: Thomas Pasquier <tfjmp@seas.harvard.edu>
*
* Copyright (C) 2017 Harvard University
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2, as
* published by the Free Software Foundation.
*
*/
#ifndef __SERVICE_MQTT_H
#define __SERVICE_MQTT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MQTTClient.h"
#include "service-config.h"
#include "service-log.h"

#define gettid() syscall(SYS_gettid)
#define TIMEOUT         10000L

static MQTTClient __service_client;

static inline void init_mqtt(void){
  int rc;
  uint32_t machine_id;
  rc = provenance_get_machine_id(&machine_id);
  if(rc<0){
    log("Failed retrieving machine ID.");
    exit(rc);
  }
  snprintf(__service_config.provenance_topic, MAX_TOPIC_LENGTH, "camflow/provenance/%u", machine_id);
  snprintf(__service_config.machine_topic, MAX_TOPIC_LENGTH, "camflow/machines/%u", machine_id);
  snprintf(__service_config.client_id, MAX_MQTT_CLIENT_ID_LENGTH, "%u", machine_id); // should be no more than 23
  log("Provenance topic: %s.", __service_config.provenance_topic);
  log("Machine topic: %s.", __service_config.machine_topic);
  MQTTClient_create(&__service_client,
    __service_config.address,
    __service_config.client_id,
    MQTTCLIENT_PERSISTENCE_NONE,
    NULL);
}

static inline void stop_mqtt(void){
  MQTTClient_disconnect(__service_client, 10000);
  MQTTClient_destroy(&__service_client);
}

static inline void mqtt_connect(bool cleansession){
  pid_t tid = gettid();
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  int rc;
  conn_opts.keepAliveInterval = 20;
  if(cleansession){
    conn_opts.cleansession = 1;
  }else{
    conn_opts.cleansession = 0;
  }
  conn_opts.reliable = 1;
  conn_opts.username = __service_config.username;
  conn_opts.password = __service_config.password;

  log("Connecting to MQTT... (%ld)", tid);
  if ((rc = MQTTClient_connect(__service_client, &conn_opts)) < 0)
  {
      log("Failed to connect, return code %d\n", rc);
      exit(-1);
  }
  log("Connected (%ld)", tid);
}

static pthread_mutex_t l_mqtt = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
/* publish payload on mqtt */
static inline void mqtt_publish(char* topic, char* payload, int qos, bool retained){
  pid_t tid = gettid();
  int rc;
  int retry=0; // give up after a while.
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;

  if(payload==NULL){
    pubmsg.payload = NULL;
    pubmsg.payloadlen = 0;
  }else{
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(payload);
  }

  pubmsg.qos = qos;

  if(retained){
    pubmsg.retained = 1;
  }else{
    pubmsg.retained = 0;
  }


  do{
    pthread_mutex_lock(&l_mqtt); // set to reliable only a message at a time

    if( !MQTTClient_isConnected(__service_client) ){
      mqtt_connect(false);
    }

    MQTTClient_publishMessage(__service_client, topic, &pubmsg, &token);
    rc = MQTTClient_waitForCompletion(__service_client, token, TIMEOUT);

    pthread_mutex_unlock(&l_mqtt);

    if(rc != MQTTCLIENT_SUCCESS){
      log("MQTT disconnected error: %d (%ld)", rc, tid);
      retry++;
    }
    if(retry > 10){
      log("Failed connect retry (%ld)", tid);
      break;
    }
  }while(rc != MQTTCLIENT_SUCCESS);
}
#endif
