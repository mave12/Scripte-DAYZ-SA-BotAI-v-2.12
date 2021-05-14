class AimProfile 
{
	int lastUpdated = -1;
	vector out_front, out_back;
	float Azmuith;
	void AimProfile(vector out_f, vector out_b) 
	{
		out_front = out_f;
		out_back = out_b;
		lastUpdated = GetGame().GetTime();
	}
};

class SurvivorBotBase: BotrBase
{
	ref Timer m_RemoveDeathBodyBot;
	protected EntityAI m_Target;
	protected HumanInputController m_Hic;
	protected HumanItemAccessor m_Hia;
	protected HumanCommandWeapons m_Hcw;
	protected HumanCommandMove m_Hcm;
	
	protected ref SurvivorBotDoFire m_DoFire;
	protected ref SurvivorBotDoMove m_DoMove;
	protected ref SurvivorBotDoTargeting m_DoTargeting;
	protected ref SurvivorBotDoIdle m_DoIdle;
	protected ref SurvivorJumpClimb m_DoJump;
	ref ActionManagerBot m_ActionManagerbot;

	protected vector m_BeginPosition;
	protected vector m_EndPosition;
	protected vector m_CurrentPosition;
	
	protected bool m_PassiveBot;
	protected bool m_HeroTraderBot;
	protected bool m_BanditTraderBot;
	protected bool m_IdleBot;
	protected bool m_SniperBot;
	protected bool m_RushBot;
	protected bool m_onRespawned;
	protected bool useVoice;
	protected bool m_onStandUp;
	protected bool bot_KillFeed;
	protected bool m_isFrendly;
	protected bool m_ChcPointC;
	protected int m_SetSpeed;
	protected int m_TimeDeathBody;
	protected bool m_IsVoice;
	protected bool m_CanOpenDoors;
	
	
	protected float m_AcuracyBot;
	protected float m_DistanceBot;
	protected float m_RandomBot;
	protected float m_TimeBot;
    protected bool m_UseCheckpoint;

	protected string m_MagazineName;
	protected string m_WeaponName;
	
    ref array<string> m_AttClassName = new array<string>;
	
	float targetAngle;
	
//    ref array<vector> m_ArrayCheckpoint = new array<vector>;
	ref TVectorArray m_ArrayCheckpoint = new TVectorArray;
	
    void SurvivorBotBase() { SetEventMask(EntityEvent.SIMULATE | EntityEvent.INIT); }
    void ~SurvivorBotBase() { }

	override void EOnSimulate(IEntity owner, float dt) { if (IsAlive()) OnUpdate(dt); }
	override void EOnInit( IEntity other, int extra) { OnEntry(); }

    void OnEntry()
    {
	//	Print("[esbfsm] OnEntry " + this);

		BotrBase l_Player;
		Class.CastTo(l_Player, this);
		
		BotNameGenerator.BotNameGeneratorInsert();
		
		m_AcuracyBot = 5;
		m_DistanceBot = 100;
		
		m_SetSpeed = 2;
		
		m_TimeDeathBody = 120;
		
		m_RandomBot = 5;
        m_TimeBot = 10000;
		
		targetAngle = -5.0;

		m_PassiveBot = false;
		m_UseCheckpoint = false;
		m_HeroTraderBot = false;
		m_BanditTraderBot = false;
		m_SniperBot = false;
		m_IsVoice = false;
		m_RushBot = true;
		m_IdleBot = true;
		useVoice = false;
		bot_KillFeed = false;
		m_onStandUp = false;
		m_IsBot = true;
		m_onRespawned = true;
		m_ChcPointC = true;
		m_isFrendly = false;
		m_MagazineName = "";
		m_WeaponName = "";
		m_CanOpenDoors = true;

		m_BeginPosition = GetPosition();
        m_EndPosition = "0 0 0";
        m_CurrentPosition = GetPosition();


		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater( OnFsm, 2000, false );
		
		m_BotName = BotNameGenerator.GetNameRandom();
		if (!m_RemoveDeathBodyBot) {
			m_RemoveDeathBodyBot = new Timer();
		}
    }
	
	void OnFsm()
	{
		StartCommand_Move();

		m_DoMove = new SurvivorBotDoMove(this, this);
		m_DoFire = new SurvivorBotDoFire(this, this);
		m_DoTargeting = new SurvivorBotDoTargeting(this, this);
		m_DoIdle = new SurvivorBotDoIdle(this, this);
		m_DoJump = new SurvivorJumpClimb(this, this);
		m_ActionManagerbot = new ActionManagerBot(this);
		
		m_Hic = GetInputController();
		m_Hia = GetItemAccessor();
		m_Hcw = GetCommandModifier_Weapons();
		m_Hcm = GetCommand_Move();
	}

	void OnExit() 
    {
		Print("[esbfsm] OnExit " + this);

		m_BeginPosition = "0 0 0";
		m_EndPosition = GetPosition();
		m_CurrentPosition = "0 0 0";

		if (m_DoFire)
			delete m_DoFire;
		
		if (m_DoMove)
			delete m_DoMove;

		if (m_DoTargeting)
			delete m_DoTargeting;

		if (m_DoIdle)
			delete m_DoIdle;
		
		if (m_DoJump)
			delete m_DoJump;
		
		if (m_ActionManagerbot)
			delete m_ActionManagerbot;

		SetTarget(null);
		m_RemoveDeathBodyBot.Run(m_TimeDeathBody * 60, this, "RemoveDeadhBody", NULL, false);
            
		SetEventMask(EntityEvent.INIT);
		this = NULL;
    }
	
	override void CheckLiftWeapon()
	{
		bool state = false;
			
		Weapon_Base weap;
			
		if ( Weapon_Base.CastTo(weap, GetItemInHands()) )
		{
			state = m_LiftWeapon_player;
			bool limited = weap.LiftWeaponCheck(this);
			if (limited)
				state = false;
		}
			
		ScriptJunctureData pCtx = new ScriptJunctureData;
		
		pCtx.Write(state);
		SendSyncJuncture(DayZPlayerSyncJunctures.SJ_WEAPON_LIFT, pCtx);
	}
	
	void SleepVoice(int sek)
	{
		ref Timer m_OnSound = new Timer(CALL_CATEGORY_GAMEPLAY);
		m_OnSound.Run( sek, this, "VoiceEnd" );	
	}
	
	void OnUpdate(float dt) 
    {
		static float l_dtAccumulator;

		l_dtAccumulator += dt;
		if (l_dtAccumulator >= 0.01)
		{		
			if (m_DoFire)
				m_DoFire.OnUpdate( dt );

			if (m_DoMove)
				m_DoMove.OnUpdate( dt );
			
			if (m_DoTargeting)
				m_DoTargeting.OnUpdate( dt );

			if (m_DoIdle)	
				m_DoIdle.OnUpdate( dt );

			if (GetHealth("", "") < 50)
			{
				if (GetHealth("", "") > 25)
					m_DoFire.OnHide();
				else
					m_DoFire.OnRunaway();
			}		
			l_dtAccumulator = 0;	
		}
    }
	

	override void EEKilled( Object killer )
	{
		if (!m_IsVoice && this.GetUseVoice())
		{
			Object posSound = Object.Cast(GetGame().CreateObject( "SoundPos", this.GetPosition()));
			GetGame().CreateSoundOnObject(posSound, "Bot_death_" + Math.RandomInt(1, 4).ToString(), 40, false);
		}
		
		if (this.IsKillFeed())
		{
			BotrBase m_deadBot = this;
			string m_deadBotName = m_deadBot.GetNameBot();

			string KillFeed_Text = "[БОТ]" + m_deadBotName + " died a heroic death!";
			Print(KillFeed_Text);
			SendMessageKillFeed(KillFeed_Text);		
		}
			
	//	SimulateDead();
		
		OnDeath();
	}
	
	void SendMessageKillFeed( string message ) 
	{ 
		ref array<Man> players = new array<Man>; 
		GetGame().GetPlayers( players ); 
		PlayerBase player;
		
		for ( int i = 0; i < players.Count(); i++ )
		{
			Class.CastTo(player, players.Get(i));
			Param1<string> m_MesParam = new Param1<string>(message); 
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MesParam, true, player.GetIdentity()); 
		} 
	}	

	bool bot_CanJump()
	{
		
		if( IsFBSymptomPlaying() || IsRestrained() )
			return false;
		
		if( m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
			return false;
		
		HumanItemBehaviorCfg b_hibcfg = GetItemAccessor().GetItemInHandsBehaviourCfg();
		if( !b_hibcfg.m_bJumpAllowed )
			return false;
		
		return true;
	}
	
	void bot_OnJumpStart()
	{
		
		m_ActionManagerbot.OnJumpStart();
		
		AbortWeaponEvent();
		GetWeaponManager().DelayedRefreshAnimationState(10);
		RequestHandAnimationStateRefresh();
	}
	
	void bot_OnJumpEnd(int b_pLandType = 0)
	{
		if(m_PresenceNotifier)
		{
			switch(b_pLandType)
			{
			case HumanCommandFall.LANDTYPE_NONE:
			case HumanCommandFall.LANDTYPE_LIGHT:
				m_PresenceNotifier.ProcessEvent(EPresenceNotifierNoiseEventType.LAND_LIGHT);
				break;
			case HumanCommandFall.LANDTYPE_MEDIUM:
			case HumanCommandFall.LANDTYPE_HEAVY:
				m_PresenceNotifier.ProcessEvent(EPresenceNotifierNoiseEventType.LAND_HEAVY);
				break;
			}
		}
		
		GetWeaponManager().RefreshAnimationState();
	}
	
	bool bot_CanClimb( int bot_climbType, SHumanCommandClimbResult bot_climbRes)
	{
		if( IsFBSymptomPlaying() || IsRestrained() )
			return false;
		
		if( m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE || m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_RAISEDPRONE)
			return false;
		
		HumanItemBehaviorCfg b_hibcfg = GetItemAccessor().GetItemInHandsBehaviourCfg();
		if( !b_hibcfg.m_bJumpAllowed )
			return false;
		
		if(bot_climbRes)
		{
			EntityAI entity;
			if (Class.CastTo(entity,bot_climbRes.m_GrabPointParent) && entity.IsHologram())
				return false;
			if (Class.CastTo(entity,bot_climbRes.m_ClimbStandPointParent) && entity.IsHologram())
				return false;
			if (Class.CastTo(entity,bot_climbRes.m_ClimbOverStandPointParent) && entity.IsHologram())
				return false;
		}

		return true;	
	}
			
	void RemoveDeadhBody()
	{	
		Print("LOG BOT: " + " Delete death bot body!");
		GetGame().ObjectDelete( this );
	}
	
	void SimulateDead()
	{
		ItemBase item = ItemBase.Cast(GetHumanInventory().GetEntityInHands());

		if (item && !item.IsInherited(SurvivorBase))	
		{
			ServerDropEntity(item);
			item.SetSynchDirty();
		}
		
		array<EntityAI> itemsArray = new array<EntityAI>;
		ItemBase itemDublicat;
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
		
		for (int i = 0; i < itemsArray.Count(); i++)
		{
			Class.CastTo(itemDublicat, itemsArray.Get(i));
			if (itemDublicat)
			{
				string itemName = itemDublicat.GetType();
				GetGame().CreateObject( itemName, GetPosition());
			}
		}
		
		itemsArray.Clear();	
    // 	RemoveAllItems();
	//	SetPosition("0 0 0");
	}
	
	void OnDeath()
	{
		Print("LOG BOT: " + " OnDeath " + this);
		OnExit();
	}
	
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
       super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	   OnHit(damageResult, damageType, source, component, dmgZone, ammo, modelPos);
	}
	
	void OnHit(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
	{
	//	Print("[esbfsm] OnHit " + this);
		if (this.IsAlive())
		{
			if (source && source != this) 
			{
				Print("LOG BOT: " + this.GetType() + " bot was attacked by " + source.GetType());
				if ( !source.IsInherited(CarScript) )
				{
					
					if (!this.IsVoice() && this.GetUseVoice())
					{
						this.SetIsVoice(true);
						GetGame().CreateSoundOnObject(this, "Bot_friendly_fire_" + Math.RandomInt(1, 5).ToString(), 40, false);
						GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VoiceEnd, Math.RandomInt(6500, 9000), false); 
					}
				//	if (GetTarget())
				//		m_DoTargeting.OnResetTarget();
					
					if (isFrendly() && source.IsInherited(PlayerBase))
					{
						PlayerBase player = PlayerBase.Cast(source.GetHierarchyRoot());
						player.SetFrendly(false);
					}
					if (source.IsInherited(PlayerBase) && !GetTarget())
						SetTarget(EntityAI.Cast(source.GetHierarchyRoot()));
				}
				if (source.IsInherited(AnimalBase) && !GetTarget())
					SetTarget(AnimalBase.Cast(source.GetHierarchyRoot()));
			}
		}
	}
	
	
	void SetPassive(bool Result) { m_PassiveBot = Result; }
	void SetUseCheckpoint() { m_UseCheckpoint = true; m_IdleBot = false; }
	void SetBandit(bool Result) { m_BanditTraderBot = Result; }
	void SetHero(bool Result) { m_HeroTraderBot = Result; }
	void SetIdle(bool Result) { m_IdleBot = Result; }
	void SetSniper(bool Result) { m_SniperBot = Result; }
	void SetRush(bool Result) { m_RushBot = Result; }
	void SetAcuracy(float Result) { m_AcuracyBot = Result; }
	void SetRandom(float Result) { m_RandomBot = Result; }
	void SetTime(float Result) { m_TimeBot = Result; }
	void SetDistance(float Result) { m_DistanceBot = Result; }
	void AddMagazine(string Mag) { m_MagazineName = Mag; }
	void AddWeapon(string Weap) { m_WeaponName = Weap; }
	void SetBeginPosition(vector Result) { m_BeginPosition = Result; }
	void SetRespawned(bool Result) { m_onRespawned = Result; }
	void SetUseVoice(bool Result) { useVoice = Result; }
	void SetStandUp(bool Result) { m_onStandUp = Result; }
	void SetUseKillFeed(bool Result) { bot_KillFeed = Result; }	
	void SetFrendly(bool Result) { m_isFrendly = Result; }
	void SetSpeedPatrol(int Result) { m_SetSpeed = Result; }
	void SetTimeDel_DeathBody(bool Result) { m_TimeDeathBody = Result; }
	void SetCicleCheckPoint(bool Result) { m_ChcPointC = Result; }
	void SetIsVoice(bool Result) { m_IsVoice = Result; }
	void SetOpenDoors(bool Result) { m_CanOpenDoors = Result; }
	
	
	bool CanOpenDoor() { return m_CanOpenDoors; }
	bool IsVoice() { return m_IsVoice; }
	bool IsPassive() { return m_PassiveBot; }
	bool IsIdle() { return m_IdleBot; }
	bool IsSniper() { return m_SniperBot; }
	bool IsRush() { return m_RushBot; }
	bool IsBanditTrader() { return m_BanditTraderBot; }
	bool IsHeroTrader() { return m_HeroTraderBot; }
    bool IsUseCheckpoint() { return m_UseCheckpoint; }
	bool IsRespawned() { return m_onRespawned; }
	bool IsKillFeed() { return bot_KillFeed; }
	bool isFrendly() { return m_isFrendly; }
	bool GetCicleCheckPoint() { return m_ChcPointC; }
	
	float GetAcuracy() { return m_AcuracyBot; }
	float GetDistance() { return m_DistanceBot; }
	float GetRandom() { return m_RandomBot; }
	float GetTime() { return m_TimeBot; }
//	int GetTimeDel_DeathBody() { return m_TimeDeathBody; }
	

	string GetMagazineName() { return m_MagazineName; }
	string GetWeaponName() { return m_WeaponName; }
	vector GetBeginPosition() { return m_BeginPosition; }
	vector GetEndPosition() { return m_EndPosition; }
	bool GetUseVoice() { return useVoice; }
	bool GetStandUp() { return m_onStandUp; }

	EntityAI GetTarget() { return m_Target; }

	HumanInputController GetHic() { return m_Hic; }
	HumanItemAccessor GetHia() { return m_Hia; }
	HumanCommandWeapons GetHcw() { return m_Hcw; }
	HumanCommandMove GetHcm() { return m_Hcm; }

	SurvivorBotDoMove GetDoMoveFSM() { return m_DoMove; }
	SurvivorBotDoFire GetDoFireFSM() { return m_DoFire; }
	SurvivorBotDoTargeting GetDoTargetingFSM() { return m_DoTargeting; }
	SurvivorBotDoIdle GetDoIdleFSM() { return m_DoIdle; }
	SurvivorJumpClimb GetDoJumpFSM() { return m_DoJump; }
	
	int GetSpeedPatrol()
	{
		if (m_SetSpeed > 3)
			return 3;
		
		if (m_SetSpeed < 1)
			return 1;
		
		return m_SetSpeed;
	}


	
    void VoiceEnd()
	{
		m_IsVoice = false;
	}	
	
	void AddWeaponAtt(string className)
	{
		m_AttClassName.Insert(className);
	}
	
	void AddCheckpoint(vector m_Checkpoint)
	{
		if (m_UseCheckpoint)
			m_ArrayCheckpoint.Insert(m_Checkpoint);
	}
	

 	void SetTarget(EntityAI Target) 
	{
		if (!m_IsVoice && this.IsAlive() && this.GetUseVoice())
		{
			m_IsVoice = true;
			
			GetGame().CreateSoundOnObject(this, "Bot_enemy_" + Math.RandomInt(1, 12).ToString(), 40, false);
			
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(VoiceEnd, Math.RandomInt(6500, 9000), false); 
		}
		m_Target = Target;
		if (m_DoMove)
			m_DoMove.ResetWaypoints();
	}
	

}