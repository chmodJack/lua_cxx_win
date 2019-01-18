#include <iostream>
#include <string.h>
#include <windows.h>
using namespace std;

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
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
			//lua_register(L, name, func);
			(lua_pushcfunction(L, (func)), lua_setglobal(L, (name)));
		}

		int load_file(const char* file)
		{
			return luaL_loadfile(L, file);
		}
		int load_string(const char* s)
		{
			return luaL_loadstring(L, s);
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

	public:
		static int demo(lua_State* L)
		{
			int n = lua_gettop(L);
			printf("Hello world! %d arguments input, \n", n);

			for(int i = 1; i <= n; i++)
			{
				printf("	%lld\n", lua_tointeger(L, i));
			}

			printf("return 233 666\n");

			lua_pushinteger(L, 233);
			lua_pushinteger(L, 666);

			//two return value
			return 2;
		}
		static int sleep(lua_State* L)
		{
			Sleep(lua_tointeger(L, -1) * 1000);
			return 0;
		}
		static int msleep(lua_State* L)
		{
			Sleep(lua_tointeger(L, -1));
			return 0;
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

const char* code = "demo(1,22,333,4444)";

int main(int argc, char* argv[])
{
	jack::lua l;

	l.register_cfunction("demo", l.demo);
	l.register_cfunction("sleep", l.sleep);
	l.register_cfunction("msleep", l.msleep);

	l.load_file("./src/main.lua");
	//l.load_string(code);
	l.call(0,0,0);

	system("pause");
	return 0;
}

#endif