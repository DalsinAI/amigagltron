/* The SDL_sound calls GLtron makes, for WAV files only (amiga/include/
 * SDL_sound.h): the whole file is read with SDL_LoadWAV_RW, converted to the
 * format asked for with SDL_ConvertAudio, and handed out a buffer at a time.
 * The AmiCradle Team. GPL 2 or later, like GLtron. */
#include <stdlib.h>
#include <string.h>
#include "SDL_sound.h"

typedef struct {
	Uint8 *data; /* the whole sound, in the format asked for */
	Uint32 len, pos;
} Lite;

static const char *wav_ext[] = { "WAV", NULL };
static const Sound_DecoderInfo wav_info = { wav_ext, "Microsoft WAVE (SDL_LoadWAV_RW)", "SDL", "" };
static const Sound_DecoderInfo *decoders[] = { &wav_info, NULL };

int Sound_Init(void) { return 1; }
int Sound_Quit(void) { return 1; }
const Sound_DecoderInfo **Sound_AvailableDecoders(void) { return decoders; }
const char *Sound_GetError(void) { return SDL_GetError(); }

Sound_Sample *Sound_NewSample(SDL_RWops *rw, const char *ext, Sound_AudioInfo *desired, Uint32 bufferSize)
{
	SDL_AudioSpec spec;
	Uint8 *wav;
	Uint32 len;
	SDL_AudioCVT cvt;
	Sound_Sample *s;
	Lite *l;

	(void)ext;
	if (!rw)
		return NULL;
	if (!SDL_LoadWAV_RW(rw, 1, &spec, &wav, &len))
		return NULL;
	if (SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq,
	                      desired->format, desired->channels, desired->rate) < 0) {
		SDL_FreeWAV(wav);
		return NULL;
	}
	l = calloc(1, sizeof *l);
	s = calloc(1, sizeof *s);
	if (!l || !s) {
		free(l); free(s); SDL_FreeWAV(wav);
		SDL_SetError("out of memory");
		return NULL;
	}
	cvt.len = len;
	cvt.buf = malloc(len * (cvt.len_mult > 0 ? cvt.len_mult : 1));
	if (!cvt.buf) {
		free(l); free(s); SDL_FreeWAV(wav);
		SDL_SetError("out of memory");
		return NULL;
	}
	memcpy(cvt.buf, wav, len);
	SDL_FreeWAV(wav);
	if (cvt.needed)
		SDL_ConvertAudio(&cvt);
	l->data = cvt.buf;
	l->len = cvt.needed ? (Uint32)cvt.len_cvt : len;

	s->opaque = l;
	s->decoder = &wav_info;
	s->desired = *desired;
	s->actual = *desired;
	s->buffer_size = bufferSize ? bufferSize : 16384;
	s->buffer = malloc(s->buffer_size);
	if (!s->buffer) {
		free(l->data); free(l); free(s);
		SDL_SetError("out of memory");
		return NULL;
	}
	return s;
}

void Sound_FreeSample(Sound_Sample *s)
{
	Lite *l;
	if (!s)
		return;
	l = s->opaque;
	if (l) {
		free(l->data);
		free(l);
	}
	free(s->buffer);
	free(s);
}

Uint32 Sound_Decode(Sound_Sample *s)
{
	Lite *l = s->opaque;
	Uint32 n = l->len - l->pos;
	if (n > s->buffer_size)
		n = s->buffer_size;
	memcpy(s->buffer, l->data + l->pos, n);
	l->pos += n;
	if (l->pos >= l->len)
		s->flags |= SOUND_SAMPLEFLAG_EOF;
	return n;
}

Uint32 Sound_DecodeAll(Sound_Sample *s)
{
	Lite *l = s->opaque;
	Uint32 n = l->len - l->pos;
	void *b = realloc(s->buffer, n ? n : 1);
	if (!b) {
		s->flags |= SOUND_SAMPLEFLAG_ERROR;
		return 0;
	}
	s->buffer = b;
	memcpy(s->buffer, l->data + l->pos, n);
	s->buffer_size = n;
	l->pos = l->len;
	s->flags |= SOUND_SAMPLEFLAG_EOF;
	return n;
}
