#include "SDL.h"
#include "gles.h"
#include "shader.h"

#ifdef _DEBUG
/* ������ʱ����־��Ϣ��ӡ���� */
static void shaderCompileLog(GLuint uShader){
	GLint length;
	GLchar *log;
	
	glGetShaderiv(uShader, GL_INFO_LOG_LENGTH, &length);
	if (length > 0){
		log = malloc(length + 1);
		glGetShaderInfoLog(uShader, length + 1, &length, log);
		SDL_Log("loadShaderByString glComileShader log : \n%s", log);
		free(log);
	}
}

/* �����ӵ���־��Ϣ��ӡ���� */
static void shaderLinkLog(GLuint uProgram){
	GLint length;
	GLchar * log;
	glGetProgramiv(uProgram, GL_INFO_LOG_LENGTH, &length);
	if (length > 0){
		log = malloc(length + 1);
		glGetProgramInfoLog(uProgram, length + 1, &length, log);
		SDL_Log("loadShaderByString glComileShader log : \n%s", log);
		free(log);
	}
}
#else
#define shaderCompileLog(x)
#define shaderLinkLog(x)
#endif

/*
 * loadShaderByString �������ַ����м�װshader�������ʧ�ܷ���-1,�ɹ�����shader id
 * ���ֻ����int,char*ѭ����int ����λ�ã�char* ָ��shader����
 */
GLuint loadShaderByString(const char *szVertexShader,const char *szFragmentShader,...)
{
	GLuint uVertexShader;
	GLuint uFragmentShader;
	GLuint uProgram;
	GLint status;

	/* �������ķ���ֵ */
	uProgram = -1;

	uVertexShader = glCreateShader(GL_VERTEX_SHADER);
	uFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(uVertexShader, 1, szVertexShader, 0);
	glShaderSource(uFragmentShader, 1, szFragmentShader, 0);

	glCompileShader(uVertexShader);
	glGetShaderiv(uVertexShader, GL_COMPILE_STATUS, &status);
	shaderCompileLog(uVertexShader);
	if (status != GL_TRUE){
		SDL_Log("loadShaderByString glCompileShader Vertex Shader return error");
		glDeleteShader(uVertexShader);
		return uProgram;
	}
	glCompileShader(uFragmentShader);
	glGetShaderiv(uFragmentShader, GL_COMPILE_STATUS, &status);
	shaderCompileLog(uFragmentShader);
	if (status != GL_TRUE){
		SDL_Log("loadShaderByString glCompileShader Fragment Shader return error");
		glDeleteShader(uVertexShader);
		glDeleteShader(uFragmentShader);
		return uProgram;
	}

	uProgram = glCreateProgram();
	glAttachShader(uProgram, uVertexShader);
	glAttachShader(uProgram, uFragmentShader);

	va_list attrList;
	va_start(attrList, szFragmentShader);
	int iArgCount = va_arg(attrList, int);
	for (int i = 0; i < iArgCount; i++){
		int index = va_arg(attrList, int);
		char *szNextArg = va_arg(attrList, char*);
		glBindAttribLocation(uProgram, index, szNextArg);
	}
	va_end(attrList);

	glLinkProgram(uProgram);
	shaderLinkLog(uProgram);
	glDeleteShader(uVertexShader);
	glDeleteShader(uFragmentShader);

	glGetProgramiv(uProgram, GL_LINK_STATUS,&status);
	if (status==GL_FALSE){
		SDL_Log("loadShaderByString glLinkProgram return error");
		glDeleteProgram(uProgram);
	}

	return uProgram;
}