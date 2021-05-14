class CfgMods
{
	class BotSoldier_ServerScripts
	{
		dir = "BotSoldier_ServerScripts";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "";
		credits = "";
		author = "VasiliyP";
		authorID = "0";
		version = 1.0;
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"BotSoldier_ServerScripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"BotSoldier_ServerScripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"BotSoldier_ServerScripts/5_Mission"};
			};
		};
	};
};
class CfgPatches
{
	class BotSoldier_ServerScripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {};
	};
};
