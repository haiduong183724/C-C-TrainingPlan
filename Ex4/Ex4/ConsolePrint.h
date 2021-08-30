#pragma once
#ifndef CONSOLEPRINT_H
#define CONSOLEPRINT_H

#include "Print.h"
#include<iostream>
/// <summary>
/// Lớp in giá trị ra màn hình console
/// </summary>
class ConsolePrint :
    public Print
{
    // Inherited via Print
    virtual void print(const char* printString) override;
};

#endif // !CONSOLEPRINT_H


