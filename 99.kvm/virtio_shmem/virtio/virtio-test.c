#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/scatterlist.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/virtio.h>
#include <linux/virtio_ids.h>
#include <uapi/linux/virtio_ids.h>
#include <linux/virtio_config.h>



#define TEST_CONFIG_NR	0x00

struct virtio_test {
	struct virtio_device *vdev;
	char data[5];
	int config;
	struct virtqueue *vq;
	struct work_struct eint_work;
};


static void work_function(struct work_struct *work)
{
	struct virtio_test *vite = container_of(work, struct virtio_test, eint_work);
	unsigned int len;
	//struct scatterlist bufhdr;
	//
	//sg_init_one(&bufhdr, vite->data, 5);
	//virtqueue_add_outbuf(vite->vq, &bufhdr, 1, vite->data, GFP_ATOMIC);
	//
	///* Tell Host to go ! */
	//virtqueue_kick(vite->vq);

	dev_info(&vite->vdev->dev, "%s: enter.===GinPot",__func__);
	
	if(virtqueue_get_buf(vite->vq, &len) == NULL)
		dev_info(&vite->vdev->dev, "%s: virtqueue_get_buf fail.===GinPot",__func__);
		
	dev_info(&vite->vdev->dev, "%s: send data form host： %s, len=%d ===GinPot",__func__,vite->data,len);
}

static void virtest_vq_isr(struct virtqueue *vq)
{
	struct virtio_test *vite = vq->vdev->priv;

	//dev_info(&vite->vdev->dev, "%s: enter.===GinPot",__func__);
	
	schedule_work(&vite->eint_work);
}

static int virtest_init_vqs(struct virtio_test *vite)
{
	struct virtqueue *vqs[1];
	vq_callback_t *cbs[] = { virtest_vq_isr };
	static const char * const names[] = { "virtest_vq_isr" };
	int err;
	
	err = vite->vdev->config->find_vqs(vite->vdev, 1, vqs, cbs, names);
	if(err){
		dev_err(&vite->vdev->dev, "find_vqs fail.===GinPot");
		return err;
	}
	
	vite->vq = vqs[0];
	
	return err;
}

static int virtest_init_hw(struct virtio_device *vdev, struct virtio_test *vite)
{
	/* read virtio test config info 
	Similar to the read-write register, when reading and writing the corresponding address, 
	it will trap into EL2. When the corresponding back-end vdev is detected, 
	the corresponding read-write interface is called to fill in the required data*/
	vite->config = virtio_cread32(vdev, TEST_CONFIG_NR);		
	
	dev_info(&vdev->dev, "read config: 0x%x ===GinPot",vite->config);
	
	if(vite->config != 0x88888888){
		dev_info(&vdev->dev, "read config fail ===GinPot");
		return -1;
	}

	return 0;
}

static int virtest_probe(struct virtio_device *vdev)
{
	struct virtio_test *vite;
	int err = 0;
	struct scatterlist bufhdr;
	
	dev_info(&vdev->dev, "probe.===GinPot");
	
	vite = kzalloc(sizeof(*vite), GFP_KERNEL);
	if(!vite)
		return -ENOMEM;
	
	vite->vdev = vdev;
	vdev->priv = vite;
	//init_waitqueue_head(&vite->inq);
	/* Initialize works */
	INIT_WORK(&vite->eint_work, work_function);
	
	err = virtest_init_vqs(vite);
	if(err)
		goto err_init_vq;
	
	err = virtest_init_hw(vdev, vite);
	if(err)
		goto err_init_hw;
	
	dev_info(&vdev->dev, "success. ===GinPot");


	vite->data[0] = 'h';
	vite->data[1] = 'e';
	vite->data[2] = 'l';
	vite->data[3] = 'l';
	vite->data[4] = 'o';
	//schedule_work(&vite->eint_work);
	sg_init_one(&bufhdr, vite->data, 5);
	virtqueue_add_outbuf(vite->vq, &bufhdr, 1, vite->data, GFP_ATOMIC);
	
	/* Tell Host to go ! */
	virtqueue_kick(vite->vq);
	//virtqueue_enable_cb(vite->vq);
	
	return 0;

err_init_hw:
	vite->vdev->config->del_vqs(vite->vdev);
err_init_vq:
	kfree(vite);
	cancel_work_sync(&vite->eint_work);
	
	return err;
}

static void virtest_remove(struct virtio_device *vdev)
{
	
	
}

static unsigned int features[] = {
	/* none */
};

static struct virtio_device_id id_table[] = {
	{ VIRTIO_ID_TEST, VIRTIO_DEV_ANY_ID },
	{ 0 },
};

static struct virtio_driver virtio_test_driver = {
	.driver.name			= KBUILD_MODNAME,
	.driver.owner			= THIS_MODULE,
	.feature_table			= features,
	.feature_table_size		= ARRAY_SIZE(features),
	.id_table				= id_table,
	.probe					= virtest_probe,
	.remove					= virtest_remove,
};

module_virtio_driver(virtio_test_driver);
MODULE_DEVICE_TABLE(virtio, id_table);

MODULE_DESCRIPTION("Virtio test frontend driver");
MODULE_LICENSE("GPL v2");