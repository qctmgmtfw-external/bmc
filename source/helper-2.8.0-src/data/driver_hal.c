/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

/*
 * Driver HAL
 *
 * Copyright (C) 2009-2015 American Megatrends Inc.
 *
 * Author : Jothiram Selvam <jothirams@ami.com>
 *
 * This driver provides driver independent hardware abstraction layer.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/list.h>
#include "driver_hal.h"
 
#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)       
#endif

static LIST_HEAD(core_hal_list);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static DEFINE_SPINLOCK(core_hal_list_slock);
#else
static spinlock_t core_hal_list_slock = SPIN_LOCK_UNLOCKED;
#endif
static DEFINE_MUTEX(core_hal_list_mlock);

int register_core_hal_module (core_hal_t *pcore_hal)
{
	core_hal_list_t *c;
	int ret = 0;

	mutex_lock (&core_hal_list_mlock);      

	list_for_each_entry (c, &core_hal_list, list)
	{
		if (c->core_hal->dev_type == pcore_hal->dev_type)
		{
			ret = -EEXIST;
			goto failed;
		}
	}

	c = (core_hal_list_t*) kmalloc (sizeof(core_hal_list_t), GFP_KERNEL);
	if (!c)
	{
		ret = -ENOMEM;
		goto failed;
	}
	dbgprint ("allcoated core hal\n");
	c->core_hal = pcore_hal;

	INIT_LIST_HEAD (&c->hw_list);

	spin_lock (&core_hal_list_slock);
	list_add_tail (&(c->list), &core_hal_list);
	spin_unlock (&core_hal_list_slock);
	
failed:
	mutex_unlock (&core_hal_list_mlock);
	return ret;
}

int unregister_core_hal_module (ehal_dev_type dev_type)
{
	core_hal_list_t *c, *ctmp;
	int ret = -1;

	mutex_lock (&core_hal_list_mlock);
	list_for_each_entry_safe (c, ctmp, &core_hal_list, list)
	{
		if (c->core_hal->dev_type == dev_type)
		{
			if (list_empty (&c->hw_list))
			{
				spin_lock (&core_hal_list_slock);				
				list_del (&c->list);
				spin_unlock (&core_hal_list_slock);				
				dbgprint ("deallcoated core hal\n");				
				if(c)
					kfree (c);
				ret = 0;
				break;
			}
		}
	}
	mutex_unlock (&core_hal_list_mlock);

	return ret;
}

int register_hw_hal_module (hw_hal_t *phw_hal, void **pcore_funcs)
{
	core_hal_list_t *c;
	hw_hal_list_t *nh;
	struct list_head *hw_list;

	int ret = -1;

	nh = (hw_hal_list_t*) kmalloc (sizeof(hw_hal_list_t), GFP_KERNEL);
	if (!nh)
		return -ENOMEM;
	dbgprint ("allcoated hw hal\n");			
	mutex_lock (&core_hal_list_mlock);       

	list_for_each_entry (c, &core_hal_list, list)
	{
		if (c->core_hal->dev_type == phw_hal->dev_type)
		{
			int i;
			hw_hal_list_t *h;
			unsigned char last_mnum = 0;
			unsigned char *opencount;

			hw_list = &c->hw_list;
#if 0			
			if (list_empty (&(c->hw_list)))
			{
				hw_list = &c->hw_list;
			}
			else
			{
#endif				
				opencount = (unsigned char*) kmalloc (phw_hal->num_instances, GFP_KERNEL);
				if (!opencount)
				{
					kfree (nh);
					mutex_unlock (&core_hal_list_mlock);
					return -ENOMEM;
				}
				
				nh->opencount = opencount;
				for (i = 0; i < phw_hal->num_instances; ++i)
				{
					nh->opencount[i] = 0;
				}

				list_for_each_entry (h, &(c->hw_list), list)
				{
					if ((h->minor_num - last_mnum) >= phw_hal->num_instances)
					{
						hw_list = &h->list;
						break;
					}
					last_mnum = h->minor_num + h->hw_hal->num_instances;
				}

				if ((last_mnum + phw_hal->num_instances) > MAX_DEV_INSTANCES)
				{
					ret = -ENOBUFS;
					goto failed;
				}			
				//			}

				nh->hw_hal = phw_hal;
				nh->minor_num = last_mnum;
				nh->dev_id = last_mnum;
				
				ret = c->core_hal->register_hal_module (nh->hw_hal->num_instances, nh->hw_hal->phal_ops, &nh->pdrv_data);
				if (ret < 0)
					goto failed;

				dbgprint ("core priv data: %p\n", nh->pdrv_data);	

				if (!try_module_get(c->core_hal->owner))
				{
					ret = -1;
					goto failed;
				}

				*pcore_funcs = (void *)c->core_hal->pcore_funcs;
				dbgprint ("corefun ptr: %p, %p\n", *pcore_funcs, c->core_hal->pcore_funcs);
				spin_lock (&core_hal_list_slock);
				list_add (&(nh->list), hw_list);
				spin_unlock (&core_hal_list_slock);
				mutex_unlock (&core_hal_list_mlock);
				return nh->dev_id;
		}
	}
failed:
	dbgprint ("deallcoated hw hal - failure\n");					
	if(nh->opencount)
		kfree (nh->opencount);
	if(nh)
		kfree (nh);
	mutex_unlock (&core_hal_list_mlock);
	return ret;
}

int unregister_hw_hal_module (ehal_dev_type dev_type, unsigned char dev_id)
{
	core_hal_list_t *c;
	hw_hal_list_t *h, *htmp;
	int ret = -1;

	mutex_lock (&core_hal_list_mlock);
	list_for_each_entry (c, &core_hal_list, list)
	{
		if (c->core_hal->dev_type == dev_type)
		{
			list_for_each_entry_safe (h, htmp, &(c->hw_list), list)
			{
				if (h->dev_id == dev_id)
				{
					dbgprint ("core priv data: %d, %p\n", dev_id, h->pdrv_data);								
					c->core_hal->unregister_hal_module (h->pdrv_data);		
					spin_lock (&core_hal_list_slock);
					list_del (&(h->list));
					spin_unlock (&core_hal_list_slock);
					module_put(c->core_hal->owner);
					if(h->opencount)
						kfree (h->opencount);
					if(h)
						kfree (h);			
					ret = 0;
					break;
				}
			}
		}
	}	
	mutex_unlock (&core_hal_list_mlock);
	return ret;
}

int hw_open (ehal_dev_type dev_type, unsigned char minor_num, unsigned char *popen_count, hw_info_t *phw_info)
{
	int ret = -1;
	core_hal_list_t *c;

	mutex_lock (&core_hal_list_mlock);

	list_for_each_entry (c, &core_hal_list, list)
	{
		if (c->core_hal->dev_type == dev_type)
		{
			hw_hal_list_t *h;

			list_for_each_entry (h, &(c->hw_list), list)
			{
				dbgprint ("hw_open devtype: %d, min: %d h->min: %d\n", dev_type, minor_num, h->minor_num);
				if ((minor_num >= h->minor_num) && (minor_num < (h->minor_num + h->hw_hal->num_instances)))
				{
					if (!try_module_get (h->hw_hal->owner))
						goto out;

					phw_info->pdrv_data = h->pdrv_data;
					phw_info->inst_num = minor_num - h->minor_num;
					*popen_count = ++h->opencount[minor_num - h->minor_num];
					ret = 0;
					break;
				}				
			}
		}
	}
out:
	mutex_unlock (&core_hal_list_mlock);
	return ret;
}

int hw_close (ehal_dev_type dev_type, unsigned char minor_num, unsigned char *popen_count)
{
	int ret = -1;
	core_hal_list_t *c;

	mutex_lock (&core_hal_list_mlock);  

	list_for_each_entry (c, &core_hal_list, list)
	{
		if (c->core_hal->dev_type == dev_type)
		{
			hw_hal_list_t *h;

			list_for_each_entry (h, &(c->hw_list), list)
			{
				if ((minor_num >= h->minor_num) && (minor_num < (h->minor_num + h->hw_hal->num_instances)))
				{
					module_put (h->hw_hal->owner);
					*popen_count = --h->opencount[minor_num - h->minor_num];
					ret = 0;
					break;
				}				
			}
		}
	}

	mutex_unlock (&core_hal_list_mlock);
	return ret;
}

void* hw_intr (ehal_dev_type dev_type, int dev_id)
{
	core_hal_list_t *c;
	unsigned long flags;

	spin_lock_irqsave (&core_hal_list_slock, flags);


	list_for_each_entry (c, &core_hal_list, list)
	{
		if (c->core_hal->dev_type == dev_type)
		{
			hw_hal_list_t *h;

			list_for_each_entry (h, &(c->hw_list), list)
			{
				if (h->dev_id == dev_id)
				{
					dbgprint ("hw_intr devtype: %d, h->min: %d\n", dev_type, h->minor_num);					
					spin_unlock_irqrestore (&core_hal_list_slock, flags);
					return h->pdrv_data;
				}			
			}
		}
	}

	spin_unlock_irqrestore (&core_hal_list_slock, flags);
	return NULL;
}

EXPORT_SYMBOL (register_core_hal_module);
EXPORT_SYMBOL (unregister_core_hal_module);
EXPORT_SYMBOL (register_hw_hal_module);
EXPORT_SYMBOL (unregister_hw_hal_module);
EXPORT_SYMBOL (hw_open);
EXPORT_SYMBOL (hw_close);
EXPORT_SYMBOL (hw_intr);
