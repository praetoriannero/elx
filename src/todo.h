#pragma once

void todo_impl(const char* file, const int line, const char* func);

#define todo(...) todo_impl(__FILE__, __LINE__, __func__)
