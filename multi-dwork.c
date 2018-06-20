#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/jiffies.h>

struct workqueue_struct *wq;
struct delayed_work dwork;

static void dwork_func(struct work_struct *work)
{
	printk("%s:%d", __func__, __LINE__);
}

static int mdwork_init(void)
{
	int i;
	bool ret;

	wq = alloc_ordered_workqueue("%s", 0, "mdwork wq");
	if (!wq) {
		printk("Failed to alloc work queue");
		return -EBUSY;
	}

	INIT_DELAYED_WORK(&dwork, dwork_func);

	/* This work will be scheduled after 3 secs */
	ret = queue_delayed_work(wq, &dwork, 3000);
	printk("%s:%d, Queued a dwork, ret=%d", __func__, __LINE__, ret);

	for (i = 0; i < 2; i++) {
		/* Will the zero-delay work be executed immediately? */
		ret = queue_delayed_work(wq, &dwork, 0);
		printk("%s:%d, Queued a dwork, ret=%d", __func__, __LINE__, ret);
		msleep(1000);
	}

	return 0;
}

static void mdwork_exit(void)
{
	cancel_delayed_work_sync(&dwork);
	flush_workqueue(wq);
	destroy_workqueue(wq);
}

module_init(mdwork_init);
module_exit(mdwork_exit);
MODULE_LICENSE("GPL");
