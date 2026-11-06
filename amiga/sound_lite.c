/* The SDL_sound calls GLtron makes, for PCM WAV files only (amiga/include/
 * SDL_sound.h). Sound_NewSampleFromFile reads the whole file with stdio (not
 * SDL: sdl12-compat's SDL_LoadWAV_RW does not come back on OpenGPU), turns
 * it into the format asked for (signed 16 bits in the machine's order, mono
 * or stereo, at the rate asked for) and hands it out a buffer at a time.
 * The AmiCradle Team. GPL 2 or later, like GLtron. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL_sound.h"

typedef struct {
	Uint8 *data; /* the whole sound, in the format asked for */
	Uint32 len, pos;
} Lite;

static const char *wav_ext[] = { "WAV", NULL };
static const Sound_DecoderInfo wav_info = { wav_ext, "Microsoft WAVE, PCM (stdio)", "the AmiCradle Team", "" };
static const Sound_DecoderInfo *decoders[] = { &wav_info, NULL };
static char error[128] = "";

static void set_error(const char *what, const char *path)
{
	snprintf(error, sizeof error, "%s: %s", path, what);
}

int Sound_Init(void) { return 1; }
int Sound_Quit(void) { return 1; }
const Sound_DecoderInfo **Sound_AvailableDecoders(void) { return decoders; }
const char *Sound_GetError(void) { return error; }

static Uint32 le32(const Uint8 *p) { return p[0] | p[1] << 8 | p[2] << 16 | (Uint32)p[3] << 24; }
static Uint16 le16(const Uint8 *p) { return p[0] | p[1] << 8; }

/* one sample of channel c at frame f, as signed 16 bits */
static Sint16 pcm_at(const Uint8 *pcm, int bits, int channels, Uint32 f, int c)
{
	const Uint8 *p = pcm + (f * channels + c) * (bits / 8);
	return bits == 8 ? (Sint16)((p[0] - 128) << 8) : (Sint16)le16(p);
}

Sound_Sample *Sound_NewSampleFromFile(const char *path, Sound_AudioInfo *desired, Uint32 bufferSize)
{
	FILE *f;
	long size;
	Uint8 *file = NULL, *p, *end, *pcm = NULL;
	Uint32 pcm_len = 0, frames, out_frames, i;
	int channels = 0, bits = 0, rate = 0, c, outc;
	Sint16 *out;
	Sound_Sample *s;
	Lite *l;

	if (desired->format != AUDIO_S16SYS || (desired->channels != 1 && desired->channels != 2) || !desired->rate) {
		set_error("only signed 16-bit output in the machine's order", path);
		return NULL;
	}
	if (!(f = fopen(path, "rb"))) {
		set_error("can't open", path);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (size < 12 || !(file = malloc(size)) || fread(file, 1, size, f) != (size_t)size) {
		fclose(f);
		free(file);
		set_error("can't read", path);
		return NULL;
	}
	fclose(f);
	if (memcmp(file, "RIFF", 4) || memcmp(file + 8, "WAVE", 4)) {
		free(file);
		set_error("not a WAV file", path);
		return NULL;
	}
	end = file + size;
	for (p = file + 12; p + 8 <= end; ) {
		Uint32 n = le32(p + 4);
		Uint8 *body = p + 8;
		if (n > (Uint32)(end - body))
			n = end - body;
		if (!memcmp(p, "fmt ", 4) && n >= 16) {
			if (le16(body) != 1) {
				free(file);
				set_error("not PCM", path);
				return NULL;
			}
			channels = le16(body + 2);
			rate = le32(body + 4);
			bits = le16(body + 14);
		} else if (!memcmp(p, "data", 4)) {
			pcm = body;
			pcm_len = n;
		}
		p = body + n + (n & 1);
	}
	if (!pcm || (bits != 8 && bits != 16) || channels < 1 || channels > 2 || rate <= 0) {
		free(file);
		set_error("no 8- or 16-bit mono or stereo PCM in it", path);
		return NULL;
	}

	frames = pcm_len / (channels * (bits / 8));
	out_frames = (Uint32)((unsigned long long)frames * desired->rate / rate);
	outc = desired->channels;
	l = calloc(1, sizeof *l);
	s = calloc(1, sizeof *s);
	out = malloc(out_frames * outc * 2 + 2);
	if (!l || !s || !out) {
		free(l); free(s); free(out); free(file);
		set_error("out of memory", path);
		return NULL;
	}
	for (i = 0; i < out_frames; i++) {
		Uint32 src = (Uint32)((unsigned long long)i * rate / desired->rate);
		for (c = 0; c < outc; c++) {
			int v;
			if (channels == outc)
				v = pcm_at(pcm, bits, channels, src, c);
			else if (channels == 1)
				v = pcm_at(pcm, bits, 1, src, 0);
			else
				v = (pcm_at(pcm, bits, 2, src, 0) + pcm_at(pcm, bits, 2, src, 1)) / 2;
			out[i * outc + c] = (Sint16)v;
		}
	}
	free(file);

	l->data = (Uint8 *)out;
	l->len = out_frames * outc * 2;
	s->opaque = l;
	s->decoder = &wav_info;
	s->desired = *desired;
	s->actual = *desired;
	s->buffer_size = bufferSize ? bufferSize : 16384;
	s->buffer = malloc(s->buffer_size);
	if (!s->buffer) {
		free(l->data); free(l); free(s);
		set_error("out of memory", path);
		return NULL;
	}
	return s;
}

Sound_Sample *Sound_NewSample(SDL_RWops *rw, const char *ext, Sound_AudioInfo *desired, Uint32 bufferSize)
{
	(void)rw; (void)ext; (void)desired; (void)bufferSize;
	set_error("Sound_NewSample is not here: use Sound_NewSampleFromFile", "SDL_RWops");
	return NULL;
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
