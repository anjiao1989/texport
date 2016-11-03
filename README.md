# texport
texport(Tchar EXPORT stub generator) 用于自动生成 TCHAR 类型程序的 char 和 wchar_t 类型的函数调用存根

当在 windows 下开发程序库时，使用 TCHAR 类型，比直接使用 char 和 wchar_t 具有更好的移植性。但是在编写动态链接库时，不能直接导出 TCHAR 类型的函数，给 TCHAR 类型函数的使用带来了一些限制。使用 texport 在 .h　文件中加入特定的标记，texport 能自动生成类似于标准 SDK 不同字符集导出函数的格式。给使用 TCHAR 类型编写动态库带来一些便利。

例：
##########################################################################
## file trans.txt
LPTSTR, LPWSTR, LPSTR
LPCTSTR, LPCWSTR, LPCSTR

##########################################################################
## file input.h

//%{

void Test(LPTSTR abc);

//%}

###########################################################################
## 命令行
texport input.h

###########################################################################
## 输出
input.texport.h
input.texport.cpp

程序的输入文件：
一个使用 texport 规则标记过的 .h　文件，一个内置的或用户指定的转换规则文件

程序的输出文件：
<输入文件名>.texport.h
<输入文件名>.texport.cpp
