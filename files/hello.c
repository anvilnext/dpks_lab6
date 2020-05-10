#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/slab.h>

//modinfo
MODULE_AUTHOR("Oleksii Cherevatenko");
MODULE_DESCRIPTION("Lab6 DPKS");
MODULE_LICENSE("Dual BSD/GPL");

//initialization of hello parameter
static uint hpar = 1;
module_param(hpar, uint, S_IRUGO);

MODULE_PARM_DESC(hpar, "Number of 'hello world's.");

//initialization of struct
static LIST_HEAD(list_node_head);
struct hello_alloc{
	struct list_head node_head;
	int event_num;
	ktime_t event_time;
	ktime_t kill_time;
};


static int __init hello_init(void)
{
	if(hpar == 0 || (hpar  >= 5 && hpar <= 10))
		printk(KERN_EMERG "Warning, contunuing..");
	
	else if(hpar > 10) {
		printk(KERN_EMERG "Error, exiting..");
		return -EINVAL;
	}

	else {
		int i = 0; 
		ktime_t start, end;
		struct hello_alloc* alloc_list;

		for(i = 0; i < hpar; i++){

			BUG_ON(hpar > 10); //lab6

			start = ktime_get();
			printk(KERN_EMERG "Hello, world!\n");
			end = ktime_get();
			
			//lab6 - set 0 for third iteration
			if(i != 3)
			{
				alloc_list = (struct hello_alloc*) kmalloc(sizeof(struct hello_alloc), GFP_KERNEL);
			}
			else
			{
				alloc_list = (struct hello_alloc*) 0;
			}
			
			*alloc_list = (struct hello_alloc){
				.event_num = i,
				.event_time = start,
				.kill_time = end
			};

			list_add_tail(&alloc_list->node_head, &list_node_head);
		}
	}
	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_alloc *md, *tmp;
	printk(KERN_EMERG "Freeing memory.");

	list_for_each_entry_safe(md, tmp, &list_node_head, node_head) {
		printk(KERN_EMERG "Freeing memory (event %d)", md->event_num);
		pr_info("%lld ns print time.\n", (long long int) (md->kill_time - md->event_time));
		list_del(&md->node_head);
		kfree(md);
	}

	BUG_ON(!list_empty(&list_node_head));
	printk(KERN_EMERG "The job has ended.");
}

module_init(hello_init);
module_exit(hello_exit);
