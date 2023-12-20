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

static int
virtaudio_cmd_queue_init(struct virtaudio_device *vad)
{
	size_t mem_size, cmd_status_desc_size;
	int i;

	if (vad->cmd_vaq.queue_size > VIRTIO_AUDIO_VIRTQ_SIZE_CTRL) {
		LOG_DEBUG(vad, "Offered queue_size %d, limited to %u", vad->cmd_vaq.queue_size, VIRTIO_AUDIO_VIRTQ_SIZE_CTRL);
		vad->cmd_vaq.queue_size = VIRTIO_AUDIO_VIRTQ_SIZE_CTRL;
	}

	/* Number of buffers is half the queue size because it is comprised of data and status descriptors */
	vad->cmd_vaq.buf_num = vad->cmd_vaq.queue_size / 2;
	vad->cmd_vaq.buf_size = sizeof(struct virtio_audio_cmd);

	LOG_DEBUG(vad, "buf_num %u buf_size %u", vad->cmd_vaq.buf_num, vad->cmd_vaq.buf_size);

	mem_size = vad->cmd_vaq.buf_size * vad->cmd_vaq.buf_num;
	vad->cmd_vaq.data_mem = kzalloc(mem_size, GFP_KERNEL);
	if (vad->cmd_vaq.data_mem == NULL) {
		LOG_ERROR(vad, "Unable to allocate data descriptor memory");
		return ENOMEM;
	}

	cmd_status_desc_size = sizeof(struct virtio_audio_cmd_status);
	mem_size = cmd_status_desc_size * vad->cmd_vaq.buf_num;
	vad->cmd_vaq.status_mem = kzalloc(mem_size, GFP_KERNEL);
	if(vad->cmd_vaq.status_mem == NULL) {
		LOG_ERROR(vad, "Unable to allocate status descriptor memory");
		kfree(vad->cmd_vaq.data_mem);
		return ENOMEM;
	}

	vad->cmd_vaq.buf_info = kcalloc(vad->cmd_vaq.buf_num, sizeof(struct virtaudio_buf_info), GFP_KERNEL);
	if (vad->cmd_vaq.buf_info == NULL) {
		LOG_ERROR(vad, "Unable to allocate memory for buf_info");
		kfree(vad->cmd_vaq.status_mem);
		kfree(vad->cmd_vaq.data_mem);
		return ENOMEM;
	}

	for (i = 0; i < vad->cmd_vaq.buf_num; i++) {
		vad->cmd_vaq.buf_info[i].data_ptr = vad->cmd_vaq.data_mem + i * vad->cmd_vaq.buf_size;
		vad->cmd_vaq.buf_info[i].status_ptr = vad->cmd_vaq.status_mem + i * cmd_status_desc_size;
		LOG_VERBOSE(vad, "frag[%d]: data_ptr %p status_ptr %p", i, vad->cmd_vaq.buf_info[i].data_ptr, vad->cmd_vaq.buf_info[i].status_ptr);
	}

	spin_lock_init(&vad->cmd_lock);
	init_waitqueue_head(&vad->cmd_vaq.rsp_done);

	return 0;
}

static inline char *
status_virt2string(int const virt_status)
{
	switch (virt_status) {
	CASE(VIRTIO_AUDIO_S_OK, "OK");
	CASE(VIRTIO_AUDIO_S_UNSUPP, "UNSUPP");
	CASE(VIRTIO_AUDIO_S_INVALID, "INVALID");
	CASE(VIRTIO_AUDIO_S_FAILURE, "FAILURE");
	CASE(VIRTIO_AUDIO_S_BADSTATE, "BADSTATE");
	CASE(VIRTIO_AUDIO_S_NOCHANNEL, "NOCHANNEL");
	CASE(VIRTIO_AUDIO_S_XRUN, "XRUN");
	CASE(VIRTIO_AUDIO_S_NOCTL, "NOCTL");
	CASE(VIRTIO_AUDIO_S_UNAVAIL, "UNAVAIL");
	default:
		BUG_ON(true);
	}

	return NULL;
}

static inline char *
cmd_virt2string(int const virt_cmd)
{
	switch (virt_cmd) {
	CASE(VIRTIO_AUDIO_CMD_PCM_ACQUIRE, "ACQUIRE");
	CASE(VIRTIO_AUDIO_CMD_PCM_PREPARE, "PREPARE");
	CASE(VIRTIO_AUDIO_CMD_PCM_DRAIN, "DRAIN");
	CASE(VIRTIO_AUDIO_CMD_PCM_DROP, "DROP");
	CASE(VIRTIO_AUDIO_CMD_PCM_PAUSE, "PAUSE");
	CASE(VIRTIO_AUDIO_CMD_PCM_RESUME, "RESUME");
	CASE(VIRTIO_AUDIO_CMD_PCM_RELEASE, "RELEASE");
	CASE(VIRTIO_AUDIO_CMD_PCM_POSITION_GET, "POSITION GET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_VOLUME_GET, "VOLUME GET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_VOLUME_SET, "VOLUME SET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_MUTE_GET, "MUTE GET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_MUTE_SET, "MUTE SET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_BALANCE_GET, "BALANCE GET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_BALANCE_SET, "BALANCE SET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_FADE_GET, "FADE GET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_FADE_SET, "FADE SET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_SWITCH_GET, "SWITCH GET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_GROUP_SWITCH_SET, "SWITCH SET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_SWITCH_GET, "CONTROL GET");
	CASE(VIRTIO_AUDIO_CMD_MIXER_SWITCH_SET, "CONTROL SET");
	default:
		BUG_ON(true);
	}

	return NULL;
}

/* Invoked by virtio framework with local interrupts disabled */
static void
virtaudio_cmd_done(struct virtqueue *vq)
{
	struct virtaudio_device *vad = vq->vdev->priv;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd_status *stat;
	unsigned int len;
	unsigned long flags, s_flags;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	while ((buf = virtqueue_get_buf(vq, &len)) != NULL) {
		BUG_ON(vad->cmd_vaq.rx_seqno == vad->cmd_vaq.tx_seqno);
		BUG_ON(len == 0);
		vad->cmd_vaq.rx_seqno++;

		if (len != buf->rsp_len) {
			LOG_ERROR(vad, "rx_seqno %u expected response len %u received %u", vad->cmd_vaq.rx_seqno, buf->rsp_len, len);

			if (buf->rsp_wait) {
				buf->rsp_len = len;
				buf->rsp_wait = false;

				spin_unlock_irqrestore(&vad->cmd_lock, flags);
				wake_up(&vad->cmd_vaq.rsp_done);
				spin_lock_irqsave(&vad->cmd_lock, flags);
			}
			continue;
		}

		stat = (struct virtio_audio_cmd_status *)buf->status_ptr;
		LOG_VERBOSE(vad, "rx_seqno %u id '%s' status '%s'", vad->cmd_vaq.rx_seqno, cmd_virt2string(stat->id), status_virt2string(stat->status));

		if (buf->rsp_wait) {
			buf->rsp_wait = false;

			spin_unlock_irqrestore(&vad->cmd_lock, flags);
			wake_up(&vad->cmd_vaq.rsp_done);
			spin_lock_irqsave(&vad->cmd_lock, flags);
			continue;
		}

		/* No one is waiting for the response, check the status here */
		if (stat->status != VIRTIO_AUDIO_S_OK) {
			/* An error occurred, if we are dealing with a PCM command let us try to stop the stream */
			if ((stat->id >= VIRTIO_AUDIO_CMD_PCM_FIRST) && (stat->id <= VIRTIO_AUDIO_CMD_PCM_LAST)) {
				struct snd_pcm_substream *substream = vad->channels[stat->pcm.channel_type]->substream;
				if (!substream) {
					continue;
				}
				snd_pcm_stream_lock_irqsave(substream, s_flags);
				snd_pcm_stop(substream, SNDRV_PCM_STATE_SETUP);
				snd_pcm_stream_unlock_irqrestore(substream, s_flags);
			}
		}
	}
	spin_unlock_irqrestore(&vad->cmd_lock, flags);
}

/* Must be called with CMD lock held */
static int32_t
virtaudio_init_cmd(struct virtaudio_device *vad, struct virtaudio_buf_info **buf)
{
	if (((vad->cmd_vaq.queue_size - vad->cmd_vaq.vq->num_free) / 2) >= vad->cmd_vaq.buf_num) {
		return -ENOMEM;
	}

	*buf = &vad->cmd_vaq.buf_info[vad->cmd_vaq.tx_seqno % vad->cmd_vaq.buf_num];

	return 0;
}

static inline int32_t
status_virt2linux(uint32_t const virt_status)
{
	switch (virt_status) {
	CASE(VIRTIO_AUDIO_S_OK, 0);
	CASE(VIRTIO_AUDIO_S_UNSUPP, -ENOTSUPP);
	CASE(VIRTIO_AUDIO_S_INVALID, -EINVAL);
	CASE(VIRTIO_AUDIO_S_FAILURE, -EINVAL);
	CASE(VIRTIO_AUDIO_S_BADSTATE, -EINVAL);
	CASE(VIRTIO_AUDIO_S_NOCHANNEL, -ECHRNG);
	CASE(VIRTIO_AUDIO_S_XRUN, -EIO);
	CASE(VIRTIO_AUDIO_S_NOCTL, -ENODEV);
	CASE(VIRTIO_AUDIO_S_UNAVAIL, -EBUSY);
	default:
		BUG_ON(true);
	}

	return 0;
}

/* Must be called with CMD lock held */
static int32_t
virtaudio_send_cmd(struct virtaudio_device *vad, struct virtaudio_buf_info *buf, unsigned int cmd_len, unsigned int stat_len, unsigned long *flags, bool wait)
{
	struct scatterlist *sgs[2], sgs_cmd, sgs_status;
	int32_t ret;
	struct virtio_audio_cmd *cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	struct virtio_audio_cmd_status *stat = (struct virtio_audio_cmd_status *)buf->status_ptr;

	sg_init_one(&sgs_cmd, cmd, cmd_len);
	sgs[0] = &sgs_cmd;
	sg_init_one(&sgs_status, stat, stat_len);
	sgs[1] = &sgs_status;

	ret = virtqueue_add_sgs(vad->cmd_vaq.vq, sgs, 1, 1, buf, GFP_ATOMIC);
	if (ret != 0) {
		LOG_ERROR(vad, "Unable to add fragment to virtqueue, err %d num_free %u", ret, vad->cmd_vaq.vq->num_free);
		return ret;
	}

	vad->cmd_vaq.tx_seqno++;

	LOG_VERBOSE(vad, "tx_seqno %u id '%s'", vad->cmd_vaq.tx_seqno, cmd_virt2string(cmd->id));

	virtqueue_kick(vad->cmd_vaq.vq);

	buf->rsp_len = stat_len;
	buf->rsp_wait = wait;

	if (!wait) {
		/* Bail out, we cannot wait for a status response */
		return 0;
	}

	spin_unlock_irqrestore(&vad->cmd_lock, *flags);
	wait_event(vad->cmd_vaq.rsp_done, !buf->rsp_wait);
	spin_lock_irqsave(&vad->cmd_lock, *flags);

	if (buf->rsp_len != stat_len) {
		return -EINVAL;
	}
	if (stat->id != cmd->id) {
		LOG_ERROR(vad, "Expected status id '%s' received '%s'", cmd_virt2string(cmd->id), cmd_virt2string(stat->id));
		return -EINVAL;
	}
	if (stat->status != VIRTIO_AUDIO_S_OK) {
		LOG_ERROR(vad, "Error status '%s' received", status_virt2string(stat->status));
		return status_virt2linux(stat->status);
	}
	if ((stat->id >= VIRTIO_AUDIO_CMD_PCM_FIRST) && (stat->id <= VIRTIO_AUDIO_CMD_PCM_LAST)) {
		if (stat->pcm.channel_type != cmd->pcm.channel_type) {
			LOG_ERROR(vad, "Expected status channel_type %u received %u", cmd->pcm.channel_type, stat->pcm.channel_type);
			return -EINVAL;
		}
	} else if ((stat->id >= VIRTIO_AUDIO_CMD_MIXER_FIRST) && (stat->id <= VIRTIO_AUDIO_CMD_MIXER_LAST)) {
		if (strncmp(stat->ctl.ctl_name, cmd->ctl.ctl_name, sizeof(cmd->ctl.ctl_name)) != 0) {
			LOG_ERROR(vad, "Expected status ctl_name '%s' received '%s'", cmd->ctl.ctl_name, stat->ctl.ctl_name);
			return -EINVAL;
		}
	} else {
		BUG_ON(true);
	}

	return 0;
}

int32_t
virtaudio_cmd_acquire(struct virtaudio_device *vad, struct virtaudio_channel *vac, struct virtio_audio_acquire_params *aparams)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	struct virtio_audio_cmd_status *stat;
	unsigned int cmd_len, stat_len;

	LOG_DEBUG(vad, "channel_type %u rate %u voices %u format 0x%X frag_size %u frags %u",
		vac->config.channel_type, aparams->rate, aparams->voices, aparams->format, aparams->frag_size, aparams->frags);

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_PCM_ACQUIRE;
	cmd->pcm.channel_type = vac->config.channel_type;
	cmd->pcm.acquire = *aparams;

	cmd_len = offsetof(struct virtio_audio_cmd, pcm) + offsetof(struct virtio_audio_pcm_cmd_params, acquire) + sizeof(struct virtio_audio_acquire_params);
	stat_len = offsetof(struct virtio_audio_cmd_status, pcm) + offsetof(struct virtio_audio_pcm_cmd_status, acquire) + sizeof(struct virtio_audio_acquire_status);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, true);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	stat = (struct virtio_audio_cmd_status *)buf->status_ptr;
	LOG_DEBUG(vad, "Optimal frag_size %u frags %u", stat->pcm.acquire.frag_size, stat->pcm.acquire.frags);
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_send_noparams(struct virtaudio_device *vad, uint32_t channel_type, uint32_t id, bool wait)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "CMD id '%s' initialization failure, ret %d", cmd_virt2string(id), ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = id;
	cmd->pcm.channel_type = channel_type;

	cmd_len = offsetof(struct virtio_audio_cmd, pcm) + offsetof(struct virtio_audio_pcm_cmd_params, acquire);
	stat_len = offsetof(struct virtio_audio_cmd_status, pcm) + offsetof(struct virtio_audio_pcm_cmd_status, acquire);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, wait);
	if (ret != 0) {
		LOG_ERROR(vad, "CMD id '%s' send failure, ret %d", cmd_virt2string(id), ret);
	}
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_volume_get(struct virtaudio_device *vad, struct virtaudio_mixer *vam, struct virtio_audio_mixergrp_volume_params *vparams)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	struct virtio_audio_cmd_status *stat;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_MIXER_GROUP_VOLUME_GET;
	strlcpy(cmd->ctl.ctl_name, vam->config.name, sizeof(cmd->ctl.ctl_name));

	cmd_len = offsetof(struct virtio_audio_cmd, ctl) + offsetof(struct virtio_audio_ctl_cmd_params, mixergrp_volume);
	stat_len = offsetof(struct virtio_audio_cmd_status, ctl) + offsetof(struct virtio_audio_ctl_cmd_status, mixergrp_volume) +	sizeof(struct virtio_audio_mixergrp_volume_params);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, true);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	stat = (struct virtio_audio_cmd_status *)buf->status_ptr;
	memcpy(vparams, &stat->ctl.mixergrp_volume, sizeof(struct virtio_audio_mixergrp_volume_params));
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_volume_set(struct virtaudio_device *vad, struct virtaudio_mixer *vam, struct virtio_audio_mixergrp_volume_params *vparams)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_MIXER_GROUP_VOLUME_SET;
	strlcpy(cmd->ctl.ctl_name, vam->config.name, sizeof(cmd->ctl.ctl_name));

	memcpy(&cmd->ctl.mixergrp_volume, vparams, sizeof(struct virtio_audio_mixergrp_volume_params));

	cmd_len = offsetof(struct virtio_audio_cmd, ctl) + offsetof(struct virtio_audio_ctl_cmd_params, mixergrp_volume) + sizeof(struct virtio_audio_mixergrp_volume_params);
	stat_len = offsetof(struct virtio_audio_cmd_status, ctl) + offsetof(struct virtio_audio_ctl_cmd_status, mixergrp_volume);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, true);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
	}
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_mute_get(struct virtaudio_device *vad, struct virtaudio_mixer *vam, uint32_t *mute)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	struct virtio_audio_cmd_status *stat;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_MIXER_GROUP_MUTE_GET;
	strlcpy(cmd->ctl.ctl_name, vam->config.name, sizeof(cmd->ctl.ctl_name));

	cmd_len = offsetof(struct virtio_audio_cmd, ctl) + offsetof(struct virtio_audio_ctl_cmd_params, mixergrp_mute);
	stat_len = offsetof(struct virtio_audio_cmd_status, ctl) + offsetof(struct virtio_audio_ctl_cmd_status, mixergrp_mute) + sizeof(struct virtio_audio_mixergrp_mute_params);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, true);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	stat = (struct virtio_audio_cmd_status *)buf->status_ptr;
	*mute = stat->ctl.mixergrp_mute.mute;
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_mute_set(struct virtaudio_device *vad, struct virtaudio_mixer *vam, uint32_t mute)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_MIXER_GROUP_MUTE_SET;
	strlcpy(cmd->ctl.ctl_name, vam->config.name, sizeof(cmd->ctl.ctl_name));

	cmd->ctl.mixergrp_mute.mute = mute;

	cmd_len = offsetof(struct virtio_audio_cmd, ctl) + offsetof(struct virtio_audio_ctl_cmd_params, mixergrp_mute) + sizeof(struct virtio_audio_mixergrp_mute_params);
	stat_len = offsetof(struct virtio_audio_cmd_status, ctl) + offsetof(struct virtio_audio_ctl_cmd_status, mixergrp_mute);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, true);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
	}
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_switch_get(struct virtaudio_device *vad, struct virtaudio_mixer *vam, uint32_t *switch_on)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	struct virtio_audio_cmd_status *stat;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_MIXER_GROUP_SWITCH_GET;
	strlcpy(cmd->ctl.ctl_name, vam->config.name, sizeof(cmd->ctl.ctl_name));

	cmd_len = offsetof(struct virtio_audio_cmd, ctl) + offsetof(struct virtio_audio_ctl_cmd_params, mixergrp_switch);
	stat_len = offsetof(struct virtio_audio_cmd_status, ctl) + offsetof(struct virtio_audio_ctl_cmd_status, mixergrp_switch) + sizeof(struct virtio_audio_mixergrp_switch_params);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, true);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	stat = (struct virtio_audio_cmd_status *)buf->status_ptr;
	*switch_on = stat->ctl.mixergrp_switch.switch_on;
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_switch_set(struct virtaudio_device *vad, struct virtaudio_mixer *vam,  uint32_t switch_on)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_MIXER_GROUP_SWITCH_SET;
	strlcpy(cmd->ctl.ctl_name, vam->config.name, sizeof(cmd->ctl.ctl_name));

	cmd->ctl.mixergrp_switch.switch_on = switch_on;

	cmd_len = offsetof(struct virtio_audio_cmd, ctl) + offsetof(struct virtio_audio_ctl_cmd_params, mixergrp_switch) + sizeof(struct virtio_audio_mixergrp_switch_params);
	stat_len = offsetof(struct virtio_audio_cmd_status, ctl) + offsetof(struct virtio_audio_ctl_cmd_status, mixergrp_switch);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, true);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
	}
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_control_get(struct virtaudio_device *vad, struct virtaudio_mixer *vam, struct virtio_audio_mixersw_params *ctlparams)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	struct virtio_audio_cmd_status *stat;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_MIXER_SWITCH_GET;
	strlcpy(cmd->ctl.ctl_name, vam->config.name, sizeof(cmd->ctl.ctl_name));

	cmd_len = offsetof(struct virtio_audio_cmd, ctl) + offsetof(struct virtio_audio_ctl_cmd_params, mixersw);
	stat_len = offsetof(struct virtio_audio_cmd_status, ctl) + offsetof(struct virtio_audio_ctl_cmd_status, mixersw) + sizeof(struct virtio_audio_mixersw_params);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, 1);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	stat = (struct virtio_audio_cmd_status *)buf->status_ptr;
	*ctlparams = stat->ctl.mixersw;
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

int32_t
virtaudio_cmd_control_set(struct virtaudio_device *vad, struct virtaudio_mixer *vam, struct virtio_audio_mixersw_params *ctlparams)
{
	int32_t ret;
	unsigned long flags;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_cmd *cmd;
	unsigned int cmd_len, stat_len;

	spin_lock_irqsave(&vad->cmd_lock, flags);
	ret = virtaudio_init_cmd(vad, &buf);
	if (ret != 0) {
		LOG_ERROR(vad, "Initialization failure, ret %d", ret);
		spin_unlock_irqrestore(&vad->cmd_lock, flags);
		return ret;
	}

	cmd = (struct virtio_audio_cmd *)buf->data_ptr;
	cmd->id = VIRTIO_AUDIO_CMD_MIXER_SWITCH_SET;
	strlcpy(cmd->ctl.ctl_name, vam->config.name, sizeof(cmd->ctl.ctl_name));

	memcpy(&cmd->ctl.mixersw, ctlparams, sizeof(struct virtio_audio_mixersw_params));

	cmd_len = offsetof(struct virtio_audio_cmd, ctl) + offsetof(struct virtio_audio_ctl_cmd_params, mixersw) + sizeof(struct virtio_audio_mixersw_params);
	stat_len = offsetof(struct virtio_audio_cmd_status, ctl) + offsetof(struct virtio_audio_ctl_cmd_status, mixersw);

	ret = virtaudio_send_cmd(vad, buf, cmd_len, stat_len, &flags, 1);
	if (ret != 0) {
		LOG_ERROR(vad, "Send failure, ret %d", ret);
	}
	spin_unlock_irqrestore(&vad->cmd_lock, flags);

	return ret;
}

/* Invoked by virtio framework with local interrupts disabled */
static void
virtaudio_data_done(struct virtqueue *vq)
{
	struct virtaudio_device *vad = vq->vdev->priv;
	struct virtaudio_channel *vac = NULL;
	struct virtaudio_buf_info *buf;
	struct virtio_audio_io_status *stat;
	unsigned int len, exp_len, err = 0, xrun = 0;
	unsigned long flags;
	int i;
	bool period_elapsed = false;

	for (i = 0; i < ARRAY_SIZE(vad->channels); i++) {
		vac = vad->channels[i];
		if (vac && (vq->index == vac->data_vaq.vq_idx)) {
			break;
		}
		vac = NULL;
	}
	BUG_ON(!vac);
	if (vac->config.channel_type == VIRTIO_AUDIO_CHANNEL_PLAYBACK) {
		exp_len = sizeof(struct virtio_audio_io_status);
	} else {
		exp_len = vac->data_vaq.buf_size + sizeof(struct virtio_audio_io_status);
	}

	snd_pcm_stream_lock_irqsave(vac->substream, flags);
	while ((buf = virtqueue_get_buf(vq, &len)) != NULL) {
		BUG_ON(vac->data_vaq.rx_seqno == vac->data_vaq.tx_seqno);
		vac->data_vaq.rx_seqno++;
		if (vac->data_vaq.rx_seqno == vac->data_vaq.seqno_wrap) {
			vac->data_vaq.rx_seqno = 0;
		}
		period_elapsed = true;

		if (len == 0) {
			LOG_VERBOSE(vad, "'%s' rx_seqno %u descriptor dropped", vac->data_vaq.name, vac->data_vaq.rx_seqno);
			continue;
		}
		if (len != exp_len) {
			LOG_ERROR(vad, "'%s' rx_seqno %u expected response len %u received %u", vac->data_vaq.name, vac->data_vaq.rx_seqno, exp_len, len);
			err = 1;
			continue;
		}

		stat = (struct virtio_audio_io_status *)buf->status_ptr;
		LOG_VERBOSE(vad, "'%s' rx_seqno %u status '%s'", vac->data_vaq.name, vac->data_vaq.rx_seqno, status_virt2string(stat->status));
		if (stat->status != VIRTIO_AUDIO_S_OK) {
			if (stat->status == VIRTIO_AUDIO_S_XRUN) {
				xrun = 1;
			} else {
				err = 1;
			}
		}
	}

	if (!period_elapsed) {
		snd_pcm_stream_unlock_irqrestore(vac->substream, flags);
		return;
	}

	if (err) {
		snd_pcm_stop(vac->substream, SNDRV_PCM_STATE_SETUP);
		snd_pcm_stream_unlock_irqrestore(vac->substream, flags);
	} else if (xrun) {
		snd_pcm_stream_unlock_irqrestore(vac->substream, flags);
		snd_pcm_stop_xrun(vac->substream);
	} else {
		snd_pcm_stream_unlock_irqrestore(vac->substream, flags);
		snd_pcm_period_elapsed(vac->substream);
	}

	wake_up(&vac->data_vaq.rsp_done);
}

/* Must be called with stream lock held */
int32_t
virtaudio_send_data(struct virtaudio_device *vad, struct virtaudio_channel *vac)
{
	uint32_t used, free_cnt, periods_avail, periods_send;
	struct snd_pcm_runtime *runtime = vac->substream->runtime;
	int i;

	if (vac->data_vaq.rx_seqno > vac->data_vaq.tx_seqno) {
		/* tx_seqno has wrapped */
		used = vac->data_vaq.tx_seqno + (vac->data_vaq.seqno_wrap - vac->data_vaq.rx_seqno);
	} else {
		used = vac->data_vaq.tx_seqno - vac->data_vaq.rx_seqno;
	}
	BUG_ON(used > vac->data_vaq.buf_num);
	free_cnt = vac->data_vaq.buf_num - used;

	if (vac->config.channel_type == VIRTIO_AUDIO_CHANNEL_PLAYBACK) {
		periods_avail = snd_pcm_playback_hw_avail(runtime) / runtime->period_size;
	} else {
		periods_avail = snd_pcm_capture_hw_avail(runtime) / runtime->period_size;
	}
	if (used > periods_avail) {
		// Something is wrong, the appl_ptr has been rewound
		return -1;
	}
	periods_avail -= used;

	periods_send = min(free_cnt, periods_avail);
	if (!periods_send) {
		LOG_DEBUG(vad, "'%s' used %u free_cnt %u periods_avail %u periods_send %u", vac->data_vaq.name, used, free_cnt, periods_avail, periods_send);
	}

	for (i = 0; i < periods_send; i++) {
		struct scatterlist *sgs[2], sgs_data, sgs_status;
		struct virtaudio_buf_info *buf = &vac->data_vaq.buf_info[vac->data_vaq.tx_seqno % vac->data_vaq.buf_num];
		int ret;

		sg_init_one(&sgs_data, buf->data_ptr, vac->data_vaq.buf_size);
		sgs[0] = &sgs_data;
		sg_init_one(&sgs_status, buf->status_ptr, sizeof(struct virtio_audio_io_status));
		sgs[1] = &sgs_status;

		if (vac->config.channel_type == VIRTIO_AUDIO_CHANNEL_PLAYBACK) {
			ret = virtqueue_add_sgs(vac->data_vaq.vq, sgs, 1, 1, buf, GFP_ATOMIC);
		} else {
			ret = virtqueue_add_sgs(vac->data_vaq.vq, sgs, 2, 0, buf, GFP_ATOMIC);
		}
		if (ret != 0) {
			LOG_ERROR(vad, "Unable to add to virtqueue, err %d num_free %u", ret, vac->data_vaq.vq->num_free);
			return ret;
		}

		vac->data_vaq.tx_seqno++;
		if (vac->data_vaq.tx_seqno == vac->data_vaq.seqno_wrap) {
			vac->data_vaq.tx_seqno = 0;
		}

		LOG_VERBOSE(vad, "'%s' tx_seqno %u", vac->data_vaq.name, vac->data_vaq.tx_seqno);
	}

	if (i) {
		virtqueue_kick(vac->data_vaq.vq);
	}

	return 0;
}

static int
virtaudio_init_vqs(struct virtaudio_device *vad, uint32_t channel_num)
{
	vq_callback_t **callbacks;
	struct virtqueue **vqs;
	int ret, i;
	unsigned num_vqs;
	const char **names;

	/* We want one queue for (shared) commands + one for each audio channel */
	num_vqs = 1 + channel_num;
	LOG_VERBOSE(vad, "num_vqs %u", num_vqs);

	vqs = kcalloc(num_vqs, sizeof(*vqs), GFP_KERNEL);
	callbacks = kmalloc_array(num_vqs, sizeof(*callbacks), GFP_KERNEL);
	names = kmalloc_array(num_vqs, sizeof(*names), GFP_KERNEL);
	if (!vqs || !callbacks || !names) {
		LOG_ERROR(vad, "Unable to allocate memory");
		ret = ENOMEM;
		goto out;
	}

	/* Command queue is first followed by each channel (order set by host device configuration) */
	vad->cmd_vaq.vq_idx = 0;
	callbacks[vad->cmd_vaq.vq_idx] = virtaudio_cmd_done;
	snprintf(vad->cmd_vaq.name, sizeof(vad->cmd_vaq.name), "CMDQ");
	names[vad->cmd_vaq.vq_idx] = vad->cmd_vaq.name;

	for (i = 0; i < ARRAY_SIZE(vad->channels); i++) {
		struct virtaudio_channel *vac = vad->channels[i];
		if (vac) {
			callbacks[vac->data_vaq.vq_idx] = virtaudio_data_done;
			names[vac->data_vaq.vq_idx] = vac->data_vaq.name;
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0)
	ret = virtio_find_vqs(vad->vdev, num_vqs, vqs, callbacks, names, NULL);
#else
	ret = vad->vdev->config->find_vqs(vad->vdev, num_vqs, vqs, callbacks, names);
#endif
	if (ret) {
		LOG_ERROR(vad, "Failed to find virt queues, err %d", ret);
		goto out;
	}

	vad->cmd_vaq.vq = vqs[vad->cmd_vaq.vq_idx];
	vad->cmd_vaq.queue_size = virtqueue_get_vring_size(vad->cmd_vaq.vq);
	LOG_DEBUG(vad, "'%s' queue_size %d", vad->cmd_vaq.name, vad->cmd_vaq.queue_size);

	for (i = 0; i < ARRAY_SIZE(vad->channels); i++) {
		struct virtaudio_channel *vac = vad->channels[i];
		if (vac) {
			vac->data_vaq.vq = vqs[vac->data_vaq.vq_idx];
			vac->data_vaq.queue_size = virtqueue_get_vring_size(vac->data_vaq.vq);
			LOG_DEBUG(vad, "'%s' queue_size %d", vac->data_vaq.name, vac->data_vaq.queue_size);
		}
	}

out:
	kfree(names);
	kfree(callbacks);
	kfree(vqs);
	return ret;
}

void
virtaudio_free(struct virtaudio_device *vad)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(vad->channels); i++) {
		struct virtaudio_channel *vac = vad->channels[i];
		if (vac) {
			/* data_mem is freed by the ALSA layer */
			kfree(vac->data_vaq.status_mem);
			kfree(vac->data_vaq.buf_info);
			kfree(vac);
		}
	}

	kfree(vad->cmd_vaq.data_mem);
	kfree(vad->cmd_vaq.status_mem);
	kfree(vad->cmd_vaq.buf_info);

	for (i = 0; i < ARRAY_SIZE(vad->mixer_controls); i++) {
		kfree(vad->mixer_controls[i]);
	}

	if (vad->vdev) {
		vad->vdev->config->reset(vad->vdev);
		vad->vdev->config->del_vqs(vad->vdev);
	}

	LOG_DEBUG(vad, "done");

	kfree(vad);
}

static void
virtaudio_chan_config_read(struct virtaudio_device *vad, int chan_idx, struct virtio_audio_chan_config *cc)
{
	int i;

#define READ_CHN_CFG(vdev, chan, name, var)  virtio_cread_bytes(vdev, offsetof(struct virtio_audio_device_config, channel_config) + \
	chan * sizeof(struct virtio_audio_chan_config) + offsetof(struct virtio_audio_chan_config, name), &(var), sizeof(var))

	READ_CHN_CFG(vad->vdev, chan_idx, channel_type, cc->channel_type);
	READ_CHN_CFG(vad->vdev, chan_idx, rates, cc->rates);
	READ_CHN_CFG(vad->vdev, chan_idx, formats, cc->formats);
	READ_CHN_CFG(vad->vdev, chan_idx, min_fragsize, cc->min_fragsize);
	READ_CHN_CFG(vad->vdev, chan_idx, max_fragsize, cc->max_fragsize);
	READ_CHN_CFG(vad->vdev, chan_idx, min_voices, cc->min_voices);
	READ_CHN_CFG(vad->vdev, chan_idx, max_voices, cc->max_voices);
	READ_CHN_CFG(vad->vdev, chan_idx, min_frags, cc->min_frags);
	READ_CHN_CFG(vad->vdev, chan_idx, fragment_align, cc->fragment_align);
	READ_CHN_CFG(vad->vdev, chan_idx, max_buffersize, cc->max_buffersize);

	LOG_DEBUG(vad, "channel_type %u rates 0x%x formats 0x%x fragsize(%u, %u) voices(%u, %u) min_frags %u fragment_align %u max_buffersize %u",
		cc->channel_type, cc->rates, cc->formats, cc->min_fragsize, cc->max_fragsize, cc->min_voices, cc->max_voices,
		cc->min_frags, cc->fragment_align, cc->max_buffersize);

	for (i = 0; i < ARRAY_SIZE(cc->chmap_pos); i++) {
		virtio_cread_bytes(vad->vdev, offsetof(struct virtio_audio_device_config, channel_config) + chan_idx * sizeof(struct virtio_audio_chan_config)
			+ offsetof(struct virtio_audio_chan_config, chmap_pos) + i * sizeof(cc->chmap_pos[0]), &cc->chmap_pos[i], sizeof(cc->chmap_pos[0]));
		if (cc->chmap_pos[i] == VIRTIO_AUDIO_CHMAP_UNKNOWN) {
			break;
		}
		LOG_DEBUG(vad, "voice %u chmap_pos 0x%x", i, cc->chmap_pos[i]);
	}
}

static int
virtaudio_init_channel(struct virtaudio_device *vad, struct virtio_audio_chan_config *cc, int channel_idx)
{
	struct virtaudio_channel *vac = NULL;

	if (vad->channels[cc->channel_type]) {
		LOG_ERROR(vad, "Channel entry already populated");
		return ENOENT;
	}

	vac = kzalloc(sizeof(*vac), GFP_KERNEL);
	if (!vac) {
		LOG_ERROR(vad, "Unable to allocate channel memory");
		return ENOMEM;
	}

	memcpy(&vac->config, cc, sizeof(vac->config));

	if (cc->channel_type == VIRTIO_AUDIO_CHANNEL_PLAYBACK) {
		snprintf(vac->data_vaq.name, sizeof(vac->data_vaq.name), "OUTQ");
	} else {
		snprintf(vac->data_vaq.name, sizeof(vac->data_vaq.name), "INQ");
	}

	init_waitqueue_head(&vac->data_vaq.rsp_done);

	vad->channels[cc->channel_type] = vac;
	vac->data_vaq.vq_idx = 1 + channel_idx;

	return 0;
}

static void
virtaudio_mixer_config_read(struct virtaudio_device *vad, int mix_idx, struct virtio_audio_mixer_ctl_config *mcc)
{
	int i;

	__virtio_cread_many(vad->vdev, offsetof(struct virtio_audio_device_config, mixer_ctl_config) + mix_idx * sizeof(struct virtio_audio_mixer_ctl_config)
		+ offsetof(struct virtio_audio_mixer_ctl_config, name), mcc->name, ARRAY_SIZE(mcc->name), sizeof(mcc->name[0]));

#define READ_MIX_CFG(vdev, mix, name, var)  virtio_cread_bytes(vdev, offsetof(struct virtio_audio_device_config, mixer_ctl_config) + \
	mix * sizeof(struct virtio_audio_mixer_ctl_config) + offsetof(struct virtio_audio_mixer_ctl_config, name), &(var), sizeof(var))

	READ_MIX_CFG(vad->vdev, mix_idx, mixer_ctl_type, mcc->mixer_ctl_type);

	if ((mcc->mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_PLAYBACK_GROUP) || (mcc->mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_CAPTURE_GROUP)) {
		READ_MIX_CFG(vad->vdev, mix_idx, group.voices, mcc->group.voices);
		READ_MIX_CFG(vad->vdev, mix_idx, group.vol_range.min, mcc->group.vol_range.min);
		READ_MIX_CFG(vad->vdev, mix_idx, group.vol_range.max, mcc->group.vol_range.max);
		READ_MIX_CFG(vad->vdev, mix_idx, group.db_vol_range.min, mcc->group.db_vol_range.min);
		READ_MIX_CFG(vad->vdev, mix_idx, group.db_vol_range.max, mcc->group.db_vol_range.max);
		READ_MIX_CFG(vad->vdev, mix_idx, group.db_scale_factor, mcc->group.db_scale_factor);
		READ_MIX_CFG(vad->vdev, mix_idx, group.balance_range.min, mcc->group.balance_range.min);
		READ_MIX_CFG(vad->vdev, mix_idx, group.balance_range.max, mcc->group.balance_range.max);
		READ_MIX_CFG(vad->vdev, mix_idx, group.fade_range.min, mcc->group.fade_range.min);
		READ_MIX_CFG(vad->vdev, mix_idx, group.fade_range.max, mcc->group.fade_range.max);
		READ_MIX_CFG(vad->vdev, mix_idx, group.capabilities, mcc->group.capabilities);

		LOG_DEBUG(vad, "name '%s' mixer_ctl_type %u voices %u vol_range(%d, %d) db_vol_range(%d, %d) db_scale_factor %u balance_range(%d, %d) fade_range(%d, %d) capabilities 0x%x",
			mcc->name, mcc->mixer_ctl_type, mcc->group.voices, mcc->group.vol_range.min, mcc->group.vol_range.max, mcc->group.db_vol_range.min, mcc->group.db_vol_range.max,
			mcc->group.db_scale_factor,  mcc->group.balance_range.min, mcc->group.balance_range.max, mcc->group.fade_range.min, mcc->group.fade_range.max, mcc->group.capabilities);

		for (i = 0; i < ARRAY_SIZE(mcc->group.chmap_pos); i++) {
			virtio_cread_bytes(vad->vdev, offsetof(struct virtio_audio_device_config, mixer_ctl_config) + mix_idx * sizeof(struct virtio_audio_mixer_ctl_config)
				+ offsetof(struct virtio_audio_mixer_ctl_config, group.chmap_pos) + i * sizeof(mcc->group.chmap_pos[0]), &mcc->group.chmap_pos[i], sizeof(mcc->group.chmap_pos[0]));
			if (mcc->group.chmap_pos[i] == VIRTIO_AUDIO_CHMAP_UNKNOWN) {
				break;
			}
			LOG_DEBUG(vad, "voice %u chmap_pos 0x%x", i, mcc->group.chmap_pos[i]);
		}
	} else if (mcc->mixer_ctl_type == VIRTIO_AUDIO_MIXER_CTL_SWITCH) {
		READ_MIX_CFG(vad->vdev, mix_idx, sw.type, mcc->sw.type);

		switch (mcc->sw.type) {
		case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER:
			READ_MIX_CFG(vad->vdev, mix_idx, sw.integer.min, mcc->sw.integer.min);
			READ_MIX_CFG(vad->vdev, mix_idx, sw.integer.max, mcc->sw.integer.max);
			READ_MIX_CFG(vad->vdev, mix_idx, sw.integer.step, mcc->sw.integer.step);

			LOG_DEBUG(vad, "name '%s' type %u integer(%d, %d, %d)", mcc->name, mcc->sw.type, mcc->sw.integer.min, mcc->sw.integer.max, mcc->sw.integer.step);
			break;
		case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_INTEGER64:
			READ_MIX_CFG(vad->vdev, mix_idx, sw.integer64.min, mcc->sw.integer64.min);
			READ_MIX_CFG(vad->vdev, mix_idx, sw.integer64.max, mcc->sw.integer64.max);
			READ_MIX_CFG(vad->vdev, mix_idx, sw.integer64.step, mcc->sw.integer64.step);

			LOG_DEBUG(vad, "name '%s' type %u integer64(%lld, %lld, %lld)", mcc->name, mcc->sw.type, mcc->sw.integer64.min, mcc->sw.integer64.max, mcc->sw.integer64.step);
			break;
		case VIRTIO_AUDIO_MIXER_SWITCH_TYPE_ENUMERATED:
			READ_MIX_CFG(vad->vdev, mix_idx, sw.enumerated.item_num, mcc->sw.enumerated.item_num);

			LOG_DEBUG(vad, "name '%s' type %u item_num %u", mcc->name, mcc->sw.type, mcc->sw.enumerated.item_num);

			BUG_ON(mcc->sw.enumerated.item_num > ARRAY_SIZE(mcc->sw.enumerated.item_list));
			for (i = 0; i < mcc->sw.enumerated.item_num; i++) {
				__virtio_cread_many(vad->vdev, offsetof(struct virtio_audio_device_config, mixer_ctl_config) + mix_idx * sizeof(struct virtio_audio_mixer_ctl_config)
					+ offsetof(struct virtio_audio_mixer_ctl_config, sw.enumerated.item_list) + i * ARRAY_SIZE(mcc->sw.enumerated.item_list[0]),
					&mcc->sw.enumerated.item_list[i], ARRAY_SIZE(mcc->sw.enumerated.item_list[0]), sizeof(mcc->sw.enumerated.item_list[0][0]));
				LOG_DEBUG(vad, "item_list[%u]: '%s'", i, mcc->sw.enumerated.item_list[i]);
			}
			break;
		}
	}
}

static int
virtaudio_init_mixer(struct virtaudio_device *vad, struct virtio_audio_mixer_ctl_config *mcc)
{
	int i;
	struct virtaudio_mixer *vam = NULL;

	for (i = 0; i < ARRAY_SIZE(vad->mixer_controls); i++) {
		if (vad->mixer_controls[i] == NULL) {
			break;
		}
	}
	if (i == ARRAY_SIZE(vad->mixer_controls)) {
		LOG_ERROR(vad, "All mixer control entries already populated");
		return ENOENT;
	}

	vam = kzalloc(sizeof(*vam), GFP_KERNEL);
	if (!vam) {
		LOG_ERROR(vad, "Unable to allocate mixer control memory");
		return ENOMEM;
	}

	memcpy(&vam->config, mcc, sizeof(vam->config));

	vad->mixer_controls[i] = vam;

	return 0;
}

int32_t
virtaudio_create(struct virtio_device *vdev, struct virtaudio_device **pvad)
{
	struct virtaudio_device *vad = NULL;
	int err, i;
	uint32_t channel_num = 0;
	uint32_t mixer_ctl_num = 0;

	if (!virtio_has_feature(vdev, VIRTIO_F_VERSION_1)) {
		dev_err(&vdev->dev, "%s: Device is not v1.0 compliant\n", __func__);
		return EINVAL;
	}

	vad = kzalloc(sizeof(*vad), GFP_KERNEL);
	if (!vad) {
		dev_err(&vdev->dev, "%s: Unable to allocate device memory\n", __func__);
		return ENOMEM;
	}

	vdev->priv = vad;
	vad->vdev = vdev;

	virtio_cread(vdev, struct virtio_audio_device_config, channel_num, &channel_num);
	LOG_DEBUG(vad, "channel_num %u", channel_num);

	for (i = 0; i < channel_num; i++) {
		struct virtio_audio_chan_config cc = { 0 };
		virtaudio_chan_config_read(vad, i, &cc);

		err = virtaudio_init_channel(vad, &cc, i);
		if (err != 0) {
			LOG_ERROR(vad, "Failure initializing channel %u, err %d", i, err);
			goto err_chan;
		}
	}

	if (virtio_has_feature(vdev, VIRTIO_AUDIO_F_MIXER_CTL)) {
		virtio_cread(vdev, struct virtio_audio_device_config, mixer_ctl_num, &mixer_ctl_num);
		LOG_DEBUG(vad, "mixer_ctl_num %u", mixer_ctl_num);

		for (i = 0; i < mixer_ctl_num; i++) {
			struct virtio_audio_mixer_ctl_config mcc  ; //= { 0 };
			virtaudio_mixer_config_read(vad, i, &mcc);

			err = virtaudio_init_mixer(vad, &mcc);
			if (err != 0) {
				LOG_ERROR(vad, "Failure initializing mixer %u, err %d", i, err);
				goto err_mix;
			}
		}
	}

	err = virtaudio_init_vqs(vad, channel_num);
	if (err) {
		LOG_ERROR(vad, "Failure creating virt queues, err %d ", err);
		goto err_vq;
	}

	err = virtaudio_cmd_queue_init(vad);
	if (err) {
		LOG_ERROR(vad, "Failure initializing CMD queue, err %d ", err);
		goto err_vq;
	}

	virtio_device_ready(vdev);

	LOG_DEBUG(vad, "bus_name '%s'\n", virtio_bus_name(vdev));

	*pvad = vad;
	return 0;

err_vq:
err_mix:
	for (i = 0; i < ARRAY_SIZE(vad->channels); i++) {
		kfree(vad->channels[i]);
	}
err_chan:
	for (i = 0; i < ARRAY_SIZE(vad->mixer_controls); i++) {
		kfree(vad->mixer_controls[i]);
	}
	kfree(vad);
	return -err;
}

#ifdef CONFIG_PM_SLEEP
/* Registered with the virtio driver-model and invoked last by virtio-pci */
int
virtaudio_freeze(struct virtio_device *vdev)
{
	vdev->config->reset(vdev);
	vdev->config->del_vqs(vdev);
	return 0;
}

int
virtaudio_restore(struct virtio_device *vdev)
{
	int err;
	uint32_t channel_num = 0;
	struct virtaudio_device *vad = vdev->priv;

	virtio_cread(vdev, struct virtio_audio_device_config, channel_num, &channel_num);
	err = virtaudio_init_vqs(vad, channel_num);
	if (err) {
		return err;
	}

	virtio_device_ready(vdev);

	return 0;
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/qpdc/mainline/linux_drivers/sound/virtio/virtaudio.c $ $Rev: 948565 $")
#endif
