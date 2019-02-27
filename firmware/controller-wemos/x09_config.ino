
#define CONFIG_MAGIC 0x29

typedef struct {
  uint8_t magic;
  //uint8_t roboId;
} Config;

Config config;

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

    configWrite();
  }

//  logValue(" roboId: ", config.roboId);
}

void configSetup() {
  EEPROM.begin(512);
  configRead();
}
