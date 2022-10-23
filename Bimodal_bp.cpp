#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

using namespace std;

int main()
{


FILE *fp;

unsigned int i=10; //index bits
unsigned long long int PC_in;
char a_tn;
char p_tn;
unsigned long long int index;


for(int i=13;i<=15;i++)
{
	
unsigned long long int arr_size = pow(2,i);

int *pred_table = (int*)calloc(arr_size,sizeof(int));

for(int k=0;k<=arr_size-1;k++)
	pred_table[k]=2;

unsigned long long int index_mask = ((arr_size-1)<<2);

	
fp = fopen("trace_jpeg.txt","r");


unsigned int p=0,mp=0,count=0;
while(!feof(fp))
{

	fscanf(fp,"%llx %c\n",&PC_in,&a_tn);//fscanf(fp,"%llx %c\n",&PC_in,&b_act);
//	printf("count=%d, %llx,%c \n",count,PC_in,a_tn);
	


	index = ((PC_in & index_mask)>>2);

//printf("Before: pred_table[%llx] = %d \n",index,pred_table[index]);	

	//predict T
	if(pred_table[index]>=2)
		{
			if(a_tn =='t')
			{
					p++;	 
					if(pred_table[index]==2)
						pred_table[index]++;
						
			}
			
			else
				{
					mp++;
					//update counter
					pred_table[index] = pred_table[index]-1;
				}				
		
		}
		
	else //predict NT
	{
		if(a_tn == 'n')
		{
		
			p++;
				if(pred_table[index]==1)
					pred_table[index]--;			
		}
		else
		{
			mp++;
			pred_table[index] = pred_table[index] +1;
		}
		
	}


//printf("After: pred_table[%llx] = %d \n",index,pred_table[index]);

	count++;
//	if(count==10)
//		break;
}
fclose(fp);


printf("\n Index_Bits = %ld, tot_access = %ld pred = %ld, misPred = %ld",i,count,p,mp);
}

return(1);
	
}