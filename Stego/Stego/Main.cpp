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
	printf(" * * * Стеганография * * * \n");
	printf(" * * * * * * * * * * * * *  \n\n");

	printf("Введите:\n * 1 - закодировать текст в изображение\n * 2 - декодировать текст из изображения\n\nВвод: ");
	int command; scanf("%d", &command); printf("\n"); clean_stdin();

	char filename[101];
	switch (command)
	{
	case 1:
		getFileName(filename);
		encoder(getRecordingDensity(), filename);
		printf("Кодирование завершено\n\n");
		break;
	case 2:
		decoder(getRecordingDensity());
		printf("Декодирование завершено\n\n");
		break;
	default:
		break;
	}

	printf("Нажмите 'Enter' для продолжения...\n");
	clean_stdin(); getchar();
	manager();
}

/* Чистит буфер ввода */
void clean_stdin()
{
	int c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}

/* Получить кол-во символов в файле */
unsigned long getTextLength(char *filePath)
{
	return getFileSize(filePath);
}

/* Получить размер файла */
unsigned long getFileSize(char *filePath)
{
	FILE *file = fopen(filePath, "rb");

	if (file == NULL || fileno(file) == -1)
	{
		printf(" * Невозможно прочитать файл\n");
		return 0;
	}

	long fileLen = filelength(fileno(file));
	fclose(file);

	return fileLen;
}

/* Записываем текст в выходной файл */
void writeText(unsigned char *text, char *file_name)
{
	unsigned long txt_len = strlen((char *)text);
	FILE* file2;
	fopen_s(&file2, file_name, "wb+");

	for (unsigned long i = 0; i < txt_len; i++)
		fputc(text[i], file2);

	fclose(file2);
}

// Получаем текст из файла
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

/* Преобразует число number в двоичный код и копирует его в binaryStr */
void convertCharToBin(int number, char *binaryStr)
{
	for (int counter = 7; counter > -1; counter--)
	{
		binaryStr[counter] = (number % 2) + 48;
		number /= 2;
	}

	*(binaryStr + 8) = '\0';
}

/* Перевод строки в бинарный формат */
void convertTextToBin(unsigned char *text, char *binaryStr)
{
	strcpy(binaryStr, "");
	for (int i = 0; i < strlen((char *)text); i++)
		strcat(binaryStr, getBits(*(text + i)));
}

/* Получаем массив символов из 8 элементов */
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

/* Переводим двоичный код в десятичный */
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
* Преобразует число строку
* Длина строки = const = 19 символов
* Состав строки: сперва идет число, после его окончания символ 'x', после ряд нулей (если число не заполняет все 19 символов)
*/
char * covertNumToString(unsigned long long int snum)
{
	const int symbNum = 19; // число символов в строке
	char *numStr = (char *)malloc((symbNum + 1) * sizeof(char));

	// Считаем кол-во цифр в числе
	int degN = 0; // счетчик кол-ва цифр в числе
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

	// Начинаем посимвольно копировать число в строку
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
* Получаем размер текста, указанный в кодируемом файле в виде числа
* На вход поступает строка из кодированного файла
*/
unsigned long getReadTextSize(char *str)
{
	// Оставляем часть до символа 'x'
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

/* Кодируем, записывая в изображение */
void encoder(int recDensity, char *textfilePath)
{
	char stringInBin[9]; // Двоичное код символа 
	bool isTextFullyEncoded = false; // Является ли текст полностью закодированным, или его часть не удалось записать в файл
	unsigned char buffer; // Буфер, в который будут вытаскиваться байты из исходного изображения
	char bufferBinary[9]; // Бинарное представление символа buffer
	int binaryOffset = 0; // Смещение в бинарной строке символа текста
	unsigned long curCharPos = 1; // Текущая позиция символа

	unsigned long textSize = getTextLength(textfilePath);

	if (textSize == 0)
	{
		printf("Ошибка: Путь к файлу некорректен или файл пуст\n");
		return;
	}
	printf("Размер файла: %d байт\n", textSize);

	// Выделяем память под исходный текст
	unsigned char *text = (unsigned char *)malloc(textSize * 2 * sizeof(unsigned char)); strcpy((char *)text, "");
	// Преобразуем размер текста в строку, чтобы вставить ее перед текстом и закодировать вместе с ним
	char *fileSizeStr = covertNumToString(textSize);
	// Вставляем строку размера файла перед текстом и изменяем textSize на новое значение: размер текста + размер fileSizeStr
	strcat((char *)text, fileSizeStr);
	textSize += strlen(fileSizeStr);
	// Читаем текст из файла и записываем его в выделенную память (text), в которую предварительно уже записан размер этого текста
	unsigned char *readText = readTextFromFile(textfilePath);
	strcat((char *)text, (char *)readText);

	// *** С данного момента начинаем шифрование ***

	FILE* inputFile = fopen("picture_input.bmp", "rb");
	FILE* outputFile = fopen("picture_output.bmp", "wb");

	textSize = strlen((char *)text);

	// Находим начало байтов цвета
	for (int i = 0; i < 54; i++)
	{
		fread(&buffer, sizeof(char), 1, inputFile);
		fwrite(&buffer, sizeof(unsigned char), 1, outputFile);
	}

	convertCharToBin(text[0], stringInBin); // Бинарная строка 1 символа		

											// Весь цикл строится на файл-контейнере т.к. сам файл может закончится раньше того момента, когда мы сможем закодировать весь текст	
	while (!feof(inputFile))
	{
		fread(&buffer, sizeof(char), 1, inputFile); // Читаем байт из файла
		convertCharToBin(buffer, bufferBinary); // Перегоняем в битовый формат
												// Пишем в файл начиная с позиции '8 минус плотность', т.е. для 2 бит кодируются позиции 7 и 8, в смещении 6 и 7 соответственно		
		for (int buff_offset = 8 - recDensity; buff_offset < 8; buff_offset++)
		{
			bufferBinary[buff_offset] = stringInBin[binaryOffset++];
			if (binaryOffset == 8) // Если буфер кодируемого символа кончился, берём следующий символ
			{
				if (curCharPos > textSize) // Если весь текст, включая последний \0 зашифрован
				{
					fwrite(&buffer, sizeof(unsigned char), 1, outputFile); // Закидываем буфер в его текущем состоянии в выходной файл

					while (!feof(inputFile)) // Копируем остаток картинки
					{
						fread(&buffer, sizeof(char), 1, inputFile);
						fwrite(&buffer, sizeof(unsigned char), 1, outputFile);
					}

					isTextFullyEncoded = true; // Устанавливаем, что весь текст был полностью закодирован
					break;
				}

				binaryOffset = 0; // Обнуляем смещение
				convertCharToBin(text[curCharPos], stringInBin); // Вытаскиваем символ
				curCharPos++; // Сдвигаем смещение по кодируемому тексту
			}
		}

		// Записываем результат в файл
		if (isTextFullyEncoded) break; // Если кодирование полностью закончилось, выходим из основного цикла

		buffer = binToDec(bufferBinary); // Переводим двоичный буфер назад в десятичное число
		fwrite(&buffer, sizeof(unsigned char), 1, outputFile); // Записываем его

		if (feof(inputFile) && !isTextFullyEncoded)printf("Текст слишком велик, не все символы были закодированы\n * Кол-во закодированных символов: %d\n", --curCharPos);//если пикча кончилась раньше текста, пишем сообщение о символах, которые не влезли
		if (feof(inputFile)) printf(" * Упущено символов: %d\n", textSize - curCharPos);
	}

	if (isTextFullyEncoded) printf("Весь файл успешно записан\n");

	fclose(outputFile);
	fclose(inputFile);
}

/* Декодируем, читая из изображения */
void decoder(int recDensity)
{
	unsigned char buffer; // Буффер, хранящий байты из картинки

						  // Строки под битное представление:
	char binStr[9]; // Строка, в которую складываются биты из каждого байта, при полном заполнении перегоняется в символ расшифрованного текста
	char binBuffer[9]; // Бинарный буфер

	int binStrOffset = 0; // Смещение бинарной строки

	bool isDecodingComplete = false; // Статус расшифровки; при 'true' текст считается расшифрованным
	unsigned long decodedTextOffset = 0; // Смещение в расшифрованном тексте (номер символа, который будет расшифрован и добавлен следующим)

	unsigned long fileSize = getFileSize("picture_output.bmp");
	printf("Размер файла: %d\n", fileSize);

	// Необходимо для считывания закодированного текста
	unsigned char *decodedText = (unsigned char*)malloc(fileSize * sizeof(unsigned char));
	FILE *sourceFile = fopen("picture_output.bmp", "rb+");

	// Необходимо для считывания размера закодированного текста
	bool isTextSizeHasBeenRead = false; // Был ли уже прочитан из файла размер закодированного текста; если false, формируем readTextSize; если true, формируем сам текст
	char *readTextSizeStr = (char *)malloc(20 * sizeof(char)); strcpy(readTextSizeStr, ""); // Строка для хранения считанного размера кодированного текста
	int readTextSizeOffset = 0; // Смещение в readTextSize
	long readTextSizeInt; // readTextSize в численном виде

						  // Пропускаем первые 53 байта изображения и дешифруем
	for (int i = 0; i < 54; fread(&buffer, sizeof(char), 1, sourceFile), i++);

	while (!feof(sourceFile))
	{
		fread(&buffer, sizeof(char), 1, sourceFile); // Заносим байт в buffer
		convertCharToBin(buffer, binBuffer); // Переводим buffer в двоичный вид -> binBuffer

											 // Поскольку зашифрованные биты идут в правильном порядке , например хххххx12 хххххx34, то
											 // ставим смещение на позицию '8 минус плотность'; Т.е. для плотности 2 закодированные биты 7 и 8, на смещениях 6 и 7 соответственно
		for (int i = 8 - recDensity; i < 8; i++)
		{
			if (isDecodingComplete) break; // Если дешифровка завершена, прерываем

			binStr[binStrOffset++] = binBuffer[i]; // Вытаскиваем в binStr нужные биты, т.е. из первого байта 1 и 2, из второго 3 и 4, и т.д.

			if (binStrOffset == 8) // Если binStr закончился (набралось 8 бит)
			{
				binStrOffset = 0; // Возвращаем позицию в начало
				buffer = binToDec(binStr); // Переводим в формат символа, тут используется buffer для меньшего количества переменных, т.к. дальше он получит значение байта картинки, а до того им можно пользоваться

				if (isTextSizeHasBeenRead) // Если размер закодированного текста был считан, формируем сам текст
				{
					if (decodedTextOffset >= readTextSizeInt) break;
					decodedText[decodedTextOffset++] = buffer; // Переводим расшифрованный текст в decodedText
				}
				else // Если нет, формируем строку с указанным размером текста до символа y
				{
					if (buffer != 'y') // 'y' - закрывающий символ считываемого размера текста; встреча с ним означает завершение считывания размера кодируемого текста
					{
						readTextSizeStr[readTextSizeOffset++] = buffer;
					}
					else
					{
						isTextSizeHasBeenRead = true;
						readTextSizeStr[readTextSizeOffset] = '\0';
						readTextSizeInt = getReadTextSize(readTextSizeStr);
						printf("Исходный размер текста (указан в закодированном файле): %d\n", readTextSizeInt);
					}
				}

				// Если значение буфера равно 0 ('\0') 1 или 3 (не знаю как так получается, но закрывающий байт строки иногда превращается в 00000001 или 3) 
				// т.е. нашли последний символ закодированной строки '\0'
				if (buffer == 3 || buffer == 1 || buffer == 0)
				{
					isDecodingComplete = true; // Устанавливаем статус "завершено", что бы на следующей итерации он вышел из основного цикла
					break;
				}
			}
		}
	}

	decodedText[decodedTextOffset] = '\0';

	fclose(sourceFile);
	writeText(decodedText, "decoded.txt");
	printf("Размер декодированного текста: %d\n", decodedTextOffset);
}

/* Получает название входного текстового файла */
void getFileName(char *file_name)
{
	while (true)
	{
		printf("Введите название текстового файла: ");
		scanf("%s", file_name); clean_stdin();

		if (fopen(file_name, "r") == NULL)
		{
			printf(" * Ошибка: Не удалось открыть файл\n");
			continue;
		}

		return;
	}
}

/* Устанавливает плотность кодирования */
int getRecordingDensity()
{
	int recDensity;
	while (true)
	{
		printf("Введите плотность кодирования (от 1 до 8): ");
		scanf("%d", &recDensity); clean_stdin();

		if (recDensity > 0 && recDensity < 9) break;
		else printf(" * Ошибка: необходима цифра от 1 до 8\n");
	}

	return recDensity;
}