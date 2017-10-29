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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

#include "provenance.h"
#include "provenanceutils.h"
#include "provenanceProvJSON.h"

#include "service-config.h"
#include "service-log.h"
#include "service-mqtt.h"

#define APP_NAME "camflowd"

void init( void ){
  pid_t tid = gettid();
  syslog(LOG_INFO, "Init audit thread (%ld)", tid);
}

void log_str(struct str_struct* data){
  append_entity(str_msg_to_json(data));
}

void log_derived(struct relation_struct* relation){
  append_derived(derived_to_json(relation));
}

void log_generated(struct relation_struct* relation){
  append_generated(generated_to_json(relation));
}

void log_used(struct relation_struct* relation){
  append_used(used_to_json(relation));
}

void log_informed(struct relation_struct* relation){
  append_informed(informed_to_json(relation));
}

void log_task(struct task_prov_struct* task){
  append_activity(task_to_json(task));
}

void log_inode(struct inode_prov_struct* inode){
  append_entity(inode_to_json(inode));
}

void log_act_disc(struct disc_node_struct* node){
  append_activity(disc_to_json(node));
}

void log_agt_disc(struct disc_node_struct* node){
  append_agent(disc_to_json(node));
}

void log_ent_disc(struct disc_node_struct* node){
  append_entity(disc_to_json(node));
}

void log_msg(struct msg_msg_struct* msg){
  append_entity(msg_to_json(msg));
}

void log_shm(struct shm_struct* shm){
  append_entity(shm_to_json(shm));
}

void log_packet(struct pck_struct* pck){
  append_entity(packet_to_json(pck));
}

void log_address(struct address_struct* address){
  append_entity(addr_to_json(address));
}

void log_file_name(struct file_name_struct* f_name){
  append_entity(pathname_to_json(f_name));
}

void log_iattr(struct iattr_prov_struct* iattr){
  append_entity(iattr_to_json(iattr));
}


void log_xattr(struct xattr_prov_struct* xattr){
  append_entity(xattr_to_json(xattr));
}

void log_packet_content(struct pckcnt_struct* cnt){
  append_entity(pckcnt_to_json(cnt));
}

void log_arg(struct arg_struct* arg){
  append_entity(arg_to_json(arg));
}

void log_error(char* error){
  syslog(LOG_ERR, "From library: %s", error);
}

struct provenance_ops ops = {
  .init=&init,
  .log_derived=&log_derived,
  .log_generated=&log_generated,
  .log_used=&log_used,
  .log_informed=&log_informed,
  .log_task=&log_task,
  .log_inode=&log_inode,
  .log_str=&log_str,
  .log_act_disc=&log_act_disc,
  .log_agt_disc=&log_agt_disc,
  .log_ent_disc=&log_ent_disc,
  .log_msg=&log_msg,
  .log_shm=&log_shm,
  .log_packet=&log_packet,
  .log_address=&log_address,
  .log_file_name=&log_file_name,
  .log_iattr=&log_iattr,
  .log_xattr=&log_xattr,
  .log_packet_content=&log_packet_content,
  .log_arg=&log_arg,
  .log_error=&log_error
};

struct provenance_ops ops_null = {
  .init=&init,
  .log_derived=NULL,
  .log_generated=NULL,
  .log_used=NULL,
  .log_informed=NULL,
  .log_task=NULL,
  .log_inode=NULL,
  .log_str=NULL,
  .log_act_disc=NULL,
  .log_agt_disc=NULL,
  .log_ent_disc=NULL,
  .log_msg=NULL,
  .log_shm=NULL,
  .log_packet=NULL,
  .log_address=NULL,
  .log_file_name=NULL,
  .log_iattr=NULL,
  .log_xattr=NULL,
  .log_packet_content=NULL,
  .log_arg=NULL,
  .log_error=&log_error
};

static inline void __init_syslog(void){
  setlogmask(LOG_UPTO(LOG_INFO));
  openlog(APP_NAME, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
}

volatile sig_atomic_t terminate = 0;

void term(int signum)
{
  terminate = 1;
  syslog(LOG_INFO, "Shutdown signal received.");
  if (!IS_CONFIG_NULL())
    flush_json();
  provenance_relay_stop();
  if(IS_CONFIG_MQTT())
    stop_mqtt();
  syslog(LOG_INFO, "Service terminated.");
  exit(0);
}

int main(void)
{
    int rc;
    uint32_t i;
    char json[4096];
    struct sigaction action;

    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGHUP, &action, NULL);

    __init_syslog();
    if(!provenance_is_present()) {
      syslog(LOG_ERR, "CamFlow not present in current kernel.");
      exit(-1);
    }
    read_config();
    syslog(LOG_INFO, "%s\n", __service_config.log);
    syslog(LOG_INFO, "%s\n", __service_config.log);
    syslog(LOG_INFO, "Output option: %s", __service_config.output);

    if(IS_CONFIG_MQTT()){
      syslog(LOG_INFO, "MQTT Provenance service");
      syslog(LOG_INFO, "Main process pid: %ld", getpid());
      init_mqtt();
      mqtt_connect(true);
      publish_json(__service_config.machine_topic, machine_description_json(json), true);
      set_ProvJSON_callback(mqtt_print_json);
    }else if(IS_CONFIG_LOG()){
      _init_logs();
      log_print_json(machine_description_json(json));
      set_ProvJSON_callback(log_print_json);
    }

    if (IS_CONFIG_NULL())
      rc = provenance_relay_register(&ops_null, NULL);
    else
      rc = provenance_relay_register(&ops, NULL);

    if(rc){
      syslog(LOG_ERR, "Failed registering audit operation.");
      exit(rc);
    }

    while(!terminate){
      if (!IS_CONFIG_NULL())
        flush_json();
      if(i%10==0 && IS_CONFIG_MQTT())
        mqtt_publish("keepalive", NULL, 0, false); // keep alive
      i++;
      sleep(1);
    }
    return 0;
}
