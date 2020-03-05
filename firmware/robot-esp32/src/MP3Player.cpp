#include "MP3Player.h"
#include <driver/i2s.h>
#include <log.h>

#define USED_I2S I2S_NUM_0


static void dumpFrameInfo(const MP3FrameInfo &fi) {
  logValue("mp3: bitrate = ", fi.bitrate);
  logValue("mp3: bps = ", fi.bitsPerSample);
  logValue("mp3: layer = ", fi.layer);
  logValue("mp3: chans = ", fi.nChans);
  logValue("mp3: outSamps = ", fi.outputSamps);
  logValue("mp3: sampRate = ", fi.samprate);
  logValue("mp3: version = ", fi.version);
}


MP3Player::~MP3Player() {
  _inputFile.close();
  if (_hMP3Decoder) {
    MP3FreeDecoder(_hMP3Decoder);
    _hMP3Decoder = NULL;
  }
}

bool MP3Player::begin(uint8_t bck, uint8_t ws, uint8_t data) {
  _hMP3Decoder = MP3InitDecoder();
  if (_hMP3Decoder == NULL) {
    logInfo("mp3: failed to init decoder");
    return false;
  }

  if (!initI2S(bck, ws, data)) {
    return false;
  }

  return true;
}

bool MP3Player::initI2S(uint8_t bck, uint8_t ws, uint8_t data) {
  i2s_config_t i2sc;
  i2sc.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2sc.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  i2sc.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB);
  i2sc.dma_buf_count = 16;
  i2sc.dma_buf_len = 256;
  i2sc.fixed_mclk = 0;
  i2sc.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  i2sc.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  i2sc.sample_rate = MP3_SAMPLERATE;
  i2sc.tx_desc_auto_clear = true;
  i2sc.use_apll = true;
  if (i2s_driver_install(USED_I2S, &i2sc, 0, NULL) != ESP_OK) {
    logInfo("mp3: failed to install driver");
    return false;
  }

  i2s_pin_config_t pins = {.bck_io_num = bck, .ws_io_num = ws, .data_out_num = data, .data_in_num = I2S_PIN_NO_CHANGE};
  if (i2s_set_pin(USED_I2S, &pins) != ESP_OK) {
    logInfo("mp3: failed to set i2s pins");
    return false;
  }

  if (i2s_zero_dma_buffer(USED_I2S) != ESP_OK) {
    logInfo("mp3: failed to flush buffer");
    return false;
  }
  return true;
}

bool MP3Player::play(const char *file) {
  stop();
  logInfo("mp3: opening");
  _inputFile = SPIFFS.open(file);
  if (_inputFile) {
    logValue("mp3: play ", file);
    _playing = true;
  } else {
    logValue("mp3: failed to open ", file);
  }
  return _playing;
}

void MP3Player::stop() {
  i2s_zero_dma_buffer(USED_I2S);
  _inputFile.close();
  _outSize = 0;
  _inputSize = 0;
  _playing = false;
}

size_t MP3Player::fetchInputData(void *buffer, size_t size) { return _inputFile.read((uint8_t *)buffer, size); }

bool MP3Player::loop() {
  if (_hMP3Decoder == NULL || !_playing)
    return false;

  if (_outSize) {
    // if we have decoded samples, send them to i2s
    size_t written;

    i2s_write(USED_I2S, _outPtr, _outSize, &written, 0);
    _outPtr += written;
    _outSize -= written;
  }

  // still something to send in outBuffer? then return
  if (_outSize)
    return _playing;

  // no more samples -> decode next mp3 frame
  if (_inputSize == 0) {
    // fetch new buffer
    _inputSize = fetchInputData(_inputBuffer, sizeof(_inputBuffer));
    if (_inputSize) {
      _inputPtr = _inputBuffer;
    } else {
      // still no data? either song ended or fetch failed, stop anyway
      logInfo("mp3: stopped");
      stop();
      return _playing;
    }
  }

  MP3FrameInfo frameInfo;
  int error = MP3Decode(_hMP3Decoder, &_inputPtr, &_inputSize, _outSamples, 0);
  if (error == ERR_MP3_NONE) {
    // reset pointers to output samples
    MP3GetLastFrameInfo(_hMP3Decoder, &frameInfo);
    _outPtr = (uint8_t *)_outSamples;
    _outSize = frameInfo.outputSamps * frameInfo.bitsPerSample / 8;

  } else {
    // decode error, cancel playback
    logValue("mp3: decoder error ", error);
    stop();
  }

  return _playing;
}
