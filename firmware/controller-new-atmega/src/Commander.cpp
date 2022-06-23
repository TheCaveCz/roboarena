#include "Commander.h"

Commander::Commander() {
  _pos = 0;
  _buf[0] = 0;
  _cmds = NULL;
  _of = 0;
}

void Commander::begin(const Command *commands, Stream *serial) {
  _cmds = commands;
  _serial = serial;
}

void Commander::check() {
  while (_serial->available()) {
    int d = _serial->read();
    if (d == '\n') {
      if (_of) {
        _of = 0;
        _pos = 0;
        //_serial->println(F("-Command too long"));
      } else if (_pos) {
        _buf[_pos] = 0;
        exec();
        _pos = 0;
      }
    } else if (d >= 0 && d != '\r') {
      _buf[_pos++] = d;
      if (_pos >= sizeof(_buf)) {
        _pos = 0;
        _of = 1;
      }
    }
  }
}

void Commander::printError() { _serial->println(F("-Error")); }

void Commander::printOk() { _serial->println(F("+OK")); }

bool Commander::checkHex(const uint8_t val) {
  return (val >= '0' && val <= '9') || (val >= 'A' && val <= 'F') || (val >= 'a' && val <= 'f');
}

bool Commander::checkHex(const char *buf, const uint8_t from, const uint8_t to) {
  for (uint8_t i = from; i <= to; i++) {
    if (!checkHex(buf[i]))
      return false;
  }
  return true;
}

uint16_t Commander::parseHexWord(const char *buf, const uint8_t from) {
  return (parseHex(buf[from], buf[from + 1]) << 8) | parseHex(buf[from + 2], buf[from + 3]);
}

uint8_t Commander::parseHex(const uint8_t v1, const uint8_t v2) { return (parseHex(v1) << 4) | parseHex(v2); }

uint8_t Commander::parseHex(const uint8_t val) {
  if (val >= '0' && val <= '9')
    return val - '0';
  if (val >= 'A' && val <= 'F')
    return val - 'A' + 10;
  if (val >= 'a' && val <= 'f')
    return val - 'a' + 10;
  return 0;
}

bool Commander::checkRange(const uint8_t val, const uint8_t minVal, const uint8_t maxVal) {
  return val >= minVal && val <= maxVal;
}

void Commander::printHex(const uint8_t b) {
  if (b < 16)
    _serial->print('0');
  _serial->print(b, HEX);
}

void Commander::printHexWord(const uint16_t w) {
  printHex(w >> 8);
  printHex(w & 0xff);
}

void Commander::printCommands() {
  uint8_t i = 0;
  while (_cmds[i].code) {
    _serial->write('>');
    _serial->write(_cmds[i].code);
    _serial->write(' ');
    _serial->println(_cmds[i].len);
    i++;
  }
  printOk();
}

void Commander::exec() {
  if (_cmds == NULL) {
    _serial->println(F("-Missing cmd table"));
    return;
  }

  uint8_t i = 0;
  while (_cmds[i].code) {
    if (_cmds[i].code == _buf[0] && _cmds[i].len == _pos) {
      if (_cmds[i].callback) {
        (*_cmds[i].callback)(_buf, _pos);
      } else {
        _serial->println(F("-Missing callback"));
      }
      return;
    }
    i++;
  }
  //_serial->println(F("-Unknown command"));
}
