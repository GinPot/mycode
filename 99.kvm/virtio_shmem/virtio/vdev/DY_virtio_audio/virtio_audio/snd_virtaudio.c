/*
 * Copyright (C) 2019 QNX Software Systems
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "virtaudio.h"

int log_debug = 0;
module_param(log_debug, int, 0644);
MODULE_PARM_DESC(log_debug, "Enable log debugging: 0 (default) off, 1 debug, 2 verbose");

static inline uint64_t
format_virtlinux(int virt2linux, int format)
{
	static const struct {
		uint64_t lformat;
		uint64_t vformat;
	} formats [] = {
		{SNDRV_PCM_FMTBIT_S8, VIRTIO_AUDIO_FMT_MSK_S8},
		{SNDRV_PCM_FMTBIT_U8, VIRTIO_AUDIO_FMT_MSK_U8},
		{SNDRV_PCM_FMTBIT_S16_LE, VIRTIO_AUDIO_FMT_MSK_S16_LE},
		{SNDRV_PCM_FMTBIT_S16_BE, VIRTIO_AUDIO_FMT_MSK_S16_BE},
		{SNDRV_PCM_FMTBIT_U16_LE, VIRTIO_AUDIO_FMT_MSK_U16_LE},
		{SNDRV_PCM_FMTBIT_U16_BE, VIRTIO_AUDIO_FMT_MSK_U16_BE},
		{SNDRV_PCM_FMTBIT_S24_LE, VIRTIO_AUDIO_FMT_MSK_S24_LE},
		{SNDRV_PCM_FMTBIT_S24_BE, VIRTIO_AUDIO_FMT_MSK_S24_BE},
		{SNDRV_PCM_FMTBIT_U24_LE, VIRTIO_AUDIO_FMT_MSK_U24_LE},
		{SNDRV_PCM_FMTBIT_U24_BE, VIRTIO_AUDIO_FMT_MSK_U24_BE},
		{SNDRV_PCM_FMTBIT_S32_LE, VIRTIO_AUDIO_FMT_MSK_S32_LE},
		{SNDRV_PCM_FMTBIT_S32_BE, VIRTIO_AUDIO_FMT_MSK_S32_BE},
		{SNDRV_PCM_FMTBIT_U32_LE, VIRTIO_AUDIO_FMT_MSK_U32_LE},
		{SNDRV_PCM_FMTBIT_U32_BE, VIRTIO_AUDIO_FMT_MSK_U32_BE},
		{SNDRV_PCM_FMTBIT_FLOAT_LE, VIRTIO_AUDIO_FMT_MSK_FLOAT_LE},
		{SNDRV_PCM_FMTBIT_FLOAT_BE, VIRTIO_AUDIO_FMT_MSK_FLOAT_BE},
		{SNDRV_PCM_FMTBIT_FLOAT64_LE, VIRTIO_AUDIO_FMT_MSK_FLOAT64_LE},
		{SNDRV_PCM_FMTBIT_FLOAT64_BE, VIRTIO_AUDIO_FMT_MSK_FLOAT64_BE},
		{SNDRV_PCM_FMTBIT_IEC958_SUBFRAME_LE, VIRTIO_AUDIO_FMT_MSK_IEC958_SUBFRAME_LE},
		{SNDRV_PCM_FMTBIT_IEC958_SUBFRAME_BE, VIRTIO_AUDIO_FMT_MSK_IEC958_SUBFRAME_BE},
		{SNDRV_PCM_FMTBIT_MU_LAW, VIRTIO_AUDIO_FMT_MSK_MU_LAW},
		{SNDRV_PCM_FMTBIT_A_LAW, VIRTIO_AUDIO_FMT_MSK_A_LAW},
		{SNDRV_PCM_FMTBIT_IMA_ADPCM, VIRTIO_AUDIO_FMT_MSK_IMA_ADPCM},
		{SNDRV_PCM_FMTBIT_MPEG, VIRTIO_AUDIO_FMT_MSK_MPEG},
		{SNDRV_PCM_FMTBIT_GSM, VIRTIO_AUDIO_FMT_MSK_GSM},
		{SNDRV_PCM_FMTBIT_SPECIAL, VIRTIO_AUDIO_FMT_MSK_SPECIAL},
	};
	int i;
	uint64_t fmt = 0ULL;
	for (i = 0; i < ARRAY_SIZE(formats); i++) {
		if (virt2linux) {
			if (format & formats[i].vformat) {
				fmt |= formats[i].lformat;
			}
		} else {
			if (format & formats[i].lformat) {
				fmt |= formats[i].vformat;
			}
		}
	}
	return fmt;
}

static inline int
rate_virtlinux(int virt2linux, int rate)
{
#define SND_PCM_RATE_KNOT (1<<30)
#define SND_PCM_RATE_CONTINUOUS (1<<31)

	static const struct {
		int lrate;
		int vrate;
	} rates[] = {
		{SNDRV_PCM_RATE_8000, VIRTIO_AUDIO_RATE_MSK_8000},
		{SNDRV_PCM_RATE_11025, VIRTIO_AUDIO_RATE_MSK_11025},
		//{, VIRTIO_AUDIO_RATE_MSK_12000},
		{SNDRV_PCM_RATE_16000, VIRTIO_AUDIO_RATE_MSK_16000},
		//{, VIRTIO_AUDIO_RATE_MSK_18900},
		{SNDRV_PCM_RATE_22050, VIRTIO_AUDIO_RATE_MSK_22050},
		//{, VIRTIO_AUDIO_RATE_MSK_24000},
		{SNDRV_PCM_RATE_32000, VIRTIO_AUDIO_RATE_MSK_32000},
		{SNDRV_PCM_RATE_44100, VIRTIO_AUDIO_RATE_MSK_44100},
		{SNDRV_PCM_RATE_48000, VIRTIO_AUDIO_RATE_MSK_48000},
		{SNDRV_PCM_RATE_64000, VIRTIO_AUDIO_RATE_MSK_64000},
		{SNDRV_PCM_RATE_88200, VIRTIO_AUDIO_RATE_MSK_88200},
		{SNDRV_PCM_RATE_96000, VIRTIO_AUDIO_RATE_MSK_96000},
		{SNDRV_PCM_RATE_176400, VIRTIO_AUDIO_RATE_MSK_176400},
		{SNDRV_PCM_RATE_192000, VIRTIO_AUDIO_RATE_MSK_192000},
		{SNDRV_PCM_RATE_CONTINUOUS, SND_PCM_RATE_CONTINUOUS},
		{SNDRV_PCM_RATE_KNOT, SND_PCM_RATE_KNOT},
	};
	int i;
	int rte = 0;

	for (i = 0; i < ARRAY_SIZE(rates); i++) {
		if (virt2linux) {
			if (rate & rates[i].vrate) {
				rte |= rates[i].lrate;
			}
		} else {
			if (rate & rates[i].lrate) {
				rte |= rates[i].vrate;
			}
		}
	}
	return rte;
}

static int
snd_virtaudio_open(struct snd_pcm_substream *substream)
{
	struct virtaudio_device *vad = snd_pcm_substream_chip(substream);
	struct virtaudio_channel *vac = vad->channels[substream->stream];
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	runtime->private_data = vad;

	runtime->hw.info = SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED | SNDRV_PCM_INFO_BLOCK_TRANSFER |
		SNDRV_PCM_INFO_BATCH | SNDRV_PCM_INFO_MMAP_VALID | SNDRV_PCM_INFO_DRAIN_TRIGGER | SNDRV_PCM_INFO_RESUME;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		if (virtio_has_feature(vad->vdev, VIRTIO_AUDIO_F_PCM_PLAYBACK_PAUSE_RESUME)) {
			runtime->hw.info |= SNDRV_PCM_INFO_PAUSE;
		}
	} else {
		if (virtio_has_feature(vad->vdev, VIRTIO_AUDIO_F_PCM_CAPTURE_PAUSE_RESUME)) {
			runtime->hw.info |= SNDRV_PCM_INFO_PAUSE;
		}
	}
	runtime->hw.formats = format_virtlinux(1, vac->config.formats);
	runtime->hw.rates = rate_virtlinux(1, vac->config.rates);
	snd_pcm_limit_hw_rates(runtime);
	runtime->hw.channels_min = vac->config.min_voices;
	runtime->hw.channels_max = vac->config.max_voices;
	runtime->hw.buffer_bytes_max = vac->config.max_buffersize;
	runtime->hw.period_bytes_min = vac->config.min_fragsize;
	runtime->hw.period_bytes_max = vac->config.max_fragsize;
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		runtime->hw.periods_min = vac->config.min_frags;
	} else {
		runtime->hw.periods_min = 2;
	}
	/* Maximum number of periods is half the queue size because it is comprised of data and status descriptors */
	runtime->hw.periods_max = vac->data_vaq.queue_size / 2;

	err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);
	if (err < 0) {
		LOG_ERROR(vad, "Failure constraining DMA buffer to multiple of period size, err %d", err);
		return err;
	}

	err = snd_pcm_hw_constraint_step(runtime, 0, SNDRV_PCM_HW_PARAM_PERIOD_BYTES, vac->config.fragment_align);
	if (err < 0) {
		LOG_ERROR(vad, "Failure applying fragment alignment constraint, err %d", err);
		return err;
	}

	vac->substream = substream;

	LOG_DEBUG(vad, "stream %u info 0x%x formats 0x%llx rates 0x%x channels(%u, %u) period_bytes(%lu, %lu) periods(%u, %u), buffer_bytes %lu",
		substream->stream, runtime->hw.info, runtime->hw.formats, runtime->hw.rates, runtime->hw.channels_min, runtime->hw.channels_max,
		runtime->hw.period_bytes_min, runtime->hw.period_bytes_max, runtime->hw.periods_min, runtime->hw.periods_max, runtime->hw.buffer_bytes_max);

	return 0;
}

static int
snd_virtaudio_close(struct snd_pcm_substream *substream)
{
	struct virtaudio_device *vad = snd_pcm_substream_chip(substream);
	struct virtaudio_channel *vac = vad->channels[substream->stream];

	LOG_DEBUG(vad, "stream %u", substream->stream);

	substream->runtime->private_data = NULL;
	vac->substream = NULL;

	return 0;
}

static int
multiple2(int f)
{
	int m, i;
	for (i = 0, m = 1; m < f; i++) {
		m *= 2;
	}
	return i;
}

static int
snd_virtaudio_pcm_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *hw_params)
{
	int i, ret = 0;
	size_t mem_size;
	struct virtaudio_device *vad = snd_pcm_substream_chip(substream);
	struct virtaudio_channel *vac = vad->channels[substream->stream];
	struct virtio_audio_acquire_params acquire_params = { 0 };

	LOG_DEBUG(vad, "stream %u DMA buffer size %u", substream->stream, params_buffer_bytes(hw_params));

	ret = snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
	if (ret < 0) {
		LOG_ERROR(vad, "DMA memory allocation failure: %d", ret);
		return ret;
	}

	acquire_params.rate = params_rate(hw_params);
	acquire_params.voices = params_channels(hw_params);
	acquire_params.format = multiple2(format_virtlinux(0, (1 << params_format(hw_params))));
	acquire_params.frag_size = params_period_bytes(hw_params);
	acquire_params.frags = params_periods(hw_params);

	ret = virtaudio_cmd_acquire(vad, vac, &acquire_params);
	if (ret != 0) {
		return ret;
	}

	vac->data_vaq.buf_num = acquire_params.frags;
	vac->data_vaq.buf_size = acquire_params.frag_size;
	vac->data_vaq.data_mem = substream->runtime->dma_area;

	mem_size = sizeof(struct virtio_audio_io_status) * vac->data_vaq.buf_num;
	BUG_ON(vac->data_vaq.status_mem);
	vac->data_vaq.status_mem = kzalloc(mem_size, GFP_KERNEL);
	if (vac->data_vaq.status_mem == NULL) {
		LOG_ERROR(vad, "Unable to allocate status descriptor memory");
		return -ENOMEM;
	}

	BUG_ON(vac->data_vaq.buf_info);
	vac->data_vaq.buf_info = kcalloc(vac->data_vaq.buf_num, sizeof(struct virtaudio_buf_info), GFP_KERNEL);
	if (vac->data_vaq.buf_info == NULL) {
		LOG_ERROR(vad, "Unable to allocate data descriptor memory");
		kfree(vac->data_vaq.status_mem);
		return -ENOMEM;
	}

	/* Sequence numbers must wrap around at a multiple of the buf_num, to be able to derive easily fragment indexes */
	vac->data_vaq.seqno_wrap = __UINT16_MAX__ / vac->data_vaq.buf_num * vac->data_vaq.buf_num;

	for (i = 0; i < vac->data_vaq.buf_num; i++) {
		vac->data_vaq.buf_info[i].data_ptr = vac->data_vaq.data_mem + i * vac->data_vaq.buf_size;
		vac->data_vaq.buf_info[i].status_ptr = vac->data_vaq.status_mem + i * sizeof(struct virtio_audio_io_status);
		LOG_VERBOSE(vad, "frag[%d]: data_ptr %p status_ptr %p", i, vac->data_vaq.buf_info[i].data_ptr, vac->data_vaq.buf_info[i].status_ptr);
	}

	return ret;
}

static int
snd_virtaudio_pcm_hw_free(struct snd_pcm_substream *substream)
{
	unsigned long flags;
	int ret = 0;
	struct virtaudio_device *vad = snd_pcm_substream_chip(substream);
	struct virtaudio_channel *vac = vad->channels[substream->stream];

	LOG_DEBUG(vad, "stream %u", substream->stream);

	ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_RELEASE, true);
	if (ret != 0) {
		return ret;
	}

	wait_event(vac->data_vaq.rsp_done, (vac->data_vaq.rx_seqno == vac->data_vaq.tx_seqno));

	snd_pcm_stream_lock_irqsave(vac->substream, flags);
	kfree(vac->data_vaq.status_mem);
	vac->data_vaq.status_mem = NULL;
	kfree(vac->data_vaq.buf_info);
	vac->data_vaq.buf_info = NULL;

	vac->data_vaq.buf_num = 0;
	vac->data_vaq.tx_seqno = vac->data_vaq.rx_seqno = 0;
	vac->drain_triggered = false;
	snd_pcm_stream_unlock_irqrestore(vac->substream, flags);

	snd_pcm_lib_free_pages(substream);

	return ret;
}

static int
snd_virtaudio_pcm_prepare(struct snd_pcm_substream *substream)
{
	unsigned long flags;
	struct virtaudio_device *vad = snd_pcm_substream_chip(substream);
	struct virtaudio_channel *vac = vad->channels[substream->stream];

	LOG_DEBUG(vad, "stream %u format 0x%x channels %d rate %d period_size %ld periods %d buffer_size %ld", substream->stream, substream->runtime->format,
		substream->runtime->channels, substream->runtime->rate, substream->runtime->period_size, substream->runtime->periods, substream->runtime->buffer_size);
	LOG_DEBUG(vad, "period_bytes %ld buffer_bytes %ld", snd_pcm_lib_period_bytes(substream), snd_pcm_lib_buffer_bytes(substream));
	LOG_VERBOSE(vad, "hw_ptr %ld appl_ptr %ld", substream->runtime->status->hw_ptr, substream->runtime->control->appl_ptr);

	/* In case the hw_free callback was never invoked (e.g. trigger stop and re-prepare),
	 * wait for any remaining fragments to return. Ideally this would be done in the
	 * trigger STOP callback but we cannot wait there. */
	wait_event(vac->data_vaq.rsp_done, (vac->data_vaq.rx_seqno == vac->data_vaq.tx_seqno));

	snd_pcm_stream_lock_irqsave(vac->substream, flags);
	vac->data_vaq.tx_seqno = vac->data_vaq.rx_seqno = 0;
	vac->drain_triggered = false;
	snd_pcm_stream_unlock_irqrestore(vac->substream, flags);

	return virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_PREPARE, true);
}

static inline char *
state_pcm2string(int const pcm_state)
{
	switch (pcm_state) {
	CASE(SNDRV_PCM_STATE_OPEN, "OPEN");
	CASE(SNDRV_PCM_STATE_SETUP, "SETUP");
	CASE(SNDRV_PCM_STATE_PREPARED, "PREPARED");
	CASE(SNDRV_PCM_STATE_RUNNING, "RUNNING");
	CASE(SNDRV_PCM_STATE_XRUN, "XRUN");
	CASE(SNDRV_PCM_STATE_DRAINING, "DRAINING");
	CASE(SNDRV_PCM_STATE_PAUSED, "PAUSED");
	CASE(SNDRV_PCM_STATE_SUSPENDED, "SUSPENDED");
	CASE(SNDRV_PCM_STATE_DISCONNECTED, "DISCONNECTED");
	default:
		BUG_ON(true);
	}

	return NULL;
}

static inline char *
cmd_pcm2string(int const pcm_cmd)
{
	switch (pcm_cmd) {
	CASE(SNDRV_PCM_TRIGGER_STOP, "STOP");
	CASE(SNDRV_PCM_TRIGGER_START, "START");
	CASE(SNDRV_PCM_TRIGGER_PAUSE_PUSH, "PAUSE PUSH");
	CASE(SNDRV_PCM_TRIGGER_PAUSE_RELEASE, "PAUSE RELEASE");
	CASE(SNDRV_PCM_TRIGGER_SUSPEND, "SUSPEND");
	CASE(SNDRV_PCM_TRIGGER_RESUME, "RESUME");
	CASE(SNDRV_PCM_TRIGGER_DRAIN, "DRAIN");
	default:
		BUG_ON(true);
	}

	return NULL;
}

/* Invoked by PCM framework with stream lock held and local interrupts disabled
 * WARNING: Trigger callback is atomic (non-schedulable). */
static int
snd_virtaudio_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	int ret = 0;
	struct virtaudio_device *vad = snd_pcm_substream_chip(substream);
	struct virtaudio_channel *vac = vad->channels[substream->stream];

	LOG_DEBUG(vad, "stream %u cmd '%s' state '%s'", substream->stream, cmd_pcm2string(cmd), state_pcm2string(substream->runtime->status->state));

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
		ret = virtaudio_send_data(vad, vac);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			if (substream->runtime->status->state != SNDRV_PCM_STATE_DRAINING) {
				ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_DROP, false);
			} else {
				if (!vac->drain_triggered) {
					/* For some reason the DRAIN trigger was not invoked, issue VIRTIO_AUDIO_CMD_PCM_DRAIN here */
					ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_DRAIN, false);
				}
			}
		} else {
			/* Always stop capture session by issuing VIRTIO_AUDIO_CMD_PCM_DRAIN */
			ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_DRAIN, false);
		}
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_PAUSE, false);
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_RESUME, false);
		break;
	case SNDRV_PCM_TRIGGER_DRAIN:
		ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_DRAIN, false);
		vac->drain_triggered = true;
		break;
	default:
		LOG_ERROR(vad, "Unknown trigger cmd %d", cmd);
		ret = -EINVAL;
		break;
	}

	return ret;
}

/* Invoked by PCM framework with stream lock held and local interrupts disabled
 * WARNING: Pointer callback is atomic (non-schedulable). */
static snd_pcm_uframes_t
snd_virtaudio_pcm_pointer(struct snd_pcm_substream *substream)
{
	snd_pcm_uframes_t frames;
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct virtaudio_device *vad = snd_pcm_substream_chip(substream);
	struct virtaudio_channel *vac = vad->channels[substream->stream];

	/* The host device now supports reporting HW position information using VIRTIO_AUDIO_CMD_PCM_POSITION_GET
	 * TODO: Investigate how this can be used to better sync position calculations
	 * NOTE: The ALSA pointer callback is called when the PCM middle layer (via buffer-update routine)
	 * inquires the current hardware position. This usually happens when snd_pcm_period_elapsed()
	 * is called in the interrupt routine, at least once per period. */
	frames = (vac->data_vaq.rx_seqno * runtime->period_size) % runtime->buffer_size;

	//LOG_VERBOSE(vad, "frames %lu", frames);
	return frames;
}

/* Invoked by PCM framework with stream lock held and local interrupts disabled
 * WARNING: Ack callback is atomic (non-schedulable). */
static int
snd_virtaudio_pcm_ack(struct snd_pcm_substream *substream)
{
	int ret = 0;
	struct virtaudio_device *vad = snd_pcm_substream_chip(substream);
	struct virtaudio_channel *vac = vad->channels[substream->stream];

	if (substream->runtime->status->state == SNDRV_PCM_STATE_RUNNING) {
		ret = virtaudio_send_data(vad, vac);
	}

	//LOG_VERBOSE(vad, "appl_ptr %ld", substream->runtime->control->appl_ptr);
	return ret;
}

static const struct snd_pcm_ops snd_virtaudio_pcm_ops = {
	.open = snd_virtaudio_open,
	.close = snd_virtaudio_close,
	.ioctl = snd_pcm_lib_ioctl,
	.hw_params = snd_virtaudio_pcm_hw_params,
	.hw_free = snd_virtaudio_pcm_hw_free,
	.prepare = snd_virtaudio_pcm_prepare,
	.trigger = snd_virtaudio_pcm_trigger,
	.pointer = snd_virtaudio_pcm_pointer,
	.ack = snd_virtaudio_pcm_ack
};

static const struct snd_pcm_chmap_elem map_table_mono = {
	.channels = 1,
	.map = {
		SNDRV_CHMAP_MONO /* mono stream */
	}
};

static const struct snd_pcm_chmap_elem map_table_full = {
	.channels = 9,
	.map = {
		SNDRV_CHMAP_FL, /* front left */
		SNDRV_CHMAP_FR, /* front right */
		SNDRV_CHMAP_RL, /* rear left */
		SNDRV_CHMAP_RR, /* rear right */
		SNDRV_CHMAP_FC, /* front center */
		SNDRV_CHMAP_LFE, /* LFE / woofer */
		SNDRV_CHMAP_SL, /* side left */
		SNDRV_CHMAP_SR, /* side right */
		SNDRV_CHMAP_RC, /* rear center */
#if 0
		/* new definitions */
		SNDRV_CHMAP_FLC, /* front left center */
		SNDRV_CHMAP_FRC, /* front right center */
		SNDRV_CHMAP_RLC, /* rear left center */
		SNDRV_CHMAP_RRC, /* rear right center */
		SNDRV_CHMAP_FLW, /* front left wide */
		SNDRV_CHMAP_FRW, /* front right wide */
		SNDRV_CHMAP_FLH, /* front left high */
		SNDRV_CHMAP_FCH, /* front center high */
		SNDRV_CHMAP_FRH, /* front right high */
		SNDRV_CHMAP_TC, /* top center */
		SNDRV_CHMAP_TFL, /* top front left */
		SNDRV_CHMAP_TFR, /* top front right */
		SNDRV_CHMAP_TFC, /* top front center */
		SNDRV_CHMAP_TRL, /* top rear left */
		SNDRV_CHMAP_TRR, /* top rear right */
		SNDRV_CHMAP_TRC, /* top rear center */
		/* new definitions for UAC2 */
		SNDRV_CHMAP_TFLC, /* top front left center */
		SNDRV_CHMAP_TFRC, /* top front right center */
		SNDRV_CHMAP_TSL, /* top side left */
		SNDRV_CHMAP_TSR, /* top side right */
		SNDRV_CHMAP_LLFE, /* left LFE */
		SNDRV_CHMAP_RLFE, /* right LFE */
		SNDRV_CHMAP_BC, /* bottom center */
		SNDRV_CHMAP_BLC, /* bottom left center */
		SNDRV_CHMAP_BRC /* bottom right center */
#endif
	}
};

static int
snd_virtaudio_chmap_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct virtaudio_channel *vac = ((struct virtaudio_device *)kcontrol->private_data)->channels[kcontrol->private_value];

	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = vac->config.max_voices;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = SNDRV_CHMAP_LAST;
	return 0;
}

static inline uint32_t
chmap_linux2virt(uint32_t const linux_chmap)
{
	switch (linux_chmap) {
	CASE(SNDRV_CHMAP_UNKNOWN, VIRTIO_AUDIO_CHMAP_UNKNOWN);
	CASE(SNDRV_CHMAP_NA, VIRTIO_AUDIO_CHMAP_NA);
	CASE(SNDRV_CHMAP_MONO, VIRTIO_AUDIO_CHMAP_MONO);
	CASE(SNDRV_CHMAP_FL, VIRTIO_AUDIO_CHMAP_FL);
	CASE(SNDRV_CHMAP_FR, VIRTIO_AUDIO_CHMAP_FR);
	CASE(SNDRV_CHMAP_RL, VIRTIO_AUDIO_CHMAP_RL);
	CASE(SNDRV_CHMAP_RR, VIRTIO_AUDIO_CHMAP_RR);
	CASE(SNDRV_CHMAP_FC, VIRTIO_AUDIO_CHMAP_FC);
	CASE(SNDRV_CHMAP_LFE, VIRTIO_AUDIO_CHMAP_LFE);
	CASE(SNDRV_CHMAP_SL, VIRTIO_AUDIO_CHMAP_SL);
	CASE(SNDRV_CHMAP_SR, VIRTIO_AUDIO_CHMAP_SR);
	CASE(SNDRV_CHMAP_RC, VIRTIO_AUDIO_CHMAP_RC);
	CASE(SNDRV_CHMAP_FLC, VIRTIO_AUDIO_CHMAP_FLC);
	CASE(SNDRV_CHMAP_FRC, VIRTIO_AUDIO_CHMAP_FRC);
	CASE(SNDRV_CHMAP_RLC, VIRTIO_AUDIO_CHMAP_RLC);
	CASE(SNDRV_CHMAP_RRC, VIRTIO_AUDIO_CHMAP_RRC);
	CASE(SNDRV_CHMAP_FLW, VIRTIO_AUDIO_CHMAP_FLW);
	CASE(SNDRV_CHMAP_FRW, VIRTIO_AUDIO_CHMAP_FRW);
	CASE(SNDRV_CHMAP_FLH, VIRTIO_AUDIO_CHMAP_FLH);
	CASE(SNDRV_CHMAP_FCH, VIRTIO_AUDIO_CHMAP_FCH);
	CASE(SNDRV_CHMAP_FRH, VIRTIO_AUDIO_CHMAP_FRH);
	CASE(SNDRV_CHMAP_TC, VIRTIO_AUDIO_CHMAP_TC);
	CASE(SNDRV_CHMAP_TFL, VIRTIO_AUDIO_CHMAP_TFL);
	CASE(SNDRV_CHMAP_TFR, VIRTIO_AUDIO_CHMAP_TFR);
	CASE(SNDRV_CHMAP_TFC, VIRTIO_AUDIO_CHMAP_TFC);
	CASE(SNDRV_CHMAP_TRL, VIRTIO_AUDIO_CHMAP_TRL);
	CASE(SNDRV_CHMAP_TRR, VIRTIO_AUDIO_CHMAP_TRR);
	CASE(SNDRV_CHMAP_TRC, VIRTIO_AUDIO_CHMAP_TRC);
	CASE(SNDRV_CHMAP_TFLC, VIRTIO_AUDIO_CHMAP_TFLC);
	CASE(SNDRV_CHMAP_TFRC, VIRTIO_AUDIO_CHMAP_TFRC);
	CASE(SNDRV_CHMAP_TSL, VIRTIO_AUDIO_CHMAP_TSL);
	CASE(SNDRV_CHMAP_TSR, VIRTIO_AUDIO_CHMAP_TSR);
	CASE(SNDRV_CHMAP_LLFE, VIRTIO_AUDIO_CHMAP_LLFE);
	CASE(SNDRV_CHMAP_RLFE, VIRTIO_AUDIO_CHMAP_RLFE);
	CASE(SNDRV_CHMAP_BC, VIRTIO_AUDIO_CHMAP_BC);
	CASE(SNDRV_CHMAP_BLC, VIRTIO_AUDIO_CHMAP_BLC);
	CASE(SNDRV_CHMAP_BRC, VIRTIO_AUDIO_CHMAP_BRC);
	default:
		BUG_ON(true);
	}

	return 0;
}

static int
snd_virtaudio_chmap_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	unsigned i, v;
	unsigned count = 0;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_channel *vac = vad->channels[kcontrol->private_value];
	const struct snd_pcm_chmap_elem *map;

	if (vac->config.max_voices == 1) {
		map = &map_table_mono;
	} else {
		map = &map_table_full;
	}
	for (v = 0; v < vac->config.max_voices; v++) {
		for (i = 0; i < map->channels; i++) {
			const uint32_t chmap_virt = chmap_linux2virt(map->map[i]);
			if (chmap_virt == vac->config.chmap_pos[v]) {
				ucontrol->value.integer.value[i] = map->map[i];
				count++;
			}
		}
	}
	if (count != vac->config.max_voices) {
		LOG_ERROR(vad, "Channel count %u expected %u", count, vac->config.max_voices);
		return -EINVAL;
	}

	return 0;
}

/* tvl channel map ctl element callback */
static int
snd_virtaudio_chmap_tlv(struct snd_kcontrol *kcontrol, int op_flag, unsigned int size, unsigned int __user *tlv1)
{
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_channel *vac = vad->channels[kcontrol->private_value];
	unsigned int __user *tlv2;
	unsigned int c, v, tlv1_length_bytes, tlv2_length_bytes;
	unsigned count = 0;
	const struct snd_pcm_chmap_elem *map;

	/* tlv2 value contains channel map data */
	tlv2_length_bytes = vac->config.max_voices * sizeof(unsigned int);
	/* tlv1 value contains entire tlv2 including type and length fields */
	tlv1_length_bytes = (2 * sizeof(unsigned int)) + tlv2_length_bytes;
	if (size < (2 * sizeof(unsigned int)) + tlv1_length_bytes) {
		return -ENOMEM;
	}

	/* set tlv1 (outer) type and length fields */
	if (put_user(SNDRV_CTL_TLVT_CONTAINER, tlv1)) {
		return -EFAULT;
	}
	if (put_user(tlv1_length_bytes, tlv1 + 1)) {
		return -EFAULT;
	}

	/* set tlv2 (inner) type and length fields */
	tlv2 = tlv1 + 2;
	if (put_user(SNDRV_CTL_TLVT_CHMAP_FIXED, tlv2)) {
		return -EFAULT;
	}
	if (put_user(tlv2_length_bytes, tlv2 + 1)) {
		return -EFAULT;
	}

	/* set tlv2 (inner) value */
	tlv2 += 2;
	if (vac->config.max_voices == 1) {
		map = &map_table_mono;
	} else {
		map = &map_table_full;
	}
	for (v = 0; v < vac->config.max_voices; v++) {
		for (c = 0; c < map->channels; c++) {
			const uint32_t chmap_virt = chmap_linux2virt(map->map[c]);
			if (chmap_virt == vac->config.chmap_pos[v]) {
				if (put_user(map->map[c], tlv2)) {
					return -EFAULT;
				}
				count++;
				tlv2++;
			}
		}
	}
	if (count != vac->config.max_voices) {
		LOG_ERROR(vad, "Channel count %u expected %u", count, vac->config.max_voices);
		return -EINVAL;
	}

	return 0;
}

static const struct snd_kcontrol_new chmap_controls = {
	.iface = SNDRV_CTL_ELEM_IFACE_PCM,
	.access = SNDRV_CTL_ELEM_ACCESS_READ | SNDRV_CTL_ELEM_ACCESS_TLV_READ | SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK,
	.info = snd_virtaudio_chmap_info,
	.get = snd_virtaudio_chmap_get,
	.tlv.c = snd_virtaudio_chmap_tlv,
};

static int
snd_virtaudio_new_stream(struct virtaudio_device *vad, struct virtaudio_channel *vac, int stream)
{
	int err;
	struct snd_kcontrol *kcontrol;

	if (!vac) {
		return 0;
	}

	snd_pcm_set_ops(vad->pcm, stream, &snd_virtaudio_pcm_ops);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,1,0)
	snd_pcm_lib_preallocate_pages(vad->pcm->streams[stream].substream, SNDRV_DMA_TYPE_CONTINUOUS,
		snd_dma_continuous_data(GFP_KERNEL), 0, vac->config.max_buffersize);
#else
	err = snd_pcm_lib_preallocate_pages(vad->pcm->streams[stream].substream, SNDRV_DMA_TYPE_CONTINUOUS,
		snd_dma_continuous_data(GFP_KERNEL), 0, vac->config.max_buffersize);
	if (err < 0) {
		LOG_ERROR(vad, "Unable to pre-allocate DMA buffer, err %d", err);
		return err;
	}
#endif

	kcontrol = snd_ctl_new1(&chmap_controls, vad);
	if (kcontrol == NULL) {
		LOG_ERROR(vad, "Error creating channel map controls");
		return -ENOMEM;
	}

	kcontrol->private_value = stream;

	strlcpy(kcontrol->id.name, (stream == SNDRV_PCM_STREAM_PLAYBACK) ?
		"Playback Channel Map" : "Capture Channel Map", sizeof(kcontrol->id.name));

	err = snd_ctl_add(vad->card, kcontrol);
	if (err < 0) {
		LOG_ERROR(vad, "Error adding channel map controls, err %d", err);
		return err;
	}
	vad->pcm->streams[stream].chmap_kctl = kcontrol;

	LOG_DEBUG(vad, "Created %s stream", (stream == SNDRV_PCM_STREAM_PLAYBACK) ? "playback" : "capture");

	return 0;
}

static int
snd_virtaudio_new_pcm(struct virtaudio_device *vad)
{
	int err;
	struct snd_pcm *pcm;
	struct virtaudio_channel *playback = vad->channels[SNDRV_PCM_STREAM_PLAYBACK];
	struct virtaudio_channel *capture = vad->channels[SNDRV_PCM_STREAM_CAPTURE];

	err = snd_pcm_new(vad->card, "virtio audio", 0, (playback != NULL), (capture != NULL), &pcm);
	if (err < 0) {
		LOG_ERROR(vad, "Failure to create PCM instance, err %d", err);
		return err;
	}

	pcm->private_data = vad;
	pcm->info_flags = 0;
	strcpy(pcm->name, "virtaudio");
	vad->pcm = pcm;

	err = snd_virtaudio_new_stream(vad, playback, SNDRV_PCM_STREAM_PLAYBACK);
	if (err < 0) {
		return err;
	}
	err = snd_virtaudio_new_stream(vad, capture, SNDRV_PCM_STREAM_CAPTURE);
	if (err < 0) {
		return err;
	}

	return 0;
}

static int
snd_virtaudio_volume_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct virtaudio_mixer *vam = ((struct virtaudio_device *)kcontrol->private_data)->mixer_controls[kcontrol->private_value];

	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_VOLUME) {
		uinfo->count = 1;
	} else {
		uinfo->count = vam->config.group.voices;
	}
	uinfo->value.integer.min = vam->config.group.vol_range.min;
	uinfo->value.integer.max = vam->config.group.vol_range.max;
	return 0;
}

static int
snd_virtaudio_volume_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int i, j, ret;
	unsigned count = 0;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_mixer *vam = vad->mixer_controls[kcontrol->private_value];
	const struct snd_pcm_chmap_elem *map;
	struct virtio_audio_mixergrp_volume_params volume_params = { 0 };

	ret = virtaudio_cmd_volume_get(vad, vam, &volume_params);
	if (ret != 0) {
		return ret;
	}

	mutex_lock(&vam->mixer_lock);
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_VOLUME) {
		ucontrol->value.integer.value[0] = volume_params.vol[0];
	} else {
		if (vam->config.group.voices == 1) {
			map = &map_table_mono;
		} else {
			map = &map_table_full;
		}
		for (i = 0; i < map->channels; i++) {
			const uint32_t chmap_virt = chmap_linux2virt(map->map[i]);
			for (j = 0; j < vam->config.group.voices; j++) {
				/* find the corresponding channel */
				if (chmap_virt == vam->config.group.chmap_pos[j]) {
					ucontrol->value.integer.value[i] = volume_params.vol[j];
					count++;
					break;
				}
			}
			if (count == vam->config.group.voices) {
				break;
			}
		}
		if (count != vam->config.group.voices) {
			LOG_ERROR(vad, "Channel count %u expected %u", count, vam->config.group.voices);
			ret = -EINVAL;
		}
	}
	mutex_unlock(&vam->mixer_lock);

	return ret;
}

static int
snd_virtaudio_volume_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int i, j, ret;
	int change = 0;
	const struct snd_pcm_chmap_elem *map;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_mixer *vam = vad->mixer_controls[kcontrol->private_value];
	struct virtio_audio_mixergrp_volume_params volume_params = { 0 };

	ret = virtaudio_cmd_volume_get(vad, vam, &volume_params);
	if (ret != 0) {
		return ret;
	}

	mutex_lock(&vam->mixer_lock);
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_VOLUME) {
		if (volume_params.vol[0] != ucontrol->value.integer.value[0]) {
			volume_params.vol[0] = ucontrol->value.integer.value[0];
			change = 1;
		}
	} else {
		if (vam->config.group.voices == 1) {
			map = &map_table_mono;
		} else {
			map = &map_table_full;
		}
		for (i = 0; i < map->channels; i++) {
			const uint32_t chmap_virt = chmap_linux2virt(map->map[i]);
			for (j = 0; j < vam->config.group.voices; j++) {
				/* find the corresponding channel */
				if (chmap_virt == vam->config.group.chmap_pos[j]) {
					if (volume_params.vol[j] == ucontrol->value.integer.value[i]) {
						/* volume is unchanged */
						break;
					} else {
						volume_params.vol[j] = ucontrol->value.integer.value[i];
						change = 1;
						break;
					}
				}
			}
		}
	}
	if (change) {
		ret = virtaudio_cmd_volume_set(vad, vam, &volume_params);
	}
	mutex_unlock(&vam->mixer_lock);

	return ret ? ret : change;
}

static const struct snd_kcontrol_new volume_controls = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.access = SNDRV_CTL_ELEM_ACCESS_READWRITE | SNDRV_CTL_ELEM_ACCESS_TLV_READ,
	.info = snd_virtaudio_volume_info,
	.get = snd_virtaudio_volume_get,
	.put = snd_virtaudio_volume_put,
};

static int
snd_virtaudio_mute_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct virtaudio_mixer *vam = ((struct virtaudio_device *)kcontrol->private_data)->mixer_controls[kcontrol->private_value];

	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_MUTE) {
		uinfo->count = 1;
	} else {
		uinfo->count = vam->config.group.voices;
	}
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int
snd_virtaudio_mute_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int i, j, ret;
	unsigned count = 0;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_mixer *vam = vad->mixer_controls[kcontrol->private_value];
	const struct snd_pcm_chmap_elem *map;
	uint32_t mute = 0;

	ret = virtaudio_cmd_mute_get(vad, vam, &mute);
	if (ret != 0) {
		return ret;
	}

	/* The host provides channel mute settings (bitmask) which are opposite of the
	 * instantiated switch controls e.g. muted (1) means switched OFF (0) and vice versa */

	mutex_lock(&vam->mixer_lock);
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_MUTE) {
		ucontrol->value.integer.value[0] = (mute & 1) ? 0 : 1;
	} else {
		if (vam->config.group.voices == 1) {
			map = &map_table_mono;
		} else {
			map = &map_table_full;
		}
		for (i = 0; i < map->channels; i++) {
			const uint32_t chmap_virt = chmap_linux2virt(map->map[i]);
			for (j = 0; j < vam->config.group.voices; j++) {
				/* find the corresponding channel */
				if (chmap_virt == vam->config.group.chmap_pos[j]) {
					ucontrol->value.integer.value[i] = ((mute >> j) & 1) ? 0 : 1;
					count++;
					break;
				}
			}
			if (count == vam->config.group.voices) {
				break;
			}
		}
		if (count != vam->config.group.voices) {
			LOG_ERROR(vad, "Channel count %u expected %u", count, vam->config.group.voices);
			ret = -EINVAL;
		}
	}
	mutex_unlock(&vam->mixer_lock);

	return ret;
}

static int
snd_virtaudio_mute_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int i, j, ret;
	int change = 0;
	const struct snd_pcm_chmap_elem *map;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_mixer *vam = vad->mixer_controls[kcontrol->private_value];
	uint32_t mute = 0;

	ret = virtaudio_cmd_mute_get(vad, vam, &mute);
	if (ret != 0) {
		return ret;
	}

	/* The channel mute settings (bitmask) sent to the host are opposite of the
	 * instantiated switch controls e.g. switched ON (1) means unmuted (0) and vice versa
	 * Note: Only store the updated value if it is different (changed) */

	mutex_lock(&vam->mixer_lock);
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_MUTE) {
		if ((mute & 1) == ucontrol->value.integer.value[0]) {
			mute = ucontrol->value.integer.value[0] ? 0 : 1;
			change = 1;
		}
	} else {
		if (vam->config.group.voices == 1) {
			map = &map_table_mono;
		} else {
			map = &map_table_full;
		}
		for (i = 0; i < map->channels; i++) {
			const uint32_t chmap_virt = chmap_linux2virt(map->map[i]);
			for (j = 0; j < vam->config.group.voices; j++) {
				/* find the corresponding channel */
				if (chmap_virt == vam->config.group.chmap_pos[j]) {
					if (((mute >> j) & 1) != ucontrol->value.integer.value[i]) {
						/* mute is unchanged */
						break;
					} else {
						mute = (mute & ~(1UL << j)) | ((ucontrol->value.integer.value[i] ? 0 : 1) << j);
						change = 1;
						break;
					}
				}
			}
		}
	}
	if (change) {
		ret = virtaudio_cmd_mute_set(vad, vam, mute);
	}
	mutex_unlock(&vam->mixer_lock);

	return ret ? ret : change;
}

static const struct snd_kcontrol_new mute_controls = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.info = snd_virtaudio_mute_info,
	.get = snd_virtaudio_mute_get,
	.put = snd_virtaudio_mute_put,
};

static int
snd_virtaudio_switch_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct virtaudio_mixer *vam = ((struct virtaudio_device *)kcontrol->private_data)->mixer_controls[kcontrol->private_value];

	uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_SWITCH) {
		uinfo->count = 1;
	} else {
		uinfo->count = vam->config.group.voices;
	}
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 1;
	return 0;
}

static int
snd_virtaudio_switch_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int i, j, ret;
	unsigned count = 0;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_mixer *vam = vad->mixer_controls[kcontrol->private_value];
	const struct snd_pcm_chmap_elem *map;
	uint32_t switch_on = 0;

	ret = virtaudio_cmd_switch_get(vad, vam, &switch_on);
	if (ret != 0) {
		return ret;
	}

	mutex_lock(&vam->mixer_lock);
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_SWITCH) {
		ucontrol->value.integer.value[0] = switch_on & 1;
	} else {
		if (vam->config.group.voices == 1) {
			map = &map_table_mono;
		} else {
			map = &map_table_full;
		}
		for (i = 0; i < map->channels; i++) {
			const uint32_t chmap_virt = chmap_linux2virt(map->map[i]);
			for (j = 0; j < vam->config.group.voices; j++) {
				/* find the corresponding channel */
				if (chmap_virt == vam->config.group.chmap_pos[j]) {
					ucontrol->value.integer.value[i] = (switch_on >> j) & 1;
					count++;
					break;
				}
			}
			if (count == vam->config.group.voices) {
				break;
			}
		}
		if (count != vam->config.group.voices) {
			LOG_ERROR(vad, "Channel count %u expected %u", count, vam->config.group.voices);
			ret = -EINVAL;
		}
	}
	mutex_unlock(&vam->mixer_lock);

	return ret;
}

static int
snd_virtaudio_switch_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int i, j, ret;
	int change = 0;
	const struct snd_pcm_chmap_elem *map;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_mixer *vam = vad->mixer_controls[kcontrol->private_value];
	uint32_t switch_on = 0;

	ret = virtaudio_cmd_switch_get(vad, vam, &switch_on);
	if (ret != 0) {
		return ret;
	}

	mutex_lock(&vam->mixer_lock);
	if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_SWITCH) {
		if ((switch_on & 1) != ucontrol->value.integer.value[0]) {
			switch_on = ucontrol->value.integer.value[0];
			change = 1;
		}
	} else {
		if (vam->config.group.voices == 1) {
			map = &map_table_mono;
		} else {
			map = &map_table_full;
		}
		for (i = 0; i < map->channels; i++) {
			const uint32_t chmap_virt = chmap_linux2virt(map->map[i]);
			for (j = 0; j < vam->config.group.voices; j++) {
				/* find the corresponding channel */
				if (chmap_virt == vam->config.group.chmap_pos[j]) {
					if (((switch_on >> j) & 1) == ucontrol->value.integer.value[i]) {
						/* switch value is unchanged */
						break;
					} else {
						switch_on = (switch_on & ~(1UL << j)) | (ucontrol->value.integer.value[i] << j);
						change = 1;
						break;
					}
				}
			}
		}
	}
	if (change) {
		ret = virtaudio_cmd_switch_set(vad, vam, switch_on);
	}
	mutex_unlock(&vam->mixer_lock);

	return ret ? ret : change;
}

static const struct snd_kcontrol_new switch_controls = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.info = snd_virtaudio_switch_info,
	.get = snd_virtaudio_switch_get,
	.put = snd_virtaudio_switch_put,
};

static int
snd_virtaudio_source_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct virtaudio_device *vad = kcontrol->private_data;

	uinfo->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
	uinfo->count = 1;
	uinfo->value.enumerated.items = vad->source_num;
	strlcpy(uinfo->value.enumerated.name, vad->source_controls[uinfo->value.enumerated.item]->config.name, sizeof(uinfo->value.enumerated.name));
	return 0;
}

static int
snd_virtaudio_source_active_idx(struct virtaudio_device *vad)
{
	int i;

	for (i = 0; i < vad->source_num; i++) {
		uint32_t switch_on = 0;
		const int ret = virtaudio_cmd_switch_get(vad, vad->source_controls[i], &switch_on);
		if (ret != 0) {
			return ret;
		}
		if (switch_on) {
			break;
		}
	}
	if (i == vad->source_num) {
		LOG_ERROR(vad, "No active source found");
		return -EINVAL;
	}

	return i;
}

static int
snd_virtaudio_source_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct virtaudio_device *vad = kcontrol->private_data;

	const int idx = snd_virtaudio_source_active_idx(vad);
	if (idx < 0) {
		return idx;
	}

	mutex_lock(&vad->source_lock);
	ucontrol->value.enumerated.item[0] = idx;
	mutex_unlock(&vad->source_lock);

	return 0;
}

static int
snd_virtaudio_source_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int ret = 0;
	int change = 0;
	struct virtaudio_device *vad = kcontrol->private_data;

	const int idx = snd_virtaudio_source_active_idx(vad);
	if (idx < 0) {
		return idx;
	}

	mutex_lock(&vad->source_lock);
	if (idx != ucontrol->value.enumerated.item[0]) {
		uint32_t switch_on = 0;
		struct virtaudio_mixer *vam = vad->source_controls[ucontrol->value.enumerated.item[0]];
		if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_JOINTLY_SWITCH) {
			switch_on = 1;
		} else {
			switch_on = (1UL << vam->config.group.voices) - 1UL;
		}
		ret = virtaudio_cmd_switch_set(vad, vam, switch_on);
		change = 1;
	}
	mutex_unlock(&vad->source_lock);

	return ret ? ret : change;
}

static const struct snd_kcontrol_new source_controls = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.info = snd_virtaudio_source_info,
	.get = snd_virtaudio_source_get,
	.put = snd_virtaudio_source_put,
};

static int
snd_virtaudio_control_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	struct virtaudio_mixer *vam = ((struct virtaudio_device *)kcontrol->private_data)->mixer_controls[kcontrol->private_value];

	uinfo->count = 1;

	switch (vam->config.sw.type) {
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER:
		if ((vam->config.sw.integer.min == 0) && (vam->config.sw.integer.max == 1)) {
			uinfo->type = SNDRV_CTL_ELEM_TYPE_BOOLEAN;
		} else {
			uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
		}
		uinfo->value.integer.min = vam->config.sw.integer.min;
		uinfo->value.integer.max = vam->config.sw.integer.max;
		uinfo->value.integer.step = vam->config.sw.integer.step;
		break;
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER64:
		uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER64;
		uinfo->value.integer64.min = vam->config.sw.integer64.min;
		uinfo->value.integer64.max = vam->config.sw.integer64.max;
		uinfo->value.integer64.step = vam->config.sw.integer64.step;
		break;
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_ENUMERATED:
		uinfo->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
		uinfo->value.enumerated.items = vam->config.sw.enumerated.item_num;
		strlcpy(uinfo->value.enumerated.name, vam->config.sw.enumerated.item_list[uinfo->value.enumerated.item], sizeof(uinfo->value.enumerated.name));
		break;
	}

	return 0;
}

static int
snd_virtaudio_control_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int ret;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_mixer *vam = vad->mixer_controls[kcontrol->private_value];
	struct virtio_audio_mixersw_params control_params;

	ret = virtaudio_cmd_control_get(vad, vam, &control_params);
	if (ret != 0) {
		return ret;
	}

	mutex_lock(&vam->mixer_lock);
	switch (vam->config.sw.type) {
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER:
		ucontrol->value.integer.value[0] = control_params.integer;
		break;
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER64:
		ucontrol->value.integer64.value[0] = control_params.integer64;
		break;
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_ENUMERATED:
		ucontrol->value.enumerated.item[0] = control_params.item;
		break;
	}
	mutex_unlock(&vam->mixer_lock);

	return ret;
}

static int
snd_virtaudio_control_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	int ret;
	int change = 0;
	struct virtaudio_device *vad = kcontrol->private_data;
	struct virtaudio_mixer *vam = vad->mixer_controls[kcontrol->private_value];
	struct virtio_audio_mixersw_params control_params;

	ret = virtaudio_cmd_control_get(vad, vam, &control_params);
	if (ret != 0) {
		return ret;
	}

	mutex_lock(&vam->mixer_lock);
	switch (vam->config.sw.type) {
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER:
		if (control_params.integer != ucontrol->value.integer.value[0]) {
			control_params.integer = ucontrol->value.integer.value[0];
			change = 1;
		}
		break;
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER64:
		if (control_params.integer64 != ucontrol->value.integer64.value[0]) {
			control_params.integer64 = ucontrol->value.integer64.value[0];
			change = 1;
		}
		break;
	case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_ENUMERATED:
		if (control_params.item != ucontrol->value.enumerated.item[0]) {
			control_params.item = ucontrol->value.enumerated.item[0];
			change = 1;
		}
		break;
	}
	if (change) {
		ret = virtaudio_cmd_control_set(vad, vam, &control_params);
	}
	mutex_unlock(&vam->mixer_lock);

	return ret ? ret : change;
}

static const struct snd_kcontrol_new controls = {
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.info = snd_virtaudio_control_info,
	.get = snd_virtaudio_control_get,
	.put = snd_virtaudio_control_put,
};

static int
snd_virtaudio_new_mixer(struct virtaudio_device *vad)
{
	int i, err;
	struct snd_kcontrol *kcontrol = NULL;

	for (i = 0; i < ARRAY_SIZE(vad->mixer_controls); i++) {
		struct virtaudio_mixer *vam = vad->mixer_controls[i];
		if (vam == NULL) {
			continue;
		}

		mutex_init(&vam->mixer_lock);

		if ((vam->config.mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_PLAYBACK_GROUP) || (vam->config.mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_CAPTURE_GROUP)) {
			if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_VOLUME_ALL) {
				kcontrol = snd_ctl_new1(&volume_controls, vad);
				if (kcontrol == NULL) {
					LOG_ERROR(vad, "Error creating volume controls");
					return -ENOMEM;
				}
				snprintf(kcontrol->id.name, sizeof(kcontrol->id.name), "%s %s Volume", vam->config.name,
					(vam->config.mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_PLAYBACK_GROUP) ? "Playback" : "Capture");
				kcontrol->private_value = i;
				vam->db_scale_vol[0] = SNDRV_CTL_TLVT_DB_MINMAX;
				vam->db_scale_vol[1] = 2 * sizeof(unsigned int);
				vam->db_scale_vol[2] = vam->config.group.db_vol_range.min;
				vam->db_scale_vol[3] = vam->config.group.db_vol_range.max;
				kcontrol->tlv.p = vam->db_scale_vol;
				err = snd_ctl_add(vad->card, kcontrol);
				if (err != 0) {
					LOG_ERROR(vad, "Error adding volume controls");
					return err;
				}
				LOG_DEBUG(vad, "Created '%s' control", kcontrol->id.name);
			}

			if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_MUTE_ALL) {
				kcontrol = snd_ctl_new1(&mute_controls, vad);
				if (kcontrol == NULL) {
					LOG_ERROR(vad, "Error creating mute controls");
					return -ENOMEM;
				}
				snprintf(kcontrol->id.name, sizeof(kcontrol->id.name), "%s %s Switch", vam->config.name,
					(vam->config.mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_PLAYBACK_GROUP) ? "Playback" : "Capture");
				kcontrol->private_value = i;
				err = snd_ctl_add(vad->card, kcontrol);
				if (err != 0) {
					LOG_ERROR(vad, "Error adding mute controls");
					return err;
				}
				LOG_DEBUG(vad, "Created '%s' control", kcontrol->id.name);
			}

			if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_SWITCH_ALL) {
				kcontrol = snd_ctl_new1(&switch_controls, vad);
				if (kcontrol == NULL) {
					LOG_ERROR(vad, "Error creating switch controls");
					return -ENOMEM;
				}
				snprintf(kcontrol->id.name, sizeof(kcontrol->id.name), "%s %s Switch", vam->config.name,
					(vam->config.mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_PLAYBACK_GROUP) ? "Playback" : "Capture");
				kcontrol->private_value = i;
				kcontrol->id.index = 1;
				err = snd_ctl_add(vad->card, kcontrol);
				if (err != 0) {
					LOG_ERROR(vad, "Error adding switch controls");
					return err;
				}

				if (vam->config.group.capabilities & VIRTIO_AUDIO_MIXER_CAP_MSK_EXCL_SWITCH) {
					vad->source_controls[vad->source_num] = vam;
					vad->source_num++;
				}
				LOG_DEBUG(vad, "Created '%s' control", kcontrol->id.name);
			}
		} else if (vam->config.mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_SWITCH) {
			kcontrol = snd_ctl_new1(&controls, vad);
			if (kcontrol == NULL) {
				LOG_ERROR(vad, "Error creating controls");
				return -ENOMEM;
			}
			snprintf(kcontrol->id.name, sizeof(kcontrol->id.name), "%s", vam->config.name);
			kcontrol->private_value = i;
			err = snd_ctl_add(vad->card, kcontrol);
			if (err != 0) {
				LOG_ERROR(vad, "Error adding controls");
				return err;
			}
			LOG_DEBUG(vad, "Created '%s' control", kcontrol->id.name);
		}
	}

	/* If supported, instantiate one global capture (input) source control
	 * Note: We assume all mixers with exclusive-capture are connected to one MUX */
	if (vad->source_num) {
		mutex_init(&vad->source_lock);

		kcontrol = snd_ctl_new1(&source_controls, vad);
		if (kcontrol == NULL) {
			LOG_ERROR(vad, "Error creating source controls");
			return -ENOMEM;
		}
		strlcpy(kcontrol->id.name, "Capture Source", sizeof(kcontrol->id.name));
		kcontrol->id.index = 1;
		err = snd_ctl_add(vad->card, kcontrol);
		if (err != 0) {
			LOG_ERROR(vad, "Error adding source controls");
			return err;
		}
		LOG_DEBUG(vad, "Created '%s' control", kcontrol->id.name);
	}

	strcpy(vad->card->mixername, "virtaudio mixer");

	return 0;
}

static int
snd_virtaudio_probe(struct virtio_device *vdev)
{
	int err;
	struct snd_card *card;
	struct virtaudio_device *vad = NULL;
	static struct snd_device_ops ops = { 0 };

	err = snd_card_new(&vdev->dev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1, THIS_MODULE, 0, &card);
	if (err < 0) {
		dev_err(&vdev->dev, "%s: Failure to create/initialize soundcard structure, err %d\n", __func__, err);
		return err;
	}

	err = virtaudio_create(vdev, &vad);
	if (err < 0) {
		dev_err(&vdev->dev, "%s: Unable to create virtio device, err %d\n", __func__, err);
		goto err_virtaudio;
	}

	vad->card = card;
	card->private_data = vad;

	strlcpy(card->driver, "virtaudio", sizeof(card->driver));
	strlcpy(card->shortname, "virtaudio", sizeof(card->shortname));
	sprintf(card->longname, "virtaudio %i", vdev->index);

	err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, vad, &ops);
	if (err) {
		LOG_ERROR(vad, "Failure creating ALSA device component, err %d", err);
		goto err_virtaudio;
	}

	err = snd_virtaudio_new_pcm(vad);
	if (err) {
		LOG_ERROR(vad, "Unable to create PCM instance(s), err %d", err);
		goto err_virtaudio;
	}
	err = snd_virtaudio_new_mixer(vad);
	if (err) {
		LOG_ERROR(vad, "Unable to create mixer instance(s), err %d", err);
		goto err_virtaudio;
	}

	err = snd_card_register(card);
	if (err) {
		LOG_ERROR(vad, "Failure registering card instance, err %d", err);
		goto err_virtaudio;
	}

	dev_set_drvdata(&vdev->dev, card);

	dev_info(&vdev->dev, "card '%s' registered\n", card->longname);
	return 0;

err_virtaudio:
	snd_card_free(card);
	return err;
}

static void
snd_virtaudio_remove(struct virtio_device *vdev)
{
	struct virtaudio_device *vad = vdev->priv;
	snd_card_free(vad->card);
	virtaudio_free(vad);
	vdev->priv = NULL;
	dev_info(&vdev->dev, "card removed\n");
}

#ifdef CONFIG_PM_SLEEP
/* Registered with the device driver-model dev_pm_ops struct and invoked first */
static int
snd_virtaudio_suspend(struct device *dev)
{
	int i, ret;
	struct snd_card *card = dev_get_drvdata(dev);
	struct virtaudio_device *vad = card->private_data;

	snd_power_change_state(card, SNDRV_CTL_POWER_D3hot);

	snd_pcm_suspend_all(vad->pcm);

	for (i = 0; i < ARRAY_SIZE(vad->channels); i++) {
		struct virtaudio_channel *vac = vad->channels[i];
		/* We only care about the streams which have been acquired */
		if (vac && vac->data_vaq.buf_info) {
			LOG_DEBUG(vad, "stream %u", i);

			ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_RELEASE, true);
			if (ret != 0) {
				return ret;
			}

			wait_event(vac->data_vaq.rsp_done, (vac->data_vaq.rx_seqno == vac->data_vaq.tx_seqno));
		}
	}

    return 0;
}

static int
snd_virtaudio_resume(struct device *dev)
{
	int i, ret;
	struct snd_card *card = dev_get_drvdata(dev);
	struct virtaudio_device *vad = card->private_data;

	for (i = 0; i < ARRAY_SIZE(vad->channels); i++) {
		struct virtaudio_channel *vac = vad->channels[i];
		if (vac && vac->data_vaq.buf_info) {
			struct snd_pcm_runtime *runtime = vac->substream->runtime;
			struct virtio_audio_acquire_params acquire_params = { 0 };

			LOG_DEBUG(vad, "stream %u", i);

			acquire_params.rate = runtime->rate;
			acquire_params.voices = runtime->channels;
			acquire_params.format = multiple2(format_virtlinux(0, (1 << runtime->format)));
			acquire_params.frag_size = frames_to_bytes(runtime, runtime->period_size);
			acquire_params.frags = runtime->periods;

			ret = virtaudio_cmd_acquire(vad, vac, &acquire_params);
			if (ret != 0) {
				return ret;
			}

			ret = virtaudio_cmd_send_noparams(vad, vac->config.channel_type, VIRTIO_AUDIO_CMD_PCM_PREPARE, true);
			if (ret != 0) {
				return ret;
			}
		}
	}

	snd_power_change_state(card, SNDRV_CTL_POWER_D0);

    return 0;
}

static SIMPLE_DEV_PM_OPS(snd_virtaudio_pm_ops, snd_virtaudio_suspend, snd_virtaudio_resume);
#endif

static unsigned int snd_virtaudio_features[] = {
	VIRTIO_AUDIO_F_PCM_AUTO_XRUN_HDL,
	VIRTIO_AUDIO_F_PCM_CAPTURE_PAUSE_RESUME,
	VIRTIO_AUDIO_F_PCM_PLAYBACK_PAUSE_RESUME,
	VIRTIO_AUDIO_F_MIXER_CTL
};

static struct virtio_device_id snd_virtaudio_id_table[] = {
	{ VIRTIO_DT_Blackberry_QNX_AUDIO, VIRTIO_DEV_ANY_ID },
	{ 0 },
};

static struct virtio_driver snd_virtaudio_driver = {
	.driver.name = KBUILD_MODNAME,
	.driver.owner = THIS_MODULE,
	.feature_table = snd_virtaudio_features,
	.feature_table_size = ARRAY_SIZE(snd_virtaudio_features),
	.id_table = snd_virtaudio_id_table,
	.probe = snd_virtaudio_probe,
	.remove = snd_virtaudio_remove,
#ifdef CONFIG_PM_SLEEP
	.freeze = virtaudio_freeze,
	.restore = virtaudio_restore,
	.driver.pm = &snd_virtaudio_pm_ops,
#endif
};

module_virtio_driver(snd_virtaudio_driver);
MODULE_DEVICE_TABLE(virtio, snd_virtaudio_id_table);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Virtio audio device driver");
MODULE_AUTHOR("Alain Magloire <alain@qnx.com>");
MODULE_AUTHOR("Ken Wilburn <kwilburn@qnx.com>");

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/qpdc/mainline/linux_drivers/sound/virtio/snd_virtaudio.c $ $Rev: 948565 $")
#endif
