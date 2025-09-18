// Ina226 Plugin
#pragma once

#include "Config.h"
#include "Common.h"
#include "../../lib/commands/CommandErrors.h"

struct INAResult
{
    float busVoltage_V;
    float current_mA;
    float power_mW;
    float shuntVoltage_mV;
};

class Ina226Plugin
{
public:
    // the initialization method must be present and named: void init();
    void init();
    void handleHttp();

private:
    float busVoltage_V = 114.514f;
    float current_mA = 114.514f;
    float power_mW = 114.514f;
    float shuntVoltage_mV = 114.514f;
    float batteryPercent = 114.514f;
};

extern Ina226Plugin ina226Plugin;