#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "pav_analysis.h"
#include "vad.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/*
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
    "UNDEF", "S", "V", "MAYBE_VOICE", "MAYBE_SILENCE", "INIT"}; // added MV & MS states

const char *state2str(VAD_STATE st)
{
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct // implemented
{
  float p; // PREGUNTA: NOMES NECESSITEM POWER NO? NO ENTENC PQ HEM DE FER UN STRUCT AQUI (VENIA PER DEFECTE)
  // float am; <-- average amplitude
  // unsigned int zcr; <-- zero crossings
} Features;

/*
 * TODO: Delete and use your own features! ***IN DEVELOPMENT***
 */

Features compute_features(const float *x, int N) // implemented
{
  /*
   * Input: x[i] : i=0 .... N-1
   * Ouput: computed features
   */
  /*
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1
   */
  Features feat;
  // feat.zcr = feat.p = feat.am = (float) rand()/RAND_MAX;
  feat.p = compute_power(x, N); // basic power computation (no hamming)
  return feat;
}

Features compute_features_hamming(const float *x, int N) // implemented
{
  /*
   * Input: x[i] : i=0 .... N-1
   * Ouput: computed features
   */
  /*
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1
   */
  Features feat;

  // hamming window generation
  float hamming_window[N]; // hamming window array
  // hamming_window generation
  for (unsigned int n = 0; n < N; ++n)
  {
    hamming_window[n] = 0.54 - 0.46 * cos(6.28 * n / (N - 1)); // 6.28 = 2*pi (aprox) <-- coefficients from the internet
  }

  // feat.zcr = feat.p = feat.am = (float) rand()/RAND_MAX;
  feat.p = abs(compute_hamming_power(hamming_window, N, 1, x)); // basic power computation (no hamming)
  return feat;
}

/*
 * TODO: Init the values of vad_data EN PRINCIPI JA ESTA
 */

VAD_DATA *vad_open(float rate)
{
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  float frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->frame_length = frame_length;

  /*initialze remaining VAD properties*/

  // seconds to wait before exiting UNDEF state (change as needed)
  float silence_standby = 10e-3;
  float voice_standby = 10e-3;
  float initial_standby = 200e-3;

  /*NOTE: realistically, standby thresholds should be an integer multiple of the frame temporal duration for them
  to take upon real effect. So if frame temporal duration is 10 ms, standby's should be multiples of 10 (10, 20, 30, ...)*/

  // seconds to frames conversion
  unsigned int silence_standby_frames = ceil((silence_standby * rate) / frame_length);
  unsigned int voice_standby_frames = ceil((voice_standby * rate) / frame_length);
  unsigned int initial_standby_frames = ceil((initial_standby * rate / frame_length));

  // VAD_DATA properties assignations
  vad_data->silence_standby = silence_standby_frames;
  vad_data->voice_standby = voice_standby_frames;
  vad_data->initial_standby = initial_standby_frames;

  /*initialize also the type of noise reference calculation method, by passing either of these 3 strings to noise_reference_calculation property of VAD

    (1) "Potencia media inicial" (2) "Media de las potencias en dBs" (3) "Valor maximo de la potencia"*/
  // vad_data->noise_reference_calculation = "Valor maximo de la potencia";
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) // implemented
{
  /*
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) // implemented
{
  return vad_data->frame_length;
}

/*
 * TODO: Implement the Voice Activity Detection
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x /*, float alfa1 /*lab*/)
{
  /*
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features_hamming(x, vad_data->frame_length);
  // Features f = compute_features(x, vad_data->frame_length); <-- secure copy
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  // VAD_DATA properties extraction (for easier handling)
  VAD_STATE vad_state = vad_data->state;
  float power_threshold = vad_data->k0;
  unsigned int silence_standby = vad_data->silence_standby;
  unsigned int voice_standby = vad_data->voice_standby;
  unsigned int frames_so_far = vad_data->frames_so_far;

  switch (vad_data->state)
  {
  case ST_INIT:
    vad_data->state = ST_SILENCE;
    break;

  case ST_SILENCE:
    if (f.p < 25)
      vad_data->state = ST_VOICE;
    break;

  case ST_VOICE:
    if (f.p > 25)
      vad_data->state = ST_SILENCE;
    break;

  case ST_UNDEF:
    break;
  }

  /*switch (vad_state)
  {
  case ST_INIT:
    // check whether init_standby has been exhausted & switch to SILENCE, otherwise remain in INIT
    if (frames_so_far < vad_data->initial_standby)
    { // we still can't exit INIT state
      vad_data->frames_so_far++;
      // here would go the calculations to compute the silence threshold (COMPLETE)
    }
    else
    {
      vad_data->state = ST_SILENCE;
      // reset frames_so_far
      vad_data->frames_so_far = 0;
    }
    break;

  case ST_SILENCE: // we have 2 options: (1) remain in SILENCE (2) transition to MAYBE SILENCE
    if (f.p > 25 /*arbitrarily set power in dBs (change as required))
      vad_data->state = ST_MAYBE_SILENCE;
    vad_data->frames_so_far++;
    break;

  case ST_VOICE:
    if (f.p < 25 /*arbitrarily set power in dBs (change as required))
      vad_data->state = ST_MAYBE_VOICE;
    vad_data->frames_so_far++;
    break;

  case ST_MAYBE_VOICE:
    if (f.p >= 25 && vada_data->frames_so_far)

      break;

  case ST_UNDEF:
    break;
  }*/

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out)
{
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
