// Ina226 plugin

#include "Ina226Plugin.h"
#include "../../Common.h"
#include "../../lib/serial/Serial_Local.h"
#include "../../lib/tasks/OnTask.h"

#include "INA226.h"

INA226 ina226(INA226_ADDRESS);

void ina226PluginWrapper() { ina226Plugin.loop(); }

static inline float estimateSOC(float voltage)
{
    const float voltage_points[] = {18.0f, 19.2f, 20.4f, 21.6f, 22.8f, 23.6f, 24.4f, 25.2f};
    const float soc_percent[] = {0.0f, 10.0f, 25.0f, 50.0f, 70.0f, 80.0f, 90.0f, 100.0f};
    const int count = sizeof(voltage_points) / sizeof(voltage_points[0]);

    if (voltage <= voltage_points[0])
        return 0.0f;
    if (voltage >= voltage_points[count - 1])
        return 100.0f;

    for (int i = 1; i < count; ++i)
    {
        if (voltage <= voltage_points[i])
        {
            float v1 = voltage_points[i - 1];
            float v2 = voltage_points[i];
            float soc1 = soc_percent[i - 1];
            float soc2 = soc_percent[i];
            float ratio = (voltage - v1) / (v2 - v1);
            return soc1 + ratio * (soc2 - soc1);
        }
    }
    return 100.0f;
}

void Ina226Plugin::init()
{
    VLF("MSG: Plugins, starting: Ina226Plugin");

    // Initialize INA device. IMPORTANT:
    // - If OnStepX core already initialized I2C/Wire, DO NOT call Wire.begin() here again.
    // - If you need to init I2C pins, ensure it won't conflict with core. Consult OnStepX pinmap.
    if (!ina226.begin())
    {
        VLF("ERR: INA initialize failed!");
        // don't return: still register task so plugin remains non-fatal (optional)
    }
    else
    {
        VLF("MSG: INA initialize succeed!");
        // example config: set expected shunt and max if your INA lib supports it
        // ina.setMaxCurrentShunt(1, 0.002);
    }
    www.on(INA226_PLUGIN_PATH, HTTP_GET, std::bind(&Ina226Plugin::handleHttp, this));
}

void Ina226Plugin::handleHttp()
{
  float busV     = ina.getBusVoltage();
  float curr_mA  = ina.getCurrent_mA();
  float power_mW = ina.getPower_mW();
  float shunt_mV = ina.getShuntVoltage_mV();
  float percent  = estimateSOC(busV);

  String out;
  out.reserve(160);
  out += "bus_V=";      out += String(busV, 3);     out += "\n";
  out += "current_mA="; out += String(curr_mA, 2);  out += "\n";
  out += "power_mW=";   out += String(power_mW, 2); out += "\n";
  out += "shunt_mV=";   out += String(shunt_mV, 2); out += "\n";
  out += "batterySOC="; out += String(percent, 1);  out += "%\n"; 

  www.send(200, "text/plain", out);
}

Ina226Plugin ina226Plugin;