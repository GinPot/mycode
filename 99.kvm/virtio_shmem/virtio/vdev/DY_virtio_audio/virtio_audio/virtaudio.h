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

#ifndef _VIRTAUDIO_H
#define _VIRTAUDIO_H

#include <linux/module.h>
#include <linux/virtio_config.h>
#include <linux/version.h>
#include <linux/delay.h>

#include <sound/initval.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/pcm_params.h>
#include <sound/tlv.h>

#include "virtio_audio.h"

#define LOG_ERROR(vad, fmt, args...) do { dev_err(&(vad)->vdev->dev, "%s: " fmt "\n", __func__, ##args); } while(0)
#define LOG_DEBUG(vad, fmt, args...) do { if (log_debug >= 1) dev_info(&(vad)->vdev->dev, "%s: " fmt "\n", __func__, ##args); } while(0)
#define LOG_VERBOSE(vad, fmt, args...) do { if (log_debug >= 2) dev_info(&(vad)->vdev->dev, "%s: " fmt "\n", __func__, ##args); } while(0)
#define CASE(lchmap, vchmap) case (lchmap): return (vchmap)

/* Transitional (experimental) devices are supported on mainline hypervisor
 * Note: The host (vdev) device will also need to set the same ID */
enum virtio_device_type {
	VIRTIO_DT_EXPERIMENTAL_BASE=0xffc0,
	VIRTIO_DT_Blackberry_QNX_AUDIO=VIRTIO_DT_EXPERIMENTAL_BASE,
	VIRTIO_DT_EXPERIMENTAL=0xffff
};

struct virtaudio_buf_info {
	uint8_t *data_ptr;
	uint8_t *status_ptr;
	unsigned int rsp_len;
	bool rsp_wait;
};

struct virtaudio_queue {
	struct virtqueue *vq;
	int queue_size;
	uint8_t *data_mem;
	uint8_t *status_mem;
	uint32_t buf_num;
	uint32_t buf_size;
	struct virtaudio_buf_info *buf_info;
	uint16_t tx_seqno;
	uint16_t rx_seqno;
	uint16_t seqno_wrap;
	char name[40];
	unsigned int vq_idx;
	wait_queue_head_t rsp_done;
};

struct virtaudio_channel {
	struct virtio_audio_chan_config config;
	struct virtaudio_queue data_vaq;
	bool drain_triggered;
	/* ALSA part */
	struct snd_pcm_substream *substream;
};

struct virtaudio_mixer {
	struct virtio_audio_mixer_ctl_config config;
	struct mutex mixer_lock;
	unsigned int db_scale_vol[4];
};

struct virtaudio_device {
	struct virtio_device *vdev;
	struct virtaudio_channel *channels[VIRTIO_AUDIO_CHANNEL_MAX];
	struct virtaudio_mixer *mixer_controls[VIRTIO_AUDIO_MIXER_CTL_MAX];
	struct virtaudio_queue cmd_vaq;
	spinlock_t cmd_lock;
	uint32_t debug;
	struct virtaudio_mixer *source_controls[VIRTIO_AUDIO_MIXER_CTL_MAX];
	uint32_t source_num;
	struct mutex source_lock;
	/* ALSA part */
	struct snd_card *card;
	struct snd_pcm *pcm;
};

int32_t virtaudio_cmd_send_noparams(struct virtaudio_device *vad, uint32_t channel_type, uint32_t id, bool wait);
int32_t virtaudio_cmd_acquire(struct virtaudio_device *vad, struct virtaudio_channel *vac, struct virtio_audio_acquire_params *aparams);
int32_t virtaudio_cmd_volume_get(struct virtaudio_device *vad, struct virtaudio_mixer *vam, struct virtio_audio_mixergrp_volume_params *vparams);
int32_t virtaudio_cmd_volume_set(struct virtaudio_device *vad, struct virtaudio_mixer *vam, struct virtio_audio_mixergrp_volume_params *vparams);
int32_t virtaudio_cmd_mute_get(struct virtaudio_device *vad, struct virtaudio_mixer *vam, uint32_t *mute);
int32_t virtaudio_cmd_mute_set(struct virtaudio_device *vad, struct virtaudio_mixer *vam, uint32_t mute);
int32_t virtaudio_cmd_switch_get(struct virtaudio_device *vad, struct virtaudio_mixer *vam, uint32_t *switch_on);
int32_t virtaudio_cmd_switch_set(struct virtaudio_device *vad, struct virtaudio_mixer *vam, uint32_t switch_on);
int32_t virtaudio_cmd_control_get(struct virtaudio_device *vad, struct virtaudio_mixer *vam, struct virtio_audio_mixersw_params *ctlparams);
int32_t virtaudio_cmd_control_set(struct virtaudio_device *vad, struct virtaudio_mixer *vam, struct virtio_audio_mixersw_params *ctlparams);

int32_t virtaudio_create(struct virtio_device *vdev, struct virtaudio_device **pvad);
void virtaudio_free(struct virtaudio_device *vad);
int32_t virtaudio_send_data(struct virtaudio_device *vad, struct virtaudio_channel *vac);
#ifdef CONFIG_PM_SLEEP
int virtaudio_freeze(struct virtio_device *vdev);
int virtaudio_restore(struct virtio_device *vdev);
#endif

extern int log_debug;

#endif /* _VIRTAUDIO_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
#ifdef __ASM__
__SRCVERSION "$URL: http://svn.ott.qnx.com/product/qpdc/mainline/linux_drivers/sound/virtio/virtaudio.h $ $Rev: 948565 $"
#else
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/qpdc/mainline/linux_drivers/sound/virtio/virtaudio.h $ $Rev: 948565 $")
#endif
#endif
