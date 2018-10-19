| Tool version    | Date       |
| --------------- | ---------- |
| 0.2.2           | N/A        |
| 0.2.1           | 27/05/2018 |
| 0.2.0           | 26/05/2018 |
| 0.1.8           | 12/12/2017 |
| 0.1.7           | 28/09/2017 |
| 0.1.6           | 28/08/2017 |
| 0.1.5           | 27/08/2017 |
| 0.1.4           | 07/08/2017 |
| 0.1.3           | 03/08/2017 |
| 0.1.2           | 02/08/2017 |
| 0.1.1           | 23/06/2017 |
| 0.1.0           | 22/05/2017 |

### v0.2.2
```
- Change to SPADE formatting.
- Update epoch on start up.
```

### v0.2.1
```
- Fix MQTT configuration bug.
```

### v0.2.0
```
- Support several output medium (file, mqtt, unix socket, fifo), see camflowd.ini.
- Support several output formats (w3c, spade_json), see camflowd.ini.
- Thread level provenance support.
```

### v0.1.8
```
- Minor change related to libprovenance API changes.
```

### v0.1.7
```
- One hook per disclosed vertex type (entity, activity, agent).
- Use libprovenance as a shared library.
```

### v0.1.6
```
- Remove unknown relation.
```

### v0.1.5
```
- Handle SIGTERM properly.
```

### v0.1.4
```
- Use BSD library for safer string concatenation see (https://github.com/camflow/libprovenance).
```

### v0.1.3
```
- Fix issue with very large log entries.
```

### v0.1.2
```
- Added null log to use in some test scenario.
```

### v0.1.1
```
- Write log to syslog.
- Build against paho.mqtt static library.
```

### v0.1.0
```
- initial release.
```
