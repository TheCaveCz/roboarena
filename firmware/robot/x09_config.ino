
#define CONFIG_MAGIC 0x30

typedef struct {
  uint8_t magic;
  uint8_t roboId;
  uint16_t m1maxPwm;
  uint16_t m2maxPwm;
} Config;

Config config;

bool configIdValid() {
  return config.roboId >= 1 && config.roboId <= 4;
}

uint32_t configColorForId() {
  switch (config.roboId) {
    case 1: return 0x600000;
    case 2: return 0x000060;
    case 3: return 0xc04000;
    case 4: return 0x006000;
    default: return 0;
  }
}

void configWrite() {
  logInfo("Writing config");

  config.magic = CONFIG_MAGIC;
  EEPROM.put(0, config);
  EEPROM.commit();
}

void configRead() {
  logInfo("Reading config");

  EEPROM.get(0, config);
  if (config.magic != CONFIG_MAGIC) {
    logInfo("Unable to read config");

    memset(&config, 0, sizeof(config));
    config.m1maxPwm = PWMRANGE;
    config.m2maxPwm = PWMRANGE;

    configWrite();
  }

  logValue(" roboId: ", config.roboId);
  logValue(" m1pwm: ", config.m1maxPwm);
  logValue(" m2pwm: ", config.m2maxPwm);
  if (!configIdValid()) {
    logInfo(" THIS ID IS INVALID!!");
  }
}

void configSetup() {
  EEPROM.begin(512);
  configRead();
}
