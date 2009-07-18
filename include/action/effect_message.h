#ifndef _EFFECT_MSG_H
#define _EFFECT_MSG_H

#include "action/bindings.h"
#include <tilesense.h>

//I wanted to use const int = stuff here, but case statements didn't like it
#define SalMessage StimUser1

struct _effect_message {
  char *message;
  char *target, *sensorID;
};
typedef struct _effect_message *EffectMessage;

EffectMessage effect_message_new();
EffectMessage effect_message_init(EffectMessage m, char *msg, char *target, char *sensorID);
void effect_message_free(EffectMessage m);

void effect_message_request_bindings(EffectMessage m, Bindings b);
void effect_message_send(EffectMessage m, Bindings b);

#endif