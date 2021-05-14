
modded class Weapon_Base 
{	
	override bool ProcessWeaponEvent(WeaponEventBase e)
	{
		
		if (GetGame().IsServer() && PlayerBase.Cast(e.m_player).IsBot()) 
		{
			ScriptRemoteInputUserData ctx = new ScriptRemoteInputUserData;			
			array<Man> players = new array<Man>;
			GetGame().GetPlayers( players );
			
			for (int i = 0; i < players.Count(); ++i)
			{
				PlayerBase t_Player = PlayerBase.Cast(players.Get(i));
				ScriptRPC m_RPC = new ScriptRPC();
				
				if (vector.Distance(t_Player.GetPosition(), e.m_player.GetPosition()) < 1500)
				{
					m_RPC.Write(e.GetPackedType());
					m_RPC.Write(e.m_player);
					m_RPC.Write(e.m_magazine);
				
					if (t_Player && !t_Player.IsBot())
						m_RPC.Send(t_Player, M_RPC_bc.BOT_ON_REMOTE_WPN, true, t_Player.GetIdentity());
				}
			}
			
			if (m_fsm.ProcessEvent(e) == ProcessEventResult.FSM_OK)
				return true;
				
			return false;
		
		}
		else if (!PlayerBase.Cast(e.m_player).IsBot())
			SyncEventToRemote(e);
		
		if (e.GetEventID() == WeaponEventID.SET_NEXT_MUZZLE_MODE)
		{
			SetNextMuzzleMode(GetCurrentMuzzle());
			return true;
		}

		if (m_fsm.ProcessEvent(e) == ProcessEventResult.FSM_OK)
			return true;

		return false;
	}	
	/*
	override bool ProcessWeaponAbortEvent(WeaponEventBase e)
	{
		if (GetGame().IsServer() && PlayerBase.Cast(e.m_player).IsBot()) 
		{ 
			ScriptRemoteInputUserData ctx = new ScriptRemoteInputUserData;
			if (m_fsm.ProcessEvent(e) == ProcessEventResult.FSM_OK)
				return true;
			
			return false;
		} 
		else if (!PlayerBase.Cast(e.m_player).IsBot())
			SyncEventToRemote(e);
		
		ProcessEventResult aa;
		m_fsm.ProcessAbortEvent(e, aa);
		return aa == ProcessEventResult.FSM_OK;
	} */
}