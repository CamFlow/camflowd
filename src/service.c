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

#include <provenance.h>
#include <provenanceutils.h>
#include <provenanceProvJSON.h>
#include <provenanceSPADEJSON.h>

#include "service-config.h"
#include "service-log.h"
#include "service-mqtt.h"

#define APP_NAME "camflowd"

void init( void ){
  pid_t tid = gettid();
  syslog(LOG_INFO, "Init audit thread (%ld)", tid);
}

void w3c_str(struct str_struct* data){
  append_entity(str_msg_to_json(data));
}

void w3c_derived(struct relation_struct* relation){
  append_derived(derived_to_json(relation));
}

void w3c_generated(struct relation_struct* relation){
  append_generated(generated_to_json(relation));
}

void w3c_used(struct relation_struct* relation){
  append_used(used_to_json(relation));
}

void w3c_informed(struct relation_struct* relation){
  append_informed(informed_to_json(relation));
}

void w3c_proc(struct proc_prov_struct* proc){
  append_entity(proc_to_json(proc));
}

void w3c_task(struct task_prov_struct* task){
  append_activity(task_to_json(task));
}

void w3c_inode(struct inode_prov_struct* inode){
  append_entity(inode_to_json(inode));
}

void w3c_act_disc(struct disc_node_struct* node){
  append_activity(disc_to_json(node));
}

void w3c_agt_disc(struct disc_node_struct* node){
  append_agent(disc_to_json(node));
}

void w3c_ent_disc(struct disc_node_struct* node){
  append_entity(disc_to_json(node));
}

void w3c_msg(struct msg_msg_struct* msg){
  append_entity(msg_to_json(msg));
}

void w3c_shm(struct shm_struct* shm){
  append_entity(shm_to_json(shm));
}

void w3c_packet(struct pck_struct* pck){
  append_entity(packet_to_json(pck));
}

void w3c_address(struct address_struct* address){
  append_entity(addr_to_json(address));
}

void w3c_file_name(struct file_name_struct* f_name){
  append_entity(pathname_to_json(f_name));
}

void w3c_iattr(struct iattr_prov_struct* iattr){
  append_entity(iattr_to_json(iattr));
}


void w3c_xattr(struct xattr_prov_struct* xattr){
  append_entity(xattr_to_json(xattr));
}

void w3c_packet_content(struct pckcnt_struct* cnt){
  append_entity(pckcnt_to_json(cnt));
}

void w3c_arg(struct arg_struct* arg){
  append_entity(arg_to_json(arg));
}

void spade_derived(struct relation_struct* relation){
  spade_json_append(derived_to_spade_json(relation));
}

void spade_generated(struct relation_struct* relation){
  spade_json_append(generated_to_spade_json(relation));
}

void spade_used(struct relation_struct* relation){
  spade_json_append(used_to_spade_json(relation));
}

void spade_informed(struct relation_struct* relation){
  spade_json_append(informed_to_spade_json(relation));
}

void spade_proc(struct proc_prov_struct* proc){
    spade_json_append(proc_to_spade_json(proc));
}

void spade_task(struct task_prov_struct* task){
  spade_json_append(task_to_spade_json(task));
}

void spade_inode(struct inode_prov_struct* inode){
  spade_json_append(inode_to_spade_json(inode));
}

void spade_act_disc(struct disc_node_struct* node){
  spade_json_append(disc_to_spade_json(node));
}

void spade_agt_disc(struct disc_node_struct* node){
  spade_json_append(disc_to_spade_json(node));
}

void spade_ent_disc(struct disc_node_struct* node){
  spade_json_append(disc_to_spade_json(node));
}

void spade_msg(struct msg_msg_struct* msg){
  spade_json_append(msg_to_spade_json(msg));
}

void spade_shm(struct shm_struct* shm){
  spade_json_append(shm_to_spade_json(shm));
}

void spade_packet(struct pck_struct* pck){
  spade_json_append(packet_to_spade_json(pck));
}

void spade_address(struct address_struct* address){
  spade_json_append(addr_to_spade_json(address));
}

void spade_file_name(struct file_name_struct* f_name){
  spade_json_append(pathname_to_spade_json(f_name));
}

void spade_iattr(struct iattr_prov_struct* iattr){
  spade_json_append(iattr_to_spade_json(iattr));
}


void spade_xattr(struct xattr_prov_struct* xattr){
  spade_json_append(xattr_to_spade_json(xattr));
}

void spade_packet_content(struct pckcnt_struct* cnt){
  spade_json_append(pckcnt_to_spade_json(cnt));
}

void spade_arg(struct arg_struct* arg){
  spade_json_append(arg_to_spade_json(arg));
}

void log_error(char* error){
  syslog(LOG_ERR, "From library: %s", error);
}

struct provenance_ops w3c_ops = {
  .init=&init,
  .log_derived=&w3c_derived,
  .log_generated=&w3c_generated,
  .log_used=&w3c_used,
  .log_informed=&w3c_informed,
  .log_proc=&w3c_proc,
  .log_task=&w3c_task,
  .log_inode=&w3c_inode,
  .log_str=&w3c_str,
  .log_act_disc=&w3c_act_disc,
  .log_agt_disc=&w3c_agt_disc,
  .log_ent_disc=&w3c_ent_disc,
  .log_msg=&w3c_msg,
  .log_shm=&w3c_shm,
  .log_packet=&w3c_packet,
  .log_address=&w3c_address,
  .log_file_name=&w3c_file_name,
  .log_iattr=&w3c_iattr,
  .log_xattr=&w3c_xattr,
  .log_packet_content=&w3c_packet_content,
  .log_arg=&w3c_arg,
  .log_error=&log_error
};

struct provenance_ops spade_json_ops = {
  .init=&init,
  .log_derived=&spade_derived,
  .log_generated=&spade_generated,
  .log_used=&spade_used,
  .log_informed=&spade_informed,
  .log_proc=&spade_proc,
  .log_task=&spade_task,
  .log_inode=&spade_inode,
  .log_str=NULL,
  .log_act_disc=&spade_act_disc,
  .log_agt_disc=&spade_agt_disc,
  .log_ent_disc=&spade_ent_disc,
  .log_msg=&spade_msg,
  .log_shm=&spade_shm,
  .log_packet=&spade_packet,
  .log_address=&spade_address,
  .log_file_name=&spade_file_name,
  .log_iattr=&spade_iattr,
  .log_xattr=&spade_xattr,
  .log_packet_content=&spade_packet_content,
  .log_arg=&spade_arg,
  .log_error=&log_error
};

struct provenance_ops ops_null = {
  .init=&init,
  .log_derived=NULL,
  .log_generated=NULL,
  .log_used=NULL,
  .log_informed=NULL,
  .log_proc=NULL,
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

    printf("Test\n");

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

    printf("Test\n");
    if(IS_CONFIG_MQTT()){
      syslog(LOG_INFO, "MQTT Provenance service");
      syslog(LOG_INFO, "Main process pid: %ld", getpid());
      init_mqtt();
      mqtt_connect(true);
      publish_json(__service_config.machine_topic, machine_description_json(json), true);
      set_ProvJSON_callback(mqtt_print_json);
    }else if(IS_CONFIG_LOG()){
      printf("Log\n");
      _init_logs();
      printf("Log init done\n");
      if (IS_FORMAT_W3C()) {
        log_print_w3c(machine_description_json(json));
        set_ProvJSON_callback(log_print_w3c);
      }else if (IS_FORMAT_SPADE_JSON()) {
        printf("We got that far.\n");
        log_print_spade_json(machine_description_spade_json());
        set_SPADEJSON_callback(log_print_spade_json);
        printf("Init went well.\n");
      }
    }

    printf("Here\n");

    if (IS_CONFIG_NULL())
      rc = provenance_relay_register(&ops_null, NULL);
    else if(IS_FORMAT_W3C())
      rc = provenance_relay_register(&w3c_ops, NULL);
    else if(IS_FORMAT_SPADE_JSON())
      rc = provenance_relay_register(&spade_json_ops, NULL);

    if(rc){
      syslog(LOG_ERR, "Failed registering audit operation.");
      exit(rc);
    }

    while(!terminate){
      if (!IS_CONFIG_NULL() && IS_FORMAT_W3C())
        flush_json();
      else if(!IS_CONFIG_NULL() && IS_FORMAT_SPADE_JSON())
        flush_spade_json();
      if(i%10==0 && IS_CONFIG_MQTT())
        mqtt_publish("keepalive", NULL, 0, false); // keep alive
      i++;
      sleep(1);
    }
    return 0;
}
