class SurvivorBotDoIdle
{
    protected BotrBase m_Player;
    protected SurvivorBotBase m_Soldier;
	protected vector m_soldierPos;
	
	AIWorld world = GetGame().GetWorld().GetAIWorld();
	ref PGFilter m_pgFilterNav = new PGFilter();
	
	protected ref Timer m_OnRandomTimer;
	
    void SurvivorBotDoIdle(BotrBase m_Owner, SurvivorBotBase m_SoldierOwner) { m_Player = m_Owner; m_Soldier = m_SoldierOwner; OnEntry(); }
    void ~SurvivorBotDoIdle() { OnExit(); }

    void OnEntry()
    {		
		if (!m_OnRandomTimer) 
		{
			m_OnRandomTimer = new Timer(CALL_CATEGORY_GAMEPLAY);
		}
		m_pgFilterNav.SetFlags(PGPolyFlags.WALK, PGPolyFlags.INSIDE, 0);
		m_soldierPos = m_Soldier.GetPosition();
		m_OnRandomTimer.Run( Math.RandomInt(10, 15), this, "OnRandom", null, true );
    }
	


    void OnRandom()
    {	
		vector Navmesh;		
        if (m_Soldier)
        {
			if (m_Soldier.IsIdle() && !m_Soldier.GetTarget())
			{
				float l_x = m_soldierPos[0] + Math.RandomFloat(-30, 30);
				float l_z = m_soldierPos[2] + Math.RandomFloat(-30, 30);
				
				bool IsNavmesh = world.SampleNavmeshPosition( Vector(l_x, GetGame().SurfaceY(l_x, l_z), l_z), 2, m_pgFilterNav, Navmesh );
				if ( IsNavmesh )
					m_Soldier.SetBeginPosition( Navmesh );
				else
					m_Soldier.SetBeginPosition( Vector(l_x, GetGame().SurfaceY(l_x, l_z), l_z) );
				
				m_Soldier.GetDoMoveFSM().ResetWaypoints();
				
				if (!m_Soldier.IsVoice() && m_Soldier.GetUseVoice())
				{
					m_Soldier.SetIsVoice(true);
					GetGame().CreateSoundOnObject(m_Soldier, "Bot_idle_" + Math.RandomInt(1, 32).ToString(), 40, false);
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(m_Soldier.VoiceEnd, Math.RandomInt(6500, 20000), false); 
				//	m_Soldier.SleepVoice(12);
				}
			}
        }
    }
	
	void OnUpdate (float dt) 
    {
		
    }
	
	void OnExit()
	{
		Print("[esbfsm] OnExit " + this);
		if (m_OnRandomTimer)
		{
			m_OnRandomTimer.Stop();
			delete m_OnRandomTimer;
		}
	}
		
	vector CalculateRandomPosition()
	{
		m_Soldier.SetTarget(null);
		float l_x = m_Soldier.GetPosition()[0] + Math.RandomFloat(20 * -1, 20);
        float l_z = m_Soldier.GetPosition()[2] + Math.RandomFloat(20 * -1, 20);
        return Vector(l_x, GetGame().SurfaceY(l_x, l_z), l_z);
	}
}