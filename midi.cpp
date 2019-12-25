#include "midi.h"
#include "scale.h"
#include "triad.h"

#include <Arduino.h>

namespace midiate
{
namespace midi
{

namespace
{

void send(byte command, byte data1, byte data2)
{
    constexpr auto channel = 0;

    Serial.write((command & 0xF0) | (channel & 0x0F));
    Serial.write(data1 & 0x7F);
    Serial.write(data2 & 0x7F);
}

} //

void play(Note root, Octave octave, Mode mode, Degree scale, Degree chord)
{
    const auto note = root
        + scale::interval(mode, scale)
        + triad::interval(
            scale::quality(mode, scale),
            chord);

    const auto number = 24 + (12 * (octave - 1)) + static_cast<char>(note);

    send(0x90, number, 0x7F); // note on on max velocity
    send(0x80, number, 0); // note off
}

} // midi
} // midiate
