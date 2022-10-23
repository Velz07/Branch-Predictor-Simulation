#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

using namespace std;

typedef struct btb_cache
{
	unsigned long long int tag;
	bool val;
	unsigned long long int lru_value;
	
};


//LRU_update on hit
void lru_update_hit(unsigned long long int set,unsigned long long int col_index,struct btb_cache **btb,unsigned long long int assoc)
{




unsigned long long int curr_lru_value = btb[set][col_index].lru_value;
btb[set][col_index].lru_value = 1;

//printf("\n curr_lru_value = %d",curr_lru_value);
if(curr_lru_value == 0)
{
    for(unsigned long long int i=0;i<=assoc-1;i++)
    {

    if( btb[set][i].c_valid !=0  && i!=col_index )
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
    if(btb[set][i].lru_value < curr_lru_value && btb[set][i].c_valid ==1 && i!=col_index) 
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

if(!btb[set][i].val) //false means empty found
{
    return(i);
}
else if(btb[set][i].lru_value == assoc)  //if empty not found
	rep_blk = i;
}

return(rep_blk);	
	
}


int main()
{


FILE *fp;

unsigned int i=10; //index bits
unsigned long long int PC_in;
char a_tn;
char p_tn;
unsigned long long int index;

unsigned int btb_i = 24;
int btb_assoc = 4;
unsigned long long int btb_n = pow(2,24)/btb_assoc;


//BTB cache
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
	
	
	
unsigned long long int arr_size = pow(2,i);

int *pred_table = (int*)calloc(arr_size,sizeof(int));

for(int k=0;k<=arr_size-1;k++)
	pred_table[k]=2;

unsigned long long int index_mask = ((arr_size-1)<<2);

	
fp = fopen("trace_gcc.txt","r");


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

//}
return(1);
	
}