enum M_RPC_bc
{
	BOT_ON_REMOTE_WPN = 194146;
}

WeaponEventBase CreateWeaponEventFromContextBot (int packedType, DayZPlayer player, Magazine magazine)
{
	WeaponEventID eventID = packedType >> 16;
	WeaponEvents animEvent = packedType & 0x0000ffff;
	WeaponEventBase b = WeaponEventFactory(eventID, animEvent, player, magazine);
	return b;
}

bool DayZPlayerInventory_OnEventForRemoteWeaponBot(int packedType, DayZPlayer player, Magazine magazine)
{
	PlayerBase pb = PlayerBase.Cast(player);
	
	if (!pb) 
	{
		return false;
	}
	
	if (!pb.GetDayZPlayerInventory()) 
	{
		return false;
	}
	
	if (pb.GetDayZPlayerInventory().GetEntityInHands())
	{
		Weapon_Base wpn = Weapon_Base.Cast(pb.GetDayZPlayerInventory().GetEntityInHands());
		if (wpn)
		{
			
			WeaponEventBase e = CreateWeaponEventFromContextBot(packedType, player, magazine);
			
			if (pb && e)
			{
				pb.GetWeaponManager().SetRunning(true);
	
				fsmDebugSpam("[wpnfsm] " + Object.GetDebugName(wpn) + " recv event from remote: created event=" + e);
				if (e.GetEventID() == WeaponEventID.HUMANCOMMAND_ACTION_ABORTED)
				{
					wpn.ProcessWeaponAbortEvent(e);
				}
				else
				{
					wpn.ProcessWeaponEvent(e);
				}
				pb.GetWeaponManager().SetRunning(false);
			}
		}
		else
			Error("OnEventForRemoteWeaponBot - entity in hands, but not weapon. item=" + pb.GetDayZPlayerInventory().GetEntityInHands());
	}
	else
		Error("OnEventForRemoteWeaponBot - no entity in hands");
	return true;
}



modded class PlayerBase
{
	//---------Only bot---------//
	protected bool m_IsBot = false;
	protected bool m_PlFrendly = true;
    protected string m_BotName;
	string GetNameBot() { return m_BotName; }
	
	bool IsBot()
	{
		return m_IsBot;
	}	

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		switch(rpc_type)
		{
			case M_RPC_bc.BOT_ON_REMOTE_WPN:
				private int packType;
				private PlayerBase man;
				private Magazine mag;
				
				ctx.Read(packType);
				ctx.Read(man);
				ctx.Read(mag);
									
				if ( GetGame().IsClient() && GetGame().IsMultiplayer() ) 
				{					
					DayZPlayerInventory_OnEventForRemoteWeaponBot(packType, man, mag);
				}
			break;
		}
	}
	

	

	
	void SetFrendly(bool Result) { m_PlFrendly = Result; }
	bool isFrendlyPl() { return m_PlFrendly; }	
}