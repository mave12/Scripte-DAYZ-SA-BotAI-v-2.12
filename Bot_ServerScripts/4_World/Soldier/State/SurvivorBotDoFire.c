class SurvivorBotDoFire
{
    protected BotrBase m_Player;
    protected SurvivorBotBase m_Soldier;
    protected Weapon_Base m_Weapon;
	protected bool OnLift = false;
	protected float rand_timerShoot;
	protected vector hitPosWS;

    protected bool m_Reset;
    protected bool m_Hidden;
    protected bool m_Crouched;
	protected Magazine auto_Magazine;
	protected string new_Magazine;
	protected ref array<string> magazine_array = new array<string>;
	
	autoptr PGFilter pgFilter = new PGFilter();
	
	protected bool isHit = false;
	protected Human mTarget;

    void SurvivorBotDoFire(BotrBase m_Owner, SurvivorBotBase m_SoldierOwner) { m_Player = m_Owner; m_Soldier = m_SoldierOwner; OnEntry(); }
    void ~SurvivorBotDoFire() { OnExit(); }

    void OnEntry()
    {
		if (m_Soldier.GetWeaponName() != "")
		{
			GiveMag();
			GiveMag();
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater( OnWeapon, 1100, false );
		}
		m_Hidden = false;
		m_Crouched = false;
    }

    void OnExit()
    {
		Print("[esbfsm] OnExit " + this);
    }
	
    void OnWeapon()
    {
     //   if (!GetWeapon())
	//	{
		if (m_Soldier.GetItemInHands() && m_Soldier.GetWeaponName() != "")
			m_Soldier.GetItemInHands().Delete();
			
		m_Weapon = Weapon_Base.Cast(m_Soldier.GetHumanInventory().CreateInHands( m_Soldier.GetWeaponName() ));
		if (m_Weapon)
        {	
			for (int i = 0; i < m_Soldier.m_AttClassName.Count(); ++i)
			{	
				m_Weapon.GetInventory().CreateInInventory(m_Soldier.m_AttClassName.Get(i));		
			}
			m_Weapon.SetHealth("","",100);
			m_Soldier.ReloadWeap(m_Weapon);
		}

 
    }
	
	void GiveMag()
	{
     //   if (m_Soldier.GetWeaponName() != "")
    //    {
			new_Magazine = m_Soldier.GetMagazineName();
					
			if (new_Magazine == "")
			{
				GetGame().ConfigGetTextArray ("cfgWeapons " + m_Soldier.GetWeaponName() + " magazines", magazine_array);
				auto_Magazine = Magazine.Cast(m_Soldier.GetInventory().CreateInInventory(magazine_array.Get(Math.RandomInt(0, magazine_array.Count()))));
			}
			else
			{
				auto_Magazine = Magazine.Cast(m_Soldier.GetInventory().CreateInInventory(new_Magazine));
			}
			if (auto_Magazine)
				auto_Magazine.ServerSetAmmoMax();
			
			//	m_Soldier.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventAttachMagazine(m_Soldier, auto_Magazine) );	
			//	m_Soldier.ReloadWeap(m_Weapon);
	//	}			
        		
	}

    void OnFire(PlayerBase Target, bool Warning = false)
    {
	//	if (m_Soldier.IsBot()) m_Soldier.GetHcm().ForceStance(DayZPlayerConstants.STANCEIDX_RAISEDERECT);

        m_Soldier.SetTarget(Target);
    }

    void OnShoot()
    {		
    //    auto magAtt = m_Weapon.GetAttachmentByConfigTypeName( "DefaultMagazine" );
		auto magAtt = m_Weapon.GetMagazine(m_Weapon.GetCurrentMuzzle());
		
        if( magAtt )
        {
            auto mag = Magazine.Cast( magAtt );

			if( mag && mag.IsMagazine() )
			{
				mag.ServerSetAmmoMax();
			}
	    }
		m_Weapon.SetHealth("","",100);
		m_Soldier.GetWeaponManager().Fire(m_Weapon);  
		m_Soldier.GetInputController().OverrideAimChangeY(true, 0);		
    }
	

	float CalculateAimUD()
	{
		float m_AimSpeed = 0;
		
		if (m_Soldier)
		{
			float aimUD = m_Soldier.GetCommandModifier_Weapons().GetBaseAimingAngleUD();
			float delta = -((aimUD - m_Soldier.targetAngle)*Math.DEG2RAD);
			
		//	Print ("-------------------------- Test Aim delta = " + delta);
			
			if (aimUD == m_Soldier.targetAngle)
			{
				m_AimSpeed = 0;
				return m_AimSpeed;
			}
			
			if (aimUD > m_Soldier.targetAngle)
			{
				m_AimSpeed = delta;
				return m_AimSpeed;
			}
			
			if (aimUD < m_Soldier.targetAngle)
			{
				m_AimSpeed = delta;
				return m_AimSpeed;
			}

		}
		return m_AimSpeed;
	}
	
    void OnLift()
    {
				
        if (m_Soldier.GetCommand_Move() && m_Soldier.IsBot() && m_Soldier) 
        {		
			HumanMovementState b_State = new HumanMovementState;
			m_Soldier.GetMovementState(b_State);
			
			if (!b_State.IsRaised())
			{
				m_Soldier.GetCommand_Move().ForceStance(DayZPlayerConstants.STANCEIDX_RAISEDERECT);
				OnLift = false;
			}
			else 
			{
				OnLift = true;
			}
        }

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater( m_Soldier.SetIronsights, 100, false, true );
		
    }
	
	void SoldierFight()
	{
		float dist;
		int randomHit;
		vector pTargetPos;
		vector m_SoldierPos;
		if (!mTarget) return;
		
		if (mTarget.GetPosition())
		{
			pTargetPos = mTarget.GetPosition();
			m_SoldierPos = m_Soldier.GetPosition();
			HumanCommandMove b_cm = m_Soldier.GetCommand_Move();
			HumanCommandMelee2 b_hmc2;								
			dist = vector.Distance( pTargetPos, m_SoldierPos );
			
			randomHit = Math.RandomInt(1, 15);
			
			if (randomHit > 4)
			{
				isHit = true;
			}
			else
			{
				isHit = false;
			}
			
			if (dist < 1.5)
			{				
				if (!b_hmc2)
					b_hmc2 = m_Soldier.GetCommand_Melee2();
				
				m_Soldier.StartCommand_Melee2(mTarget, isHit, 1.2);
				if (b_cm)
				{
				//	b_cm.StartMeleeEvade();		
				}
				if (b_hmc2)
				{
					if (b_hmc2.IsInComboRange())
						b_hmc2.ContinueCombo(isHit, 1.2);		
				}
				
				hitPosWS = mTarget.ModelToWorld(mTarget.GetDefaultHitPosition());
				int randDamag = Math.RandomInt(1, 20);
				if (randDamag > 18)
				{
					DamageSystem.CloseCombatDamage(mTarget, mTarget, randDamag, "MeleeZombie", hitPosWS);
				}
			}
		}
	}
	
	void OnUpdate (float dt) 
    {
        if (m_Soldier.GetTarget() && m_Soldier.GetTarget().IsAlive() && m_Soldier.IsAlive()) 
		{
            m_Reset = true;
 			m_Soldier.GetInputController().OverrideAimChangeX(true, 0);
			m_Soldier.GetInputController().OverrideAimChangeY(true, CalculateAimUD());
			
            float m_Distance = vector.Distance(m_Soldier.GetTarget().GetPosition(), m_Soldier.GetPosition());
			if ((m_Distance < m_Soldier.GetDistance() || (m_Soldier.IsSniper() && (m_Distance < 1000))) && m_Soldier.GetDoTargetingFSM().IsVisible()) 
			{
	              
				if (m_Weapon && m_Distance > 1 && m_Weapon.GetMagazine(m_Weapon.GetCurrentMuzzle())) 
				{
									
					m_Soldier.GetDoMoveFSM().SetTarget(m_Soldier.GetTarget().GetPosition(), true, false, 1, m_Soldier.GetDistance() / 2);
					OnLift = false;
					int ___M___ = m_Weapon.GetCurrentMuzzle();
						
					if (m_Weapon.CanFire(___M___)) 
				//	if (m_Weapon.CanFire())
					{ 
                        Object Target = m_Soldier.GetTarget();
                        string Bullet = GetGame().ConfigGetTextOut( "CfgMagazines " + m_Weapon.GetChamberAmmoTypeName(m_Weapon.GetCurrentMuzzle()) + " " + "ammo" );
                       	OnLift();	
						
						vector groundCheckContactPos, groundCheckContactDir;
						
						pgFilter.SetFlags(PGPolyFlags.WALK | PGPolyFlags.ALL, 0, 0);
						bool hitAnObjNav = GetGame().GetWorld().GetAIWorld().RaycastNavMesh(m_Soldier.GetPosition(), m_Soldier.GetTarget().GetPosition(), pgFilter, groundCheckContactPos, groundCheckContactDir);						
						
						if (Math.RandomInt(1, 30) == 1) 
						{		
							if (m_Soldier.GetTarget().IsInherited(PlayerBase) && m_Soldier.GetDoTargetingFSM().IsVisible() && IsFacingTarget(Target)) 
							{		                             
								if (OnLift)
								{										
								//	OnLift = false;
									OnShoot();
									hitPosWS = Target.WorldToModel(Target.GetPosition());
								//	int randDamag = Math.RandomInt(2, 20);
									if (Math.RandomInt(1, m_Soldier.GetAcuracy()) == 1 && !hitAnObjNav)
										Target.ProcessDirectDamage(DT_FIRE_ARM, m_Soldier, "Torso", Bullet, hitPosWS, 1.0);
									//	DamageSystem.CloseCombatDamage(Target, Target, randDamag, Bullet, hitPosWS);					
								}
							}
							else if (m_Soldier.GetTarget().IsInherited(AnimalBase) && m_Soldier.GetDoTargetingFSM().IsVisible())
							{		                             
								if (OnLift)
								{
								//	Print("Attack bi animal!!!!!! " + m_Soldier.GetTarget().GetType());
								//	OnLift = false;
									OnShoot();
									hitPosWS = Target.WorldToModel(Target.GetPosition());
								//	int randDamag = Math.RandomInt(2, 20);
									if (!hitAnObjNav)
										Target.ProcessDirectDamage(DT_FIRE_ARM, m_Soldier, "Torso", Bullet, hitPosWS, 1.0);				
								}
							}	
						}				
                    }
					else
					{
						int mi = m_Weapon.GetCurrentMuzzle();						
						if (m_Weapon.GetMagazine(mi))
						{
							m_Soldier.ReloadWeaponBot(m_Weapon, m_Weapon.GetMagazine(mi));
							m_Weapon.GetMagazine(m_Weapon.GetCurrentMuzzle()).ServerSetAmmoMax();
						//	m_Soldier.ReloadWeap(m_Weapon);
						//	GiveMag();	
						}
					}
                }
                else 
				{
					mTarget = Human.Cast(m_Soldier.GetTarget());
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater( SoldierFight, 1000, false );
					OnLift = false;
				}
            }
            else
			{
				
                if (m_Distance > (m_Soldier.GetDistance() * 4)) 
				{					
                    OnReset();
                }   
				else if (m_Distance < 2)
				{
					mTarget = Human.Cast(m_Soldier.GetTarget());
					GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater( SoldierFight, 1000, false );
					OnLift = false;				
				}
                else 
				{
					
					if (m_Distance < (m_Soldier.GetDistance())) 
					{
		
						
						if (m_Soldier.IsRush() && m_Weapon)
						{
							if (m_Soldier.GetTarget())
								m_Soldier.GetDoMoveFSM().SetTarget(m_Soldier.GetTarget().GetPosition(), true, false, 1, 1.0);
						}
						else
						{
							if (m_Soldier.GetTarget())
								m_Soldier.GetDoMoveFSM().SetTarget(m_Soldier.GetTarget().GetPosition(), true, false, 1, 0.5);
						}
                    }
                } 
            }  
        }
        else 
		{		
            OnReset();
        }
    }

    void OnHide()
    {
        if (!m_Hidden && m_Soldier.GetTarget() && m_Soldier.GetTarget().IsAlive()) {
            float m_Distance = vector.Distance(m_Soldier.GetTarget().GetPosition(), m_Soldier.GetPosition());
            if (m_Distance < m_Soldier.GetDistance()) {
                m_Soldier.GetDoMoveFSM().SetTarget(m_Soldier.GetDoIdleFSM().CalculateRandomPosition());
                m_Hidden = true;
            }
        }
    }

    void OnRunaway()
    {
        if (!m_Crouched && m_Soldier.GetTarget() && m_Soldier.GetTarget().IsAlive()) {
            float m_Distance = vector.Distance(m_Soldier.GetTarget().GetPosition(), m_Soldier.GetPosition());
            if (m_Distance < m_Soldier.GetDistance()) {
                m_Soldier.GetDoMoveFSM().SetTarget(m_Soldier.GetDoIdleFSM().CalculateRandomPosition());
                if (m_Soldier.GetCommand_Move()) 
                {
                    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater( m_Soldier.GetCommand_Move().ForceStance, 10000, false, DayZPlayerConstants.STANCEIDX_RAISEDPRONE );
                    m_Crouched = true;
                }
            }
        }
    }

    void OnReset()
    {
        if (m_Reset)
        {

            if (m_Soldier.GetTarget())
            {
                m_Soldier.SetTarget(null);
            } 

			if (m_Soldier.GetItemInHands() && m_Soldier.GetWeaponName() != "")
			{
				if (!m_Weapon.GetMagazine(m_Weapon.GetCurrentMuzzle()))
				{
					GiveMag();
					m_Soldier.ReloadWeap(m_Weapon);
				}
			}
			
            if (m_Soldier.GetCommandModifier_Weapons())
            {
                m_Soldier.GetCommandModifier_Weapons().LiftWeapon(false);
			//	m_Soldier.GetHcw().SetADS(false);
            }
			
			m_Soldier.GetInputController().OverrideRaise(true, true);
			
            m_Soldier.GetDoMoveFSM().SetMove();

            m_Reset = false;
        }
    }

    bool IsFacingTarget( Object Target )
	{
		vector pdir = m_Soldier.GetDirection();
		vector ptv = Target.GetPosition() - m_Soldier.GetPosition();

		pdir.Normalize();
		ptv.Normalize();

		if (Math.AbsFloat(pdir[0] - ptv[0]) < 0.5 && Math.AbsFloat(pdir[2] - ptv[2]) < 0.5 )
		{
			return true;			
		}

		return false;
	}
 	void SendLog(string message) 
	{ 
		Print("AI BOT LOG: " + message);
	}
	
    void SetHidden(bool State) { m_Hidden = State; }
    void SetCrouched(bool State) { m_Crouched = State; }
    void SetWeapon(Weapon_Base Weap) { m_Weapon = Weap; }

    bool GetHidden() { return m_Hidden; }
    bool GetCrouched() { return m_Crouched; }
    
    Weapon_Base GetWeapon() { return m_Weapon; }
}