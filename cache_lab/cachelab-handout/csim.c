#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

typedef struct {
	int valid, block, tag, count;
}line;

typedef struct {
	line *lines;
}set;

typedef struct {
	set *sets;
}cache;

int hits, misses, evictions, localcount=0;
cache myCache;

void buildCache(int s, int E) {										// s = no.of set index bits, E = no.of lines in each set
	int S = 1<<s;													// S = no.of sets
	myCache.sets = (set *)malloc(sizeof(set)*S);					// allocating size for sets in myCache based on the no.of sets
	for(int i=0;i<S;i++){
		myCache.sets[i].lines = (line *)malloc(sizeof(line)*E);		// allocating size for lines in each set in myCache based on the no.of lines
		for(int j=0;j<E;j++){										// setting all the parameters in the each line of each set in myCache to 0
			myCache.sets[i].lines[j].valid = 0;
			myCache.sets[i].lines[j].block = 0;
			myCache.sets[i].lines[j].tag = 0;
			myCache.sets[i].lines[j].count = 0;
		}
	}
}

void function(long address, int s, int b, int E){
	int tagBits = address>>(b+s);
	int setBits = (address<<(64-b-s))>>(64-s);
	int i;
	localcount++;
	for(i=0;i<E;i++){
		if((myCache.sets[setBits].lines[i].tag==tagBits)&&(myCache.sets[setBits].lines[i].valid==1)){
			hits++;													// the if condition says that there is a hit
			myCache.sets[setBits].lines[i].count = localcount;		// the most recent access is being set for that particular block
			return;
		}
	}
	misses++;														// if there is no hit, it means that there is a miss

	int line=0, j, check=0x7fffffff;
	for(j=0;j<E;j++){											// check = least no.of times a block is been accessed, line = line number in that particular set
		if(myCache.sets[setBits].lines[j].count<check){
			check=myCache.sets[setBits].lines[j].count;
			line=j;
		}
	}

	if(myCache.sets[setBits].lines[line].valid==1){					// if there is a requirement of replacing the least accessed bit, then evictions has to be incremented
		evictions++;
	}

	myCache.sets[i].lines[j].valid = 1;
	myCache.sets[i].lines[j].tag = tagBits;
	myCache.sets[i].lines[j].count = localcount;
}

void readfile(char *filename, int s, int b, int E){
	FILE *fp;
	fp = fopen(filename,"r");
	char identifier;
	long address;
	int size;
	while(fscanf(fp," %c %lx,%d",&identifier,&address,&size)>0){
		switch(identifier){
			case 'L':
				function(address,s,b,E);
				break;
			case 'M':
				function(address,s,b,E);
				function(address,s,b,E);
				break;
			case 'S':
				function(address,s,b,E);
				break;
			
		}
	}
}


int main(int argc, char** argv)
{
	int opt, s, E, b;
	char* t;
	while((opt=getopt(argc,argv,"s:E:b:t:"))!=-1){
		switch(opt){
			case 's':
				s=atoi(optarg);printf("s=%d\n",s);break;

			case 'E':
				E=atoi(optarg);printf("E=%d\n",E);break;

			case 'b':
				b=atoi(optarg);printf("b=%d\n",b);break;

			case 't':
				t=optarg;printf("t=%s\n",t);break;

			default:
				printf("wrong argument");break;
		}
	}
	buildCache(s,E);
	readfile(t,s,b,E);
	printSummary(hits,misses,evictions);
	// printf("%d %d %d",hits,misses,evictions);
	return 0;
}

// int xyz(){
// 	int s=1, b=1, E=1;
// 	char* t="traces/yi2.trace";
//	buildCache(s,E);
//	readfile(t,s,b,E);
//	printSummary(hits,misses,evictions);
	
//}
