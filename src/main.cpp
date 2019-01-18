#include <iostream>
#include <string.h>
#include <windows.h>
using namespace std;

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
	//#include "lobject.h"
}

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

namespace jack
{
	class lua
	{
	public:
		lua(void)
		{
			L = luaL_newstate();
			if(L == nullptr)
			{ 
				exit(-1);
			}

			luaL_openlibs(L);
		}
		~lua(void)
		{
			lua_close(L);
			L = nullptr;
		}

		void register_cfunction(const char* name, lua_CFunction func)
		{
			lua_register(L, name, func);
		}

		int load_file(const char* file)
		{
			return luaL_loadfile(L, file);
		}
		int call(int nargs, int nresults, int errfunc)
		{
			return lua_pcall(L, nargs, nresults, errfunc);
		}

		int getglobal(const char* name)
		{
			return lua_getglobal(L, name);
		}
		int getfield(int idx, const char *k)
		{
			return lua_getfield(L, idx, k);
		}

		void pushnil(void)
		{
			lua_pushnil(L);
		}
		void pushnumber(lua_Number num)
		{
			lua_pushnumber(L, num);
		}
		void pushinteger(lua_Integer integer)
		{
			lua_pushinteger(L, integer);
		}
		const char* pushstring(const char* s)
		{
			return lua_pushstring(L, s);
		}
		
		int iscfunction(int idx = -1)
		{
			return lua_iscfunction(L, idx);
		}
		int isinteger(int idx = -1)
		{
			return lua_isinteger(L, idx);
		}
		int isnumber(int idx = -1)
		{
			return lua_isnumber(L, idx);
		}
		int isstring(int idx = -1)
		{
			return lua_isstring(L, idx);
		}
		int isuserdata(int idx = -1)
		{
			return lua_isuserdata(L, idx);
		}

		lua_Number tonumber(int idx=-1)
		{
			return lua_tonumber(L, idx);
		}
		lua_Integer tointeger(int idx=-1)
		{
			return lua_tointeger(L, idx);
		}
		const char* tostring(int idx = -1)
		{
			return lua_tostring(L, idx);
		}
		lua_CFunction tocfunction(int idx = -1)
		{
			return lua_tocfunction(L, idx);
		}
		const void* topointer(int idx = -1)
		{
			return lua_topointer(L, idx);
		}

		static int lua_c_func_demo(lua_State* L)
		{
			int n = lua_gettop(L);
			printf("Hello world! %d arguments input, \n", n);

			for(int i = 1; i <= n; i++)
			{
				printf("	%lf\n", lua_tonumber(L, i));
			}

			printf("return 233 666\n");

			lua_pushnumber(L, 233);
			lua_pushnumber(L, 666);

			//two return value
			return 2;
		}
	public:
		lua_State* L;
	};
}

#define OLD 0
#if OLD

int main(int argc, char* argv[])
{
	//1.创建Lua状态
	lua_State* L = luaL_newstate();
	if(L == NULL){return 0;}
	
	luaL_openlibs(L);
	lua_register(L, "cxx_func", cxx_func);
	lua_register(L, "cxx_print_num", cxx_print_num);
	
	//2.加载lua文件
	int bRet = luaL_loadfile(L, "./src/hello.lua");
	if(bRet){cout << "load file error" << endl;return 0;}

	//3.运行lua文件
	bRet = lua_pcall(L, 0, 0, 0);
	if(bRet){cout << "pcall error" << endl;return 0;}

	//4.读取变量
	lua_getglobal(L, "str");
	string str = lua_tostring(L, -1);
	cout << "str = " << str.c_str() << endl;//str = I am so cool~

	//5.读取table
	lua_getglobal(L, "tbl");
	lua_getfield(L, -1, "name");
	str = lua_tostring(L, -1);
	cout << "tbl:name = " << str.c_str() << endl; //tbl:name = shun

	//6.读取函数
	lua_getglobal(L, "add");		 // 获取函数，压入栈中
	lua_pushnumber(L, 10);			 // 压入第一个参数
	lua_pushnumber(L, 20);			 // 压入第二个参数
	int iRet = lua_pcall(L, 2, 1, 0);// 调用函数，调用完成以后，会将返回值压入栈中，2表示参数个数，1表示返回结果个数。
	if(iRet)						 // 调用出错
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

	
	//至此，栈中的情况是：
	//=================== 栈顶 =================== 
	//  索引  类型      值
	//   4   int：      30 
	//   3   string：   shun 
	//	 2	 table:		tbl
	//   1   string:	I am so cool~
	//=================== 栈底 =================== 


	while(1)
	{
		lua_getglobal(L, "all");
		lua_pcall(L, 0, 0, 0);
		Sleep(1000);
	}

	//7.关闭state
	lua_close(L);
	return 0;
}


#else

int main(int argc, char* argv[])
{
	jack::lua l;
	return 0;
}

#endif