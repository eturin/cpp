#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "expat.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

struct DeepAndContent{
	int    depth;
	char * content;
	size_t content_len;
};

static void XMLCALL startElement(void *userData, const char *name, const char **atts){
	struct DeepAndContent *deepCont = (struct DeepAndContent *)userData;
	
	for (int i = 0; i < deepCont->depth; i++)
		putchar('\t');
	printf(name);

	for (int i = 0; atts[i]; i += 2) {
		printf(" %s='%s'", atts[i], atts[i + 1]);
	}
	printf("\n");
	deepCont->depth += 1;
}

void startData(void *userData, const char *content, int length){
	struct DeepAndContent *deepCont = (struct DeepAndContent *)userData;	
	deepCont->content_len = length + 1;
	deepCont->content = (char *)malloc(deepCont->content_len);	
	strncpy(deepCont->content, content,length);
	deepCont->content[length] = '\0';
}

static void XMLCALL endElement(void *userData, const char *name){
	struct DeepAndContent *deepCont = (struct DeepAndContent *)userData;
	if (deepCont->content != NULL){
		printf("--> %s", deepCont->content);
		free(deepCont->content);
		deepCont->content = NULL;
	}	
	deepCont->depth -= 1;
}

int main(int argc, char *argv[]){
	//в этой структуре будут храниться глубина вложенности и знаение элемента(узла)
	struct DeepAndContent deepCont = { 0 };

	//создаем парсер
	XML_Parser parser = XML_ParserCreate("UTF-8");
	//связываем его с нашей структурой
	XML_SetUserData(parser, &deepCont);
	//устанавливаем обработчики событий
	XML_SetElementHandler(parser, startElement, endElement);	
	XML_SetCharacterDataHandler(parser, startData);
	
	//открываем xml-файл
	FILE * file = fopen("\\\\As-msk-n7060\\edo_ead_out$\\WinMerge 15.08.2017 135931 safedorova@svyaznoy.ru.result.xml", "rb");
	
	int done;
	char buf[BUFSIZ];	
	do {
		//очередной блок байт
		size_t len = fread(buf, 1, sizeof(buf), file);
		done = len < sizeof(buf); //если прочитали меньше, чем размер буфера, значит повторять цикл не нужно
		
		//парсим блок байт
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
			fprintf(stderr,	"%s at line %" XML_FMT_INT_MOD "u\n", XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser));
			return 1;
		}
	} while (!done);

	//освобождение ресурсов
	free(deepCont.content);
	fclose(file);
	XML_ParserFree(parser);

	
	return 0;
}