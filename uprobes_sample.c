/*
 * uprobes sample : sample of probing user-space apps using uprobes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uprobes.h>
#include <linux/namei.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define DEBUGGEE_FILE "/home/kentaost/debuggee_app"
#define DEBUGGEE_FILE_OFFSET (0x526)
static struct inode *debuggee_inode;

static int uprobe_sample_handler(struct uprobe_consumer *con,
		struct pt_regs *regs)
{
	printk("handler is executed\n");
	return 0;
}

static int uprobe_sample_ret_handler(struct uprobe_consumer *con,
					unsigned long func,
					struct pt_regs *regs)
{
	printk("ret_handler is executed\n");
	return 0;
}

static struct uprobe_consumer uc = {
	.handler = uprobe_sample_handler,
	.ret_handler = uprobe_sample_ret_handler
};

static int __init init_uprobe_sample(void)
{
	int ret;
	struct path path;

	ret = kern_path(DEBUGGEE_FILE, LOOKUP_FOLLOW, &path);
	if (ret) {
		return -1;
	}

	debuggee_inode = igrab(path.dentry->d_inode);
	path_put(&path);

	ret = uprobe_register(debuggee_inode,
			DEBUGGEE_FILE_OFFSET, &uc);
	if (ret < 0) {
		return -1;
	}

	printk(KERN_INFO "insmod uprobe_sample\n");
	return 0;
}

static void __exit exit_uprobe_sample(void)
{
	uprobe_unregister(debuggee_inode,
			DEBUGGEE_FILE_OFFSET, &uc);
	printk(KERN_INFO "rmmod uprobe_sample\n");
}

module_init(init_uprobe_sample);
module_exit(exit_uprobe_sample);

MODULE_LICENSE("GPL");
