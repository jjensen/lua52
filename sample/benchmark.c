#include <time.h>
#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static int pmain(lua_State *L)
{
	luaL_openlibs(L);
	return 0;
}

void RefTest()
{
	int loop;
	int index;

	lua_State *L = luaL_newstate();
	lua_pushcfunction(L, &pmain);
	lua_pcall(L, 0, 0, 0);

#if LUA_FASTREF_SUPPORT
	lua_gc(L, LUA_GCCOLLECT, 0);

	/* lua_fastref */
	for (loop = 0; loop < 3; ++loop)
	{
		int* refs = malloc(1000000 * sizeof(int));
		clock_t time = clock();
		for (index = 1; index < 1000000; ++index)
		{
			lua_getglobal(L, "table");
			refs[index - 1] = lua_fastref(L);
		}
		for (index = 1; index < 1000000; ++index)
		{
//			lua_builtin_getref(L, refs[index - 1]);
//			lua_type(L, -1);
//			lua_pop(L, 1);
//			lua_builtin_getref(L, refs[index - 1]);
			lua_type(L, refs[index - 1]);
//			lua_pop(L, 1);
		}
		for (index = 1; index < 1000000; ++index)
		{
			lua_fastunref(L, refs[index - 1]);
		}
		free(refs);
		time = clock() - time;
		printf("lua_fastref(%d): %f\n", loop, (double)time / (double)CLOCKS_PER_SEC);
	}
#endif /* LUA_FASTREF_SUPPORT */

#if LUA_C41FASTREF_SUPPORT
	lua_gc(L, LUA_GCCOLLECT, 0);

	/* c41fastref */
	for (loop = 0; loop < 3; ++loop)
	{
		int* refs = malloc(1000000 * sizeof(int));
		clock_t time = clock();
		for (index = 1; index < 1000000; ++index)
		{
			lua_getglobal(L, "table");
			refs[index - 1] = lua_c41fastref(L);
		}
		for (index = 1; index < 1000000; ++index)
		{
			lua_type(L, refs[index - 1]);
		}
		for (index = 1; index < 1000000; ++index)
		{
			lua_c41fastunref(L, refs[index - 1]);
		}
		free(refs);
		time = clock() - time;
		printf("c41fastref(%d): %f\n", loop, (double)time / (double)CLOCKS_PER_SEC);
	}
#endif /* LUA_C41FASTREF_SUPPORT */

	lua_gc(L, LUA_GCCOLLECT, 0);

	/* luaL_ref */
	for (loop = 0; loop < 3; ++loop)
	{
		int* refs = malloc(1000000 * sizeof(int));
		clock_t time = clock();
		for (index = 1; index < 1000000; ++index)
		{
			lua_getglobal(L, "table");
			refs[index - 1] = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		for (index = 1; index < 1000000; ++index)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, refs[index - 1]);
			lua_type(L, -1);
			lua_pop(L, 1);
		}
		for (index = 1; index < 1000000; ++index)
		{
			luaL_unref(L, LUA_REGISTRYINDEX, refs[index - 1]);
		}
		free(refs);
		time = clock() - time;
		printf("luaL_ref(%d): %f\n", loop, (double)time / (double)CLOCKS_PER_SEC);
	}

#if LUA_FASTREF_SUPPORT
	lua_gc(L, LUA_GCCOLLECT, 0);

	/* lua_fastref-oneloop */
	for (loop = 0; loop < 3; ++loop)
	{
		clock_t time = clock();
		for (index = 1; index < 1000000; ++index)
		{
			int ref;
			lua_getglobal(L, "table");
			ref = lua_fastref(L);
			lua_type(L, ref);
			lua_fastunref(L, ref);
		}
		time = clock() - time;
		printf("lua_fastref-oneloop(%d): %f\n", loop, (double)time / (double)CLOCKS_PER_SEC);
	}
#endif /* LUA_FASTREF_SUPPORT */

#if LUA_C41FASTREF_SUPPORT
	lua_gc(L, LUA_GCCOLLECT, 0);

	/* lua_c41fastref-oneloop */
	for (loop = 0; loop < 3; ++loop)
	{
		clock_t time = clock();
		for (index = 1; index < 1000000; ++index)
		{
			int ref;
			lua_getglobal(L, "table");
			ref = lua_c41fastref(L);
			lua_type(L, ref);
			lua_c41fastunref(L, ref);
		}
		time = clock() - time;
		printf("c41fastref-oneloop(%d): %f\n", loop, (double)time / (double)CLOCKS_PER_SEC);
	}
#endif /* LUA_C41FASTREF_SUPPORT */

	lua_gc(L, LUA_GCCOLLECT, 0);

	/* luaL_ref-oneloop */
	for (loop = 0; loop < 3; ++loop)
	{
		clock_t time = clock();
		for (index = 1; index < 1000000; ++index)
		{
			int ref;
			lua_getglobal(L, "table");
			ref = luaL_ref(L, LUA_REGISTRYINDEX);

			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			lua_type(L, -1);
			lua_pop(L, 1);

			luaL_unref(L, LUA_REGISTRYINDEX, ref);
		}
		time = clock() - time;
		printf("luaL_ref-oneloop(%d): %f\n", loop, (double)time / (double)CLOCKS_PER_SEC);
	}

	lua_gc(L, LUA_GCCOLLECT, 0);

	/* Use light userdata */
	for (loop = 0; loop < 3; ++loop)
	{
		int* refs = malloc(1000000 * sizeof(int));
		clock_t time = clock();
		for (index = 1; index < 1000000; ++index)
		{
			lua_pushlightuserdata(L, &refs[index - 1]);
			lua_getglobal(L, "table");
			lua_rawset(L, LUA_REGISTRYINDEX);
		}
		for (index = 1; index < 1000000; ++index)
		{
			lua_pushlightuserdata(L, &refs[index - 1]);
			lua_rawget(L, LUA_REGISTRYINDEX);
			lua_type(L, -1);
			lua_pop(L, 1);
		}
		for (index = 1; index < 1000000; ++index)
		{
			lua_pushlightuserdata(L, &refs[index - 1]);
			lua_pushnil(L);
			lua_rawset(L, LUA_REGISTRYINDEX);
		}
		free(refs);
		time = clock() - time;
		printf("lightuserdata(%d): %f\n", loop, (double)time / (double)CLOCKS_PER_SEC);
	}

	lua_gc(L, LUA_GCCOLLECT, 0);

	for (loop = 0; loop < 3; ++loop)
	{
		clock_t time = clock();
		for (index = 1; index < 1000000; ++index)
		{
			int ref;
			lua_pushlightuserdata(L, &ref);
			lua_getglobal(L, "table");
			lua_rawset(L, LUA_REGISTRYINDEX);

			lua_pushlightuserdata(L, &ref);
			lua_rawget(L, LUA_REGISTRYINDEX);
			lua_type(L, -1);
			lua_pop(L, 1);

			lua_pushlightuserdata(L, &ref);
			lua_pushnil(L);
			lua_rawset(L, LUA_REGISTRYINDEX);
		}
		time = clock() - time;
		printf("lightuserdata-oneloop(%d): %f\n", loop, (double)time / (double)CLOCKS_PER_SEC);
	}

}


int main()
{
	RefTest();
}
