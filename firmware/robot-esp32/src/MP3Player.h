#pragma once

#include <Arduino.h>
#include <mp3dec.h>
#include <SPIFFS.h>

// plays only mpeg-1 layer 3 stereo files with specified bitrate and samplerate
// no ID3 tags or padding!

#define MP3_SAMPLERATE 48000
#define MP3_BITRATE 128000
#define MP3_FRAME_SIZE (144 * MP3_BITRATE / MP3_SAMPLERATE)

class MP3Player {
public:
  virtual ~MP3Player();
  bool begin(uint8_t bck, uint8_t ws, uint8_t data);

  bool loop();

  void stop();
  bool play(const char *file);
  bool play(const String &file) { return play(file.c_str()); }

  bool isPlaying() { return _playing; }

protected:
  virtual size_t fetchInputData(void *buffer, size_t size);

private:
  HMP3Decoder _hMP3Decoder;
  uint8_t _inputBuffer[MP3_FRAME_SIZE * 4];
  uint8_t *_inputPtr;
  int _inputSize;

  int16_t _outSamples[1152 * 2];
  uint8_t *_outPtr;
  size_t _outSize;

  bool _playing;
  File _inputFile;

  bool initI2S(uint8_t bck, uint8_t ws, uint8_t data);
};
