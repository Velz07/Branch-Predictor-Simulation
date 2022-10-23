#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

using namespace std;

unsigned int yh_p=0,yh_mp=0,count=0;



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

unsigned int h=9; //h_index
unsigned int p=11; //p history bits
unsigned long long int PC_in;
char a_tn;

int bim_tn,gsh_tn,yh_tn;

unsigned long long int h_size = pow(2,h);
unsigned long long int p_size = pow(2,p);
//for(int i=10;i<=15;i++)
//{





//Yeh_Patt Init
int *h_table = (int*)calloc(h_size,sizeof(int));
for(int k=0;k<=h_size-1;k++)
	h_table[k]=0;
	
int *p_table = (int*)calloc(p_size,sizeof(int));
for(int k=0;k<=p_size-1;k++)
	p_table[k]=2;
	
unsigned long long int h_index_mask = ((h_size-1)<<2);
	
fp = fopen("trace_gcc.txt","r");

while(!feof(fp))
{

	fscanf(fp,"%llx %c\n",&PC_in,&a_tn);


//	bim_tn = bimodal(i,b_index_mask,PC_in,a_tn,bim_pred_table);


//	gsh_tn = gshare(i,h,g_index_mask,PC_in,a_tn,gsh_pred_table);	
	
	yh_tn = yh_pred(p,h_index_mask,PC_in,a_tn,p_table,h_table);

	count++;
//	if(count==10)
//		break;
}
fclose(fp);


//printf("\n Index_Bits = %ld, tot_access = %ld pred = %ld, misPred = %ld",i,count,bim_p,bim_mp);
printf("\n HIndex_Bits h = %ld,history bits p = %ld, tot_access = %ld, pred = %ld, misPred = %ld",h,p,count,yh_p,yh_mp);

//}


return(1);
	
}