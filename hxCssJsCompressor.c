/*
*	v 29.11.2018
*	Компрессор .css и .js файлов.
*	github.com/TyUser/hxCssJsCompressor
*/
#include <stdio.h>
#include <string.h>

#define HX_TEMP_FILE "tempCssJs.dat"

int HxFileValid(const char *sName)
{
	size_t iLen = strlen(sName);
	if (iLen > 5)
	{
		if (iLen < 250)
		{
			if ((sName[iLen-4] == '.')
			 && (sName[iLen-3] == 'c')
			 && (sName[iLen-2] == 's')
			 && (sName[iLen-1] == 's')
			 && (sName[iLen] == '\0'))
			{
				return 1;
			}

			if ((sName[iLen-3] == '.')
			 && (sName[iLen-2] == 'j')
			 && (sName[iLen-1] == 's')
			 && (sName[iLen] == '\0'))
			{
				return 2;
			}
		}
	}
	return 0;
}

int HxSkip(char *sBuf2)
{ /*! Поиск спец коментарий	*/
	size_t i = 0;
	static int ig_skip = 0;

	while (sBuf2[i] != '\0')
	{
		if ((sBuf2[i]=='/') 
		 && (sBuf2[i+1]=='*') 
		 && (sBuf2[i+2]=='!'))
		{
			ig_skip = 1;
			break;
		}

		if ((sBuf2[i]=='*') 
		 && (sBuf2[i+1]=='/'))
		{
			ig_skip = 0;
			break;
		}
		i += 1;
	}

	return ig_skip;
}

void HxWhile1(char *sBuf1, short iFile)
{ /*	Чистка строки от коментариев вида //	*/
	size_t i = 0;
	short iHttp = 0;

	while (sBuf1[i] != '\0')
	{
		if ((sBuf1[i] == '/') && (sBuf1[i+1] == '/'))
		{
			iHttp = 0;
			if (i > 4)
			{
				if (sBuf1[i-1] == ':')
				{
					if ((sBuf1[i-5] == 't') 
					 && (sBuf1[i-4] == 't') 
					 && (sBuf1[i-3] == 'p') 
					 && (sBuf1[i-2] == 's'))
					{		/* https:// */
						iHttp = 1;
					}

					if ((sBuf1[i-5] == 'h') 
					 && (sBuf1[i-4] == 't') 
					 && (sBuf1[i-3] == 't') 
					 && (sBuf1[i-2] == 'p'))
					{		/* http:// */
						iHttp = 1;
					}

					if (iFile == 2)
					{
						if ((sBuf1[i-5] == 's') 
						 && (sBuf1[i-4] == 'a') 
						 && (sBuf1[i-3] == 'p') 
						 && (sBuf1[i-2] == 'p'))
						{		/* whatsapp://send */
							if (sBuf1[i+2] == 's')
							{
								iHttp = 1;
							}
						}

						if ((sBuf1[i-3] == 't') 
						 && (sBuf1[i-2] == 'g'))
						{		/* tg://msg_url */
							if ((sBuf1[i+2] == 'm')
							 && (sBuf1[i+3] == 's'))
							{
								iHttp = 1;
							}
						}

						if ((sBuf1[i-5] == 'i') 
						 && (sBuf1[i-4] == 'b') 
						 && (sBuf1[i-3] == 'e') 
						 && (sBuf1[i-2] == 'r'))
						{		/* viber://forward */
							if ((sBuf1[i+2] == 'f')
							 && (sBuf1[i+3] == 'o'))
							{
								iHttp = 1;
							}
						}
					}
				}
			}

			if (!iHttp)
			{
				sBuf1[i] = '\0';
				break;
			}
		}

		i += 1;
	}
}

void HxWhile2(char *sBuf2)
{ /*	Замена табуляции и перевод строки в пробел	*/
	size_t i = 0;
	int iSkip = HxSkip(sBuf2);

	while (sBuf2[i] != '\0')
	{
		if (sBuf2[i] == '\t')
		{
			sBuf2[i] = ' ';
		}

		if ((sBuf2[i] == '\n') 
		 || (sBuf2[i] == '\r'))
		{
			if (!iSkip)
			{
				sBuf2[i] = ' ';
			}
		}
		i += 1;
	}
}

void HXfileC(const char *sNameF, short iFile)
{
	char sBuf1[65528];
	char sBuf2[65528];  /* max iso 65535 */
	FILE *fileR;
	FILE *fileW;
	unsigned int iTemp1 = 0;
	unsigned int iTemp2 = 0;
	unsigned short iError = 1;
	unsigned short iComments = 0;
	unsigned short iFix = 0;

	fileR = fopen(HX_TEMP_FILE, "r");
	if (fileR)
	{
		fileW = fopen(sNameF, "w+");
		if (fileW)
		{
			while (1)
			{
				sBuf1[0] = '\0';
				if (!fgets(sBuf1, sizeof(sBuf1)-10, fileR))
				{
					break;
				}

				sBuf2[0] = '\0';
				iTemp1 = 0;
				iTemp2 = 0;

				while (sBuf1[iTemp1] != '\0')
				{
					if ((sBuf1[iTemp1]=='/') && (sBuf1[iTemp1+1]=='*'))
					{
						if (sBuf1[iTemp1+2] !='!')
						{
							iComments = 1;
						}
					}
					if ((sBuf1[iTemp1]=='*') && (sBuf1[iTemp1+1]=='/'))
					{
						if (iComments)
						{
							iComments = 0;
							iTemp1 += 2;
						}
					}

					if (iComments)
					{
						iTemp1 += 1;
						continue;
					}

					sBuf2[iTemp2] = sBuf1[iTemp1];
					iTemp1 += 1;
					iTemp2 += 1;
				}
				sBuf2[iTemp2] = '\0';
				sBuf1[0] = '\0';
				iTemp1 = 0;
				iTemp2 = 0;

				while (sBuf2[iTemp1] != '\0')
				{		/* Фильтрация двойного пробела */
					if ((sBuf2[iTemp1] == ' ') && (sBuf2[iTemp1+1] == ' '))
					{
						iTemp1 += 1;
						continue;
					}

					sBuf1[iTemp2] = sBuf2[iTemp1];
					iTemp1 += 1;
					iTemp2 += 1;
				}
				sBuf1[iTemp2] = '\0';
				sBuf2[0] = '\0';
				iTemp1 = 0;
				iTemp2 = 0;

				while (sBuf1[iTemp1] != '\0')
				{		/* Фильтрация одинарного пробела */
					if (sBuf1[iTemp1] == ' ')
					{
						iFix = 0;
						if (sBuf1[iTemp1+1] == '{')
						{
							iFix = 1;
						}
						if (sBuf1[iTemp1+1] == '}')
						{
							iFix = 1;
						}
						if (sBuf1[iTemp1+1] == ':')
						{
							iFix = 1;
						}
						if (sBuf1[iTemp1+1] == ';')
						{
							iFix = 1;
						}
						if (sBuf1[iTemp1+1] == ')')
						{
							iFix = 1;
						}
						if (sBuf1[iTemp1+1] == '=')
						{
							iFix = 1;
						}
						if (sBuf1[iTemp1+1] == '>')
						{
							iFix = 1;
						}
						if (sBuf1[iTemp1+1] == '<')
						{
							iFix = 1;
						}

						if (iFix)
						{
							iTemp1 += 1;
						}
					}

					if (sBuf1[iTemp1+1] == ' ')
					{
						if (iFile == 1)
						{
							if (sBuf1[iTemp1] == ',')
							{
								iTemp1 += 1;
								sBuf1[iTemp1] = ',';
							}
						}

						if (sBuf1[iTemp1] == '{')
						{
							iTemp1 += 1;
							sBuf1[iTemp1] = '{';
						}
						if (sBuf1[iTemp1] == '}')
						{
							iTemp1 += 1;
							sBuf1[iTemp1] = '}';
						}
						if (sBuf1[iTemp1] == ';')
						{
							iTemp1 += 1;
							sBuf1[iTemp1] = ';';
						}
						if (sBuf1[iTemp1] == ':')
						{
							iTemp1 += 1;
							sBuf1[iTemp1] = ':';
						}
						if (sBuf1[iTemp1] == '(')
						{
							iTemp1 += 1;
							sBuf1[iTemp1] = '(';
						}

						if (iFile == 2)
						{
							if (sBuf1[iTemp1] == ')')
							{
								iTemp1 += 1;
								sBuf1[iTemp1] = ')';
							}
						}

						if (sBuf1[iTemp1] == '=')
						{
							iTemp1 += 1;
							sBuf1[iTemp1] = '=';
						}
						if (sBuf1[iTemp1] == '<')
						{
							iTemp1 += 1;
							sBuf1[iTemp1] = '<';
						}
						if (sBuf1[iTemp1] == '>')
						{
							iTemp1 += 1;
							sBuf1[iTemp1] = '>';
						}
					}

					sBuf2[iTemp2] = sBuf1[iTemp1];
					iTemp1 += 1;
					iTemp2 += 1;
				}
				sBuf2[iTemp2] = '\0';
				sBuf1[0] = '\0';

				fprintf(fileW, "%s", sBuf2);
			}

			iError = 0;
			fclose(fileW);
		}
		fclose(fileR);
	}

	if (!iError)
	{
		remove(HX_TEMP_FILE);
	}
}

void HXfile(const char *sNameF, short iFile)
{
	char sBuf1[65528];
	char sNewName[512];
	short iError = 1;

	FILE *fileR;
	FILE *fileW;
	fileR = fopen(sNameF, "r");
	if (fileR)
	{
		fileW = fopen(HX_TEMP_FILE, "w+");
		if (fileW)
		{
			while (1)
			{
				sBuf1[0] = '\0';
				if (!fgets(sBuf1, sizeof(sBuf1)-10, fileR))
				{
					break;
				}

				HxWhile1(sBuf1, iFile);
				HxWhile2(sBuf1);
				fprintf(fileW, "%s", sBuf1);
			}

			iError = 0;
			fclose(fileW);
		}
		fclose(fileR);
	}

	if (!iError)
	{
		sprintf(sNewName, "%s.old", sNameF);
		if (!rename(sNameF, sNewName))
		{
			HXfileC(sNameF, iFile);
		}
	}
}

int main(int argc, char * argv[])
{
	short iRr = 0;
	short iResult = 1;

	if (argc > 1)
	{
		iRr = HxFileValid(argv[1]);
		if (iRr > 0)
		{
			HXfile(argv[1], iRr);
			iResult = 0;
		}
	}
	return iResult;
}
