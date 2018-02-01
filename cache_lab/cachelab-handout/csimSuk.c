// Vo Tran Thanh Luong - 1551020
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <math.h>
#include "cachelab.h"

typedef unsigned long long int address;


typedef struct{
	int s; 
	int b; 
	int E; 
	int S; 
	int B; 
	int hits;
	int misses;
	int evictions;
}chacheparameters;

typedef struct{
	int count;
	int valid;
	address tag;
	char *block;
}line;

typedef struct{
	line *lines;
}set; 

typedef struct{
	set *sets;
}cache;

cache buildCache(chacheparameters cp) {
	int S = 1<<cp.s, E = cp.E;
	cache myCache;	 
	set myset;
	line myline;
	int i;		// index of set
	int l;		// index of line

	myCache.sets = (set *) malloc(sizeof(set) * S);

	for(i=0;i<S;i++){
		
		myset.lines =  (line *) malloc(sizeof(line) * E);
		myCache.sets[i] = myset;

		for(l=0;l<E;l++){
			myline.count = 0;
			myline.valid = 0;
			myline.tag = 0; 
			myset.lines[l] = myline;	
		}
		
	} 

	return myCache;
	
}

void clean(cache myCache, chacheparameters cp){
	int S = 1<<cp.S;
	for(int i=0;i<S;i++){
		set myset = myCache.sets[i];
		if(myset.lines!=NULL){
			free(myset.lines);
		}
	}

	if(myCache.sets!=NULL){
		free(myCache.sets);
	}
}

chacheparameters accessCache(cache myCache, chacheparameters cp, address addr){
		int l;
		int checkCacheFull = 1;
		int E = cp.E;
		int tagSize = (64 - (cp.s + cp.b));
		address inputTag = addr >> (cp.s + cp.b);
		address setIndex = addr << (tagSize) >> (tagSize + cp.b);
		set myset = myCache.sets[setIndex];

		for(l=0;l<E;l++){	
			if (myset.lines[l].valid) {
				if (myset.lines[l].tag == inputTag) {
					myset.lines[l].count ++;
					cp.hits ++;
					return cp;
				}
			}
		}
		cp.misses++;

		int leastAccessedInstant = myset.lines[0].count, leastAccessedLine = 0, mostAccessedInstant = myset.lines[0].count;
	
		for(int i=0;i<cp.E;i++){
		if(leastAccessedInstant > myset.lines[i].count){
			leastAccessedInstant = myset.lines[i].count;
			leastAccessedLine = i;
		}
		if(mostAccessedInstant < myset.lines[i].count){
			mostAccessedInstant = myset.lines[i].count;
		}
		if(myset.lines[i].valid==0){checkCacheFull=0;}
	}
	
	if(checkCacheFull){
		cp.evictions++;
		myset.lines[leastAccessedLine].tag = inputTag;
		myset.lines[leastAccessedLine].count = mostAccessedInstant+1;
	}
	else{
		int emptyLinenumber=0;
		for(int i=0;i<cp.E;i++){
			if(myset.lines[i].valid==0){emptyLinenumber=i;break;}
		}

		myset.lines[emptyLinenumber].tag = inputTag;
		myset.lines[emptyLinenumber].valid = 1;
		myset.lines[emptyLinenumber].count = mostAccessedInstant+1;
	}

	return cp;
}

int main(int argc, char **argv)
{
	
	cache myCache;
	chacheparameters cp;
	bzero(&cp, sizeof(cp));
	// long long S;
	// long long B;	// B = size of block
	FILE *openTrace;
	char instructionInTraceFile;
	address addr;
	int size;
	char *trace_file;
	char c;
	while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1)
	{
		switch(c)
		{
		case 's':
			cp.s = atoi(optarg);
			break;
		case 'E':
			cp.E = atoi(optarg);
			break;
		case 'b':
			cp.b = atoi(optarg);
			break;
		case 't':
			trace_file = optarg;
			break;
		case 'h':
			exit(0);
		default:
			exit(1);
		}
	}

	// long long S = pow(2.0, cp.s);
	// long long B = pow(2.0, cp.b);
	cp.hits = 0;
	cp.misses = 0;
	cp.evictions = 0;
	myCache = buildCache(cp);

	openTrace  = fopen(trace_file, "r");
	if (openTrace != NULL) {
		while (fscanf(openTrace, " %c %llx,%d", &instructionInTraceFile, &addr, &size) == 3) {
			switch(instructionInTraceFile) {
				case 'I':
					break;
				case 'L':
					cp = accessCache(myCache, cp, addr);
					break;
				case 'S':
					cp = accessCache(myCache, cp, addr);
					break;
				case 'M':
					cp = accessCache(myCache, cp, addr);
					cp = accessCache(myCache, cp, addr);	
					break;
				default:
					break;
			}
		}
	}
	printSummary(cp.hits, cp.misses, cp.evictions);
	clean(myCache,cp);
	fclose(openTrace);
	return 0;
}
