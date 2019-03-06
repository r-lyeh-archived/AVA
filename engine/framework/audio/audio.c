// [x] mp3
// [x] mp1
// [ ] sfxr (hack)
// [x] ogg
// [x] wav
// [x] flac
// [x] mod
// [x] xm

#ifndef AUDIO_H
#define AUDIO_H
#include <stdbool.h>

API typedef struct audio_handle* audio;

API bool audio_init( int flags );
API void audio_drop();

API audio audio_loadclip( const char *pathfile );
API audio audio_loadstream( const char *pathfile );
API audio audio_play( audio s, int flags );
API audio audio_delete( audio s );

#endif



#ifdef AUDIO_C
#pragma once
#include "engine.h" // detect, realloc

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define DR_MP3_IMPLEMENTATION
#include "3rd/dr_mp3.h"

#include "3rd/jo_mp1.c"

#include "3rd/sfxr.h"

#define error stb_vorbis_error
#define get_bits stb_vorbis_get_bits
#include "3rd/stb_vorbis.c"
#undef error

#define DR_WAV_IMPLEMENTATION
#include "3rd/dr_wav.h"

#define DR_FLAC_IMPLEMENTATION
#include "3rd/dr_flac.h"

#define channel jar_channel
#define JAR_MOD_IMPLEMENTATION
#include "3rd/jar_mod.h"
#undef DEBUG
#undef channel

#define JAR_XM_IMPLEMENTATION
#include "3rd/jar_xm.h"

#define STS_MIXER_IMPLEMENTATION
#include "3rd/sts_mixer.h"

#define MINIAUDIO_IMPLEMENTATION
#include "3rd/miniaudio.h"


// encapsulate drflac and some buffer with the sts_mixer_stream_t
enum { UNK, WAV, MOD, XM, FLAC, OGG, SFXR, MP1, MP3 };
typedef struct {
    int type;
    union {
        drwav wav;
        jar_mod_context_t mod;
        jar_xm_context_t *xm;
        stb_vorbis *ogg;
        void *opaque;
        drflac*         flac;               // FLAC decoder state
        drmp3           mp3_;
    };
    sts_mixer_stream_t  stream;             // mixer stream
    union {
    int32_t             data[4096*2];       // static sample buffer
    float               dataf[4096*2];
    };
} mystream_t;

void downmix_to_mono( int channels, short *buffer, int samples ) {
    if( channels > 1 ) {
        float mix;
        short *output = buffer;
        while( samples-- > 0 ) {
            float mix = 0;
            for( int i = 0; i < channels; ++i ) mix += *buffer++;
            *output++ = (short)(mix / channels);
        }
    }
}
void downmix_to_monof( int channels, float *buffer, int samples ) {
    if( channels > 1 ) {
        float mix;
        float *output = buffer;
        while( samples-- > 0 ) {
            float mix = 0;
            for( int i = 0; i < channels; ++i ) mix += *buffer++;
            *output++ = (float)(mix / channels);
        }
    }
}

void *readfile( const char *filename, int *bytes ) {
    void *ptr = 0;
    for( FILE *fp = fopen( filename, "rb" ); fp; (fclose(fp), fp = 0)) {
        fseek( fp, 0L, SEEK_END );
        size_t tell = ftell( fp );
        fseek( fp, 0L, SEEK_SET );
        ptr = realloc( ptr, tell );
        if( ptr ) {
            fread( ptr, 1, tell, fp );
            if( bytes ) {
                *bytes = (int)tell;
            }
        }
    }
    return ptr;
}

// destroystream()    if( ss->type == WAV ) drwav_uninit(&ss->wav);
// destroystream()    if( ss->type == MOD ) jar_mod_unload(&ss->mod);
// destroystream()    if( ss->type == XM && ss->xm ) jar_xm_free_context(ss->xm);

// the callback to refill the (stereo) stream data
static void refill_stream(sts_mixer_sample_t* sample, void* userdata) {
    mystream_t* stream = (mystream_t*)userdata;
    switch( stream->type ) {
        default:
        break; case FLAC: {
            if (drflac_read_s32(stream->flac, sample->length, stream->data) < sample->length) {
                drflac_seek_to_sample(stream->flac, 0);
            }
        }
        break; case WAV: {
            if (drwav_read_s16(&stream->wav, sample->length, (short*)stream->data) < sample->length) {
                drwav_seek_to_sample(&stream->wav, 0);
            }
        }
        break; case MP3: {
            int sl = sample->length / 2; /*sample->channels*/;
            if (drmp3_read_pcm_frames_f32(&stream->mp3_, sl, stream->dataf) < sl) {
                drmp3_seek_to_pcm_frame(&stream->mp3_, 0);
            }
        }
        break; case OGG: {
            stb_vorbis *ogg = (stb_vorbis*)stream->ogg;
            if( stb_vorbis_get_samples_short_interleaved(ogg, 2, (short*)stream->data, sample->length) == 0 )  {
                stb_vorbis_seek(stream->ogg, 0);
            }
        }
        break; case MOD: {
            jar_mod_context_t *mod = (jar_mod_context_t*)&stream->mod;
            jar_mod_fillbuffer(mod, (ma_int16*)stream->data, sample->length / 2, 0);
        }
        break; case XM: {
            jar_xm_context_t *xm = (jar_xm_context_t*)stream->xm;
            jar_xm_generate_samples_16bit(xm, (short*)stream->data, sample->length / 2 );
        }
    }
}

// load a (stereo) stream
static bool load_stream(mystream_t* stream, const char *filename) {
    int error;
    int HZ = 44100;
    stream->type = UNK;
    if( stream->type == UNK && (stream->ogg = stb_vorbis_open_filename(filename, &error, NULL)) ) {
        stb_vorbis_info info = stb_vorbis_get_info(stream->ogg);
        if( info.channels != 2 ) goto end;
        stream->type = OGG;
        stream->stream.sample.frequency = info.sample_rate;
        stream->stream.sample.audio_format = STS_MIXER_SAMPLE_FORMAT_16;
    }
    if( stream->type == UNK && (drwav_init_file(&stream->wav, filename))) {
        if( stream->wav.channels != 2 ) goto end;
        stream->type = WAV;
        stream->stream.sample.frequency = stream->wav.sampleRate;
        stream->stream.sample.audio_format = STS_MIXER_SAMPLE_FORMAT_16;
    }
    if( stream->type == UNK && (stream->flac = drflac_open_file(filename)) ) {
        if( stream->flac->channels != 2 ) goto end;
        stream->type = FLAC;
        stream->stream.sample.frequency = stream->flac->sampleRate;
        stream->stream.sample.audio_format = STS_MIXER_SAMPLE_FORMAT_32;
    }
    if( stream->type == UNK && (jar_xm_create_context_from_file(&stream->xm, HZ, filename) == 0)) {
        stream->type = XM;
        stream->stream.sample.frequency = HZ;
        stream->stream.sample.audio_format = STS_MIXER_SAMPLE_FORMAT_16;
    }
    if( stream->type == UNK && ((jar_mod_init(&stream->mod), jar_mod_load_file(&stream->mod, filename)) != 0) ) {
        stream->type = MOD;
        jar_mod_setcfg(&stream->mod, HZ, 16/*bits*/, 1/*stereo*/, 1/*stereo_separation*/, 1/*filter*/);
        stream->stream.sample.frequency = HZ;
        stream->stream.sample.audio_format = STS_MIXER_SAMPLE_FORMAT_16;
    }
    drmp3_config mp3_cfg = { 2, HZ };
    if( stream->type == UNK && (drmp3_init_file(&stream->mp3_, filename, &mp3_cfg) != 0) ) {
        stream->type = MP3;
        stream->stream.sample.frequency = stream->mp3_.sampleRate;
        stream->stream.sample.audio_format = STS_MIXER_SAMPLE_FORMAT_FLOAT;
    }

    if( stream->type == UNK ) {
        return false;
    }

    end:;
    stream->stream.userdata = stream;
    stream->stream.callback = refill_stream;
    stream->stream.sample.length = sizeof(stream->data) / sizeof(stream->data[0]);
    stream->stream.sample.data = stream->data;
    refill_stream(&stream->stream.sample, stream);

    return true;
}

// load a (mono) sample
static bool load_sample(sts_mixer_sample_t* sample, const char *filename) {
    int error;
    int channels = 0;
    if( !channels ) for( drwav* wav = drwav_open_file(filename); wav; ) {
        sample->frequency = wav->sampleRate;
        sample->audio_format = STS_MIXER_SAMPLE_FORMAT_32;
        sample->length = wav->totalSampleCount;
        sample->data = malloc(sample->length * sizeof(int32_t));
        channels = wav->channels;
        drwav_read_s32(wav, sample->length, (int32_t*)sample->data);
        drwav_close(wav);
        wav = 0;
    }
    if( !channels ) for( stb_vorbis *ogg = stb_vorbis_open_filename(filename, &error, NULL); ogg; ) {
        stb_vorbis_info info = stb_vorbis_get_info(ogg);
        channels = info.channels;
        sample->frequency = info.sample_rate;
        sample->audio_format = STS_MIXER_SAMPLE_FORMAT_16;
        sample->length = (int)stb_vorbis_stream_length_in_samples(ogg);
        stb_vorbis_close(ogg);

        short *buffer;
        int sample_rate;
        stb_vorbis_decode_filename(filename, &channels, &sample_rate, (short **)&buffer);
        sample->data = buffer;
        ogg = 0;
    }
    if( !channels ) for( drflac* flac = drflac_open_file(filename); flac; ) {
        sample->frequency = flac->sampleRate;
        sample->audio_format = STS_MIXER_SAMPLE_FORMAT_32;
        sample->length = flac->totalSampleCount;
        sample->data = malloc(sample->length * sizeof(int32_t));
        channels = flac->channels;
        drflac_read_s32(flac, sample->length, (int32_t*)sample->data);
        drflac_close(flac);
        flac = 0;
    }
    drmp3_config mp3_cfg = { 2, 44100 };
    drmp3_uint64 mp3_fc;
    if( !channels ) for( float *fbuf; 0 != (fbuf = drmp3_open_file_and_read_f32(filename, &mp3_cfg, &mp3_fc)); ) {
        sample->frequency = mp3_cfg.outputSampleRate;
        sample->audio_format = STS_MIXER_SAMPLE_FORMAT_FLOAT;
        sample->length = mp3_fc / sizeof(float) / mp3_cfg.outputChannels;
        sample->data = fbuf;
        channels = mp3_cfg.outputChannels;
        break;
    }
    if( !channels ) {
        int inputSize;
        void *inputData = readfile( filename, &inputSize );
        if( inputData ) {
            short *output = 0;
            int outputSize, hz, mp1channels;
            bool ok = jo_read_mp1(inputData, inputSize, &output, &outputSize, &hz, &mp1channels);
            if( ok ) {
                sample->frequency = hz;
                sample->audio_format = STS_MIXER_SAMPLE_FORMAT_16;
                sample->length = outputSize / sizeof(int16_t) / channels;
                sample->data = malloc(sample->length * sizeof(int16_t) * channels );
                memcpy( sample->data, output, outputSize );
                channels = mp1channels;
            } 
            free( inputData );
        }
    }
/*
    if( !channels ) {
        //loadPreset(1, 0);
        //SaveSettings("test.sfxr");
        LoadSettings(filename);
        ExportWAV("sfxr.wav");
        load_sample(sample, "sfxr.wav");
        return;
    }
*/
    if( !channels ) {
        return false;
    }

    if( channels > 1 ) {
        if( sample->audio_format == STS_MIXER_SAMPLE_FORMAT_FLOAT ) {
            downmix_to_monof( channels, sample->data, sample->length );
            sample->data = realloc( sample->data, sample->length * sizeof(float));
        } else {
            downmix_to_mono( channels, sample->data, sample->length );
            sample->data = realloc( sample->data, sample->length * sizeof(short));
        }
    }

    return true;
}

// HAL

#include <stdbool.h>
#include <stdint.h>

ma_device  device;
ma_context context;
sts_mixer_t mixer;

// This is the function that's used for sending more data to the device for playback.
static
ma_uint32 audio_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    int len = frameCount;
    sts_mixer_mix_audio(&mixer, pOutput, len / (sizeof(int32_t) / 4));
    return len / (sizeof(int32_t) / 4);
}

bool audio_init( int flags ) {
    // init sts_mixer
    sts_mixer_init(&mixer, 44100, STS_MIXER_SAMPLE_FORMAT_32);

    // The prioritization of backends can be controlled by the application. You need only specify the backends
    // you care about. If the context cannot be initialized for any of the specified backends ma_context_init()
    // will fail.
    ma_backend backends[] = {
        ma_backend_wasapi, // Higest priority.
        ma_backend_dsound,
        ma_backend_winmm,
        ma_backend_pulseaudio,
        ma_backend_alsa,
        ma_backend_oss,
        ma_backend_jack,
        ma_backend_opensl,
        //ma_backend_openal,
        //ma_backend_sdl,
        ma_backend_null    // Lowest priority.
    };

    if (ma_context_init(backends, sizeof(backends)/sizeof(backends[0]), NULL, &context) != MA_SUCCESS) {
        printf("Failed to initialize context.");
        return false;
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback); // Or ma_device_type_capture or ma_device_type_duplex.
    config.playback.pDeviceID = NULL; // &myPlaybackDeviceID; // Or NULL for the default playback device.
    config.playback.format    = ma_format_s32;
    config.playback.channels  = 2;
    //config.capture.pDeviceID  = &myCaptureDeviceID;  // Or NULL for the default capture device.
    //config.capture.format     = ma_format_s16;
    //config.capture.channels   = 1;
    config.sampleRate         = 44100;
    config.dataCallback       = audio_callback;
    config.pUserData          = NULL;

    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.");
        ma_context_uninit(&context);
        return false;
    }

    ma_device_start(&device);
    return true;
}

void audio_drop() {
    ma_device_stop(&device);
    ma_device_uninit(&device);
    ma_context_uninit(&context);
}

typedef struct audio_handle {
    bool is_clip;
    bool is_stream;
    union {
    sts_mixer_sample_t clip;
    mystream_t         stream;
    };
} audio_handle;

audio audio_loadclip( const char *pathfile ) {
    audio_handle *a = REALLOC(0, sizeof(audio_handle) );
    memset(a, 0, sizeof(audio_handle));
    a->is_clip = load_sample( &a->clip, pathfile );
    return a;
}
audio audio_loadstream( const char *pathfile ) {
    audio_handle *a = REALLOC(0, sizeof(audio_handle) );
    memset(a, 0, sizeof(audio_handle));
    a->is_stream = load_stream( &a->stream, pathfile );
    return a;
}

// helper to get random [0.0f..1.0f values
#include <stdlib.h>
static float randf() {
    return (float)(rand()) / (float)RAND_MAX;
}

audio audio_play( audio a, int flags ) {
    if( a->is_clip ) {
        float gain = randf(); // [0..1]
        float pitch = 1.f; // (0..N]
        float pan =  -1.0f + randf() * 2.0f; // [-1..+1]
        int voice = sts_mixer_play_sample(&mixer, &a->clip, gain, pitch, pan);
        if( voice == -1 ) { /* all voices busy */ }
    }
    else if( a->is_stream ) {
        float gain = 0.7f;
        int voice = sts_mixer_play_stream(&mixer, &a->stream.stream, gain);
        if( voice == -1 ) { /* all voices busy */ }
    }
    return a;
}

#endif





#ifdef AUDIO_DEMO
#include <stdio.h>

#ifdef _WIN32
#define sleep(x) Sleep((x) * 1000)
#endif

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: %s sample.snd [stream.snd]\n", argv[0]);
        printf("filetypes: %s\n", "WAV, MOD, XM, FLAC, OGG, SFXR, MP1, MP3");
        return -1;
    }

    if( audio_init(0) ) {
        if( argc == 2 ) {
            audio stream = audio_loadstream(argv[1]);
            audio_play(stream, 0 );
            for(;;) {
            }
        }
        if( argc == 3 ) {
            audio stream = audio_loadclip(argv[2]); // <-- @todo: fixme: why order does matter? cant stream be loaded first?
            audio sample = audio_loadstream(argv[1]);
            audio_play(stream, 0 );
            for(;;) {
                Sleep( 2000 );
                puts("play a sample with random gain, regular pitch, and random panning");
                audio_play(sample, 0 );
            }
        }

        audio_drop();
    }

    return 0;
}

#endif


