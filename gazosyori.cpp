#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>

#define SIZEX 2048//入力画像の大きさX
#define SIZEY 1366//入力画像のの大きさY
#define M_PI 3.14159265358979323846



//ヘッダーデータ
typedef struct{
	int fsize;
	short Reservation1;
	short Reservation2;
	int fsize2;
	int infosize;
	int sizex;
	int sizey;
	short plane;
	short scale;
	int compact1;
	int compact2;
	int Xpxperm;
	int Ypxperm;
	int used;
	int important;
}Header;

//bmpファイルのヘッダーデータを挿入する
void BMPheader(unsigned char *dst){
	Header *hed;
	
	*dst='B';
	*(dst+1)='M';
	hed = (Header *)(dst+2);
	
		
		hed->fsize=54+(3*SIZEX+3)*SIZEY;
		hed->Reservation1=0;
		hed->Reservation2=0;
		hed->fsize2=54;
		hed->infosize=40;
		hed->sizex=SIZEX;
		hed->sizey=SIZEY;
		hed->plane=1;
		hed->scale=24;
		hed->compact1=0;
		hed->compact2=(3*SIZEX+3)*SIZEY;
		hed->Xpxperm=2834;
		hed->Ypxperm=2834;
		hed->used=0;
		hed->important=0;
	
	
}

//
void bmp2raw( unsigned char *src,unsigned char *dst, int sizeX, int sizeY)
{
	int i,j;
	for(j=0;j<sizeY;j++)
		for(i=0;i<sizeX;i++)
		{
			*(dst+3*sizeX*j+3*i+0)=*(src+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+0);
			*(dst+3*sizeX*j+3*i+1)=*(src+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+1);
			*(dst+3*sizeX*j+3*i+2)=*(src+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+2);
		}

}
//グレースケール化
int Monolize(unsigned char *src, unsigned char *dst, int X, int Y)
{
	//	使用する変数を宣言及び定義
	int i, j;
	int r, g, b;
	int val;

	//	エラー処理対策
	if( (X > SIZEX) || (Y > SIZEY) )return -1;

	for(j=0; j<Y; j++)
		for(i=0; i<X; i++)
		{
			r=(*(src+3*X*j+3*i+0));
			g=(*(src+3*X*j+3*i+1));
			b=(*(src+3*X*j+3*i+2));

			val=(int)(0.3*r+0.59*g+0.11*b);
			*(dst+X*j+i)=(unsigned char)val;
		}

	return 0;
}
//平均値フィルタ
int Average(unsigned char *src, unsigned char *dst, int X, int Y)
{
	//	使用する変数を宣言及び定義
	int i, j,k,l,m;
	int r, g, b;
	double r_ave=0,g_ave=0,b_ave=0;
	m=0;

	//	エラー処理対策
	if( (X > SIZEX) || (Y > SIZEY) )return -1;

	for(j=0; j<Y; j++)
		for(i=0; i<X; i++)
		{
			for(k=-1; k<=1; k++)
			{
				for(l=-1; l<=1; l++)
				{
					if(i+l>=X||i+l<0||j+k>=Y||j+k<0){}
					else{
						r_ave+=(*(src+3*X*(j+k)+3*(i+l)+0));
						g_ave+=(*(src+3*X*(j+k)+3*(i+l)+1));
						b_ave+=(*(src+3*X*(j+k)+3*(i+l)+2));
						m++;
					}
				}
			}//9pix

			*(dst+3*X*j+3*i+0)=r_ave/m;
			*(dst+3*X*j+3*i+1)=g_ave/m;
			*(dst+3*X*j+3*i+2)=b_ave/m;
			r_ave=0;g_ave=0;b_ave=0;m=0;
			//val=(int)(0.3*r+0.59*g+0.11*b);
			//*(dst+X*j+i)=(unsigned char)val;
		}

	return 0;
}

//ガウシアンフィルタ
int Gauss(unsigned char *src, unsigned char *dst, int X, int Y)
{
	//	使用する変数を宣言及び定義
	int i, j,k,l,m;
	int r, g, b;
	double r_ave=0,g_ave=0,b_ave=0;
	m=0;

	//	エラー処理対策
	if( (X > SIZEX) || (Y > SIZEY) )return -1;

	for(j=0; j<Y; j++)
		for(i=0; i<X; i++)
		{
			for(k=-1; k<2; k++)
			{
				for(l=-1; l<2; l++)
				{
					if(i+l>=X||i+l<0||j+k>=Y||j+k<0){}
					else if((l==-1&&k==-1)||(l==-1&&k==1)||(l==1&&k==-1)||(l==1&&k==1)){				
						r_ave+=(*(src+3*X*(j+k)+3*(i+l)+0));
						g_ave+=(*(src+3*X*(j+k)+3*(i+l)+1));
						b_ave+=(*(src+3*X*(j+k)+3*(i+l)+2));
						m++;
						//1
					}else if((l==-1&&k==0)||(l==0&&k==1)||(l==0&&k==-1)||(l==1&&k==0)){
						r_ave+=2*(*(src+3*X*(j+k)+3*(i+l)+0));
						g_ave+=2*(*(src+3*X*(j+k)+3*(i+l)+1));
						b_ave+=2*(*(src+3*X*(j+k)+3*(i+l)+2));
						m=m+2;
						//2
					}else{
						r_ave+=4*(*(src+3*X*(j+k)+3*(i+l)+0));
						g_ave+=4*(*(src+3*X*(j+k)+3*(i+l)+1));
						b_ave+=4*(*(src+3*X*(j+k)+3*(i+l)+2));
						m=m+4;
					}
				}
			}//9pix

			*(dst+3*X*j+3*i+0)=r_ave/m;
			*(dst+3*X*j+3*i+1)=g_ave/m;
			*(dst+3*X*j+3*i+2)=b_ave/m;
			r_ave=0;g_ave=0;b_ave=0;m=0;
			//val=(int)(0.3*r+0.59*g+0.11*b);
			//*(dst+X*j+i)=(unsigned char)val;
		}

	return 0;
}
//ミディアムフィルタ
int Mid(unsigned char *src, unsigned char *dst, int X, int Y)
{
	//	使用する変数を宣言及び定義
	int i, j,k,l,m,n=0;
	int r, g, b;
	double r_ave=0,g_ave=0,b_ave=0;
	int val[9]={255,255,255,255,255,255,255,255,255},temp;
	m=0;

	//	エラー処理対策
	if( (X > SIZEX) || (Y > SIZEY) )return -1;

	for(j=0; j<Y; j++)
		for(i=0; i<X; i++)
		{
			for(k=-1; k<=1; k++)
			{
				for(l=-1; l<=1; l++)
				{
					if(i+l>=X||i+l<0||j+k>=Y||j+k<0){}
					else{
						val[n]=(*(src+X*(j+k)+(i+l)));
						n++;
					}
				}
			}//9pix
			for(m=0;m<n;m++){//ソート
				if(!(m-1<0)){
					if(val[m-1]>val[m]){
						temp=val[m];
						val[m]=val[m-1];
						val[m-1]=temp;
					}
				}
				if(!(m+1>8)){
					if(val[m+1]<val[m]){
						temp=val[m];
						val[m]=val[m+1];
						val[m+1]=temp;
					}
				}
			}
			*(dst+X*j+i+0)=val[(int)(n/2)];
			for(m=0;m<9;m++){
				val[m]=255;
			}
			m=0;n=0;
			//val=(int)(0.3*r+0.59*g+0.11*b);
			//*(dst+X*j+i)=(unsigned char)val;
		}

	return 0;
}
//大津二値化
int Otu_Binalization(unsigned char *src, unsigned char *dst, int X, int Y)
{
	//	使用する変数を宣言及び定義
	int i, j,k=0;
	int pix=0;
	double val,average=0;
	int max=128,min=128;
	int hist[256]={};
	int T,Threshold;
	int pix1=0,pix2=0;
	double n1=0,n2=0;
	double mu1=0.0,mu2=0.0,s=0.0;
	double good_T[2]={};//閾値,分散
	//平均とヒストグラムを求める
	for(j=0; j<Y; j++){
		for(i=0; i<X; i++)
		{
			pix=(*(src+X*j+i));
			hist[pix]=hist[pix]+1;
			average+=(int)pix;
			//k=k+1;
		}
	}
	
	average=average/(X*Y);//平均値算出
	max=average+1;min=average;
	for(j=0; j<256; j++){
		if(hist[j]>0){
			if(j>max){
				max=j;
			}else if(j<min){
				min=j;
			}
		}
		printf("%d %d\n",j,hist[j]);	
	}
	//printf("%d %d\n",max,min);
	//閾値算出
	//最小画素から最大画素まで試す	
	for(j=min; j<=max; j++){
		T=j;
		for(i=min; i<=max; i++){
			if(i<T){
				pix1=pix1+i*hist[i];n1=n1+hist[i];
			}else{
				pix2=pix2+i*hist[i];n2=n2+hist[i];
			}
		}
		if(n1==0)mu1=0;
		else mu1=(double)(pix1/n1);
		if(n2==0)mu2=0;
		else mu2=(double)(pix2/n2);
		
		s=(n1/(double)(X*Y))*(n2/(double)(X*Y))*(mu1-mu2)*(mu1-mu2);
		printf("s %f mu %f %f n %f %f pix %d %d \n",s,mu1,mu2,n1,n2,pix1,pix2);
		if(good_T[1]<s){
			good_T[0]=j;
			good_T[1]=s;
			printf("threhold %f\n",good_T[1]);
		}
		pix1=0;pix2=0;n1=0;n2=0;	
	}
	Threshold=(int)good_T[0];
	printf("last %d\n",Threshold);
	for(j=0; j<Y; j++){
		for(i=0; i<X; i++)
		{
			pix=(*(src+X*j+i));
			if(Threshold>pix)val=0;
			else val=255;
			*(dst+X*j+i)=(unsigned char)val;
		}
	}
	return 0;
}

//グレースケール画像の平均値を閾値とし二値化する
int binalize(unsigned char *src, unsigned char *dst, int X, int Y)
{
	//	使用する変数を宣言及び定義
	int i, j,k=0;
	int pix=0;
	int val,average=0;

	for(j=0; j<Y; j++){
		for(i=0; i<X; i++)
		{
			pix=(*(src+X*j+i));

			average+=(int)pix;
			//k=k+1;
		}
	}
	
	average=average/(X*Y);
	for(j=0; j<Y; j++){
		for(i=0; i<X; i++)
		{
			pix=(*(src+X*j+i));
			if(average>pix)val=0;
			else val=255;

			*(dst+X*j+i)=(unsigned char)val;
		}
	}
	return 0;
}

/*
*グレースケールの配列をbmpファイル用の配列に格納する。
*/
void raw2bmpmono( unsigned char *src,unsigned char *dst, int sizeX, int sizeY)
{
	int i,j;
	for(j=0;j<sizeY;j++)
		for(i=0;i<sizeX;i++)
		{
			*(dst+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+2)=*(src+sizeX*j+i);
			*(dst+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+1)=*(src+sizeX*j+i);
			*(dst+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+0)=*(src+sizeX*j+i);
		}

}
void raw2bmp( unsigned char *src,unsigned char *dst, int sizeX, int sizeY)
{
	int i,j;
	for(j=0;j<sizeY;j++)
		for(i=0;i<sizeX;i++)
		{
			*(dst+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+0)=*(src+3*sizeX*j+3*i+0);
			*(dst+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+1)=*(src+3*sizeX*j+3*i+1);
			*(dst+40+(3*sizeX+3)/4*4*(sizeY-j-1)+3*i+2)=*(src+3*sizeX*j+3*i+2);
		}

}

int main(void)
{
	//	使用する変数を宣言及び定義
	int X=SIZEX, Y=SIZEY, color,suc;
	unsigned char *src,*src1,*src2,*src3, *dst;
	unsigned char header[54];
	FILE *fp;
	//領域の確保
	src=(unsigned char *)malloc( (54+(3*X+3)*Y)*sizeof(unsigned char));
	src1=(unsigned char *)malloc( (3*X*Y)*sizeof(unsigned char));
	src2=(unsigned char *)malloc( (X*Y)*sizeof(unsigned char));
	src3=(unsigned char *)malloc( (X*Y)*sizeof(unsigned char));

	//ファイルの読み込み
	fp=fopen("10333755_10207430436421932_7757327680141530366_o.bmp", "rb");
	fread(src, 54+(3*SIZEX+3)*SIZEY, sizeof(unsigned char), fp);
	fclose(fp);
	//ヘッダー情報を取り除く
	bmp2raw(src+14, src1, X, Y);//ok
	free(src);
	printf("bmpraw_program_fin\n");
	suc=Monolize(src1,src2,X,Y);

	suc=Mid(src2,src3,X,Y);
	free(src1);
	free(src2);
	printf("mid_fin\n");
	dst=(unsigned char *)malloc((54+(3*SIZEX+3)*SIZEY)*sizeof(unsigned char));
	BMPheader(dst);
	raw2bmpmono(src3, dst+14, SIZEX, SIZEY);

	fp=fopen("mid25.bmp", "wb");
	fwrite(dst, 54+(3*SIZEX+3)*SIZEY, sizeof(unsigned char), fp);
	fclose(fp);

	//	領域の解放
	free(src3);
	free(dst);
	printf("program_fin\n");
	return 0;
}
