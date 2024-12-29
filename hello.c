#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

MODULE_AUTHOR("Vovchenko Kostya <vovchenko.kostiantyn@lll.kpi.ua>");
MODULE_DESCRIPTION("Hello, world in Linux ");
MODULE_LICENSE("Dual BSD/GPL");

// Parameter definition
static uint count = 1;
module_param(count, uint, 0444);
MODULE_PARM_DESC(count, "Number of times 'Hello, world!' should be printed.");

// Linked list structure
struct time_list {
    struct list_head list;
    ktime_t time;
};

// Static head of the list
static LIST_HEAD(time_head);

// Initialization function
static int __init hello_init(void)
{
    struct time_list *entry;
    int i;

    // Parameter validation
    if (count == 0 || (count >= 5 && count <= 10)) {
        pr_warn("Parameter 'count' == 0 or 5<'count'<10.\n");
    } else if (count > 10) {
        pr_err("Parameter 'count' > 10. Exiting with -EINVAL.\n");
        return -EINVAL;
    }

    // Print messages and record times
    for (i = 0; i < count; i++) {
        entry = kmalloc(sizeof(*entry), GFP_KERNEL);
        if (!entry) {
            pr_err("Memory allocation failed.\n");
            return -ENOMEM;
        }

        entry->time = ktime_get();
        list_add_tail(&entry->list, &time_head);
        pr_info("Hello, world!\n");
    }

    return 0;
}

// Cleanup function
static void __exit hello_exit(void)
{
    struct time_list *entry, *tmp;

    pr_info("Exiting module and printing recorded times.\n");

    list_for_each_entry_safe(entry, tmp, &time_head, list) {
        pr_info("Time: %lld ns\n", ktime_to_ns(entry->time));
        list_del(&entry->list);
        kfree(entry);
    }
}

module_init(hello_init);
module_exit(hello_exit);

