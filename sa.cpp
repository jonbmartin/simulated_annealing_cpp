#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <iostream>
using namespace std;
extern double drand48();
extern long lrand48(/*long*/);
extern int rand();
extern void srand48(long seedval);
#define NO_SQUARES 11
#define SQUARE_SIZE_LIMT 6
#define INITIAL_TEMPERATURE 70.0
#define R 0.8
#define NO_ITERATIONS 20
#define NO_STEPS 10000
#define PLUS NO_SQUARES
#define TIMES NO_SQUARES+1
#define TRUE 1
#define FALSE 0
#define TEST
int op[2*NO_SQUARES-1];
int present_op[2*NO_SQUARES-1];
int data[NO_SQUARES];
int stack_x[NO_SQUARES], stack_y[NO_SQUARES];
int stack_pointer;
int x0_val,y0_val;
int x1_val,y1_val;
#ifdef TEST
int test_set[NO_SQUARES]={1,1,1,1,1,2,3,3,3,3,6};
#endif
//list structure passed to program

class square_list {
public:
  int side;
  square_list * next;
};

void get_list_start(square_list ** list)
{
  int i;
  struct square_list * build_list, * list_start;
  build_list= new square_list;
  list_start = build_list;
  for(i=0;i<NO_SQUARES;i++)
    {
      #ifndef TEST
      build_list->side=(lrand48()>>4)%(SQUARE_SIZE_LIST)+1;
      #endif
      #ifdef TEST
      build_list->side = test_set[i];
      #endif
      build_list->next = new square_list;
      build_list=build_list->next;
    }
  #ifndef TEST
  build_list->side = rand()%(SQUARE_SIZE_LIMIT)+1;
  #endif
  build_list->next = NULL;
  *list = list_start;
}

/************************************************************************/
/* create initial operation array */
/* operations are alternately chosen PLUS and TIMES*/
void create_operation_array()
{
  int i,j;
  for(i=0;i<NO_SQUARES;i++) op[i]=i;
  for(j=0;j<NO_SQUARES;j++) op[i++]=PLUS + i%2;
}


/***********************************************************************/
/* remove data from stack */
void pop()
{
  stack_pointer--;
  x1_val=x0_val;
  y1_val=y0_val;
  x0_val=stack_x[stack_pointer];
  y0_val=stack_y[stack_pointer];
}
/**********************************************************************/
/* push data on to stack */
void push(int x, int y)
{
  stack_x[stack_pointer]=x;
  stack_y[stack_pointer]=y;
  stack_pointer++;
}
/**********************************************************************/
/* pieces side by side */
void merge_times()
{
  if(y0_val <= y1_val) y0_val = y1_val;
  x0_val += x1_val;
}
/* pieces on top of each other */
void merge_plus()
{
  if(x0_val <= x1_val) x0_val = x1_val;
  y0_val += y1_val;
}
/**********************************************************************/
/* calculate the cost of op array */
int calculate_cost()
{
  int i;
  stack_pointer=0;
  for(i=0;i<2*NO_SQUARES-1;i++)
    {
      switch(op[i])
	{
	case PLUS:
	  pop();
	  pop();
	  merge_plus();
	  push(x0_val,y0_val);
	  break;

	case TIMES:
	  pop();
	  pop();
	  merge_times();
	  push(x0_val,y0_val);
	  break;

	  /* data */
	default:
	  push(data[op[i]],data[op[i]]);
	  break;
	}
    }
  pop();
  if(x0_val>=y0_val) return(x0_val*x0_val);
  else return(y0_val*y0_val);
}
//*********************************************************************/
// function to determine if an item is data or an operation */
int is_data(int x)
{
  if((op[x]!=TIMES) && (op[x]!=PLUS)) return(TRUE);
  else return(FALSE);
}
//*********************************************************************/
// neighbor solution calculations */
void switch_op()
{
  int i,j,k,loc;
  k=-1;

  /* choose random operator */
  j = lrand48()%(NO_SQUARES-1);

  /* search for location */
  for(i=0;i<2*NO_SQUARES-1;i++)
    {
      if((op[i]==PLUS)||(op[i]==TIMES))
	{
	  k++;
	  if(k==j) loc=i;
	}
    }

  // swap */
  if(op[loc]==PLUS) op[loc]=TIMES;
  else op[loc]=PLUS;
}
/**********************************************************************/
/* neighbor solution calculations */
void ABC_op_to_AB_op_C()
{
  int i,j,k,temp;
  k=0;
  for(i=0;i<2*NO_SQUARES-4;i++)
    if((is_data(i)) && (is_data(i+1)) && (is_data(i+2)) &&(!is_data(i+3))) k++;
  if(k==0) return;
  j=lrand48()%k;
  k=0;
  for(i=0;i<2*NO_SQUARES-4;i++)
    if((is_data(i)) && (is_data(i+1)) && (is_data(i+2)) && (!is_data(i+3))
							    && (k++==j))
      {
	temp=op[i+2];
	op[i+2]=op[i+3];
	op[i+3]=temp;
	return;
      }
}
/*********************************************************************/
/* neighbor solution calculations */
void op_A_to_A_op()
{
  int i,j,k,temp;
  k = 0;
  for(i=0;i<2*NO_SQUARES-2;i++)
    if((!is_data(i)) && (is_data(i+1))) k++;
  if(k==0) return;
  j=lrand48()%k;
  k=0;
  for(i=0;i<2*NO_SQUARES-2;i++)
    if((!is_data(i)) && (is_data(i+1)) && (k++==j))
      {
	temp = op[i];
	op[i]=op[i+1];
	op[i+1]=temp;
	return;
      }
}
/***********************************************************************/
/* neighbor solution calculation */
void A_op_to_op_A()
{
  int i,j,k,temp;
  int operations=0;
  int data_item=0;
  k=0;
  for(i=0;i<2*NO_SQUARES-3;i++)
    {
      if(is_data(i)) data_item++; else operations++;
      if((is_data(i)) && (!is_data(i+1)) && (data_item>=(operations+3))) k++;
    }
    if(k==0) return;

    j=lrand48()%k;
    k=0;
    data_item=0;
    operations=0;
    for(i=0;i<2*NO_SQUARES-3;i++) {
	if(is_data(i)) data_item++; else operations++;
	if((is_data(i)) && (!is_data(i+1)) && (data_item>=(operations+3)) && (k++==j))
	  {
	    temp=op[i];
	    op[i]=op[i+1];
	    op[i+1]=temp;
	    return;
	  }
      }
}
/***********************************************************************/
/* neighbor solution calculations */
void AB_to_BA()
{
  int i,j,k,m,n;
  int k1,temp;
  i=lrand48()%NO_SQUARES;
  j=lrand48()%NO_SQUARES;
  while(i==j) i=lrand48()%NO_SQUARES;
  k1 = 0;
  for(k=0;k<2*NO_SQUARES-1;k++)
    {
      if(is_data(k))
	{
	  if(i==k1) m=k;
	  if(j==k1) n=k;
	  k1++;
	}
    }
  temp=op[m];
  op[m]=op[n];
  op[n]=temp;
}
/**********************************************************************/
/* sample data for which optimal is known = 81 */
void sample_data()
{
  int i;
  for(i=0;i<NO_SQUARES;i++) data[i]=2;
}
/**********************************************************************/
/* randomly selected neighbor of op */
void neighbor_solution()
{
  switch((lrand48()>>4)%5)
    {
    case 4: A_op_to_op_A();
      break;
    case 3:
      op_A_to_A_op();
      break;
    case 2:
      AB_to_BA();
      break;
    case 1:
      switch_op();
      break;
    case 0:
      ABC_op_to_AB_op_C();
      break;
    default:
      break;
    }
}

/*******************************************************************/
/* function to accept neighbor */
void accept_neighbor()
{
  int i;
  for(i=0;i<2*NO_SQUARES-1;i++) present_op[i]=op[i];
}

/********************************************************************/
/* print final output */
void print_results(int optimal_cost)
{
  int i;
  cout << "Calculated cost" << optimal_cost << endl;
  for(i=0;i<2*NO_SQUARES-1;i++)
    { switch(present_op[i])
	{
	case PLUS: cout << "+";
	  break;
	case TIMES: cout << "*";
	  break;
	default:
	  cout << data[present_op[i]] << " ";
	  break;
	}
    }
  cout << endl;
}

/********************************************************************/
/* main program */
int main()
{
  int i,j,k;
  class square_list * list_start;
  int optimal_cost,random_neighbor_cost;
  int cost;
  double temperature=INITIAL_TEMPERATURE;
  double probability,random_0_1;
  /* set random number generator */
  struct timeval tp;
  struct timezone tzp;
  gettimeofday(&tp,&tzp);
  srand48(tp.tv_usec);
  /* get external list */
  get_list_start(&list_start);
  /* create operation array for neighbor solution */
  create_operation_array();
  /* create present active optimal operatyion_array */
  for(i=0;i<2*NO_SQUARES-1;i++) present_op[i] = op[i];
  /* get data */
  i=0;
  while(list_start!=NULL)
    {
      data[i++]=list_start->side;
      list_start=list_start->next;
    }
  /* use sample data from handout if defined at beginning of program */
  optimal_cost=calculate_cost();
  /* perform annealing */
  for(j=0;j<NO_ITERATIONS;j++)
    {
      for(i=0;i<NO_STEPS;i++)
	{
	  for(k=0;k<2*NO_SQUARES-1;k++)
	    op[k]=present_op[k];
	  neighbor_solution();
	  cost=calculate_cost();
	  if(cost<=optimal_cost)
	    {
	      accept_neighbor();
	      optimal_cost=cost;
	    }
	  else{
	    probability = exp(-(cost-optimal_cost)/temperature);
	    random_0_1 = drand48();
	    if(random_0_1 <= probability)
	      {
		accept_neighbor();
		optimal_cost=cost;
	      };
	  }
	}
	  temperature*=R;
	}
      print_results(optimal_cost);
      return 1;
}
      
	 
	  
	 
		 
