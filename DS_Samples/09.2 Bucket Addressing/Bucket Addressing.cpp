﻿//
// HASHING with BUCKET ADDRESSING
//

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

FILE *dic, *rel, *opt;
char c;
int Adres;
int Bucket;
int Temp;
int nDic;
int nRel;
int oBucket = 0;
char sorgu[12];

struct word
{
	char ingilizce[12];
	char turkce[14];
}kelime;

int Hash (char* key)  // R(KEY) fonksiyonu
{
	int sum = 0;
	for (int j=0; j<4; j += 2)
		sum = (sum + 10*key[j] + key[j+1]);

	sum = sum % nRel ;  
	return sum;
}

bool isPrime(int n) 
{
    if (n <= 1)	return false;
    if (n == 2) return true;

    for (int i = 2; i <= sqrt((double)n) + 1; i++) 
	{
        if (n % i == 0) return false;
    }
    return true;
}

int primeOver(int x) 
{
	while(!isPrime(x)) x++;
	return x;
}

int sizeofRel() 
{
	dic = fopen("dictionary.txt", "r");

	nDic = 0;
	while(!feof(dic))
	{
		fscanf(dic, "%s", &kelime.ingilizce);
		fscanf(dic, "%s", &kelime.turkce);	
		nDic++;
	}
	fclose(dic);

	// %75 doluluk oranı için prime nRel
	nRel = (int)((100 * nDic)/ (float)75);
	nRel = primeOver(nRel);

	return nRel;
}

void sizeofBucket() 
{
	dic = fopen("dictionary.txt", "r");

	int* bucketSize = new int[nRel];
	for(int i=0; i<nRel; i++) bucketSize[i]=0;

	while(!feof(dic))
	{
		fscanf(dic, "%s", &kelime.ingilizce);
		fscanf(dic, "%s", &kelime.turkce);	
		Adres = Hash(kelime.ingilizce);
		bucketSize[Adres] += 1;
	}
	fclose(dic);
	
	printf(" \n"); printf(" \n");

	for (int i = 0; i < nRel; i++)
		printf("bukcetSize[%d] = %d\n", i, bucketSize[i]);

	// find optimum bucket size
	oBucket = 0;
	for(int i=0; i<nRel; i++)
		if(bucketSize[i]>oBucket) oBucket = bucketSize[i];
}

void RelativeOlustur()
{
	nRel = sizeofRel();

	sizeofBucket();
	printf("\nOptimum bucket size : %d", oBucket);

	dic = fopen("dictionary.txt", "r");
	rel = fopen("relative.txt", "w+");	

	for(int i=0; i<nRel*oBucket; i++)
	{
		fseek(rel,i*sizeof(kelime),0);
		fputc('*', rel );
	}

	printf("\n\n");

	while(!feof(dic))
	{
		fscanf(dic, "%s", &kelime.ingilizce);
		fscanf(dic, "%s", &kelime.turkce);	

		Bucket = Hash(kelime.ingilizce);	
		fseek(rel,sizeof(kelime)*Bucket*oBucket,0);		

		Adres=Bucket*oBucket;
		Temp =Bucket*oBucket;
		c=fgetc(rel);
		while( c != '*')
		{
			Adres = (Adres + 1) % (nRel*oBucket) ;
			if(Adres==Temp)
			{
				printf("DOSYA DOLU ! \n");
				return;
			}

			fseek(rel,Adres*sizeof(kelime),0);
			c=fgetc(rel);
		}

		printf("Bucket = %d \t Kelime = %s\n", Bucket, kelime.ingilizce);
		//printf("%d\n", Bucket);

		fseek(rel,Adres*sizeof(kelime),0); 
		fwrite(&kelime,sizeof(kelime),1,rel);

		strcpy(kelime.ingilizce, "            ");
		strcpy(kelime.turkce, "              ");
	}
	fcloseall();

	printf("\n\n BAGIL DOSYA OLUSTURULDU ");
	printf("\n\n Press any key... ");

	getch();
}

void kelimeSorgula()
{
	rel = fopen("relative.txt", "r");

	char test;

	while(true)
	{
		system("cls");
		printf("\n\n Sorgulamak istediginiz kelimeyi giriniz... = ");
		scanf("%s",sorgu);

		Bucket = Hash(kelime.ingilizce);	
		fseek(rel,sizeof(kelime)*Bucket*oBucket,0);	
		fread(&kelime,sizeof(kelime),1,rel);		
		
		Adres=Bucket*oBucket;
		Temp =Bucket*oBucket;
		int compare = strcmp(sorgu,kelime.ingilizce); // aynı iseler strcmp 0 döndürür
		while (compare != 0)
		{
			Adres = (Adres + 1) % (nRel*oBucket) ;
			if(Adres==Temp)
			{
				printf("\n KELIME DOSYADA YOK ! \n");
				printf(" Press any key... ");
				getch();
				return;
			}

			fseek(rel,Adres*sizeof(kelime),0);
			fread(&kelime,sizeof(kelime),1,rel);
			compare = strcmp(sorgu,kelime.ingilizce);
		}	

		fseek(rel,Adres*sizeof(kelime),0); 
		fread(&kelime, sizeof(kelime), 1, rel);
		
		system("cls");
		printf("\n %s kelimesinin Turkce'si %s 'dir \n", kelime.ingilizce, kelime.turkce);
		printf("\n\n Baska bir kelime sorgulamak ister misiniz? (Y or N) = ");
		scanf(" %c", &test );
		if(tolower(test) == 'n')      break;		
	}
	fclose(rel);
}

void kelimeEkle()
{
	rel = fopen("relative.txt", "r+");

	char test;

	while(true)
	{
		system("cls");
		printf("\n Eklemek istediginiz kelimenin Ingilizce'sini giriniz... = ");
		scanf("%s", &kelime.ingilizce);

		printf("\n Eklemek istediginiz kelimenin Turkce'sini giriniz... = ");
		scanf("%s", &kelime.turkce);

		Bucket = Hash(kelime.ingilizce);	
		fseek(rel,sizeof(kelime)*Bucket*oBucket,0);	
		
		Adres=Bucket*oBucket;
		Temp =Bucket*oBucket;
		c=fgetc(rel);
		while( c != '*')
		{
			Adres = (Adres + 1) % (nRel*oBucket) ;
			
			if(Adres==Temp)
			{
				printf("DOSYA DOLU ! \n");
				printf(" Press any key... ");
				return;
			}

			fseek(rel,Adres*sizeof(kelime),0);
			c=fgetc(rel);
		}

		fseek(rel,Adres*sizeof(kelime),0); 
		fwrite(&kelime,sizeof(kelime),1,rel);

		system("cls");
		printf("\n %s KELIMESI BAGIL DOSYAYA EKLENMISTIR \n", kelime.ingilizce);
		printf("\n\n Baska bir kelime eklemek ister misiniz? (Y or N) = ");
		scanf(" %c", &test );
		if(tolower(test) == 'n')      break;
	}
	fclose(rel);
}

void kelimeSil()
{
	rel = fopen("relative.txt", "r+");

	char test;

	while(true)
	{
		system("cls");
		printf("\n\n Silmek istediginiz kelimeyi giriniz... = ");
		scanf("%s", &sorgu);

		Bucket = Hash(kelime.ingilizce);	
		fseek(rel,sizeof(kelime)*Bucket*oBucket,0);	
		
		Adres=Bucket*oBucket;
		Temp =Bucket*oBucket;
		int compare = strcmp(sorgu,kelime.ingilizce);
		while (compare != 0)
		{
			Adres = (Adres + 1) % (nRel*oBucket) ;
			if(Adres==Temp)
			{
				printf("\n KELIME DOSYADA YOK ! \n");
				printf(" Press any key... ");
				getch();
				return;
			}

			fseek(rel,Adres*sizeof(kelime),0);
			fread(&kelime,sizeof(kelime),1,rel);
			compare = strcmp(sorgu,kelime.ingilizce);
		}	

		fseek(rel,Adres*sizeof(kelime),0); 		
		fprintf(rel, "*                         ");

		system("cls");
		printf("\n %s KELIMESI BAGIL DOSYADAN SILINMISTIR \n", kelime.ingilizce, kelime.turkce);
		printf("\n\n Baska bir kelime silmek ister misiniz? (Y or N) = ");
		scanf(" %c", &test );
		if(tolower(test) == 'n')      break;		
	}
	fclose(rel);
}

void printRelative_txt()
{
	system("cls");

	printf("\n");

	rel = fopen("relative.txt", "r");

	for (int i = 0; i < nRel; i++)
	{
		printf(" Bucket = %d : ", i);

		for (int j = 0; j < oBucket; j++)
		{
			fread(&kelime, sizeof(kelime), 1, rel);
			printf("%s %s\t", kelime.ingilizce, kelime.turkce);

			strcpy(kelime.ingilizce, "            ");
			strcpy(kelime.turkce, "              ");
		}
		printf("\n");
	}
	fclose(rel);

	printf("\n Press any key... ");
	getchar();
}

void main(int argc, char* argv[])
{
	sizeofRel();
	sizeofBucket();

	char s;

	strcpy(kelime.ingilizce, "            ");
	strcpy(kelime.turkce, "              ");

	do
	{
		system("cls");

		printf(" \n");
		printf(" HASHING with BUCKET ADDRESSING \n\n");
		printf(" [0] BAGIL DOSYA OLUSTUR \n");
		printf(" [1] BAGIL DOSYAYI GOSTER \n");
		printf(" [2] KELIME SORGULA \n");
		printf(" [3] KELIME EKLE\n");
		printf(" [4] KELIME SIL \n");
		printf(" [5] PROGRAMDAN CIK \n");
		printf(" \n SECIMINIZ [0..5] = ");

		s=getch();

		switch(s)
		{
			case '0' :	RelativeOlustur();		break;
			case '1':	printRelative_txt();	break;
			case '2' :	kelimeSorgula();		break;
			case '3' :	kelimeEkle();			break;
			case '4' :	kelimeSil();			break;
			case '5' :	return ;
		}
		
	}  while(1);
}