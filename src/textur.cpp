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

void Load_Texture( char *filename, GLuint *textureID, int swarp, int twarp, int filter) //���������� ������� Load_Texture, ������� ��������� �������� �� �����.
{
    int twidth, thight, tcnt; //���������� ���������� ��� ������, ������ � ���������� �������� ������� �����������.
    unsigned char *data=stbi_load(filename,&twidth,&thight,&tcnt,0); //�������� ����������� � ������� stbi_load �� ����� filename, � ��������� ������� ���������� twidth, thight, � tcnt. data ����� ��������� ���������� ������ �����������.

    glGenTextures(1, textureID); //�������� ����� �������� � ���������� ��������������, �� ������� ��������� textureID.
    glBindTexture(GL_TEXTURE_2D, *textureID); //�������� �������� � ��������������� *textureID � �������� 2D-��������.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,swarp); //��������� ��������� ������� �������� �� ��� S.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,twarp); //��������� ��������� ������� �������� �� ��� T.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter); //��������� ��������� ���������� �������� ��� ����������.
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter); //��������� ��������� ���������� �������� ��� ����������.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,twidth,thight, 0, tcnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data); //����������� 2D-��������
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);
}
