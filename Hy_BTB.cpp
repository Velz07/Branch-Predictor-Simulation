#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

using namespace std;

unsigned int bim_p=0,bim_mp=0,count=0;
unsigned int gsh_p=0,gsh_mp=0;
unsigned int hyb_p=0,hyb_mp=0;
unsigned long long int GHR=0;

unsigned int btb_p=0,btb_mp=0;
unsigned int yh_p=0,yh_mp=0;


typedef struct btb_cache
{
	unsigned long long int tag;
	bool val;
	unsigned long long int lru_value;
	
};



unsigned long long int cache_access(unsigned long long int set,unsigned long long int tag,struct btb_cache **btb,unsigned long long int assoc)
{
	for(unsigned long long int y=0;y<=assoc-1;y++)
		if(btb[set][y].val && btb[set][y].tag == tag)
			return(y); //hit
	
	
	return(-1); //miss
	
}


//LRU_update on hit
void btb_update_hit(unsigned long long int set,unsigned long long int col_index,struct btb_cache **btb,unsigned long long int assoc)
{




unsigned long long int curr_lru_value = btb[set][col_index].lru_value;
btb[set][col_index].lru_value = 1;

//printf("\n curr_lru_value = %d",curr_lru_value);
if(curr_lru_value == 0)
{
    for(unsigned long long int i=0;i<=assoc-1;i++)
    {

    if( btb[set][i].val  && i!=col_index )
    {
    //printf("\n valid = %d",mem[set][i].c_valid);
    //printf("\n i=%d -- lru_curr = %d",i,mem[set][i].lru_value);
    btb[set][i].lru_value ++;

    }

    }
}

else 
{
for(unsigned long long int i=0;i<=assoc-1;i++)
{
    if(btb[set][i].lru_value < curr_lru_value && btb[set][i].val && i!=col_index) 
        {
            btb[set][i].lru_value ++;
        }

}

}


}



int btb_replace(struct btb_cache **btb,unsigned long long int set,unsigned long long int assoc)
{
int rep_blk = -1;
//chk if empty available
for(unsigned long long int i=0;i<=assoc-1;i++)
{

//printf("\n chk set= %llx, i= %llx\n",set,i);
if(!btb[set][i].val) //false means empty found
{
    return(i);
}
else if(btb[set][i].lru_value == assoc)  //if empty not found
	rep_blk = i;
}

return(rep_blk);	
	
}



int bimodal(unsigned int i,unsigned long long int index_mask,unsigned long long int PC_in,char a_tn,int *pred_table)
{
	
	unsigned long long int index = ((PC_in & index_mask)>>2);

//printf("Before: pred_table[%llx] = %d \n",index,pred_table[index]);	

	//predict T
	if(pred_table[index]>=2)
		{
			if(a_tn =='t')
			{
					bim_p++;	 
					if(pred_table[index]==2)
						pred_table[index]++;
						
			}
			
			else
				{
					bim_mp++;
					//update counter
					pred_table[index] = pred_table[index]-1;
				}				
		
				return(1);		
		}
		
	else //predict NT
	{
		if(a_tn == 'n')
		{
		
			bim_p++;
				if(pred_table[index]==1)
					pred_table[index]--;	
			
		}
		else
		{
			bim_mp++;
			pred_table[index] = pred_table[index] +1;
			

		}
		
		return(0);
		
	}


//printf("After: pred_table[%llx] = %d \n",index,pred_table[index]);	
	
}


int gshare(unsigned int i,unsigned int h,unsigned long long int index_mask,unsigned long long int PC_in,char a_tn,int *pred_table)
{
	
	unsigned long long int index = ((PC_in & index_mask)>>2);

//printf("Index chk1 %llx \n",index);
	//unsigned int gh_shift= (i-h);
	index = (index ^ (GHR<<(i-h)));
		
//printf("Index Check: PC_in = %llx,index_mask = %llx,index = %llx \n",PC_in,index_mask,index);
//printf("Before: GHR = %llx, actual = %c , pred_table[%llx] = %d \n",GHR,a_tn,index,pred_table[index]);	

	//predict T
	if(pred_table[index]>=2)
		{
			if(a_tn =='t')
			{
					gsh_p++;	 
					if(pred_table[index]==2)
						pred_table[index]++;
				
				GHR = (GHR>>1)^(1<< (h-1));		
//printf("After: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);								

						
			}
			
			else
				{
					gsh_mp++;
					//update counter
					pred_table[index] = pred_table[index]-1;
					
					GHR = (GHR>>1);
//printf("After: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);	

				}				

		return(1);					
	
		}
		
	else //predict NT
	{
		if(a_tn == 'n')
		{
		
			gsh_p++;
				if(pred_table[index]==1)
					pred_table[index]--;	
			
			GHR = (GHR>>1);
//printf("After: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);							

		}
		else
		{
			gsh_mp++;
			pred_table[index] = pred_table[index] +1;
			GHR = (GHR>>1)^(1<< (h-1));
//printf("After: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);				
	
		}
	
		return(0);	
	}



	
}

//Hybrid Predictor
void hybrid(unsigned int ig,unsigned int h,unsigned long long int g_index_mask,int *gsh_pred_table,
			unsigned int ib,unsigned long long int b_index_mask,int *bim_pred_table,
			unsigned long long int index_mask,unsigned long long int PC_in,char a_tn,int *ch_table)
{

	int bim,gsh,a_tn1;
	int bim_ret,gsh_ret;
	
	if(a_tn == 't')
		a_tn1 = 1;
	else
		a_tn1 =0;
	
		
	unsigned long long int hyb_index = ((PC_in & index_mask)>>2);

	unsigned long long int gsh_index = ((PC_in & g_index_mask)>>2);
	gsh_index = (gsh_index ^ (GHR<<(ig-h)));
	
	unsigned long long int bim_index = ((PC_in & b_index_mask)>>2);


//printf("Before: ch_table[%llx] = %d, actual = %c,%d \n",hyb_index,ch_table[hyb_index],a_tn,a_tn1);

	if(gsh_pred_table[gsh_index] >= 2)
		gsh = 1;
	else
		gsh = 0;
	
	if(bim_pred_table[bim_index] >=2)
		bim = 1;
	else
		bim = 0;


//printf("Pred: gsh= %d, bim= %d \n",gsh,bim);
		
	//Choose Gshare
	if(ch_table[hyb_index]>=2)
		{


		if(gsh == a_tn1)
			hyb_p++;
		else
			hyb_mp++;
		

		gsh_ret =gshare(ig,h,g_index_mask,PC_in,a_tn,gsh_pred_table);				


	
		

										
			}	
		
	else //Choose Bimodal
	{
		
		if(bim == a_tn1)
			hyb_p++;
		else
			hyb_mp++;

		//update GHR
		if(a_tn=='t')
			GHR = (GHR>>1)^(1<< (h-1));	
		else
			GHR = (GHR>>1);

		bim_ret = bimodal(ib,b_index_mask,PC_in,a_tn,bim_pred_table);
			
		

		}
		
		//update chooser before return
		if(gsh!=bim) 
		{
			if(gsh==a_tn1)  // gsh favorable
			{
			
				if(ch_table[hyb_index]<3)
				{
				
					ch_table[hyb_index]++;
					//printf("updated ch_table++\n");
				}
			}
			else if(bim == a_tn1) // bim favorable
			{
			
				if(ch_table[hyb_index]>0)	
				{
				
					ch_table[hyb_index]--;
					//printf("updated ch_table--\n");
				}
			}
		}	


//printf("After: ch_table[%llx] = %d \n",hyb_index,ch_table[hyb_index]);
//printf("--------------------------------------------------------\n");	
}


//YH predictor
int yh_pred(unsigned int p,unsigned long long int h_index_mask,unsigned long long int PC_in,char a_tn,int *pred_table,int *h_table)
{
	
	unsigned long long int h_index = ((PC_in & h_index_mask)>>2);
	
	unsigned long long int index = h_table[h_index];
	
		
//printf("Index Check: PC_in = %llx,h_index_mask = %llx,h_index = %llx,p_index = %llx \n",PC_in,h_index_mask,h_index,index);
//printf("Before: LHRp = %llx, actual = %c , pred_table[%llx] = %d \n",index,a_tn,index,pred_table[index]);	

	//predict T
	if(pred_table[index]>=2)
		{
			if(a_tn =='t')
			{
					yh_p++;	 
					if(pred_table[index]==2)
						pred_table[index]++;
				
				h_table[h_index] = (h_table[h_index]>>1)^(1<< (p-1));		
				
							

						
			}
			
			else
				{
					yh_mp++;
					//update counter
					pred_table[index] = pred_table[index]-1;
					
					h_table[h_index] = (h_table[h_index]>>1);

				}				

//printf("After ret1: LHRp = %llx, pred_table[%llx] = %d \n",h_table[h_index],index,pred_table[index]);
//printf("----------------------------------------------------------------------------\n");
		return(1);					
	
		}
		
	else //predict NT
	{
		if(a_tn == 'n')
		{
		
			yh_p++;
				if(pred_table[index]==1)
					pred_table[index]--;	
			
			h_table[h_index] = (h_table[h_index]>>1);
	

		}
		else
		{
			yh_mp++;
			pred_table[index] = pred_table[index] +1;
			
			h_table[h_index] = (h_table[h_index]>>1)^(1<< (p-1));

	
		}

//printf("After ret0: LHRp = %llx, pred_table[%llx] = %d \n",h_table[h_index],index,pred_table[index]);	
//printf("----------------------------------------------------------------------------\n");
		return(0);	
	}



	
}

int main()
{


FILE *fp;

unsigned int k=5; //chooser bits
unsigned int ig=10; //index bits
unsigned int h=11; //history bits or h_index for YH
unsigned int ib=5; //index bits
unsigned int p=9; //p history bits YH pred
unsigned long long int PC_in;
char a_tn;
int a_tn1;




int bim_tn,gsh_tn,hyb_tn,btb_hm,yh_tn;

//BTB cache
unsigned int btb_i = 7;
unsigned long long int btb_t = pow(2,24-btb_i)-1;
int btb_assoc = 4;
unsigned long long int btb_n = pow(2,btb_i);
unsigned long long int blk_replace=0;
unsigned long long int btb_index_mask = ((btb_n-1)<<2);
unsigned long long int btb_tag_mask = ( btb_t << (btb_i + 2) );


printf("\nmask check btb_i_mask = %llx, btb_t_mask= %llx \n",btb_index_mask,btb_tag_mask);

unsigned long long int btb_index;
unsigned long long int btb_tag;

btb_cache  **btb = (struct btb_cache**)calloc(btb_n,sizeof(struct btb_cache));
for(int x=0;x<=btb_n-1;x++)
	btb[x] = (struct btb_cache*)calloc(btb_assoc,sizeof(struct btb_cache));

for(unsigned long long int x=0;x<=btb_n-1;x++)
	for(unsigned long long int y=0;y<=btb_assoc-1;y++)
	{
	
		btb[x][y].val = false;
		btb[x][y].tag = 0;		
		btb[x][y].lru_value = 0;
		
	}



//for(int i=10;i<=15;i++)
//{


//Bimodal Init	
unsigned long long int arr_size = pow(2,ib);
int *bim_pred_table = (int*)calloc(arr_size,sizeof(int));

for(int k=0;k<=arr_size-1;k++)
	bim_pred_table[k]=2;

unsigned long long int b_index_mask = ((arr_size-1)<<2);

//Gshare Init
unsigned long long int arr_size_gshare = pow(2,ig);
int *gsh_pred_table = (int*)calloc(arr_size_gshare,sizeof(int));

for(int z=0;z<=arr_size_gshare-1;z++)
	gsh_pred_table[z]=2;

unsigned long long int g_index_mask = ((arr_size_gshare-1)<<2);

//Yeh_Patt Init
unsigned long long int h_size = pow(2,h);
unsigned long long int p_size = pow(2,p);

int *h_table = (int*)calloc(h_size,sizeof(int));
for(int k=0;k<=h_size-1;k++)
	h_table[k]=0;
	
int *p_table = (int*)calloc(p_size,sizeof(int));
for(int k=0;k<=p_size-1;k++)
	p_table[k]=2;
	
unsigned long long int h_index_mask = ((h_size-1)<<2);	
//Hybrid Choose Init
unsigned long long int ch_table_size = pow(2,k);
int *ch_table = (int*)calloc(ch_table_size,sizeof(int));

for(int k=0;k<=ch_table_size-1;k++)
	ch_table[k]=1;

unsigned long long int ch_index_mask = ((ch_table_size-1)<<2);

	
fp = fopen("trace_jpeg.txt","r");

while(!feof(fp))
{

	fscanf(fp,"%llx %c\n",&PC_in,&a_tn);

	if(a_tn=='t')
		a_tn1 = 1;
	else
		a_tn1 = 0;
		
	 btb_index = ((PC_in & btb_index_mask)>>2);
	 btb_tag = ((PC_in & btb_tag_mask)>>(btb_i + 2));	
	
	//printf("\n index check btb_i = %llx, btb_tag= %llx \n",btb_index,btb_tag);
	
	btb_hm = cache_access(btb_index,btb_tag,btb,btb_assoc);
	
	if(btb_hm >=0 ) //hit use predictor
	{
	
		//bim_tn = bimodal(ib,b_index_mask,PC_in,a_tn,bim_pred_table);
		//gsh_tn = gshare(ig,h,g_index_mask,PC_in,a_tn,gsh_pred_table);
// 		hybrid(ig,h,g_index_mask,gsh_pred_table,
//			   ib,b_index_mask,bim_pred_table,
//			   ch_index_mask,PC_in,a_tn,ch_table);		
	
	yh_tn = yh_pred(p,h_index_mask,PC_in,a_tn,p_table,h_table);
		
		btb_update_hit(btb_index,btb_hm,btb,btb_assoc);
		
//		if(bim_tn == a_tn1)
//			btb_p++;
//		else
//			btb_mp++;
//		
		
	}
	
	else //miss predit NT
	{
		
		if(a_tn == 't')
		{
		
			btb_mp++;
			

		}
		else
		{
		
			btb_p++;
			
		}
		
		
		blk_replace = btb_replace(btb,btb_index,btb_assoc);
		
		btb_update_hit(btb_index,blk_replace,btb,btb_assoc);
		
		//update values
		btb[btb_index][blk_replace].val = true;
		btb[btb_index][blk_replace].tag = btb_tag;			
	

		 
		
		
	}
	



//	gsh_tn = gshare(i,h,g_index_mask,PC_in,a_tn,gsh_pred_table);	
	
// hybrid(i,h,g_index_mask,gsh_pred_table,
//					b_index_mask,bim_pred_table,
//					ch_index_mask,PC_in,a_tn,ch_table);
	

	count++;
//	if(count==10)
//		break;
}
fclose(fp);


//printf("\n Index_Bits = %ld, tot_access = %ld pred = %ld, misPred = %ld",i,count,bim_p,bim_mp);

//printf("\n Index_Bits = %ld,history bits = %ld, tot_access = %ld, pred = %ld, misPred = %ld",ig,h,count,gsh_p,gsh_mp);
//printf("\n Index_Bits = %ld,history bits = %ld, tot_access = %ld, pred = %ld, misPred = %ld",i,h,count,hyb_p,hyb_mp);

//printf("\n Index_Bits = %ld, tot_access = %ld pred = %ld, misPred = %ld",ib,count,bim_p,bim_mp);
//printf("\n ic=%ld,ig = %ld,h = %ld,ib=%ld, tot_access = %ld, pred = %ld, misPred = %ld",k,ig,h,ib,count,hyb_p,hyb_mp);
printf("\n h=%ld,p = %ld, tot_access = %ld, pred = %ld, misPred = %ld",h,p,count,yh_p,yh_mp);
printf("\n Index_Bits = %ld,btb_assoc = %ld, tot_access = %ld, pred = %ld, misPred = %ld",btb_i,btb_assoc,count,btb_p,btb_mp);

//}
//for(int k=0;k<=ch_table_size-1;k++)
//	printf("ch_table[%d] = %d\n",k,ch_table[k]);


return(1);
	
}