#ifndef __CELL_H__
#define __CELL_H__

CellPartType getCARType(any cell);
CellPartType getCDRType(any cell);
void setCARType(any cell, CellPartType type);
void setCDRType(any cell, CellPartType type);
void setList(any cell);
int isList(any cell);
void setMark(any cell, int m);
int getMark(any cell);

#endif
