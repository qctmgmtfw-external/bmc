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
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include "rsa.h"

typedef struct {
    unsigned int bits;
    unsigned char *num;	
} _BIGNUM;

int nm = 0;
int dwm;
int mdwm;
int shl_cnt=0;

unsigned char Getbit(unsigned int *X, int k)
{
     return ((X[k/32]>>(k%32))&1);
}

void Mul2(unsigned int *T)
{
    unsigned int msb, temp;
    int j;

    temp = 0;
    for (j=0; j<mdwm; j++) {
        msb = (T[j]>>31)&1;
        T[j] = (T[j]<<1)|temp;
        temp = msb;
    }
}

void Sub2by32(unsigned int *Borrow, unsigned int *Sub, unsigned int C, unsigned int S, unsigned int M)
{
    unsigned long long Sub2;

     Sub2  = (unsigned long long)S - (unsigned long long)M;
     if (C) Sub2 -= (unsigned long long)1; 
     
     if ((Sub2 >>32)>0) *Borrow = 1;
     else               *Borrow = 0;
     *Sub = (unsigned int)(Sub2 & 0xffffffff);
}

void MCompSub(unsigned int *S, unsigned int *M)
{ 
    int flag;
    int j;
    unsigned int Borrow, Sub;
   
    flag = 0;    //0: S>=M, 1: S<M
    for (j=mdwm-1; j>=0; j--) {
        if      (S[j]>M[j])           break;
        else if (S[j]<M[j]) {flag =1; break;};
    }

    if (flag==0) {
    	Borrow = 0;
        for (j=0; j<mdwm; j++) {
            Sub2by32(&Borrow, &Sub, Borrow, S[j], M[j]);
            S[j] = Sub;
        }
    }
    
}

void MulRmodM(unsigned int *X, unsigned int *M)
{
    int k;
    for (k=0; k<nm; k++) {
    	Mul2(X);
    	MCompSub(X, M);
    }
}
int nmsb(unsigned int *X)
{
    int i, j;
    int nmsb;
    nmsb = MAX_RSA_KEY_LEN*32;
    for (j=MAX_RSA_KEY_LEN-1; j>=0; j--){ 
        if (X[j]==0) nmsb-=32;
        else {
            for (i=32-1; i>=0; i--)
                 if ((X[j]>>i)&1) {i=0; j=0; break;}
                 else  nmsb --;
        }
    }
    return(nmsb);

}

void init_0(unsigned int *D0)
{
    int j;
    for (j=0; j<MAX_RSA_KEY_LEN; j++)
        D0[j] = 0;	
}

void init_1(unsigned int *D1)
{
    int j;
    for (j=0; j<MAX_RSA_KEY_LEN; j++)
        if (j==0) D1[j] = 1;
        else      D1[j] = 0;	
}

void init_m1(unsigned int *Dm1)
{
    int j;
    for (j=0; j<MAX_RSA_KEY_LEN; j++)
        Dm1[j] = 0xffffffff;		
}

void init_T(unsigned int *T)
{
    int j;
    for (j=0; j<MAX_RSA_KEY_LEN; j++)
        if (j<1)         T[j] = 0x12345678; //0x12345678(0x10001)==>0xBF64FF05; //0x2(0x10001)==>0x67bbef82;
        else if (j==1)   T[j] = 0xBF64FF05; //0xBB33FF05;
        else             T[j] = 0;	
}

void Copy(unsigned int *D, unsigned int *S)
{
    int j;
    for (j=0; j<MAX_RSA_KEY_LEN; j++)
        D[j] = S[j];
}

int Compare(unsigned int *X, unsigned int *Y)
{
    int j;
    int result;
    
    result = 0;
    for (j=MAX_RSA_KEY_LEN-1; j>=0; j--)
        if      (X[j]>Y[j]) {result =  1; break;}
        else if (X[j]<Y[j]) {result = -1; break;}
    return(result);
}

void Add(unsigned int *X, unsigned int *Y)
{
    int j;
    unsigned long long t1;
    unsigned long long t2;
    unsigned long long sum;
    unsigned long long carry;

    carry =0;
    for (j=0; j<MAX_RSA_KEY_LEN - 1; j++) {
        t1 = X[j];
        t2 = Y[j];
        sum = t1 + t2 + carry;
        X[j]  = sum & 0xffffffff;
        carry = (sum >>32) & 0xffffffff;
    }
    //if (carry>0) printk("!!!overflow!!!\n");
    X[MAX_RSA_KEY_LEN - 1] = carry;
}

void Substrate(unsigned int *X, unsigned int *Y)
{
    int j;
    unsigned long long t1;
    unsigned long long t2;
    unsigned long long sum;
    unsigned int carry;

    carry =0;
    for (j=0; j<MAX_RSA_KEY_LEN - 1; j++) {
        t1 = X[j];
        t2 = Y[j];
        if (carry) sum = t1 - t2 -1;
        else       sum = t1 - t2;
        X[j]  = sum & 0xffffffff;
        carry = (sum >>32) & 0xffffffff;
    }
    //X[MAX_RSA_KEY_LEN - 1] = 0xffffffff;
    if (carry>0) X[MAX_RSA_KEY_LEN - 1] = 0xffffffff;
    else         X[MAX_RSA_KEY_LEN - 1] = 0x0;
}

void ShiftLeft(unsigned int *X, int i)
{
    int j;
    int msb;
    int temp;

    msb = i;
    for (j=0; j<MAX_RSA_KEY_LEN; j++) {
         temp = X[j]>>31;
         X[j] = (X[j]<<1)|msb;
         msb = temp;
    }     
}

void ShiftLeftFast(unsigned int *R, unsigned int *X, int nx, int ny)
{
    int j;
    unsigned int cntb;
    unsigned int shldw, shrbit;
    unsigned int shloffset;
    unsigned int bitbuf;

    cntb = nx/32;
    if ((nx%32)>0) cntb++;
    
    shldw  = cntb - ((nx-ny)/32);

    shrbit = (nx-ny) % 32;
    shloffset = (nx-ny) /32;
    bitbuf =0;
    //printk("nx=%d, ny=%d, cntb=%d, shldw=%d, shrbit=%d, shloffset=%d\n", nx, ny, cntb, shldw, shrbit, shloffset);
    for (j=shldw-1; j>=0; j--) {
         if (shrbit==0){
             R[j] = (X[shloffset+j]>>shrbit);
             bitbuf = X[shloffset+j]; 
         }
         else {
             R[j] = (X[shloffset+j]>>shrbit)|bitbuf;
             bitbuf = X[shloffset+j] <<(32-shrbit);
         }
         //printk("%x, ", bitbuf);
    }
    //printk("\n");    
}

void Positive(unsigned int *X)
{
    unsigned int *D0;
    
    D0 = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
    init_0(D0);
    Substrate(D0, X);
    Copy(X, D0);
    kfree(D0);
}

void Divide(unsigned int *Q, unsigned int *R, unsigned int *X, unsigned int *Y)
{
    int j;
    int nx, ny;
    
    nx = nmsb(X);
    ny = nmsb(Y);
    init_0(Q);
    init_0(R);

    ShiftLeftFast(R, X, nx, ny);
    for (j=nx-ny; j>=0; j--) {
         shl_cnt++;
         if(Compare(R, Y)>=0) {
             Substrate(R, Y); 
             ShiftLeft(Q, 1); 
         } else {             
             ShiftLeft(Q, 0); 
         }
         if (j>0)
             ShiftLeft(R, Getbit(X, j-1));
    }                
}

void MultiplyLSB(unsigned int *X, unsigned int *Y)
{
    int i, j;
    unsigned int *T;
    unsigned long long t1;
    unsigned long long t2;
    unsigned long long product;
    unsigned int carry;
    unsigned int temp;
    
    T = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
    init_0(T);
    for (i=0; i<128; i++) {
    	carry =0;
        for (j=0; j<130; j++) {
            if (i+j<130) {
            	t1=X[i];
            	t2=Y[j];
            	product = t1*t2 + carry + T[i+j];
            	temp = (product >>32)& 0xffffffff;
            	T[i+j] = product & 0xffffffff;
            	carry = temp;
            }
        }
    }
    Copy(X, T);
    kfree(T);  
	
}

//x = lastx - q * t;
void CalEucPar(unsigned int *x, unsigned int *lastx, unsigned int *q, unsigned int *t)
{
    unsigned int *temp;
    
    temp = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
    //printk("Start CalEucPar\n");
    Copy(temp, t);
    Copy(x, lastx);
    if (Getbit(temp, 4095)) {
        Positive(temp);
        MultiplyLSB(temp, q);
        Add(x, temp);
    } else {
        MultiplyLSB(temp, q);
        Substrate(x, temp);
    }
    kfree(temp); 
}

int Euclid(unsigned int *Mp, unsigned int *M, unsigned int *S)
{
		int j;
		int check;
		unsigned int *a = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *b = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *q = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *r = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *x = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *y = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *lastx = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *lasty = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *t  = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		unsigned int *D1 = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		int div_cnt=0;

    Copy(a, M);
    Copy(b, S);

    init_1(D1);
    init_1(x);
    init_0(lastx);

    init_m1(y);
    init_1(lasty);
    div_cnt = 0;
    shl_cnt = 0;
    //step 2
    while (Compare(b, D1)>0) {
        Divide(q, r, a, b);
        div_cnt++;
        Copy(a, b);
        Copy(b, r);
        
        Copy(t, x);
        CalEucPar(x, lastx, q, t);
        Copy(lastx, t);

        Copy(t, y);
        CalEucPar(y, lasty, q, t);
        Copy(lasty, t);
    }
    kfree(a);
    kfree(b);
    kfree(q);
    kfree(lastx);
    kfree(lasty);
    kfree(t);

    init_1(r);
    for (j=0; j<nm; j++) ShiftLeft(r, 0);
    if (Getbit(x, 4095)) {
    	Add(x, M);
    	Substrate(y, r);
    }
    Positive(y);
    Copy(Mp, y);

    //Check Euclide
    
    MultiplyLSB(x, r);
    MultiplyLSB(y, M);
    Substrate(x, y);
    check=Compare(x, D1);
    if (check==0)
    {
    	  //printk("***PASS for Eculde check\n");
    }
    else
    {
        printk("***FAIL for Eculde check\n");
        init_0(Mp);
        Mp[0] = 0x08000000;
        kfree(r);
        kfree(x);
        kfree(y);
        kfree(D1);
        return -1;
    }
    //printk("***total %d divides, %d shift-substrates\n", div_cnt, shl_cnt);
    
    kfree(r);
    kfree(x);
    kfree(y);
    kfree(D1);
    return 0;
}

int FillMp(rsa_mp_info_t *rsa_mp_info, cipher_data_t  *cipher_data)
{
	  //Here to find Mp and fill it back to rsa_mp_info
	  
	  _BIGNUM exp;
	  _BIGNUM mod;
	  int ret = -1;
	  
		//initial value T, E, M 
		//output data   S
		// S = (T ^ E) mod M
		unsigned int *E;
		unsigned int *M;
		unsigned int *S;
		unsigned int *Mp;
		int j;
		
		int msb = 0;
		int len = 0;
		int ne = 0;

	  exp.bits = cipher_data->key_len * 8;
	  exp.num = cipher_data->key;
	  
	  mod.bits = cipher_data->iv_len * 8;
	  mod.num = cipher_data->iv;
	  
    //fill E
    E = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
    init_0(E);
    len = (cipher_data->key_len/sizeof(uint32_t)) + ((cipher_data->key_len%sizeof(uint32_t) == 0)?0:1);
    //printk("E len = %d\n", len);
    for (j=0; j< len; j++)
    {
				if (j==len-1)
            memcpy(&E[j], &exp.num[j*4], cipher_data->key_len - (j*4));
        else
            E[j] = *((unsigned int *)(exp.num +(j*4)));
        //printk("E[%d]=%08X\n",j, E[j]);
    }
    ne = nmsb(E);
    //printk("ne = %d\n", ne);
    kfree(E);
        
    //fill M
    M = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
    init_0(M);
    len = (cipher_data->iv_len/sizeof(uint32_t)) + ((cipher_data->iv_len%sizeof(uint32_t) == 0)?0:1);
    //printk("M len = %d\n", len);
    for (j=0; j< len; j++)
    {
				if (j==len-1)
            memcpy(&M[j], &mod.num[j*4], cipher_data->iv_len - (j*4));
        else
            M[j] = *((unsigned int *)(mod.num +(j*4)));
        //printk("M[%d]=%08X\n",j, M[j]);
    }          
    nm = nmsb(M);
    //printk("nm = %d\n", nm);
     
    if ((nm%32) > 0)
        dwm = (nm/32) + 1;
    else
        dwm = (nm/32);
    //printk("dwm=%d\n", dwm);

    mdwm = dwm;
    if ((nm%32)==0) mdwm++;

		// init S <= 1
		// S = 1 * R mod M = R-M
		S = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
		init_1(S);
    MulRmodM(S, M);

		// calculate Mp, R*1/R - Mp*M = 1
		// Because R div M = 1 rem (R-M), S=R-M, so skip first divide.
		Mp = (unsigned int *) kmalloc(MAX_RSA_KEY_LEN * sizeof(unsigned int), GFP_KERNEL);
    ret = Euclid(Mp, M, S);
    if (ret != 0)
    {
        //Euclid Check Fail
        kfree(M);
        kfree(S);
        kfree(Mp);
        return -1;
    }
    
    kfree(M);
    kfree(S);
    
    //fill n' (Mp)
    Mp[MAX_RSA_KEY_LEN - 1] = 0;
    msb = nmsb(Mp);
    rsa_mp_info->mp_len = (msb + 7 ) / 8;
    if (rsa_mp_info->mp_len > MAX_RSA_KEY_LEN) rsa_mp_info->mp_len=MAX_RSA_KEY_LEN;
    len = (msb + 31 ) / 32;
    //printk("Mp len = %d\n", len);
    for (j=0; j< len; j++)
    {
		    if (j==len-1)
            memcpy(&rsa_mp_info->rsa_mp[j*4], &Mp[j], rsa_mp_info->mp_len - (j*4));
        else
    	      memcpy(&rsa_mp_info->rsa_mp[j*4], &Mp[j], sizeof(unsigned int));
    	  //printk("rsa_mp_info->rsa_mp[%d]=%08X\n",j*4, *((unsigned int *) &rsa_mp_info->rsa_mp[j*4]));
    }
    kfree(Mp);
    
    //fill exp
    rsa_mp_info->mod_exp_bitnum =  (nm << 16) | ne;
    return 0;
}
