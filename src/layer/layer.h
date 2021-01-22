#pragma once

#include "midier/config/config.h"
#include "midier/debug/debug.h"
#include "midier/degree/degree.h"
#include "midier/mode/mode.h"
#include "midier/midi/midi.h"
#include "midier/note/note.h"
#include "midier/octave/octave.h"
#include "midier/rhythm/rhythm.h"
#include "midier/time/time.h"

namespace midier
{

struct Layer
{
    Layer() = default;
    Layer(
#ifdef DEBUG
        unsigned char id,
#endif
        Degree chord,
        unsigned char delay,
        Config * config);

    // queries
    bool idle() const;
    bool running() const;
    bool waiting() const;
    bool wandering() const;
    bool looping() const;

    // actions
    void stop();
    void record();
    void revoke();

    // runs all the logic of this layer once
    void click();

#ifdef DEBUG
    unsigned char id;
#endif

    Degree chord;
    Time start;
    config::Viewed config;
    midi::Velocity velocity = midi::Velocity::High;

    // Layer::Period is the number represents the periodity of the bar index. the bar index is incremented
    // every time a full bar has passed since the layer was first started. for finite layers, the bar index
    // is set to (-1) when leaving the recorded area, and is reset to 0 when we re-enter the recorded area.
    // for infinite layers, the bar index is (logically) never reset to 0. in order to support infinite layers,
    // we must be able to cyclically increment and reset the bar index once in reaches a certain value. this
    // is the value. the bar index is only used as an argument to the rhythmer, which outputs the index of the
    // note that is being played, since the beginning of time, and this note index is being used as an argument
    // for the styler, which outputs the chord degree that should be played. we support styles with periodity
    // of 3/4/5/6 notes (4/6/8/10 when looped), therefore we want the note index - which is the output of the rhythmer,
    // and an input for the styler - to be with periodity of any multiplication of 120, so it would be divisible by
    // all possible numbers of notes in all styles. therefore, we have to make the rhythmer output indexes with
    // such periodity. the following formula descibes the output (y) of the rhythmer: y = i + (b/l) * c. where 'i'
    // is the note index within the rhythm, 'b' is the bar index, 'l' is the number of bars a rhythm is spanned over,
    // and 'c' is the number of notes played in the rhythm (over 'l' bars). the following table show the values
    // of those variables every click:
    //
    // i: 0, 1, ... c-1, 0, 1, ... c-1, 0,  1, ... c-1, ... 0    ... 0      ... 0
    // b: 0, ?, ... ?,   l, ?, ... ?,   2l, ?, ... ?,   ... B    ... 2B     ... 4B
    // y: 0, 1, ...                                         120x ... 120x*2 ... 120x*4
    //
    // we are looking for 'B', and we know that, when 'b' will be equal to 'B':
    // 1) y = i + B/l * c = B/l * c (because 'i' must be 0)
    // 2) y = 120 * x
    // from (1) and (2) we get: B/l * c = 120 * x -> B = 120 * x * l / c
    //
    // 'B' has to be an integer, and if we choose 'x' to be equal to 'c', 'B' is guaranteed to be an integer,
    // and 'y' will have the periodity of 120 * c, which is good for us (as it's a multiplication of 120).
    // the (currently) supported values for 'l' are 1/2/3/4, and therefore we have to find a number that
    // is a multiplication of 120, 240, 360, 480 and we get 1440.
    //
    constexpr static auto Period = 1440;

    // the index of the bar within the logical loop it's being played in
    short bar = -1; // must be of a big enough size in order to hold Layer::Period

private:
    enum class State : char
    {
        Idle,
        Wait,
        Wander,
        Record,
        Playback,
    };

    State _state = State::Idle;

    struct {
        // the layer is played at `start` and is not played at `end`; [start,end)
        Time start;
        Time end;
        short bar = -1; // the bar index in which we recorded
    } _loop;

    struct {
        // information about the last MIDI note number that was played
        // at most one MIDI note is played at every moment by a layer
        // this means that we support gate values of no more than 100% (legato)
        char subdivisions = -1; // how many subdivisions the note has been playing for
        midi::Number numbers[3];
    } _played;
};

} // midier
