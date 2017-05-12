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

#include "provenancelib.h"
#include "provenanceutils.h"
#include "provenancePovJSON.h"

#include "service-config.h"
#include "service-log.h"
#include "service-mqtt.h"

void init( void ){
  pid_t tid = gettid();
  log("Init audit thread (%ld)", tid);
}

void log_str(struct str_struct* data){
  append_entity(str_msg_to_json(data));
}

void log_unknown_relation(struct relation_struct* relation){
  append_relation(relation_to_json(relation));
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

void log_disc(struct disc_node_struct* node){
  switch(node->identifier.node_id.type){
    case ACT_DISC:
      append_activity(disc_to_json(node));
      break;
    case AGT_DISC:
      append_agent(disc_to_json(node));
      break;
    case ENT_DISC:
    default:
      append_entity(disc_to_json(node));
      break;
  }
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

void log_error(char* error){
  log("From library: %s", error);
}

struct provenance_ops ops = {
  .init=&init,
  .log_unknown_relation=&log_unknown_relation,
  .log_derived=&log_derived,
  .log_generated=&log_generated,
  .log_used=&log_used,
  .log_informed=&log_informed,
  .log_task=&log_task,
  .log_inode=&log_inode,
  .log_str=&log_str,
  .log_disc=&log_disc,
  .log_msg=&log_msg,
  .log_shm=&log_shm,
  .log_packet=&log_packet,
  .log_address=&log_address,
  .log_file_name=&log_file_name,
  .log_iattr=&log_iattr,
  .log_xattr=&log_xattr,
  .log_packet_content=&log_packet_content,
  .log_error=&log_error
};

int main(int argc, char* argv[])
{
    int rc;
    uint32_t i;
    char json[4096];
    printf("Hello world!\n");
    read_config();
    printf("%s\n", __service_config.log);
    _init_logs();
    printf("%s\n", __service_config.log);
    log("Output option: %s", __service_config.output);

    if(IS_CONFIG_MQTT()){
      log("MQTT Provenance service");
      log("Main process pid: %ld", getpid());
      init_mqtt();
      mqtt_connect(true);
      publish_json(__service_config.machine_topic, machine_description_json(json), true);
      set_ProvJSON_callback(mqtt_print_json);
    }else{
      log_print_json(machine_description_json(json));
      set_ProvJSON_callback(log_print_json);
    }
    rc = provenance_register(&ops);
    if(rc){
      log("Failed registering audit operation.");
      exit(rc);
    }
    while(1){
      sleep(1);
      flush_json();
      if(i++%10==0){
        if(IS_CONFIG_MQTT())
          mqtt_publish("keepalive", NULL, 0, false); // keep alive
      }
    }
    // never reached
    provenance_stop();
    if(IS_CONFIG_MQTT())
      stop_mqtt();
    return 0;
}
