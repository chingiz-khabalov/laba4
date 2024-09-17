#include <gl/gl.h>
#include <stdio.h> //Подключение функций стандартного ввода/вывода.
#include <stdlib.h> //Подключение функций работы с памятью и общих функций C.
#include <malloc.h> //Подключение функций управления динамической памятью.
#include "menu.h"
#include "textur.h"
#include "stb_easy_font.h"
#define nameLen 20 //Макрос, определяющий максимальную длину имени кнопки.

typedef struct //Определение структуры Button, которая представляет кнопку.
{
    char name[nameLen]; //Имя кнопки.
    float vert[8]; //Координаты вершин прямоугольника кнопки.
    char isHover, isDown; //Флаги состояния кнопки (наведена и нажата).
    float buffer[50*nameLen]; //Буфер для хранения данных о тексте.
    int num_quads; //Количество квадов для отображения текста.
    float textPosX,textPosY,textS; //Параметры для позиционирования и масштабирования текста.
} Button;
Button *btn; //Указатель на массив кнопок.
int btnCnt = 0; //Количество кнопок.
float mouseX,mouseY; //Координаты мыши.

int Menu_AddButton(char *name, float x, float y, float width, float height, float textS) //Добавление кнопки
{
    btnCnt++; //Увеличение счетчика кнопок.
    btn = (Button*)realloc(btn, sizeof(btn[0])*btnCnt); //Изменение размера массива кнопок.

    snprintf(btn[btnCnt-1].name, nameLen, "%s", name);  //Копирование имени кнопки.
    float *vert = btn[btnCnt-1].vert; //Инициализация координат вершин кнопки.
    vert[0]=vert[6]=x;
    vert[2]=vert[4]=x+width;
    vert[1]=vert[3]=y;
    vert[5]=vert[7]=y+height;
    btn[btnCnt-1].isHover=0; //Установка начальных значений для флагов isHover и isDown.
    btn[btnCnt-1].isDown=0;

    Button *b = btn + btnCnt - 1;
    b->num_quads = stb_easy_font_print(0, 0, name,0,b->buffer, sizeof(b->buffer)); //Подготовка буфера для отображения текста.
    b->textPosX = x +(width-stb_easy_font_width(name)*textS)/2.0; //Позиция текста.
    b->textPosY = y +(height-stb_easy_font_height(name)*textS)/2.0;
    b->textPosY += textS*2;
    b->textS = textS; //Масштаб текста.

    return btnCnt-1;
}

void ShowButton(int buttonId)
{
    Button btn1 = btn[buttonId]; //Получение кнопки по идентификатору.
    glVertexPointer(2, GL_FLOAT, 0, btn1.vert);  //Указание вершин кнопки.
    glEnableClientState(GL_VERTEX_ARRAY); //Включение массива вершин.
    if(btn1.isDown) //Установка цвета в зависимости от состояния кнопки.
    {
        glColor3f(0.3, 0.6, 0.9);
    }
    else if (btn1.isHover)
    {
        glColor3f(0.3, 0.6, 0.9);
    }
    else
    {
        glColor3f(1.0, 1.0, 1.0);
    }

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4); //Рисование прямоугольника кнопки.

    glLineWidth(1);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glTranslatef(btn1.textPosX,btn1.textPosY,0);
    glScalef(btn1.textS,btn1.textS,0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, btn1.buffer);
    glDrawArrays(GL_QUADS, 0, btn1.num_quads*4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
}

void Menu_ShowMenu() //Цикл для отображения всех кнопок меню.
{
    for(int i=0; i<btnCnt; i++)
    {
        ShowButton(i);
    }
}

char isCordInButton(int buttonID, float x, float y) //Проверка, находятся ли координаты (x, y) внутри прямоугольника кнопки.
{
    float *vert=btn[buttonID].vert;
    return (x>vert[0]) && (y>vert[1]) && (x<vert[4]) && (y<vert[5]);
}

int Menu_MouseMove (float x, float y) //Обработка движения мыши
{
    mouseX = x; //Обновление координат мыши.
    mouseY = y;
    int movebtn =- 1;
    for (int i = 0; i < btnCnt; i++) //Проверка, находится ли мышь над кнопкой, и установка соответствующих флагов.
    {
        if (isCordInButton(i, mouseX, mouseY))
        {
            btn[i].isHover =1;
            movebtn = i;
        }
        else
        {
            btn[i].isDown = 0;
            btn[i].isHover = 0;
        }
    }
    return movebtn;
}

int Menu_MouseDown() //Обработка нажатия мыши
{
    int downbtn =- 1;
    for (int i = 0; i < btnCnt; i++) //Проверка, нажата ли кнопка мыши над кнопкой, и установка соответствующих флагов.
    {
        if (isCordInButton(i, mouseX, mouseY))
        {
            btn[i].isDown =1;
            downbtn = i;
        }
    }
    return downbtn;
}

void Menu_MouseUp(){ //Обработка отпускания мыши
    for (int i = 0; i < btnCnt; i++) //Сброс флага нажатия для всех кнопок.
    {
        btn[i].isDown = 0;
    }
}

char *Menu_GetButtonName(int buttonID) //Получение имени кнопки
{
    return btn[buttonID].name; //Возвращение имени кнопки по идентификатору.
}

void Menu_Clear() //Очистка кнопок
{
    btnCnt = 0;
    free(btn);
    btn = 0;
}
