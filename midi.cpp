#include "midi.h"

#include <Arduino.h>

namespace midiate
{
namespace midi
{

namespace
{

void send(byte command, byte data1, byte data2, byte channel)
{
    Serial.write((command & 0xF0) | (channel & 0x0F));
    Serial.write(data1 & 0x7F);
    Serial.write(data2 & 0x7F);
}

} //

void play(const Pitch & pitch)
{
    constexpr auto channel = 0;

    const auto note = static_cast<int>(pitch); // the actual midi note number

    send(0x90, note, 0x7F, channel); // note on on max velocity
    send(0x80, note, 0, channel); // note off
}

} // midi
} // midiate
