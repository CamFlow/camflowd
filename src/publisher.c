#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <pthread.h>

#include "MQTTClient.h"
#include "provenancelib.h"
#include "provenanceutils.h"
#include "provenancePovJSON.h"
#include "simplog.h"
#include "ini.h"

#define	LOG_PATH "/tmp/audit.log"
#define CONFIG_PATH "/etc/camflow-mqtt.ini"
#define gettid() syscall(SYS_gettid)
#define TIMEOUT         10000L

MQTTClient client;

#define MAX_TOPIC_LENGTH 256
#define MAX_MQTT_CLIENT_ID_LENGTH 23 // see https://www.eclipse.org/paho/files/mqttdoc/Cclient/_m_q_t_t_client_8h.html#a5cb44bc0e06bcc95a314d51320a0cd1b

typedef struct{
  int qos;
  char address[PATH_MAX]; // assuming we could use unix socket
  char username[1024];
  char password[1024];
  char provenance_topic[MAX_TOPIC_LENGTH];
  char machine_topic[MAX_TOPIC_LENGTH];
  char client_id[MAX_MQTT_CLIENT_ID_LENGTH];
} configuration;

static configuration config;

#define MATCH(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)
/* call back for configuation */
static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;

    if(MATCH("mqtt", "qos")) {
      pconfig->qos = atoi(value);
      simplog.writeLog(SIMPLOG_INFO, "MQTT QOS %d", pconfig->qos);
    }else if (MATCH("mqtt", "address")) {
      strncpy(pconfig->address, value, PATH_MAX);
      simplog.writeLog(SIMPLOG_INFO, "MQTT address %s", pconfig->address);
    }else if(MATCH("mqtt", "username")){
      strncpy(pconfig->username, value, 1024);
      simplog.writeLog(SIMPLOG_INFO, "MQTT username %s", pconfig->username);
    }else if(MATCH("mqtt", "password")){
      strncpy(pconfig->password, value, 1024);
      simplog.writeLog(SIMPLOG_INFO, "MQTT password %s", pconfig->password);
    } else{
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

void mqtt_connect(bool cleansession){
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
  conn_opts.username = config.username;
  conn_opts.password = config.password;

  simplog.writeLog(SIMPLOG_INFO, "Connecting to MQTT... (%ld)", tid);
  if ((rc = MQTTClient_connect(client, &conn_opts)) < 0)
  {
      simplog.writeLog(SIMPLOG_ERROR, "Failed to connect, return code %d\n", rc);
      exit(-1);
  }
  simplog.writeLog(SIMPLOG_INFO, "Connected (%ld)", tid);
}

static pthread_mutex_t l_mqtt = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
/* publish payload on mqtt */
void mqtt_publish(char* topic, char* payload, int qos, bool retained){
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

    if( !MQTTClient_isConnected(client) ){
      mqtt_connect(false);
    }

    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

    pthread_mutex_unlock(&l_mqtt);

    if(rc != MQTTCLIENT_SUCCESS){
      simplog.writeLog(SIMPLOG_ERROR, "MQTT disconnected error: %d (%ld)", rc, tid);
      retry++;
    }
    if(retry > 10){
      simplog.writeLog(SIMPLOG_ERROR, "Failed connect retry (%ld)", tid);
      break;
    }
  }while(rc != MQTTCLIENT_SUCCESS);
}

void _init_logs( void ){
  simplog.setLogFile(LOG_PATH);
  simplog.setLineWrap(false);
  simplog.setLogSilentMode(true);
  simplog.setLogDebugLevel(SIMPLOG_VERBOSE);
  provenance_opaque_file(LOG_PATH, true);
}

void init( void ){
  pid_t tid = gettid();
  simplog.writeLog(SIMPLOG_INFO, "Init audit thread (%ld)", tid);
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
  simplog.writeLog(SIMPLOG_ERROR, "From library: %s", error);
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

void publish_json(char* topic, const char* json, bool retain){
  size_t len;
  char* buf;
  const size_t inlen = strlen(json);
  len = compress64encodeBound(inlen);
  buf = (char*)malloc(len);
  compress64encode(json, inlen, buf, len);
  mqtt_publish(topic, buf, config.qos, retain);
  free(buf);
}

void print_json(char* json){
  publish_json(config.provenance_topic, json, false);
}

int main(int argc, char* argv[])
{
    int rc;
    uint32_t i;
    uint32_t machine_id;
    char json[4096];

    _init_logs();
    simplog.writeLog(SIMPLOG_INFO, "MQTT Provenance service");
    simplog.writeLog(SIMPLOG_INFO, "Main process pid: %ld", getpid());

    memset(&config, 0, sizeof(configuration));

    if (ini_parse(CONFIG_PATH, handler, &config) < 0) {
        simplog.writeLog(SIMPLOG_ERROR, "Can't load '%s'", CONFIG_PATH);
        exit(-1);
    }

    rc = provenance_get_machine_id(&machine_id);
    if(rc<0){
      simplog.writeLog(SIMPLOG_ERROR, "Failed retrieving machine ID.");
      exit(rc);
    }
    snprintf(config.provenance_topic, MAX_TOPIC_LENGTH, "camflow/provenance/%u", machine_id);
    snprintf(config.machine_topic, MAX_TOPIC_LENGTH, "camflow/machines/%u", machine_id);
    snprintf(config.client_id, MAX_MQTT_CLIENT_ID_LENGTH, "%u", machine_id); // should be no more than 23
    simplog.writeLog(SIMPLOG_INFO, "Provenance topic: %s.", config.provenance_topic);
    simplog.writeLog(SIMPLOG_INFO, "Machine topic: %s.", config.machine_topic);

    MQTTClient_create(&client, config.address, config.client_id,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    mqtt_connect(true);

    publish_json(config.machine_topic, machine_description_json(json), true);

    rc = provenance_register(&ops);
    if(rc){
      simplog.writeLog(SIMPLOG_ERROR, "Failed registering audit operation.");
      exit(rc);
    }
    set_ProvJSON_callback(print_json);
    while(1){
      sleep(1);
      flush_json();
      if(i++%10==0){
        mqtt_publish("keepalive", NULL, 0, false); // keep alive
      }
    }

    // never reached
    provenance_stop();
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return 0;
}
