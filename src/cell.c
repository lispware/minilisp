#include "lisp.h"
#include "cell.h"

CellPartType getCARType(any cell)
{
    return cell->type.parts[0];
}

CellPartType getCDRType(any cell)
{
    return cell->type.parts[1];
}

void setCARType(any cell, CellPartType type)
{
    if (!cell) return;
    cell->type.parts[0] = type;
}

void setCDRType(any cell, CellPartType type)
{
    if (!cell) return;
    cell->type.parts[1] = type;
}

void setList(any cell)
{
    cell->type.parts[2] = 1;
}

void setMark(any cell, int m)
{
    cell->type.parts[3] = m;
}

int getMark(any cell)
{
    return cell->type.parts[3];
}

