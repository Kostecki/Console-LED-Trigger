#include "serial_mux.h"

// This file needs access to the REAL Serial/Serial0.
// The macros above will be in effect here too (because we include the header),
// so immediately undo them in THIS translation unit:
#ifdef Serial
#undef Serial
#endif
#ifdef Serial0
#undef Serial0
#endif

SerialMirror DebugSerial;

void SerialBegin(unsigned long baud)
{
  ::Serial.begin(baud);                      // USB CDC (baud ignored but fine)
  ::Serial0.begin(baud, SERIAL_8N1, 20, 21); // UART0 on RX=20, TX=21
}
