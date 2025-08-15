#pragma once
#include <Arduino.h>
#include <HardwareSerial.h>

// Mirror that writes to both USB CDC (::Serial) and UART0 (::Serial0)
class SerialMirror : public Stream
{
public:
  // Print
  size_t write(uint8_t c) override
  {
    ::Serial.write(c); // always USB
    if (::Serial0.availableForWrite() > 0)
      ::Serial0.write(c); // non-blocking-ish UART
    return 1;
  }
  using Print::write;

  // Stream (minimal)
  int available() override { return ::Serial.available() + ::Serial0.available(); }
  int read() override
  {
    if (::Serial.available())
      return ::Serial.read();
    if (::Serial0.available())
      return ::Serial0.read();
    return -1;
  }
  int peek() override
  {
    if (::Serial.available())
      return ::Serial.peek();
    if (::Serial0.available())
      return ::Serial0.peek();
    return -1;
  }
  void flush() override
  {
    ::Serial.flush();
    ::Serial0.flush();
  }
};

// Global instance + init helper
extern SerialMirror DebugSerial;
void SerialBegin(unsigned long baud);

// --------- IMPORTANT: place macros at the VERY BOTTOM ---------
// Use in YOUR files to make every Serial.* print go to both ports.
#define Serial DebugSerial
#define Serial0 DebugSerial
