#pragma once

#include "../note/note.h"
#include "../octave/octave.h"

namespace midier
{
namespace midi
{

// represents a MIDI note number
using Number = unsigned char;

// calculate MIDI note number from a musical note and an octave
Number number(Note note, Octave octave);

// send a 'NOTE_ON' MIDI command
void on(Number number, unsigned velocity = 127); // by default max velocity

// send a 'NOTE_OFF' MIDI command
void off(Number number);

// play a musical note for a specific duration of time (in ms)
void play(Note note,                unsigned duration = 200);
void play(Note note, Octave octave, unsigned duration = 200);

} // midi
} // midier
