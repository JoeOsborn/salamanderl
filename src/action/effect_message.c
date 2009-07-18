#include "action/effect_message.h"
#include <stdlib.h>
#include <string.h>

EffectMessage effect_message_new() {
  return calloc(1, sizeof(struct _effect_message));
}
EffectMessage effect_message_init(EffectMessage m, char *msg, char *target, char *sensorID) {
  m->message = strdup(msg);
  m->target = strdup(target);
  m->sensorID = sensorID ? strdup(sensorID) : NULL;
  return m;
}
void effect_message_free(EffectMessage m) {
  free(m->message);
  free(m->target);
  if(m->sensorID) { 
    free(m->sensorID);
  }
  free(m);
}
void effect_message_request_bindings(EffectMessage m, Bindings b) {
  bindings_insert(b, m->target, NULL);
}
void effect_message_send(EffectMessage m, Bindings b) {
  Object o = bindings_get_value_path(b, m->target);
  Sensor s=NULL;
  if(m->sensorID) {
    s = object_get_sensor_named(o, m->sensorID);
  }
  if(!s && object_sensor_count(o) > 0) {
    #warning potentially unsafe assumption
    s = object_get_sensor(o, 0);
  }
  if(s) {
    sensor_push_stimulus(s, stimulus_init_user(stimulus_new(), SalMessage, strdup(m->message)));
  }
}
