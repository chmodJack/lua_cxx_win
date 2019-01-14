#include <iostream>
#include <string.h>
#include<windows.h>
using namespace std;

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}


const char source_code[] =
"str=\"I am so cool\" \
tbl = {name = \"shun\",id = 20114442} \
function add(a, b) \
return a + b \
end \
avg, sum = cxx_func(1, 3, 5, 7) \
print(\"avg: \", avg) \
print(\"sum: \", sum) \
cxx_print_num(avg, sum) \
function all() \
cxx_print_num(avg + 3, sum + 3) \
end";


int cxx_func(lua_State* L)
{
	/* 得到参数个数 */
	int n = lua_gettop(L);

	double sum = 0;
	/* 循环求参数之和 */
	for(int i = 1; i <= n; i++)
	{
		//printf("i: %d -> v: %lf\n",i,lua_tonumber(L,i));
		/* 求和 */
		sum += lua_tonumber(L, i);
	}
	/* 压入平均值 */
	lua_pushnumber(L, sum / n);
	/* 压入和 */
	lua_pushnumber(L, sum);
	/* 返回返回值的个数 */
	return 2;
}
int cxx_print_num(lua_State* L)
{
	printf("hello, this is func: %s, file: %s, line: %d\n", __func__, __FILE__, __LINE__);
	int n = lua_gettop(L);

	for(int i = 1; i <= n; i++)
	{
		printf("%lf, ", lua_tonumber(L, i));
	}
	printf("\b\b.\n");
	return 0;
}

int main(int argc, char* argv[])
{
	//1.创建Lua状态
	lua_State* L = luaL_newstate();
	if(L == NULL)
	{
		return 0;
	}

	luaL_openlibs(L);
	lua_register(L, "cxx_func", cxx_func);
	lua_register(L, "cxx_print_num", cxx_print_num);

	//2.加载lua文件
	#if 1
	int bRet = luaL_loadfile(L, "./src/hello.lua");
	#else
	int bRet = luaL_loadstring(L, source_code);
	#endif
	if(bRet)
	{
		cout << "load file error" << endl;
		return 0;
	}

	//3.运行lua文件
	bRet = lua_pcall(L, 0, 0, 0);
	if(bRet)
	{
		cout << "pcall error" << endl;
		return 0;
	}

	//4.读取变量
	lua_getglobal(L, "str");
	string str = lua_tostring(L, -1);
	cout << "str = " << str.c_str() << endl;		//str = I am so cool~

													//5.读取table
	lua_getglobal(L, "tbl");
	lua_getfield(L, -1, "name");
	str = lua_tostring(L, -1);
	cout << "tbl:name = " << str.c_str() << endl; //tbl:name = shun

												  //6.读取函数
	lua_getglobal(L, "add");		// 获取函数，压入栈中
	lua_pushnumber(L, 10);			// 压入第一个参数
	lua_pushnumber(L, 20);			// 压入第二个参数
	int iRet = lua_pcall(L, 2, 1, 0);// 调用函数，调用完成以后，会将返回值压入栈中，2表示参数个数，1表示返回结果个数。
	if(iRet)						// 调用出错
	{
		const char* pErrorMsg = lua_tostring(L, -1);
		cout << pErrorMsg << endl;
		lua_close(L);
		return 0;
	}
	if(lua_isnumber(L, -1))        //取值输出
	{
		double fValue = lua_tonumber(L, -1);
		cout << "Result is " << fValue << endl;
	}

	while(1)
	{
		lua_getglobal(L, "all");
		lua_pcall(L, 0, 0, 0);
		Sleep(1000);
	}

	//至此，栈中的情况是：
	//=================== 栈顶 =================== 
	//  索引  类型      值
	//   4   int：      30 
	//   3   string：   shun 
	//	 2	 table:		tbl
	//   1   string:	I am so cool~
	//=================== 栈底 =================== 

	//7.关闭state
	lua_close(L);
	return 0;
}
