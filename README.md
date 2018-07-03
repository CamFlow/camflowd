# CamFlow Service [![Master Build Status](https://api.travis-ci.org/CamFlow/camflowd.svg?branch=master)](https://travis-ci.org/CamFlow/camflow-service/branches)

## Build Status

| Branch | Status                                                                                  |
|--------|-----------------------------------------------------------------------------------------|
| master | [![Master Build Status](https://api.travis-ci.org/CamFlow/camflowd.svg?branch=master)](https://travis-ci.org/CamFlow/camflow-service/branches)  |
| dev    | [![Dev Build Status](https://api.travis-ci.org/CamFlow/camflowd.svg?branch=dev)](https://travis-ci.org/CamFlow/camflow-service/branches)      |

Automated Travis test run the following operation:
- run [SonarQube](https://sonarqube.com).

## Configuration

The file `/etc/camflow-service.ini` allows to modify the configuration of the MQTT publisher service. The service need to be restarted for a new configuration to be applied (through `systemctl restart camflow-provenance.service`).

``` INI
[general]
; output=null
; output=mqtt
; output=unix_socket
; output=fifo
output=log

format=w3c
;format=spade_json

[log]
path=/tmp/audit.log

[mqtt]
address=m12.cloudmqtt.com:17065
username=camflow
password=test
; message delivered: 0 at most once, 1 at least once, 2 exactly once
qos=2

[unix]
address=/tmp/camflowd.sock

[fifo]
path=/tmp/camflowd-pipe
```

## Checking logs

```
journalctl -b | grep camflowd
```
