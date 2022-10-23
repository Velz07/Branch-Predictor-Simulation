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



int main()
{


FILE *fp;

unsigned int k=5; //chooser bits
unsigned int ig=10; //index bits
unsigned int h=8; //history bits
unsigned int ib=5; //index bits
unsigned long long int PC_in;
char a_tn;

int bim_tn,gsh_tn,hyb_tn;


//for(int i=10;i<=15;i++)
//{


//Bimodal Init	
unsigned long long int arr_size = pow(2,ib);
int *bim_pred_table = (int*)calloc(arr_size,sizeof(int));

for(int z=0;z<=arr_size-1;z++)
	bim_pred_table[z]=2;

unsigned long long int b_index_mask = ((arr_size-1)<<2);

//Gshare Init
unsigned long long int arr_size_gshare = pow(2,ig);
int *gsh_pred_table = (int*)calloc(arr_size_gshare,sizeof(int));

for(int z=0;z<=arr_size_gshare-1;z++)
	gsh_pred_table[z]=2;

unsigned long long int g_index_mask = ((arr_size_gshare-1)<<2);

//Hybrid Choose Init
unsigned long long int ch_table_size = pow(2,k);
int *ch_table = (int*)calloc(ch_table_size,sizeof(int));

for(int z=0;z<=ch_table_size-1;z++)
	ch_table[z]=1;

unsigned long long int ch_index_mask = ((ch_table_size-1)<<2);

	
fp = fopen("trace_jpeg.txt","r");

while(!feof(fp))
{

	fscanf(fp,"%llx %c\n",&PC_in,&a_tn);


//	bim_tn = bimodal(i,b_index_mask,PC_in,a_tn,bim_pred_table);
//	gsh_tn = gshare(i,h,g_index_mask,PC_in,a_tn,gsh_pred_table);	
	
 hybrid(ig,h,g_index_mask,gsh_pred_table,
		ib,b_index_mask,bim_pred_table,
					ch_index_mask,PC_in,a_tn,ch_table);
	

	count++;
//	if(count==100)
//		break;
}
fclose(fp);


//printf("\n Index_Bits = %ld, tot_access = %ld pred = %ld, misPred = %ld",i,count,bim_p,bim_mp);
//printf("\n Index_Bits = %ld,history bits = %ld, tot_access = %ld, pred = %ld, misPred = %ld",i,h,count,gsh_p,gsh_mp);
printf("\n ic=%ld,ig = %ld,h = %ld,ib=%ld, tot_access = %ld, pred = %ld, misPred = %ld",k,ig,h,ib,count,hyb_p,hyb_mp);
//}
//for(int k=0;k<=ch_table_size-1;k++)
//	printf("ch_table[%d] = %d\n",k,ch_table[k]);


return(1);
	
}