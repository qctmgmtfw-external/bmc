/****************************************************************
 ****************************************************************
 **                                                            **
 **        Copyright 2012 Quanta Compter Inc.                  **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * ast_net_mdio.c
 * NET driver Module functions
 *
 *****************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cacheflush.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/version.h>
#include <linux/cdev.h>

#include "mdio.h"
#define DO_PRINT(args...) printk(": " args)

/* Local Macros */
#define NET_MAJOR_NUM		249
#define NET_MINOR_NUM		0
#define NET_MAX_DEVICES     1
#define NET_DEV_NAME        "ast_net_mdio"

#define AST_NET_REG_BASE                       0x1E680000	/* NET */
#define ASPEED_ETHER_IO_REG_SIZE		SZ_64K

static void *ast_net_virt_base = NULL;

static dev_t net_devno = MKDEV(NET_MAJOR_NUM, NET_MINOR_NUM);
static struct cdev *net_cdev;

static void b5387_mdiobus_write(void* ioaddr,
                                              u8 phyaddr, u8 page_num , u8 phyreg, u16 phydata, u16 phydata1 ,u16 phydata2,u16 phydata3 );
static u16 b5387_mdiobus_read(void* ioaddr, u8 phyaddr, u8 page_num , u8 phyreg );
static int net_open(struct inode *inode, struct file *file);
static ssize_t net_write (struct file *file, const char *buf, size_t count, loff_t *offset);
static ssize_t net_read (struct file *file, char *buf, size_t count, loff_t *offset);

static void ftgmac100_write_phy_register(unsigned int ioaddr, u8 phyaddr, u8 phyreg, u16 phydata);
static u16 ftgmac100_read_phy_register(unsigned int ioaddr, u8 phyaddr, u8 phyreg);
static void initial_link_without_phy(void);
//static uint16_t input[2];	


/* Module Variables	*/
static struct file_operations net_ops = {
        owner:      THIS_MODULE,
        read:       net_read,
        write:      net_write,
        open:       net_open,
};



static void initial_link_without_phy()
{

#if 0  //Elmo+, 5387 no need this setting  	
  	u8 page_num = 0x02, reg = 0x00;
	u16 phydata=0x0082,phydata1=0x0000,phydata2=0x0000,phydata3=0x0000;

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);
	
	page_num = 0x00;
	reg = 0x20;
	phydata=0x0007;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);
	
	page_num = 0x00;
	reg = 0x86;
	phydata=0x0020;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);	

	page_num = 0x00;
	reg = 0x70;
	phydata=0x008B;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);

	page_num = 0x00;
	reg = 0x10;
	phydata=0x003C;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);

#else

	u8 page_num = 0x02, reg = 0x00;
	u16 phydata=0x0082,phydata1=0x0000,phydata2=0x0000,phydata3=0x0000;
	//u8 buf[8];

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);
	
	page_num = 0x00;
	reg = 0x0B;
	phydata=0x0003;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);
	
	page_num = 0x02;
	reg = 0x00;
	phydata=0x0086;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);	

	page_num = 0x00;
	reg = 0x08;
	phydata=0x001C;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);

	page_num = 0x00;
	reg = 0x0E;
	phydata=0x008B;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);

	page_num = 0x34;
	reg = 0x05;
	phydata=0x0013;
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);


#if 0
	//Set VLAN1 untag port
	page_num = 0x05; 
	reg = 0x63;
	buf[0] = 0x1F; //7-0
	buf[1] = 0x3F; //15-8
	buf[2] = 0x06; //23-16
	buf[3] = 0x00; //31-24
	buf[4] = 0x00; //39-32
	buf[5] = 0x00; //47-40
	buf[6] = 0x00; //55-48
	buf[7] = 0x00; //63-56

	phydata = (buf[1] << 8)| buf[0];
	phydata1 = (buf[3] << 8)| buf[2];
	phydata2 = (buf[5] << 8)| buf[4];
	phydata3 = (buf[7] << 8)| buf[6];

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);
	
	//set VLAN ID 1 to VLAN Table Address Index Register
	page_num = 0x05; 
	reg = 0x61;
	buf[0] = 0x01;
	buf[1] = 0x00;
	
	phydata = (buf[1] << 8)| buf[0];
	phydata1 = 0x00;
	phydata2 = 0x00;
	phydata3 = 0x00; 

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);

	//set VLAN ID 1 to VLAN Table Address Index Register
	page_num = 0x05; 
	reg = 0x60;
	buf[0] = 0x80;
	buf[1] = 0x00;
	
	phydata = (buf[1] << 8)| buf[0];
	phydata1 = 0x00;
	phydata2 = 0x00;
	phydata3 = 0x00; 
	
	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);


	//Set Global IEEE 820.1Q Register to enable VLAN function
	page_num = 0x34; 
	reg = 0x00; 
	buf[0] = 0xE2;
	buf[1] = 0x00;
	
	phydata = (buf[1] << 8)| buf[0];
	phydata1 = 0x00;
	phydata2 = 0x00;
	phydata3 = 0x00; 

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);

	//Set VLAN99 untag port
	page_num = 0x05; 
	reg = 0x63; 
	buf[0] = 0x1F; //7-0
	buf[1] = 0x3F; //15-8
	buf[2] = 0x06; //23-16
	buf[3] = 0x00; //31-24
	buf[4] = 0x00; //39-32
	buf[5] = 0x00; //47-40
	buf[6] = 0x00; //55-48
	buf[7] = 0x00; //63-56

	phydata = (buf[1] << 8)| buf[0];
	phydata1 = (buf[3] << 8)| buf[2];
	phydata2 = (buf[5] << 8)| buf[4];
	phydata3 = (buf[7] << 8)| buf[6];

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);
	
	//set VLAN ID 99 to VLAN Table Address Index Register
	page_num = 0x05; 
	reg = 0x61; 
	buf[0] = 0x63;
	buf[1] = 0x00;

	phydata = (buf[1] << 8)| buf[0];
	phydata1 = 0x00;
	phydata2 = 0x00;
	phydata3 = 0x00; 

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);
	
	//set VLAN ID 99 to VLAN Table Address Index Register
	page_num = 0x05; 
	reg = 0x60;
	buf[0] = 0x80;
	buf[1] = 0x00;
	
	phydata = (buf[1] << 8)| buf[0];
	phydata1 = 0x00;
	phydata2 = 0x00;
	phydata3 = 0x00; 

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);

	//Set Global IEEE 820.1Q Register to enable VLAN function
	page_num = 0x34; 
	reg = 0x00; 
	buf[0] = 0xEA;
	buf[1] = 0x00;
	
	phydata = (buf[1] << 8)| buf[0];
	phydata1 = 0x00;
	phydata2 = 0x00;
	phydata3 = 0x00; 

	b5387_mdiobus_write(ast_net_virt_base,0x1E, page_num ,reg, phydata, phydata1 , phydata2, phydata3);
#endif

#endif
	
}


int __init
net_module_init(void)
{
	int	ret;

	/* gpio device initialization */
	if ((ret = register_chrdev_region (net_devno, NET_MAX_DEVICES, NET_DEV_NAME)) < 0)
	{
	   printk (KERN_ERR "failed to register net device <%s> (err: %d)\n", NET_DEV_NAME, ret);
	   return -1;
	}

	net_cdev = cdev_alloc ();
	if (!net_cdev)
	{
	   unregister_chrdev_region (net_devno, NET_MAX_DEVICES);
	   printk (KERN_ERR "%s: failed to allocate net cdev structure\n", NET_DEV_NAME);
	   return -1;
	}

	cdev_init (net_cdev, &net_ops);
	net_cdev->owner = THIS_MODULE;

	if ((ret = cdev_add (net_cdev, net_devno, NET_MAX_DEVICES)) < 0)
	{
		cdev_del (net_cdev);
		unregister_chrdev_region (net_devno, NET_MAX_DEVICES);
		printk	(KERN_ERR "failed to add <%s> char device\n", NET_DEV_NAME);
		ret = -ENODEV;
		return ret;
	}
	
//	if (!request_mem_region(AST_NET_REG_BASE, ASPEED_ETHER_IO_REG_SIZE, NET_DEV_NAME)) {
//		ret = -EBUSY;
//		goto out_cdev_add;
//	}

			
	if((ast_net_virt_base = ioremap(AST_NET_REG_BASE, ASPEED_ETHER_IO_REG_SIZE)) == NULL)
	{
		printk("failed to map adc  IO space to memory\n");
		ret = -ENOMEM;
		goto out_mem_region;
	}


//   	register_reboot_notifier(&adc_driver_notifier);

	initial_link_without_phy();

	printk(KERN_INFO "Initial AST2520 NET MDIO module ...\n");
	
	return 0;
	
out_mem_region:
//	release_mem_region(AST_NET_REG_BASE, ASPEED_ETHER_IO_REG_SIZE);	
//out_cdev_add:
//	cdev_del(net_cdev);	

	return ret;
}

static void
net_module_exit(void)
{
	printk ("Unloading NET driver ..\n");
	
	iounmap(ast_net_virt_base);
	
//	release_mem_region(AST_NET_REG_BASE, ASPEED_ETHER_IO_REG_SIZE);	
//	
	unregister_chrdev_region (net_devno, NET_MAX_DEVICES);
		
	
	if (NULL != net_cdev)
	{
		cdev_del (net_cdev);
	}
	
	ast_net_virt_base = NULL;

}

static int
net_open(struct inode *inode, struct file *file)
{		
	return 0;
}



static ssize_t net_write (struct file *file, const char *buf, size_t count, loff_t *offset)
{
	u8 input[11]={0},index = 0;
	u8 phyaddr = 0x00 ,b5387_page = 0x00, b5387_reg = 0x00; //phy address 
	u16 phydata = 0x0000,phydata1 = 0x0000,phydata2 = 0x0000,phydata3 = 0x0000;
	unsigned long ret;
	
	ret = copy_from_user(input,buf,count);
			
	if (input[0] == 0xFF)
	{
		phyaddr = input[1];
		b5387_reg = input[2];
		
		phydata = (input[4] << 8)| input[3];
		phydata1 = (input[6] << 8)| input[5];
		phydata2 = (input[8] << 8)| input[7];
		phydata3 = (input[10] << 8)| input[9];

		printk ("\n!!!! DIRECT write phyaddr = %x b5387_reg=%x, phydata=%x\n",phyaddr,b5387_reg, phydata);
		ftgmac100_write_phy_register((unsigned int) ast_net_virt_base, phyaddr, b5387_reg,  phydata);
	}
	else
	{
		b5387_page = input[0];
		b5387_reg  = input[1];

		for(index = 2 ; index <= 9; index ++)
		{
			if(input[index] == -1)
				input[index]=0x00;
		}

		phydata = (input[3] << 8)| input[2];
		phydata1 = (input[5] << 8)| input[4];
		phydata2 = (input[7] << 8)| input[6];
		phydata3 = (input[9] << 8)| input[8];


		printk ("\n!!!! write phydata = %04x phydata1 = %04x phydata2 = %04x phydata3 = %04x\n",phydata,phydata1,phydata2,phydata3);
		b5387_mdiobus_write(ast_net_virt_base,phyaddr, b5387_page ,b5387_reg, phydata, phydata1 , phydata2, phydata3);

	}

	return 0;
}


static ssize_t net_read (struct file *file, char *buf, size_t count, loff_t *offset)
{

	u8 input[3] = {0};
	u8 phyaddr = 0x00; //phy address 
	u8 b5387_page = 0,b5387_reg = 0;
	u16 phydata = 0;
	unsigned long ret;
	
	ret = copy_from_user(input,buf,count);

		
	if (input[0] == 0xFF)
	{
		phyaddr = input[1];
		b5387_reg = input[2];
		
		printk ("\n!!!! DIRECT read phyaddr = %x b5387_reg=%x\n",phyaddr,b5387_reg);
		phydata = ftgmac100_read_phy_register((unsigned int) ast_net_virt_base, phyaddr, b5387_reg );
		printk("Direct Read MDIO Data 2: 0x%X\n.",phydata);
		
		input[0] = (phydata & 0xFF00) >> 8;
		input[1] = phydata & 0x00FF;
	}
	else
	{
		b5387_page = input[0];
		b5387_reg  = input[1];
		printk ("\n!!!! read b5387_page = %x b5387_reg=%x\n",b5387_page,b5387_reg);

		//	phydata = ftgmac100_read_phy_register(ast_net_virt_base, phyaddr, phy_offset);
		//	printk("Read PHY Data 2: 0x%X\n.",phydata);
		phydata = b5387_mdiobus_read(ast_net_virt_base, phyaddr,  b5387_page ,b5387_reg);
		printk("Read MDIO Data 2: 0x%X\n.",phydata);

		input[0] = (phydata & 0xFF00) >> 8;
		input[1] = phydata & 0x00FF;
	}
	
	ret = copy_to_user(buf,input,sizeof(input));	
	
	return 0;	
}

void ftgmac100_phy_rw_waiting(unsigned int ioaddr)
{
	unsigned int tmp;

	do {
		mdelay(10);
		tmp =inl(ioaddr + PHYCR_REG);
	} while ((tmp&(PHY_READ_bit|PHY_WRITE_bit)) > 0);
}

static u16 ftgmac100_read_phy_register(unsigned int ioaddr, u8 phyaddr, u8 phyreg)
{
	unsigned int tmp;

	if (phyaddr > 0x1f)	// MII chip IDs are 5 bits long
	    return 0xffff;

	tmp = inl(ioaddr + PHYCR_REG);
	tmp &= 0x3000003F;
	tmp |=(phyaddr<<16);
	tmp |=(phyreg<<(16+5));
	tmp |=PHY_READ_bit;

	outl( tmp, ioaddr + PHYCR_REG );
	ftgmac100_phy_rw_waiting(ioaddr);

	return (inl(ioaddr + PHYDATA_REG)>>16);
}

static void ftgmac100_write_phy_register(unsigned int ioaddr,
	u8 phyaddr, u8 phyreg, u16 phydata)
{
	unsigned int tmp;

	if (phyaddr > 0x1f)	// MII chip IDs are 5 bits long
	    return;

	tmp = inl(ioaddr + PHYCR_REG);
	tmp &= 0x3000003F;
	tmp |=(phyaddr<<16);
	tmp |=(phyreg<<(16+5));
	tmp |=PHY_WRITE_bit;

	outl( phydata, ioaddr + PHYDATA_REG );
	outl( tmp, ioaddr + PHYCR_REG );
	ftgmac100_phy_rw_waiting(ioaddr);
}

static u16 b5387_mdiobus_read(void* ioaddr, u8 phyaddr, u8 page_num , u8 phyreg )
{
	u8 addr_pseudo_phy;
	u8 success = 0;
	u16 w_data = 0 , r_data_15_0 = 0, r_data_31_16 = 0, r_data_47_32 = 0, r_data_63_48 = 0;
	u32 r_data = 0xFFFFFFFF , loop = 0;
	
	DO_PRINT("In %s-%d \n",__FUNCTION__,__LINE__);
	DO_PRINT("[%d]MDIO-W page_num 0x%X \n",__LINE__,page_num);
		
	addr_pseudo_phy= 0x1E;
	w_data = page_num << 8 | 0x01;
	ftgmac100_write_phy_register((unsigned int)ioaddr, addr_pseudo_phy, 16 , w_data);
	DO_PRINT("MDIO-W reg 16 data 0x%X\n",w_data);
	
	//w_data = phyreg << 8 | 0x00;
	//ftgmac100_write_phy_register((unsigned int) ioaddr, addr_pseudo_phy, 17 , w_data);
	//DO_PRINT("MDIO-W reg 17 data 0x%X\n",w_data);

	w_data = phyreg << 8 | 0x02;
	ftgmac100_write_phy_register((unsigned int) ioaddr, addr_pseudo_phy, 17 , w_data);
	DO_PRINT("MDIO-W reg 17 data 0x%X\n",w_data);
	
	loop = 0;
	do{
		r_data = ftgmac100_read_phy_register((unsigned int) ioaddr , addr_pseudo_phy, 17 );//read MII reg17
		r_data = r_data & 0x00000003;
		mdelay(1);
		loop++;
		if(r_data == 0x00){
			success = 1;
			DO_PRINT("MDIO-R reg17 OO=0x%X \n",r_data);
			break;
		}
			
	}while((r_data != 0x00) && (loop < 10 ));
	
	if(!success){
		DO_PRINT("MDIO-R reg17 Failed \n");
		return -1;
	}
	
	r_data_15_0 = ftgmac100_read_phy_register((unsigned int) ioaddr , addr_pseudo_phy, 24 );//read MII reg24
	r_data_31_16 = ftgmac100_read_phy_register((unsigned int) ioaddr , addr_pseudo_phy, 25 );//read MII reg25
	r_data_47_32 = ftgmac100_read_phy_register((unsigned int) ioaddr , addr_pseudo_phy, 26 );//read MII reg26
	r_data_63_48 = ftgmac100_read_phy_register((unsigned int) ioaddr ,addr_pseudo_phy, 27 );//read MII reg27
	
	DO_PRINT("MDIO-R r_data_15_0 = 0x%X  \n",r_data_15_0);
//	DO_PRINT("MDIO-R r_data_31_16 = 0x%X  \n",r_data_31_16);
//	DO_PRINT("MDIO-R r_data_47_32 = 0x%X  \n",r_data_47_32);
//	DO_PRINT("MDIO-R r_data_63_48 = 0x%X  \n",r_data_63_48);
	
	
	r_data = r_data_15_0;
	DO_PRINT("MDIO-R data = 0x%X  \n",r_data);

	return r_data;
}


static void b5387_mdiobus_write(void* ioaddr,
                                              u8 phyaddr, u8 page_num , u8 phyreg, u16 phydata, u16 phydata1 ,u16 phydata2,u16 phydata3 )
{
	u8 addr_pseudo_phy;//= 0x1E;//,ret = 0;
	u8 success = 0;
	u16 w_data = 0 ;
	u32 r_data = 0xFFFFFFFF , loop = 0;
	
	addr_pseudo_phy = 0x1E;
	
	DO_PRINT("In %s-%d \n",__FUNCTION__,__LINE__);
	w_data = page_num << 8 | 0x01;
	ftgmac100_write_phy_register((unsigned int) ioaddr, addr_pseudo_phy, 16 , w_data);
	DO_PRINT("MDIO-W reg 16 data 0x%02X\n",w_data);
	
	ftgmac100_write_phy_register((unsigned int) ioaddr, addr_pseudo_phy, 24 , phydata);//reg24
	DO_PRINT("MDIO-W reg 24 data 0x%02X\n",phydata);
	
//	ftgmac100_write_phy_register(ioaddr, addr_pseudo_phy, 25 , phydata1);//reg25
//	DO_PRINT("MDIO-W reg 25 data 0x%02X\n",phydata1);
//	
//	ftgmac100_write_phy_register(ioaddr, addr_pseudo_phy, 26 , phydata2);//reg26
//	DO_PRINT("MDIO-W reg 26 data 0x%02X\n",phydata2);
//	
//	ftgmac100_write_phy_register(ioaddr, addr_pseudo_phy, 27 , phydata3);//reg27
//	DO_PRINT("MDIO-W reg 27 data 0x%02X\n",phydata3);			
	
	//w_data = phyreg << 8 | 0x00;
	//ftgmac100_write_phy_register((unsigned int) ioaddr, addr_pseudo_phy, 17 , w_data);
	//DO_PRINT("MDIO-W reg 17 data 0x%02X\n",w_data);

	w_data = phyreg << 8 | 0x01;
	ftgmac100_write_phy_register((unsigned int) ioaddr, addr_pseudo_phy, 17 , w_data);
	DO_PRINT("MDIO-W reg 17 data 0x%02X\n",w_data);
	
	loop = 0;
	do{
		r_data = ftgmac100_read_phy_register((unsigned int) ioaddr , addr_pseudo_phy, 17 );//read MII reg17
		r_data = r_data & 0x00000003;
		DO_PRINT("[390]MDIO-R reg17 OO=0x%X \n",r_data);
		mdelay(1);
		loop++;
		if(r_data == 0x00){
			success = 1;
			DO_PRINT("MDIO-R reg17 OO=0x%X \n",r_data);
			break;
		}
			
	}while((r_data != 0x00) && (loop < 10 ));
	
	if(!success){
		DO_PRINT("MDIO-R reg17 Failed \n");
	}else{
		DO_PRINT("MDIO-R reg17 success!!!! \n");
	}
	
}



module_init(net_module_init);
module_exit(net_module_exit);

MODULE_AUTHOR("Fran Hsu");
MODULE_DESCRIPTION("NET MDIO driver module");
MODULE_LICENSE("GPL");
