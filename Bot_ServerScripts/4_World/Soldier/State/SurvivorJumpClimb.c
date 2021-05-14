class SurvivorJumpClimb
{
	
	protected BotrBase m_Player;
	protected SurvivorBotBase m_Soldier;
	
    void SurvivorJumpClimb(BotrBase m_Owner, SurvivorBotBase m_SoldierOwner) { m_Player = m_Owner; m_Soldier = m_SoldierOwner; OnEntry();}
    void ~SurvivorJumpClimb() { OnExit(); }
	
	void bot_JumpOrClimb()
	{
		SHumanCommandClimbSettings b_hcls = m_Soldier.GetDayZPlayerType().CommandClimbSettingsW();
		
		if( m_Soldier.m_MovementState.m_iMovement != DayZPlayerConstants.MOVEMENTIDX_IDLE )
			b_hcls.m_fFwMaxDistance = 2.5;
		else
			b_hcls.m_fFwMaxDistance = 1.2;
		
		SHumanCommandClimbResult bot_climbRes = new SHumanCommandClimbResult();
		
		HumanCommandClimb.DoClimbTest(m_Soldier, bot_climbRes, 0);
		if( bot_climbRes.m_bIsClimb || bot_climbRes.m_bIsClimbOver )
		{
			int bot_climbType = GetClimbTypeBot(bot_climbRes.m_fClimbHeight);

			if ( !m_Soldier.bot_CanClimb( bot_climbType, bot_climbRes ) )
				return;

			if( bot_Climb(bot_climbRes) )
			{
				return;
			}
		}
		
		if ( !m_Soldier.bot_CanJump() )
			return;
		
		bot_Jump();
	}
	
	void bot_CheckAndFinishJump(int b_pLandType = 0)
	{
		if( bot_bIsJumpInProgress )
		{
			bot_bIsJumpInProgress = false;
			m_Soldier.bot_OnJumpEnd(b_pLandType);
		}
	}
	
	private bool bot_Climb(SHumanCommandClimbResult bot_pClimbRes)
	{
		int bot_climbType = GetClimbTypeBot(bot_pClimbRes.m_fClimbHeight);	
		if( bot_climbType != -1 )
			m_Soldier.StartCommand_Climb(bot_pClimbRes, bot_climbType);
		
		return bot_climbType != -1;
	}
	
	private void bot_Jump()
	{
		if (m_Soldier)
		{
			bot_bIsJumpInProgress = true;
			m_Soldier.SetFallYDiff(m_Soldier.GetPosition()[1]);

			m_Soldier.bot_OnJumpStart();
			m_Soldier.StartCommand_Fall(2.6);
			
		}
	}
	
	private int GetClimbTypeBot(float bHeight)
	{		
		int bot_climbType = -1;
        if( bHeight < 1.1 )
            bot_climbType = 0;
        else if( bHeight >= 1.1 && bHeight < 1.7 )
            bot_climbType = 1;
        else if( bHeight >= 1.7 && bHeight < 2.75 )
            bot_climbType = 2;    
        
        return bot_climbType;
	}
	
	void OnExit () 
    {
        Print("[esbdt] OnExit " + this);
	}
	
    void OnEntry()
    {
		
	}
		
	// Public variable members
	bool bot_bIsJumpInProgress = false;	
	
}
