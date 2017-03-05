#include<stdio.h>
#include<stdlib.h>
#include<time.h>

struct node
{
	int key[4],level;
	struct node *ptr[5],*parent;
};
typedef struct node *NODE;

NODE child=NULL;
int ind=0,index_val=0;
NODE insert(NODE,int);
NODE getNode();

FILE *outputFile;

void preorderDotDump (NODE root, FILE* outputFile)
{
	int i;	
	if (root != NULL) 
	{
		fprintf (outputFile, "node%d[label=\"<f0>%d",root->level,root->key[0]);
		i=1;
		//while(root->key[i]!='\0')
		while(i<4)
		{			
			fprintf (outputFile, "|<f%d>%d",i,root->key[i]);
			i++;			
		}
		fprintf (outputFile,"\"];\n");	
		i=0;
		while(root->ptr[i]!=NULL && i < 5)
		{		
			fprintf (outputFile, "\"node%d\":f%d -> \"node%d\":f%d;\n", root->level,0, root->ptr[i]->level,0);
			i++;			
		}
        i=0;
        while(root->ptr[i]!=NULL && i<5)
		{			
			preorderDotDump (root->ptr[i], outputFile);
			i++;			
		}
	}
}

void dotDump(NODE root, FILE *outFile)
{
	//gName++;
	fprintf (outFile, "digraph tree{\n node [shape = record];\n");
	//if(flg==1)
	preorderDotDump (root, outFile);
    fprintf (outFile, "}\n");
}

void main()
{
	int ch,data;
	FILE *pipe;
	NODE root=NULL;
	system("rm -rf bplus.dot");
	srand(time(NULL));
	outputFile = fopen ("bplus.dot", "w");
	fclose (outputFile);
	for(;;)
	{
		printf("1.Insert\n");
		printf("Enter the choice\n");
			scanf("%d",&ch);
		switch(ch)
		{
			case 1: printf("Enter the data:\n");
						scanf("%d",&data);
					root=insert(root,data);
					outputFile = fopen ("bplus.dot", "a");
					if (outputFile != NULL) 
						dotDump (root,  outputFile);
					fclose (outputFile);
					break;
			case 2: pipe=popen("dot -Tps bplus.dot -o bplus.ps","w");
					pclose(pipe);	
					pipe=popen("evince bplus.ps","r"); 
					pclose(pipe);
					break;
			default: exit(1);
		}
	}
}

NODE sortAndInsert(NODE root,int entry)
{
	int i=0,j=0,k=0,t;
	NODE NN=getNode();
	// Count number of keys in the node and enter the new entry in blank space
	for(j=0;j<4;j++)
	{
		// For counting the keys
		if(root->key[j]!=0)
			i++;
		// Enter the new entry at the end
		if(root->key[j]==0)
		{
			root->key[j]=entry;
			i++;
			break;
		}
	}
	// Sort the node
	for(j=0;j<i;j++)
		for(k=j;k<i;k++)
			if(root->key[j] > root->key[k])
			{
				t=root->key[j];
				root->key[j]=root->key[k];
				root->key[k]=t;
			}
	return root;
}
NODE rootSort(NODE root,NODE child,int val)
{
	NODE t;
	int temp;
	
	if(val < root->key[0])
	{
		t=root->ptr[1];
		root->ptr[1]=child;
		temp=root->key[0];
		root->key[0]=val;
		root->ptr[4]=root->ptr[3];
		root->ptr[3]=root->ptr[2];
		root->ptr[2]=t;
		root->key[3]=root->key[2];
		root->key[2]=root->key[1];
		root->key[1]=temp;
	}
	else if(val < root->key[1] || root->key[1]==0)
	{
		root->ptr[4]=root->ptr[3];
		root->ptr[3]=root->ptr[2];
		root->ptr[2]=child;
		root->key[3]=root->key[2];
		root->key[2]=root->key[1];
		root->key[1]=val;
	}
	else if(val < root->key[2] | root->key[2]==0)
	{
		root->ptr[4]=root->ptr[3];
		root->ptr[3]=child;
		root->key[3]=root->key[2];
		root->key[2]=val;
	}
}
NODE insert(NODE root,int entry)
{
	int i=0;
	NODE NN,t;
	
	// If root is null
	if(root==NULL)
	{
		NN=getNode();
		NN->key[0]=entry;
		NN->parent=NULL;
		NN->level=1;
		return NN;
	}
	// If the node is leaf
	if(root->ptr[0]==NULL)
	{
		i=0;
		// For every entry in the node check
		while(i<4)
		{
			// If there is a space in the node
			if(root->key[i]==0)
			{
				// If there is a space, sort the entries and insert
				root=sortAndInsert(root,entry);
				return root;
			}
			i++;
		}
		// If there is no space in the node 
		NN=getNode();
		// Keep d entries and shift rest to new node
		// Check if new entry belongs to first or second node
		
		if(entry < root->key[1])
		{
			NN->key[1]=root->key[2];
			NN->key[2]=root->key[3];
			root->key[2]=root->key[3]=0;
			NN->key[0]=root->key[1];
			root->key[1]=0;
			root=sortAndInsert(root,entry);
		}
		else
		{
			NN->key[0]=root->key[2];
			NN->key[1]=root->key[3];
			root->key[2]=root->key[3]=0;
			NN=sortAndInsert(NN,entry);
		}
		// Set sibling pointers
		//NN->ptr[0]=root;
		//root->ptr[4]=NN;
		// New node's parent pointer is same as that of sibling
		NN->parent=root->parent;
		// New child
		child=NN;

		if(child->parent == NULL)
		{
			NN=getNode();
			NN->key[0]=child->key[0];
			//NN->key[1]=child->key[0];
			NN->ptr[0]=root;
			NN->ptr[1]=child;
			child->parent=root->parent=NN;
			NN->level=1;
			child->level=rand(); // right child
			root->level=rand();	// left child
			child=NULL;
			return NN;
		}
		child->level=rand(); // right child
		root->level=rand();	// left child
		return root;
	}
	// If root is not leaf
	// Find place for insertion
	if(entry < root->key[0])
		root->ptr[0]=insert(root->ptr[0],entry);
	else 
		for(i=1;i<4;i++)
		{
			// If entry is greater than key i-1 and less than key i, then proceed in that direction
			if((entry > root->key[i-1] && entry < root->key[i]) || root->key[i]==0)
			{
				root->ptr[i]=insert(root->ptr[i],entry);
				break;
			}
			if(i==3)
				root->ptr[i+1]=insert(root->ptr[i+1],entry);
			// If no entry in index, break
			if(root->key[i]==0)
				break;
		}
	// If there is no new child entry, return root
	if(child==NULL)
		return root;
	i=0;
	// Insert the new child entry in root if space.
	while(i<4)
	{
		// If there is a space in the root
		if(root->key[i]==0 && ind==0)
		{
			// If there is a space
			root=rootSort(root,child,child->key[0]);
			//root->key[i]=child->key[0];
			//root->ptr[i+1]=child;
			child=NULL;
			return root;
		}
		else if(root->key[i]==0 && ind==1)
		{
			// If there is a space
			root=rootSort(root,child,index_val);
			//root->key[i]=index_val;
			//root->ptr[i+1]=child;
			child=NULL;
			ind=0;
			return root;
		}
		i++;
	}
	// If no space in the root, split the root.
	NN=getNode();

	// Keep d entries and d+1 pointers. Split the rest.
	// If new child entry is less than key[1]
	if(child->key[0] < root->key[1])
	{
		NN->key[0]=root->key[2];
		NN->key[1]=root->key[3];
		NN->ptr[0]=root->ptr[2];
		NN->ptr[1]=root->ptr[3];
		NN->ptr[2]=root->ptr[4];
		ind=1;
		index_val=root->key[1];
		root->key[1]=child->key[0];
		root->ptr[2]=child;
		root->key[2]=root->key[3]=0;
		root->ptr[3]=root->ptr[4]=NULL;
		root->level=rand();
		child->level=rand();
		NN->parent=root->parent;
		child=NN;
	}
	else if(child->key[0] < root->key[2])
	{
		ind=1;
		index_val=child->key[0];
		NN->ptr[0]=child;
		NN->key[0]=root->key[2];
		NN->key[1]=root->key[3];
		NN->ptr[1]=root->ptr[3];
		NN->ptr[2]=root->ptr[4];
		root->key[2]=root->key[3]=0;
		root->ptr[3]=root->ptr[4]=NULL;
		root->level=rand();
		child->level=rand();
		NN->parent=root->parent;
		child=NN;
	}
	else if(child->key[0] < root->key[3])
	{
		ind=1;
		index_val=root->key[2];
		NN->key[0]=child->key[0];
		NN->ptr[0]=root->ptr[3];
		NN->key[1]=root->key[3];
		NN->ptr[1]=child;
		NN->ptr[2]=root->ptr[4];
		root->key[2]=root->key[3]=0;
		root->ptr[3]=root->ptr[4]=NULL;
		root->level=rand();
		child->level=rand();
		NN->parent=root->parent;
		child=NN;
	}
	else
	{
		ind=1;
		index_val=root->key[2];
		NN->key[0]=root->key[3];
		NN->ptr[0]=root->ptr[3];
		NN->key[1]=child->key[0];
		NN->ptr[1]=root->ptr[4];
		NN->ptr[2]=child;
		root->key[2]=root->key[3]=0;
		root->ptr[3]=root->ptr[4]=NULL;
		NN->parent=root->parent;
		root->level=rand();
		child->level=rand();
		child=NN;
	}
	if(root->parent == NULL) // If it is root of tree itself
	{
		NN=getNode();
		NN->ptr[0]=root;
		NN->key[0]=index_val;
		NN->ptr[1]=child;
		child->parent=NN;
		root->parent=NN;
		root->level=rand();
		child->level=rand();
		ind=0;
		child=NULL;
		NN->level=rand();
		NN->parent=NULL;

		return NN;
	}
	return root;
}

NODE getNode()
{
	int i;
	NODE NN;
	NN=(NODE)malloc(sizeof(struct node));
	if(NN==NULL)
		exit(1);
	for(i=0;i<4;i++)
		NN->key[i]=0;
	for(i=0;i<5;i++)
		NN->ptr[i]=NULL;
	NN->parent=NULL;
	return NN;
}