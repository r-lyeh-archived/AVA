// [ref] https://github.com/OculusVR/Flicks
//
// A flick (frame-tick) is a very small unit of time. It is 1/705600000 of a second, exactly.
// 
// 1 flick = 1/705600000 second
// 
// This unit of time is the smallest time unit which is LARGER than a nanosecond, 
// and can in integer quantities exactly represent a single frame duration for 
// 24 Hz, 25 Hz, 30 Hz, 48 Hz, 50 Hz, 60 Hz, 90 Hz, 100 Hz, 120 Hz, and also 
// 1/1000 divisions of each, as well as a single sample duration for 8 kHz, 
// 16 kHz, 22.05 kHz, 24 kHz, 32 kHz, 44.1 kHz, 48 kHz, 88.2 kHz, 96 kHz, and 
// 192kHz, as well as the NTSC *variations* frame durations for 24 * (1000/1001) Hz, 
// 30 * (1000/1001) Hz, 60 * (1000/1001) Hz, and 120 * (1000/1001) Hz.

#ifndef FLICKS_PER_SECOND
#define FLICKS_PER_SECOND          705600000ULL

#define FLICKS_PER_24_HZ            29400000ULL
#define FLICKS_PER_24_HZ_NTSC       29429400ULL // ~23.976 Hz
#define FLICKS_PER_25_HZ            28224000ULL
#define FLICKS_PER_30_HZ            23520000ULL
#define FLICKS_PER_30_HZ_NTSC       23543520ULL // ~29.97 Hz
#define FLICKS_PER_48_HZ            14700000ULL
#define FLICKS_PER_50_HZ            14112000ULL
#define FLICKS_PER_60_HZ            11760000ULL
#define FLICKS_PER_60_HZ_NTSC       11771760ULL // ~59.94 Hz
#define FLICKS_PER_90_HZ             7840000ULL
#define FLICKS_PER_100_HZ            7056000ULL
#define FLICKS_PER_120_HZ            5880000ULL
#define FLICKS_PER_120_HZ_NTSC       5885880ULL // ~119.88 Hz

#define FLICKS_PER_8000_HZ             88200ULL
#define FLICKS_PER_16000_HZ            44100ULL
#define FLICKS_PER_22050_HZ            32000ULL
#define FLICKS_PER_24000_HZ            29400ULL
#define FLICKS_PER_32000_HZ            22050ULL
#define FLICKS_PER_44100_HZ            16000ULL
#define FLICKS_PER_48000_HZ            14700ULL
#define FLICKS_PER_88200_HZ             8000ULL
#define FLICKS_PER_96000_HZ             7350ULL
#define FLICKS_PER_192000_HZ            3675ULL
#endif
