#pragma once

#include <Arduino.h>
#include "kwconfiginterface.h"

class KWCLI
{
public:
    KWCLI();
    KWCLI(KWConfigInterface* cfg);

    void begin();
    void process();
    void processCommand(String command, int mode=1);//1-->entry from board 2-->entry from user

    bool isActive();
    void setActive(bool active);

private:
    bool active;
    bool unlocked;

    String inputBuffer;

    KWConfigInterface* config;

};