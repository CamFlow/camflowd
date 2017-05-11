# MQTT publisher service

## Build Status

| Branch | Status                                                                                  | SonarQube |
|--------|-----------------------------------------------------------------------------------------|-----------|
| master | [![Master Build Status](https://api.travis-ci.org/CamFlow/mqtt-publisher-service.svg?branch=master)](https://travis-ci.org/CamFlow/mqtt-publisher-service/branches)  |[![SonarQube Status](https://sonarqube.com//api/badges/gate?key=camflow%3Amqtt)](https://sonarqube.com/dashboard?id=camflow%3Amqtt)   |
| dev    | [![Dev Build Status](https://api.travis-ci.org/CamFlow/mqtt-publisher-service.svg?branch=dev)](https://travis-ci.org/CamFlow/mqtt-publisher-service/branches)      |[![SonarQube Status](https://sonarqube.com//api/badges/gate?key=camflow%3Amqtt%3Adev)](https://sonarqube.com/dashboard?id=camflow%3Amqtt%3Adev)   |

Automated Travis test run the following operation:
- run [SonarQube](https://sonarqube.com).

## Install

We assume CamFlow has been installed on your Linux machine, please see instruction [here](https://github.com/CamFlow/camflow-install) (bare-OS installation), or [here](https://github.com/CamFlow/vagrant) (setting up a guest VM).

``` SHELL
git clone https://github.com/tfjmp/mqtt-publisher-service.git
cd mqtt-publisher-service
make prepare
make all
make install
```

## Configuration

The file `/etc/camflow-mqtt.ini` allows to modify the configuration of the MQTT publisher service. The service need to be restarted for a new configuration to be applied (through `systemctl restart camflow-provenance.service`).

``` INI
[mqtt]
address=tcp://m12.cloudmqtt.com:17065
client_id=ExampleClientPub
username=camflow
password=test
; message delivered: 0 at most once, 1 at least once, 2 exactly once
qos=1
```

## Checking logs

```
cat /tmp/audit.log # CamFlow service logs
cat /tmp/mosquitto.log # Mosquitto logs
```
