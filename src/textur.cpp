#include <gl/gl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <malloc.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

void Load_Texture( char *filename, GLuint *textureID, int swarp, int twarp, int filter) //Объявление функции Load_Texture, которая загружает текстуру из файла.
{
    int twidth, thight, tcnt; //Объявление переменных для ширины, высоты и количества цветовых каналов изображения.
    unsigned char *data=stbi_load(filename,&twidth,&thight,&tcnt,0); //Загрузка изображения с помощью stbi_load из файла filename, с указанием адресов переменных twidth, thight, и tcnt. data будет содержать пиксельные данные изображения.

    glGenTextures(1, textureID); //Создание одной текстуры и присвоение идентификатора, на который указывает textureID.
    glBindTexture(GL_TEXTURE_2D, *textureID); //Привязка текстуры с идентификатором *textureID в качестве 2D-текстуры.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,swarp); //Установка параметра обертки текстуры по оси S.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,twarp); //Установка параметра обертки текстуры по оси T.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter); //Установка параметра фильтрации текстуры при уменьшении.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter); //Установка параметра фильтрации текстуры при увеличении.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,twidth,thight, 0, tcnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data); //Определение 2D-текстуры
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}
