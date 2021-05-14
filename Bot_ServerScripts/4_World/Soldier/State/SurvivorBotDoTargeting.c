class SurvivorBotDoTargeting
{
    protected BotrBase m_Player;
    protected SurvivorBotBase m_Soldier;

    ref Timer m_ResetTimer;
    ref Timer m_TargetTimer;

    void SurvivorBotDoTargeting(BotrBase m_Owner, SurvivorBotBase m_SoldierOwner) { m_Player = m_Owner; m_Soldier = m_SoldierOwner; OnEntry(); }
    void ~SurvivorBotDoTargeting() { OnExit(); }

    void OnEntry()
    {
	//	Print("[esbdt] OnEntry " + this);

        if (!m_TargetTimer) {
			m_TargetTimer = new Timer();
		}

		if (!m_ResetTimer) {
			m_ResetTimer = new Timer();
		}

        m_ResetTimer.Run(30, this, "OnResetTarget", NULL, true);
		m_TargetTimer.Run(1, this, "OnSelectTarget", NULL, true);
    }

	void OnExit () 
    {
        Print("[esbdt] OnExit " + this);

        if (m_ResetTimer)
        {
            m_ResetTimer.Stop();
            delete m_ResetTimer;
        }

        if (m_TargetTimer)
        {
            m_TargetTimer.Stop();
            delete m_TargetTimer;
        }
    }

    void OnTargeting()
	{
    }

	void OnUpdate (float dt) 
    {
    }
	


    bool IsVisible()
	{
		if (m_Soldier.GetTarget()) 
        {
			int headIndex = m_Soldier.GetBoneIndexByName("Head");
			
			vector rayStart = m_Soldier.GetBonePositionWS(headIndex);
			vector rayEnd = m_Soldier.GetTarget().GetPosition() + Vector(0, 1.5, 0);

   			auto objs = GetObjectsAt( rayStart, rayEnd, m_Soldier );

			if (objs) 
            {
			//	for (int o = 0; o < objs.Count(); ++o)
			//	{
			//		Print("Object = "+ " num " + o + " object - " + objs[o].GetType()));
			//	}
				if( objs.Count() > 0 ) 
                {
			if (objs[0].IsInherited(DayZPlayer) || objs[0].IsTransport() || objs[0].IsMan() || objs[0].IsTree() || objs[0].IsInherited(AnimalBase)) 
                    {
						if (!m_Soldier.IsVoice() && m_Soldier.GetUseVoice())
						{
							m_Soldier.SetIsVoice(true);
							GetGame().CreateSoundOnObject(m_Soldier, "Bot_attack_" + Math.RandomInt(1, 9).ToString(), 40, false);
							GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(m_Soldier.VoiceEnd, Math.RandomInt(6500, 15000), false); 
						}
						return true;
					}
					else if (!m_Soldier.IsVoice() && m_Soldier.GetUseVoice())
					{
						m_Soldier.SetIsVoice(true);
						GetGame().CreateSoundOnObject(m_Soldier, "Bot_search_" + Math.RandomInt(1, 12).ToString(), 40, false);
						GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(m_Soldier.VoiceEnd, Math.RandomInt(6500, 20000), false); 
					}
    			}
			}
		}
			
		return false;
	}
	
    set< Object > GetObjectsAt( vector from, vector to, Object ignore = NULL, float radius = 0.5, Object with = NULL )
	{
		vector contact_pos;
		vector contact_dir;
		int contact_component;

		set< Object > geom = new set< Object >;
		set< Object > view = new set< Object >;

		DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contact_component, geom, with, ignore, false, false, ObjIntersectGeom, radius );
		DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contact_component, view, with, ignore, false, false, ObjIntersectView, radius );

		if ( geom.Count() > 0 ) 
		{
			return geom;
		}
		if ( view.Count() > 0 ) 
		{
			return view;
		}
		return NULL;
	}
	
    set< Object > GetObjectsAtEnt( vector from, vector to, Object ignore = NULL, float radius = 1, Object with = NULL )
	{
		vector contact_pos;
		vector contact_dir;
		int contact_component;

		set< Object > geom = new set< Object >;
		set< Object > view = new set< Object >;

		DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contact_component, geom, with, ignore, false, false, ObjIntersectGeom, radius );
		DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contact_component, view, with, ignore, false, false, ObjIntersectView, radius );

		if ( geom.Count() > 0 ) 
		{
			return geom;
		}
		if ( view.Count() > 0 ) 
		{
			return view;
		}
		return NULL;
	}
	
 	void SendLog(string message) 
	{ 
		Print("AI BOT LOG: " + message);
	}
	
    bool IsEntityVisible(EntityAI Target)
	{
		if (Target) 
        {
			int headIndex = m_Soldier.GetBoneIndexByName("Head");
			
			vector rayStart = m_Soldier.GetBonePositionWS(headIndex);
			vector rayEnd = Target.GetPosition() + Vector(0, 1.5, 0);

   			auto objs2 = GetObjectsAtEnt( rayStart, rayEnd, m_Soldier );
			if (objs2) 
            {
				if( objs2.Count() > 0 ) 
                {
        			if (objs2[0] == Target || objs2[0].IsInherited(DayZPlayer) || objs2[0].IsTransport()) 
                    {
						return true;
					}
    			}
			}
		}

		return false;
	}


    void OnSelectTarget()
	{
		if (m_Soldier.IsPassive())
			return;
		
		if (m_Soldier.GetTarget())
			return;
		

		ref array<Man> players = new array<Man>;
		GetGame().GetPlayers( players );
		HumanMovementState b_State = new HumanMovementState;
		
		for ( int u = 0; u < players.Count(); u++ )
		{
					
			PlayerBase player;
			Weapon_Base weapon;
			Class.CastTo(player, players.Get(u));
			vector pos = player.GetPosition();
			float dist = vector.Distance( pos, m_Soldier.GetPosition() );
			player.GetMovementState(b_State);
			
				
			if ( dist < m_Soldier.GetDistance() && m_Soldier.IsAlive() && !player.IsBot() && player.IsAlive() && IsEntityVisible( player )) 
			{
				if ( m_Soldier.isFrendly() )
				{
					if (!player.isFrendlyPl())
					{
						m_Soldier.SetTarget( player );
						return;
					}
					
					if ( Weapon_Base.CastTo(weapon, player.GetItemInHands()) )
					{
						if (weapon.CanFire() && b_State.IsRaised() && !m_Soldier.GetTarget())
						{	
							m_Soldier.SetTarget( player );
						}
					}
				}
				else
				{
					if (!m_Soldier.GetTarget())
						m_Soldier.SetTarget( player );
				}
			} 
			else if (dist < m_Soldier.GetDistance() / 2 && !player.IsBot() && player.IsAlive() && m_Soldier.IsAlive())
			{
				if ( m_Soldier.isFrendly() )
				{
					if (!player.isFrendlyPl())
					{
						m_Soldier.SetTarget( player );
						return;
					}
					
					if ( Weapon_Base.CastTo(weapon, player.GetItemInHands()) )
					{
						if (weapon.CanFire() && b_State.IsRaised() && !m_Soldier.GetTarget())
						{	
							m_Soldier.SetTarget( player );
						}

					}
				}
				else
				{
					if (!m_Soldier.GetTarget())
						m_Soldier.SetTarget( player );
				}					
			}			
		}		
	}

    void OnResetTarget() 
	{ 
		if (m_Soldier.GetTarget() && !IsVisible()) 
		{ 
			m_Soldier.SetTarget(null);
			
			m_Soldier.GetDoMoveFSM().ResetWaypoints();
			
			if (!m_Soldier.GetDoMoveFSM().GetMove() && m_Soldier.IsBot())
            {
                m_Soldier.GetDoMoveFSM().SetMove();
            }

		//	if (m_Soldier.GetHcm() && m_Soldier.IsBot()) 
        //	{
         //   	m_Soldier.GetCommand_Move().ForceStance(DayZPlayerConstants.STANCEIDX_RAISEDERECT);
        //	}
		} 
	}
}