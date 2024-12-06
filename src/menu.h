#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED



int Menu_AddButton(char *name, float x, float y, float width, float height, float textS);
void Menu_ShowMenu();
int Menu_MouseMove(float x, float y);
int Menu_MouseDown();
void Menu_MouseUp();
char *Menu_GetButtonName(int buttonID);
void Menu_Clear();
void MouseDown();

#endif // MENU_H_INCLUDED
