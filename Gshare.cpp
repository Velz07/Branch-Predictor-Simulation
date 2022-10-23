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
printf("Before: GHR = %llx, actual = %c , pred_table[%llx] = %d \n",GHR,a_tn,index,pred_table[index]);	

	//predict T
	if(pred_table[index]>=2)
		{
			if(a_tn =='t')
			{
					gsh_p++;	 
					if(pred_table[index]==2)
						pred_table[index]++;
				
				GHR = (GHR>>1)^(1<< (h-1));		
printf("After shift+1: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);								

						
			}
			
			else
				{
					gsh_mp++;
					//update counter
					pred_table[index] = pred_table[index]-1;
					
					GHR = (GHR>>1);
printf("After Rshift: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);	

				}				
//printf("After: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);	

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
printf("After Rshift: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);								

		}
		else
		{
			gsh_mp++;
			pred_table[index] = pred_table[index] +1;
			GHR = (GHR>>1)^(1<< (h-1));
printf("After shift+1: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);
	
		}
	
//printf("After: GHR = %llx, pred_table[%llx] = %d \n",GHR,index,pred_table[index]);	
		return(0);	
	}



	
}




int main()
{


FILE *fp;

unsigned int i=7; //index bits
unsigned int h=6; //history bits
unsigned int k=7; //chooser bits
unsigned long long int PC_in;
char a_tn;

int bim_tn,gsh_tn;

printf("\n IndexBits, Historybits, Total, Pred, Mispred\n");

//for(int i=7;i<=12;i++)
//{
//for(int h=2;h<=i;h++)
//{
//
//::gsh_p=0,::gsh_mp=0;
//::GHR=0,::count=0;

//Bimodal Init	
unsigned long long int arr_size = pow(2,i);
int *bim_pred_table = (int*)calloc(arr_size,sizeof(int));

for(int k=0;k<=arr_size-1;k++)
	bim_pred_table[k]=2;

unsigned long long int b_index_mask = ((arr_size-1)<<2);

//Gshare Init
int *gsh_pred_table = (int*)calloc(arr_size,sizeof(int));

for(int k=0;k<=arr_size-1;k++)
	gsh_pred_table[k]=2;

unsigned long long int g_index_mask = ((arr_size-1)<<2);

//Hybrid Choose Init
unsigned long long int ch_table_size = pow(2,k);
int *ch_table = (int*)calloc(ch_table_size,sizeof(int));

for(int k=0;k<=ch_table_size-1;k++)
	ch_table[k]=1;

unsigned long long int ch_index_mask = ((ch_table_size-1)<<2);

	
fp = fopen("trace_perl.txt","r");

while(!feof(fp))
{

	fscanf(fp,"%llx %c\n",&PC_in,&a_tn);


//	bim_tn = bimodal(i,b_index_mask,PC_in,a_tn,bim_pred_table);


	gsh_tn = gshare(i,h,g_index_mask,PC_in,a_tn,gsh_pred_table);	
	



	count++;
	if(count==10)
		break;
}
fclose(fp);


//printf("\n Index_Bits = %ld, tot_access = %ld pred = %ld, misPred = %ld",i,count,bim_p,bim_mp);
//printf("\n Index_Bits = %ld,history bits = %ld, tot_access = %ld, pred = %ld, misPred = %ld",i,h,count,gsh_p,gsh_mp);
printf("\n %ld, %ld, %ld, %ld, %ld \n",i,h,count,gsh_p,gsh_mp);

//}
//}


return(1);
	
}