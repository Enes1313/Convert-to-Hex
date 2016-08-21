#include <stdio.h>
#include <stdlib.h>

#define FILE_SIZE (1024*1024*1024*4U-1)
/* Gerçekte dosya boyutu (1024*1024*1024*1024*1024*1024*16llu-1)  yani 16777216TB altýndakiler için geçerli.
Lakin raminiz kadar açabilirsiniz. Boþ ram kadar. Bu sebeple 4 GB rami olanlar 10 GB lýk bir dosya açamsýnlar :D
Öylesine 4GB sýnýr koyuyorum :) Allta if li komutta.
*/
#define SATIR_SAYISI 16
#define SATIR_BYTE_SAYISI 16
#define TOPLAM_BYTE_SAYISI (SATIR_BYTE_SAYISI*SATIR_SAYISI)
/* Bir sayfada gösterilecek byte sayýsý. Önerilen SATIR_BYTE_SAYISI*SATIR_SAYISI 16*16 dýr. 
Þuanlýk kesinlikle SATIR_BYTE_SAYISI nýn katlarý þeklinde olmalý. 
Sonraki versiyonlarda deðiþik haller eklenir ki gereði yok aslýnda.
*/

#ifdef _WIN32
	#include<windows.h>
	#include <conio.h>
	
    #define TeMiZ system("CLS")
    #define YON_TUSU 224
    #define YON_TUSU_UP 72
    #define YON_TUSU_DOWN 80
    #define YON_TUSU_RIGHT 77
    #define YON_TUSU_LEFT 75

    void gotoxy(short x, short y)
	{
    	COORD Cursor_Pos = {x, y};
    	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cursor_Pos);
	}
#elif __linux
	#include <termios.h>
	#include <unistd.h>
	
    #define TeMiZ do { system("clear"); tigetstr( "clear" ); } while(false)
    #define YON_TUSU 27
    #define YON_TUSU 224
    #define YON_TUSU_UP 65
    #define YON_TUSU_DOWN 66
    #define YON_TUSU_RIGHT 67
    #define YON_TUSU_LEFT 68
    
	void gotoxy(short x,short y)
	{
 		printf("\x1B[%d;%df",y,x);
	}

	int getch(void)
	{
		struct termios oldattr, newattr;
		int ch;
		tcgetattr( STDIN_FILENO, &oldattr );
		newattr = oldattr;
		newattr.c_lflag &= ~( ICANON | ECHO );
		tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
		ch = getchar();
		tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
		return ch;
	}
#else
    #error "Valla artýk uyumluluðu sen yap :D (gotoxy,getch,defineler)"
#endif


FILE *dosya;

char j;
unsigned char *icerik;
char dosya_yolu[PATH_MAX+1]={0};
unsigned long int say, dosya_boyutu;

void HexeGit();
void YeniHex(short, short);

int main()
{
	int i;
	unsigned long int toplam_sayfa, s, sol, sag;	// 2 üzeri 64 den 16777216 TB a kadar daha doðrusu ram in ne kadarsa her programý açar.
	
	printf("HexNS v1.3!\n\nDosya yolu girin D:\\Dosya\\yolu\\naber.exe gibi\n"
	"Olmaz ise : D:\\\\Dosya\\\\yolu\\\\naber.exe \nO da olmaz ise Run as Administrator %c %c\n\n"
	"KULLANIM:\nDosya yolu girdikten sonraki ekran bilgilerdir.\n"
	"Y%cn tu%clar%c ile hareket ettirebilirsiniz. \nBo%cluk (space)' e basarak yeni de%cer girebilirsiniz.\n"
	"Enter'a basarak onaylan%cr. + ileriki sayfa - geri sayfa q bitirir ve kaydeder.\n\n\n",1,2,148,159,141,159,167,141);

	do{
		printf("Dosya yolu girin : ");
		gets(dosya_yolu);
		
		if((dosya=fopen(dosya_yolu,"rb"))==NULL)
			puts("\nTekrar gir iyi yaz!\n");
	}while(dosya==NULL);
	
	fseek(dosya, 0, SEEK_END);
   	dosya_boyutu = ftell(dosya);
    fseek(dosya, 0, SEEK_SET);
    
	icerik=(unsigned char *)malloc(dosya_boyutu*sizeof(char));
	fread(icerik,sizeof(char),dosya_boyutu,dosya);	
	fclose(dosya);
	// unsigned long int ile tüm dosyalar açýlabilir. yeterki ram olsun. Ram kontrolü de yapýp parça parça okutabilme özelliði sonra eklenir.
	if(FILE_SIZE<=dosya_boyutu)
	{
		puts("\nBu dosya fazla kusura bakma :D ");
		free(icerik);
		getch();
		return 0;
	}
	
	toplam_sayfa=dosya_boyutu/TOPLAM_BYTE_SAYISI+!!(dosya_boyutu%TOPLAM_BYTE_SAYISI);
	
	printf("\nDosya boyutu\t\t\t:~%.2f Mb\nToplam sayfa\t\t\t: %d\nBir sayfada olacak toplam byte\t: %d\n",(float)dosya_boyutu/1024/1024,toplam_sayfa,TOPLAM_BYTE_SAYISI);
	printf("\nDevam etmek istiyorsan entera bas :D");
	
	while(getch()!=13);
	
	TeMiZ;
	
	for(say=0;;say++)								//Aslýnda gotoxy ile daha rahat yapýlabilirdi lakin böyle yapmayý istedi caným :D
	{
		j=(say==dosya_boyutu);

		if(say%SATIR_BYTE_SAYISI==0 || j)
		{
			for(i=0;i<3*SATIR_BYTE_SAYISI*(!!(dosya_boyutu%SATIR_BYTE_SAYISI))-3*(dosya_boyutu%SATIR_BYTE_SAYISI) && j;i++)	
				printf(" ");												// Son bytler için boþluklar
			printf("  ");
			
			for(i=say-SATIR_BYTE_SAYISI+j*(SATIR_BYTE_SAYISI-dosya_boyutu%SATIR_BYTE_SAYISI)*(!!(dosya_boyutu%SATIR_BYTE_SAYISI));i<say && say;i++)	 
				if(icerik[i]>=0x20 && icerik[i]<0x7F)// Hex deðerleri yazýlýr sað tarafa, son sýralama dosya boyutunun kalaný kadardýr
					printf("%c",icerik[i]);
				else
					printf(".");
					
			if(say%TOPLAM_BYTE_SAYISI==0 || j)
			{
				if(say)																//Alttaki yazýlar için. Burasý say 0 da çalýþmayacak
				{
					s=say/TOPLAM_BYTE_SAYISI+(dosya_boyutu<TOPLAM_BYTE_SAYISI);		//þuan olan sayfa, say/dosya_boyutu ise sadece son sayfada artýk olayý olduðundan
					sol=s-1;
					sag=s+1;
					
					if(s==1)
						sol=toplam_sayfa;
					if(s==toplam_sayfa)
						sag=1;
						
					printf("\n\nTOPLAM %lu SAYFA VAR. SEN %lu. SAYFADASIN\n\n",toplam_sayfa, s);
					printf("- ile geri git, + ile ileri git. q ise bitir ve kaydet:\n");
					
					do{
						i=getch();
						if(i==YON_TUSU)
						{
					#ifdef __linux
							getch();
					#endif	
							i=getch();
							
							if(i==YON_TUSU_UP || i== YON_TUSU_DOWN || i==YON_TUSU_RIGHT || i==YON_TUSU_LEFT)
							{
							 	i=1;
								if(say==TOPLAM_BYTE_SAYISI)
									say=0;
								else if (say==dosya_boyutu)
									say=TOPLAM_BYTE_SAYISI*(toplam_sayfa-1);
								else
									say=say-TOPLAM_BYTE_SAYISI;
								HexeGit();
							}
						}		
					}while(i!=45 && i!=43 && i!=1 && i!='q' && i!='Q');
					
					if(i==0x2D) //45
						if(say==TOPLAM_BYTE_SAYISI)						// 1. sayfanýn öncesi son sayfa olduðu için 
							say=TOPLAM_BYTE_SAYISI*(toplam_sayfa-1);
						else if (say==dosya_boyutu)
							if(toplam_sayfa==1)
							say=0;
							else
								say=TOPLAM_BYTE_SAYISI*(toplam_sayfa-2);
						else
							say=say-2*TOPLAM_BYTE_SAYISI;
					else if(i=='q' || i=='Q')
						break;
					else
						if(say==dosya_boyutu)
							say=0;
					TeMiZ;
				}
				printf("\t   ");									// Üstteki adresler yazýlýr
				for(i=0;i<0x10;i++)					
					printf("%.2x ",i);
				printf("\n");
			}
			printf("\n%.8x   ",say);								// Soldaki adresler yazýlýr
		}
		printf("%.2x ",icerik[say]);								// Bytelar hex sistemiyle yazýlýr
	}
	
	if((dosya=fopen("yeni.exe","wb"))!=NULL)
		fwrite(icerik,sizeof(unsigned char),dosya_boyutu,dosya);
	else
		puts("Kaydedilemedi...");
		
	fclose(dosya);
	free(icerik);
	
	getch();
	return 0;
}

void HexeGit()
{
	int x=11, y=2, z;
	
	do{
		gotoxy(x,y);
		
		z=getch();
		
		if(z==YON_TUSU)
		{
#ifdef __linux
			getch();
#endif	
			z=getch();
			
			if(z==YON_TUSU_UP)
			{
				if(j==1)
					if(x>(dosya_boyutu%SATIR_BYTE_SAYISI-1)*3+11)
						if(y==2)
							y=(dosya_boyutu%TOPLAM_BYTE_SAYISI)/SATIR_SAYISI+1;
						else
							y--;
					else
						if(y==2)
							y=(dosya_boyutu%TOPLAM_BYTE_SAYISI)/SATIR_SAYISI+2;
						else
							y--;
				else
					if(y==2)
						y=SATIR_SAYISI+1;
					else
						y--;
			}
			else if(z==YON_TUSU_DOWN)
			{
				if(j==1)
					if(x>(dosya_boyutu%SATIR_BYTE_SAYISI-1)*3+11)
						if(y==(dosya_boyutu%TOPLAM_BYTE_SAYISI)/SATIR_SAYISI+1)
							y=2;
						else
							y++;
					else
						if(y==(dosya_boyutu%TOPLAM_BYTE_SAYISI)/SATIR_SAYISI+2)
							y=2;
						else
							y++;
				else
					if(y==SATIR_SAYISI+1)
						y=2;
					else
						y++;
			}
			else if(z==YON_TUSU_RIGHT)
			{
				if(j==1 && y==((dosya_boyutu%TOPLAM_BYTE_SAYISI)/SATIR_SAYISI+2))
					if(x==(dosya_boyutu%SATIR_BYTE_SAYISI-1)*3+11)
						x=11;
					else
						x+=3;
				else
					if(x==(SATIR_BYTE_SAYISI-1)*3+11)
						x=11;
					else
						x+=3;
			}
			else if(z==YON_TUSU_LEFT)
			{
				if(j==1 && y==((dosya_boyutu%TOPLAM_BYTE_SAYISI)/SATIR_SAYISI+2))
					if(x==11)
						x=(dosya_boyutu%SATIR_BYTE_SAYISI-1)*3+11;
					else
						x-=3;
				else
					if(x==11)
						x=(SATIR_BYTE_SAYISI-1)*3+11;
					else
						x-=3;
			}
			else{			}
		}	
		else if(z==32)
		{
			YeniHex(x,y);
		}
		else{		}
	}while(z!=13);
	/*Eklenecekler : 10 tabanýnda hex degistirme (kendisi 16 tabanýna çevirecek) belki yön tuþlarý boþluk tuþu orginizasyonu deðiþebilir.
	birde optimizasyon*/
}

void YeniHex(short x, short y)
{
	int a[2],b=0;
	
	while(b!=3)
	{
		a[b]=getch();
	
		if(a[b]>=0x30 && a[b]<=0x39 && !(b==2))
		{
			printf("%c",a[b]);
			a[b]=a[b]-48;
			b++;
		}
		else if (a[b]>=0x41 && a[b]<=0x46 && !(b==2))
		{
			printf("%c",a[b]);
			a[b]=a[b]-55;
			b++;
		}
		else if(a[b]>=0x61 && a[b]<=0x66 && !(b==2))
		{
			printf("%c",a[b]);
			a[b]=a[b]-87;
			b++;
		}
		else if(a[b]==8 && b!=0)
		{
			printf("\b \b");
			b--;
		}
		else if(a[b]==13)
		{
			icerik[say+(y-2)*SATIR_BYTE_SAYISI+(x-11)/3]=a[0]*16+a[1];
			b++;
		}
		else
		{
			if(a[b]==YON_TUSU)
			{
#ifdef __linux
				getch();
#endif	
				getch();
			}
		}
	}
	/* Eklenecekler : Tek 4 bit degistirme, deðiþim yaparken ayni anda saðdaki içerik deðiþimi (belki)*/
}
