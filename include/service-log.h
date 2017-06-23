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
#ifndef __SERVICE_LOG_H
#define __SERVICE_LOG_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "service-config.h"

static FILE *__log_fp=NULL;

static inline void _init_logs( void ){
  __log_fp = fopen(__service_config.log, "a+");
  if(!__log_fp){
    syslog(LOG_ERR, "Cannot open file\n");
    exit(-1);
  }
  syslog(LOG_INFO, "Starting audit service...\n");
  provenance_opaque_file(__service_config.log, true);
}

static inline void log_print_json(char* json){
  fprintf(__log_fp, json);
  fprintf(__log_fp, "\n");
  fflush(__log_fp);
}
#endif
