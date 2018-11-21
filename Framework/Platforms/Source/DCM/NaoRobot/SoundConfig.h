#ifndef __SOUND_CONFIG__
#define __SOUND_CONFIG__

#define NUM_CHANNELS_RX         (2)
#define NUM_CHANNELS_RX_REC     (4)
#define SOUND_DEVICE_RX         "default"
#define SOUND_DEVICE_RX_VOL     "default"
#define SOUND_SUBDEVICE_RX      "Capture"
#define SAMPLE_RATE_RX          (8000)
#define SAMPLE_RATE_RX_REC      (48000)
#define SAMPLE_COUNT            (1024) //amount of samples to read must be power of 2
#define SAMPLE_COUNT_REC        (4096) //amount of samples to read must be power of 2
// TODO: do we really use overlap? is it implemented correctly?
#define SAMPLE_OVERLAP_COUNT    (256) //amount of samples to read must be power of 2
#define SAMPLE_NEW_COUNT        (SAMPLE_COUNT - SAMPLE_OVERLAP_COUNT) //amount of samples to read must be power of 2
#define BUFFER_SIZE_RX          (NUM_CHANNELS_RX * SAMPLE_COUNT)
#define BUFFER_SIZE_RX_REC      (NUM_CHANNELS_RX_REC * SAMPLE_COUNT_REC)
#define VOLUME_RX               (50 + 1)

#define WHISTLE_BUFF_LEN        (SAMPLE_COUNT)
#define WHISTLE_FFT_LEN         (WHISTLE_BUFF_LEN * 2)
#define MS_IDLE                 (128)

#endif


/**
 * B-Human Ansatz:
 * - 8 kHz Abtastfrequenz => max. Info bis 4kHz
 * - Detektor Buffer Größe = 1024 Samples bei 8 kHz => 128 ms Zeitfenster
 * - 1/4 Überlappung => 256 Samples => 32 ms zeitliche Überlappung
 * - Rechteckfenster
 * - Kreuzkorrelation über Multiplikation des komplex konjugierten Spekrums von Referenz
 *   -pfeifen mit dem komplexen Spektrum des aufgenommen Signals im Frequenzbereich
 *
 *
 * B-Human approach:
 * - 8 kHz sampling frequency => max. infomartion up to 4kHz
 * - detector buffer size = 1024 samples @8 kHz => 128 ms time window
 * - 1/4 overlapping => 256 samples => 32 ms overlapping in time
 * - rectangular window
 * - cross correlation using the multiplication of the complex conjugate spectrum of reference
 *   whistles with the comple specrum of the recorded signal in the frequency domain
 *
 **/
