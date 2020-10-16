#include "hexns.h"
#include <locale.h>
#include <limits.h>
#include <string.h>

#define SUM_OF_BYTES_ON_ONE_FRAME ((long)LINE * BYTES_ON_LINE)

FILE * file;
int is_full_frame;
unsigned char * context;
char filename[FILENAME_MAX + 1];
short bytes_last_line, last_full_lines;
long size_of_file, sum_of_frames, current_frame;

/*
	Static
*/

static void changeHexOnTheCoord(short x, short y)
{
	int a[3], b = 0;
	/*
		TODO: : Sadece 4 bit değişimi
	*/
	while (1)
	{
		a[b] = getch();
	
		if ((a[b] >= '0') && (a[b] <= '9') && (b != 2))
		{
			printf("%c", a[b]);
			a[b] = a[b] - '0';
			b++;
		}
		else if ((a[b] >= 'A') && (a[b] <= 'F') && (b != 2))
		{
			printf("%c", a[b]);
			a[b] = a[b] - 'A' + 10;
			b++;
		}
		else if ((a[b] >= 'a') && (a[b] <= 'f') && (b != 2))
		{
			printf("%c", a[b]);
			a[b] = a[b] - 'a' + 10;
			b++;
		}
		else if ((a[b] == 8 || a[b] == 127) && (b != 0))
		{
			printf("\b \b");
			b--;
		}
		else if (a[b] == 13 || a[b] == 10)
		{
			if (b == 2)
			{
				unsigned char chr = (unsigned char)(a[0] * 16 + a[1]);
				context[(current_frame - 1) *  SUM_OF_BYTES_ON_ONE_FRAME + (y - 2) * BYTES_ON_LINE + (x - 11) / 3] = chr;
				gotoxy((short)(61 + (x - 11) / 3), y);
				printf("%c", chr);
			}
			break;
		}
		else
		{
			if(a[b] == RDN_FOR_ARROW_KEYS)
			{
				#ifdef __linux
				getch();
				#endif	
				getch();
			}
		}
	}
}

static void navigateInTheFrame()
{
	int z;
	short x = 11, y = 2;
	
	do {
		gotoxy(x, y);
		
		z = getch();
		
		if (z == ' ')
		{
			changeHexOnTheCoord(x, y);
		}
		else if (z == RDN_FOR_ARROW_KEYS)
		{
			#ifdef __linux
			getch();
			#endif	
			z = getch();
			
			if (z == UP_ARROW_KEY)
			{
				if (is_full_frame)
					if (y == 2)
						y = LINE + 1;
					else
						y--;
				else
					if (x > (bytes_last_line - 1) * 3 + 11)
						if (y == 2)
							y = last_full_lines + 1;
						else
							y--;
					else
						if (y == 2)
							y = last_full_lines + 2;
						else
							y--;
			}
			else if (z == DOWN_ARROW_KEY)
			{
				if (is_full_frame == 0)
					if (x > (bytes_last_line - 1) * 3 + 11)
						if (y == last_full_lines + 1)
							y = 2;
						else
							y++;
					else
						if (y == last_full_lines + 2)
							y = 2;
						else
							y++;
				else
					if (y == LINE + 1)
						y = 2;
					else
						y++;
			}
			else if (z == RIGHT_ARROW_KEY)
			{
				if (is_full_frame == 0 && y == (last_full_lines + 2))
					if (x == (bytes_last_line - 1) * 3 + 11)
						x = 11;
					else
						x += 3;
				else
					if (x == (BYTES_ON_LINE - 1) * 3 + 11)
						x = 11;
					else
						x += 3;
			}
			else if (z == LEFT_ARROW_KEY)
			{
				if (is_full_frame == 0 && y == (last_full_lines + 2))
					if (x == 11)
						x = (short)((bytes_last_line - 1) * 3 + 11);
					else
						x -= 3;
				else
					if (x ==11)
						x = (BYTES_ON_LINE - 1) * 3 + 11;
					else
						x -= 3;
			}
		}
	} while (z != 13 && z != 10);

	gotoxy(0, 20);
}

static int init()
{
	setlocale(LC_ALL, "turkish");

	puts("HexNS v2.0!");

	do {
		size_t in_len;

		puts("Dosya yolu girin : ");
		fgets(filename, FILENAME_MAX, stdin);

		in_len = strlen(filename);
		if (in_len > 0 && filename[in_len - 1] == '\n')
			filename[in_len - 1] = 0;
	} while (NULL == (file = fopen(filename, "rb")));

	fseek(file, 0, SEEK_END);
	size_of_file = ftell(file);
	fseek(file, 0, SEEK_SET);

	context = (unsigned char *) malloc((unsigned long) size_of_file * sizeof(unsigned char));
	fread(context, sizeof(unsigned char), (unsigned long) size_of_file, file);	
	fclose(file);
	
	if (MAX_FILESIZE < size_of_file || size_of_file == 0)
	{
		puts("\n:(");
		getch();
		return EXIT_FAILURE;
	}
	
	sum_of_frames = size_of_file / SUM_OF_BYTES_ON_ONE_FRAME + !!(size_of_file % SUM_OF_BYTES_ON_ONE_FRAME);
	
	printf("\nToplam sayfa\t\t\t: %ld\nBir sayfada olacak toplam byte\t: %ld\n", sum_of_frames, SUM_OF_BYTES_ON_ONE_FRAME);
	printf( "Yön tuşları ile hareket edin.\nBoşluk (space) tuşuna basarak yeni değer girebilirsiniz.\n"
			"Enter'a basarak onaylanır. + tuşu ileriki sayfaya gitmeyi, - tuşu bir önceki sayfaya gitmeyi, q tuşu ise kayıt edip çıkmayı sağlar.\n\n\n"
			"Devam etmek istiyorsan 'e' ye bas...");
	
	while (getch() != 'e');

	return EXIT_SUCCESS;
}

/*
	PUBLIC
*/

#ifdef _WIN32
	void gotoxy(short x, short y)
	{
		COORD Cursor_Pos = {x, y};
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cursor_Pos);
	}
#elif __linux
	void gotoxy(short x,short y)
	{
 		printf("\x1B[%d;%df", y + 1, x + 1);
	}

	int getch()
	{
		struct termios oldattr, newattr;
		int ch;
		tcgetattr(STDIN_FILENO, &oldattr );
		newattr = oldattr;
		newattr.c_lflag &= (unsigned int)~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newattr );
		ch = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldattr );
		return ch;
	}
#else
	#error "TODO"
#endif

int hexns(void)
{
	int input, is_last_frame;
	long i, j, remaining, index_of_context, full_lines_in_a_frame;

	current_frame = 1;

	if (init() == EXIT_FAILURE)
		return EXIT_FAILURE;

	bytes_last_line = (short)(size_of_file % BYTES_ON_LINE);
	remaining = (size_of_file % SUM_OF_BYTES_ON_ONE_FRAME);
	last_full_lines = (short)(remaining ? remaining / LINE : LINE);

	while (1)
	{
		CLEAR();

		is_last_frame = (current_frame == sum_of_frames);

		printf("\t   ");
		for (i = 0; i < BYTES_ON_LINE; i++) /* Up addresses */
			printf("%.2x ", (unsigned int) i);
		printf("\n");

		index_of_context = (current_frame - 1) * SUM_OF_BYTES_ON_ONE_FRAME;

		is_full_frame = !(is_last_frame && remaining);
		full_lines_in_a_frame = (is_full_frame ? LINE : last_full_lines);
		
		for (i = 0; i < full_lines_in_a_frame; i++) /* lines */
		{
			printf("\n%.8lx   ", (unsigned long int) index_of_context); /* Left addresses */

			for (j = index_of_context; j < index_of_context + BYTES_ON_LINE; j++) /* context */
				printf("%.2x ", context[j]);
			printf("  ");

			for (j = index_of_context; j < index_of_context + BYTES_ON_LINE; j++) /* grap. char. area */
				if ((context[j] >= ' ') && (context[j] <= '~'))
					printf("%c", context[j]);
				else
					printf(".");
			
			index_of_context += BYTES_ON_LINE;
		}

		if (full_lines_in_a_frame != LINE) /* Last not full line if exists */
		{
			short bytes_loc_remaining = (short)(3 * (!!bytes_last_line) * (BYTES_ON_LINE - bytes_last_line));

			printf("\n%.8lx   ", (unsigned long int) index_of_context); /* Left addresses */

			for (j = index_of_context; j < index_of_context + bytes_last_line; j++) /* context */
				printf("%.2x ", context[j]);
			
			for (i = 0; i < bytes_loc_remaining; i++) /* spaces for lastest line */
				printf(" ");
			printf("  ");

			for (j = index_of_context; j < index_of_context + bytes_last_line; j++) /* grap. char. area */
				if ((context[j] >= ' ') && (context[j] <= '~'))
					printf("%c", context[j]);
				else
					printf(".");
			index_of_context += bytes_last_line;
		}
		
		printf("\n\n%ld. frame.\n", current_frame);
		
		do {
			input = getch();

			if (input == RDN_FOR_ARROW_KEYS)
			{
				#ifdef __linux
				getch();
				#endif
				input = getch();
			}

			if (input == UP_ARROW_KEY || input == DOWN_ARROW_KEY || input == RIGHT_ARROW_KEY || input == LEFT_ARROW_KEY)
			{
				navigateInTheFrame();
			}
		} while ((input != '+') && (input != '-') && (input != 'q') && (input != 'Q'));

		if (input == '+')
		{
			current_frame++;
			if (current_frame > sum_of_frames)
				current_frame = 1;
		}
		else if (input == '-')
		{
			current_frame--;
			if (current_frame == 0)
				current_frame = sum_of_frames;
		}
		else if (input == 'q' || input == 'Q')
		{
			break;
		}
	}
	
	CLEAR();

	if ((file = fopen(filename, "wb")))
	{
		fwrite(context, sizeof(unsigned char), (unsigned long) size_of_file, file);
		fclose(file);
	}
	else
	{
		free(context);
		puts("Kaydedilemedi...");
		return EXIT_FAILURE;
	}

	free(context);
	return EXIT_SUCCESS;
}