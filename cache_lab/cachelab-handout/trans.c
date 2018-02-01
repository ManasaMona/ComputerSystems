#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

char transpose_submit_desc[] = "Transpose submission";

void transpose_submit(int M, int N, int A[N][M], int B[M][N]){
	int i; 
	int j; 
	int r; 
	int c;
	int diagonal = 0;
	int temp = 0;

	if (N == 32) {
		for(c=0;c<N;c+=8){
			for(r=0;r<N;r+=8){
				for(i=r;i<r+8;i++){
					for(j=c;j<c+8;j++){
						if(i!=j){
							B[j][i]=A[i][j];
						}else{
							temp=A[i][j];
							diagonal=i;
						}
					}
					if(r==c){
						B[diagonal][diagonal]=temp;
					}
				}	
			}
		}
	}else if(N==64){
		for(c=0;c<N;c+=4){
			for(r=0;r<N;r+=4){
				for(i=r;i<r+4;i++){
					for(j=c;j<c+4;j++){
						if(i!=j){
							B[j][i]=A[i][j];
						}else{
							temp=A[i][j];
							diagonal=i;
						}
					}
					if(r==c){
						B[diagonal][diagonal]=temp;
					}
				}	
			}
		}
	}
	else{
		for(c=0;c<M;c+=16){
			for(r=0;r<N;r+=16){		
				for(i=r;(i<r+16)&&(i<N);i++){
					for(j=c;(j<c+16)&&(j<M);j++){
						if(i!=j){
							B[j][i]=A[i][j];
						}else{
							temp=A[i][j];
							diagonal=i;
						}
					}
					if(r==c){
						B[diagonal][diagonal]=temp;
					}
				}
			}
		}
	}
}

void registerFunctions()
{
	registerTransFunction(transpose_submit, transpose_submit_desc); 
}

int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
	int i, j;
	for(i=0;i<N;i++){
		for(j=0;j<M;++j){
			if(A[i][j]!=B[j][i]){
				return 0;
			}
		}
	}
	return 1;
}
