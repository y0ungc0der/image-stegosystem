#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <locale.h>

void manager();
void clean_stdin();
unsigned long getTextLength(char *);
unsigned long getFileSize(char *);
void writeText(unsigned char *, char *);
unsigned char * readTextFromFile(char *filePath);
void convertCharToBin(int, char *);
void convertTextToBin(unsigned char *, char *);
char * getBits(const unsigned char);
int binToDec(char *);
void encoder(int, char*);
void decoder(int);
void getFileName(char *);
int getRecordingDensity();
char * covertNumToString(unsigned long long int);
unsigned long getReadTextSize(char *str);


void main()
{
	setlocale(LC_ALL, "RUS");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	manager();

	system("pause");
}

void manager()
{
	system("cls");

	printf(" * * * * * * * * * * * * * \n");
	printf(" * * * ������������� * * * \n");
	printf(" * * * * * * * * * * * * *  \n\n");

	printf("�������:\n * 1 - ������������ ����� � �����������\n * 2 - ������������ ����� �� �����������\n\n����: ");
	int command; scanf("%d", &command); printf("\n"); clean_stdin();

	char filename[101];
	switch (command)
	{
	case 1:
		getFileName(filename);
		encoder(getRecordingDensity(), filename);
		printf("����������� ���������\n\n");
		break;
	case 2:
		decoder(getRecordingDensity());
		printf("������������� ���������\n\n");
		break;
	default:
		break;
	}

	printf("������� 'Enter' ��� �����������...\n");
	clean_stdin(); getchar();
	manager();
}

/* ������ ����� ����� */
void clean_stdin()
{
	int c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}

/* �������� ���-�� �������� � ����� */
unsigned long getTextLength(char *filePath)
{
	return getFileSize(filePath);
}

/* �������� ������ ����� */
unsigned long getFileSize(char *filePath)
{
	FILE *file = fopen(filePath, "rb");

	if (file == NULL || fileno(file) == -1)
	{
		printf(" * ���������� ��������� ����\n");
		return 0;
	}

	long fileLen = filelength(fileno(file));
	fclose(file);

	return fileLen;
}

/* ���������� ����� � �������� ���� */
void writeText(unsigned char *text, char *file_name)
{
	unsigned long txt_len = strlen((char *)text);
	FILE* file2;
	fopen_s(&file2, file_name, "wb+");

	for (unsigned long i = 0; i < txt_len; i++)
		fputc(text[i], file2);

	fclose(file2);
}

// �������� ����� �� �����
unsigned char * readTextFromFile(char *filePath)
{
	FILE *sourceFile = fopen(filePath, "rb");

	unsigned char *str = (unsigned char *)malloc((getFileSize(filePath) + 1) * sizeof(unsigned char));

	unsigned char currentChar = 'a';
	int length = 0;
	while (!feof(sourceFile))
	{
		currentChar = fgetc(sourceFile);
		str[length] = currentChar;
		length++;
		if (feof(sourceFile))break;
	}

	str[length] = '\0';
	fclose(sourceFile);

	return str;
}

/* ����������� ����� number � �������� ��� � �������� ��� � binaryStr */
void convertCharToBin(int number, char *binaryStr)
{
	for (int counter = 7; counter > -1; counter--)
	{
		binaryStr[counter] = (number % 2) + 48;
		number /= 2;
	}

	*(binaryStr + 8) = '\0';
}

/* ������� ������ � �������� ������ */
void convertTextToBin(unsigned char *text, char *binaryStr)
{
	strcpy(binaryStr, "");
	for (int i = 0; i < strlen((char *)text); i++)
		strcat(binaryStr, getBits(*(text + i)));
}

/* �������� ������ �������� �� 8 ��������� */
char * getBits(const unsigned char byte)
{
	char *bits = (char *)malloc(9 * sizeof(char));

	int mask = 0x80;
	for (int i = 0; i < 8; i++)
	{
		*(bits + i) = (byte & mask) ? '1' : '0';
		mask >>= 1;
	}

	*(bits + 8) = '\0';

	return bits;
}

/* ��������� �������� ��� � ���������� */
int binToDec(char *binStr)
{
	int num = 0, d, curChar;
	for (int i = 0; i < 8; i++)
	{
		d = pow(2, 7 - i);
		curChar = binStr[i] - 48;
		num += curChar * d;
	}

	return num;
}

/*
* ����������� ����� ������
* ����� ������ = const = 19 ��������
* ������ ������: ������ ���� �����, ����� ��� ��������� ������ 'x', ����� ��� ����� (���� ����� �� ��������� ��� 19 ��������)
*/
char * covertNumToString(unsigned long long int snum)
{
	const int symbNum = 19; // ����� �������� � ������
	char *numStr = (char *)malloc((symbNum + 1) * sizeof(char));

	// ������� ���-�� ���� � �����
	int degN = 0; // ������� ���-�� ���� � �����
	unsigned long long int num = snum;
	if (num / 10 == 0) degN = 1;
	else
	{
		while (num > 0)
		{
			num = num / 10;
			degN += 1;
		}
	}

	// �������� ����������� ���������� ����� � ������
	char curChar;
	for (int i = 0; i < symbNum; i++)
	{
		if (i < degN)
		{
			curChar = ((unsigned long long int) ((snum / (unsigned long long int) pow(10, degN - 1 - i))) % 10) + '0';
			*(numStr + i) = curChar;
		}
		else if (i == degN) *(numStr + i) = 'x';
		else *(numStr + i) = '0';
	}

	*(numStr + symbNum - 1) = 'y';
	*(numStr + symbNum) = '\0';

	return numStr;
}

/*
* �������� ������ ������, ��������� � ���������� ����� � ���� �����
* �� ���� ��������� ������ �� ������������� �����
*/
unsigned long getReadTextSize(char *str)
{
	// ��������� ����� �� ������� 'x'
	while (*(str + strlen(str) - 1) != 'x')
	{
		*(str + strlen(str) - 1) = '\0';
	} *(str + strlen(str) - 1) = '\0';

	int numOfDeg = strlen(str);
	int num = 0;

	for (int i = numOfDeg - 1; i >= 0; i--)
	{
		num += (*(str + (numOfDeg - i - 1)) - '0') * pow(10, i);
	}

	return num;
}

/* ��������, ��������� � ����������� */
void encoder(int recDensity, char *textfilePath)
{
	char stringInBin[9]; // �������� ��� ������� 
	bool isTextFullyEncoded = false; // �������� �� ����� ��������� ��������������, ��� ��� ����� �� ������� �������� � ����
	unsigned char buffer; // �����, � ������� ����� ������������� ����� �� ��������� �����������
	char bufferBinary[9]; // �������� ������������� ������� buffer
	int binaryOffset = 0; // �������� � �������� ������ ������� ������
	unsigned long curCharPos = 1; // ������� ������� �������

	unsigned long textSize = getTextLength(textfilePath);

	if (textSize == 0)
	{
		printf("������: ���� � ����� ����������� ��� ���� ����\n");
		return;
	}
	printf("������ �����: %d ����\n", textSize);

	// �������� ������ ��� �������� �����
	unsigned char *text = (unsigned char *)malloc(textSize * 2 * sizeof(unsigned char)); strcpy((char *)text, "");
	// ����������� ������ ������ � ������, ����� �������� �� ����� ������� � ������������ ������ � ���
	char *fileSizeStr = covertNumToString(textSize);
	// ��������� ������ ������� ����� ����� ������� � �������� textSize �� ����� ��������: ������ ������ + ������ fileSizeStr
	strcat((char *)text, fileSizeStr);
	textSize += strlen(fileSizeStr);
	// ������ ����� �� ����� � ���������� ��� � ���������� ������ (text), � ������� �������������� ��� ������� ������ ����� ������
	unsigned char *readText = readTextFromFile(textfilePath);
	strcat((char *)text, (char *)readText);

	// *** � ������� ������� �������� ���������� ***

	FILE* inputFile = fopen("picture_input.bmp", "rb");
	FILE* outputFile = fopen("picture_output.bmp", "wb");

	textSize = strlen((char *)text);

	// ������� ������ ������ �����
	for (int i = 0; i < 54; i++)
	{
		fread(&buffer, sizeof(char), 1, inputFile);
		fwrite(&buffer, sizeof(unsigned char), 1, outputFile);
	}

	convertCharToBin(text[0], stringInBin); // �������� ������ 1 �������		

											// ���� ���� �������� �� ����-���������� �.�. ��� ���� ����� ���������� ������ ���� �������, ����� �� ������ ������������ ���� �����	
	while (!feof(inputFile))
	{
		fread(&buffer, sizeof(char), 1, inputFile); // ������ ���� �� �����
		convertCharToBin(buffer, bufferBinary); // ���������� � ������� ������
												// ����� � ���� ������� � ������� '8 ����� ���������', �.�. ��� 2 ��� ���������� ������� 7 � 8, � �������� 6 � 7 ��������������		
		for (int buff_offset = 8 - recDensity; buff_offset < 8; buff_offset++)
		{
			bufferBinary[buff_offset] = stringInBin[binaryOffset++];
			if (binaryOffset == 8) // ���� ����� ����������� ������� ��������, ���� ��������� ������
			{
				if (curCharPos > textSize) // ���� ���� �����, ������� ��������� \0 ����������
				{
					fwrite(&buffer, sizeof(unsigned char), 1, outputFile); // ���������� ����� � ��� ������� ��������� � �������� ����

					while (!feof(inputFile)) // �������� ������� ��������
					{
						fread(&buffer, sizeof(char), 1, inputFile);
						fwrite(&buffer, sizeof(unsigned char), 1, outputFile);
					}

					isTextFullyEncoded = true; // �������������, ��� ���� ����� ��� ��������� �����������
					break;
				}

				binaryOffset = 0; // �������� ��������
				convertCharToBin(text[curCharPos], stringInBin); // ����������� ������
				curCharPos++; // �������� �������� �� ����������� ������
			}
		}

		// ���������� ��������� � ����
		if (isTextFullyEncoded) break; // ���� ����������� ��������� �����������, ������� �� ��������� �����

		buffer = binToDec(bufferBinary); // ��������� �������� ����� ����� � ���������� �����
		fwrite(&buffer, sizeof(unsigned char), 1, outputFile); // ���������� ���

		if (feof(inputFile) && !isTextFullyEncoded)printf("����� ������� �����, �� ��� ������� ���� ������������\n * ���-�� �������������� ��������: %d\n", --curCharPos);//���� ����� ��������� ������ ������, ����� ��������� � ��������, ������� �� ������
		if (feof(inputFile)) printf(" * ������� ��������: %d\n", textSize - curCharPos);
	}

	if (isTextFullyEncoded) printf("���� ���� ������� �������\n");

	fclose(outputFile);
	fclose(inputFile);
}

/* ����������, ����� �� ����������� */
void decoder(int recDensity)
{
	unsigned char buffer; // ������, �������� ����� �� ��������

						  // ������ ��� ������ �������������:
	char binStr[9]; // ������, � ������� ������������ ���� �� ������� �����, ��� ������ ���������� ������������ � ������ ��������������� ������
	char binBuffer[9]; // �������� �����

	int binStrOffset = 0; // �������� �������� ������

	bool isDecodingComplete = false; // ������ �����������; ��� 'true' ����� ��������� ��������������
	unsigned long decodedTextOffset = 0; // �������� � �������������� ������ (����� �������, ������� ����� ����������� � �������� ���������)

	unsigned long fileSize = getFileSize("picture_output.bmp");
	printf("������ �����: %d\n", fileSize);

	// ���������� ��� ���������� ��������������� ������
	unsigned char *decodedText = (unsigned char*)malloc(fileSize * sizeof(unsigned char));
	FILE *sourceFile = fopen("picture_output.bmp", "rb+");

	// ���������� ��� ���������� ������� ��������������� ������
	bool isTextSizeHasBeenRead = false; // ��� �� ��� �������� �� ����� ������ ��������������� ������; ���� false, ��������� readTextSize; ���� true, ��������� ��� �����
	char *readTextSizeStr = (char *)malloc(20 * sizeof(char)); strcpy(readTextSizeStr, ""); // ������ ��� �������� ���������� ������� ������������� ������
	int readTextSizeOffset = 0; // �������� � readTextSize
	long readTextSizeInt; // readTextSize � ��������� ����

						  // ���������� ������ 53 ����� ����������� � ���������
	for (int i = 0; i < 54; fread(&buffer, sizeof(char), 1, sourceFile), i++);

	while (!feof(sourceFile))
	{
		fread(&buffer, sizeof(char), 1, sourceFile); // ������� ���� � buffer
		convertCharToBin(buffer, binBuffer); // ��������� buffer � �������� ��� -> binBuffer

											 // ��������� ������������� ���� ���� � ���������� ������� , �������� �����x12 �����x34, ��
											 // ������ �������� �� ������� '8 ����� ���������'; �.�. ��� ��������� 2 �������������� ���� 7 � 8, �� ��������� 6 � 7 ��������������
		for (int i = 8 - recDensity; i < 8; i++)
		{
			if (isDecodingComplete) break; // ���� ���������� ���������, ���������

			binStr[binStrOffset++] = binBuffer[i]; // ����������� � binStr ������ ����, �.�. �� ������� ����� 1 � 2, �� ������� 3 � 4, � �.�.

			if (binStrOffset == 8) // ���� binStr ���������� (��������� 8 ���)
			{
				binStrOffset = 0; // ���������� ������� � ������
				buffer = binToDec(binStr); // ��������� � ������ �������, ��� ������������ buffer ��� �������� ���������� ����������, �.�. ������ �� ������� �������� ����� ��������, � �� ���� �� ����� ������������

				if (isTextSizeHasBeenRead) // ���� ������ ��������������� ������ ��� ������, ��������� ��� �����
				{
					if (decodedTextOffset >= readTextSizeInt) break;
					decodedText[decodedTextOffset++] = buffer; // ��������� �������������� ����� � decodedText
				}
				else // ���� ���, ��������� ������ � ��������� �������� ������ �� ������� y
				{
					if (buffer != 'y') // 'y' - ����������� ������ ������������ ������� ������; ������� � ��� �������� ���������� ���������� ������� ����������� ������
					{
						readTextSizeStr[readTextSizeOffset++] = buffer;
					}
					else
					{
						isTextSizeHasBeenRead = true;
						readTextSizeStr[readTextSizeOffset] = '\0';
						readTextSizeInt = getReadTextSize(readTextSizeStr);
						printf("�������� ������ ������ (������ � �������������� �����): %d\n", readTextSizeInt);
					}
				}

				// ���� �������� ������ ����� 0 ('\0') 1 ��� 3 (�� ���� ��� ��� ����������, �� ����������� ���� ������ ������ ������������ � 00000001 ��� 3) 
				// �.�. ����� ��������� ������ �������������� ������ '\0'
				if (buffer == 3 || buffer == 1 || buffer == 0)
				{
					isDecodingComplete = true; // ������������� ������ "���������", ��� �� �� ��������� �������� �� ����� �� ��������� �����
					break;
				}
			}
		}
	}

	decodedText[decodedTextOffset] = '\0';

	fclose(sourceFile);
	writeText(decodedText, "decoded.txt");
	printf("������ ��������������� ������: %d\n", decodedTextOffset);
}

/* �������� �������� �������� ���������� ����� */
void getFileName(char *file_name)
{
	while (true)
	{
		printf("������� �������� ���������� �����: ");
		scanf("%s", file_name); clean_stdin();

		if (fopen(file_name, "r") == NULL)
		{
			printf(" * ������: �� ������� ������� ����\n");
			continue;
		}

		return;
	}
}

/* ������������� ��������� ����������� */
int getRecordingDensity()
{
	int recDensity;
	while (true)
	{
		printf("������� ��������� ����������� (�� 1 �� 8): ");
		scanf("%d", &recDensity); clean_stdin();

		if (recDensity > 0 && recDensity < 9) break;
		else printf(" * ������: ���������� ����� �� 1 �� 8\n");
	}

	return recDensity;
}