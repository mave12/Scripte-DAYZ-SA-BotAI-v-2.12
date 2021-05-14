class BotrBase: PlayerBase
{

	void ReloadWeaponBot( EntityAI weapon, EntityAI magazine )
	{
//		if ( GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER )
//		{
			ActionManagerBot mngr_bot;
			CastTo(mngr_bot, GetActionManager());
			
			if (mngr_bot && FirearmActionLoadMultiBulletRadial.Cast(mngr_bot.GetRunningAction()))
			{
				mngr_bot.Interrupt();
			}
			else if ( GetHumanInventory().GetEntityInHands()!= magazine )
			{
				Weapon_Base wpn;
				Magazine mag;
				Class.CastTo( wpn,  weapon );
				Class.CastTo( mag,  magazine );
				if ( GetWeaponManager().CanUnjam(wpn) )
				{
					GetWeaponManager().Unjam();
				}
				else if ( GetWeaponManager().CanAttachMagazine( wpn, mag ) )
				{
					GetWeaponManager().AttachMagazine( mag );
				}
				else if ( GetWeaponManager().CanSwapMagazine( wpn, mag ) )
				{
					GetWeaponManager().SwapMagazine( mag );
				}
				else if (GetWeaponManager().CanEjectBullet(wpn))
				{
					GetWeaponManager().EjectBullet();
				}
				else if ( GetWeaponManager().CanLoadBullet( wpn, mag ) )
				{
					GetWeaponManager().LoadMultiBullet( mag );

					ActionTarget atrg = new ActionTarget(mag, this, -1, vector.Zero, -1.0);
					if ( mngr_bot && !mngr_bot.GetRunningAction() && mngr_bot.GetAction(FirearmActionLoadMultiBulletRadial).Can(this, atrg, wpn) )
						mngr_bot.PerformActionStart(mngr_bot.GetAction(FirearmActionLoadMultiBulletRadial), atrg, wpn);
				}
			}
		}		
//	}

	void ReloadWeap(EntityAI weapon)
	{
		EntityAI magazine = GetMagazineToReload( weapon );
		if (IsBot())
			ReloadWeaponBot(weapon, magazine);
	}

	override void SetActions(out TInputActionMap InputActionMap)
	{
		if (this.IsBot())
			return;		
	}
	
	override void SetActionsRemoteTarget( out TInputActionMap InputActionMap)
	{
	}
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		return true;		
	}
	override void InitializeActions()
	{
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
	}
	
	override void AddAction(typename actionName, out TInputActionMap InputActionMap )
	{
	}
	
	override void AddAction(typename actionName)
	{
	}
	
	override void RemoveAction(typename actionName, out TInputActionMap InputActionMap)
	{
	}
	
	override void RemoveAction(typename actionName)
	{
	}	
	
	override void SetStamina( int value, int range )
	{
	}
	
	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)	
	{
		// lower implement 
		super.CommandHandler(pDt,pCurrentCommandID,pCurrentCommandFinished);
	}

	override void OnQuickBarSingleUse(int slotClicked)
	{	
	}
	
	override void OnQuickBarContinuousUseStart(int slotClicked)
	{
	}	

	override void OnQuickBarContinuousUseEnd(int slotClicked)
	{
	}
	
	override void OnSelectPlayer()
	{
	}	
	
	override void SetNewCharName()
	{
	}

	override void CheckForBurlap()
	{
	}
	
	override void CheckForGag()
	{
	}	

	override void OnStoreSave( ParamsWriteContext ctx )
	{

	}

	override void AfterStoreLoad()
	{

	}

	override void OnStoreSaveLifespan( ParamsWriteContext ctx )
	{		

	}

	
	override void OnPlayerLoaded()
	{

	}
	
	override void OnConnect()
	{

	}
	
	override void OnReconnect()
	{

	}
	
	override void OnDisconnect()
	{	
	}
	
	override void UpdateQuickBarExtraSlots()
	{
	}
/*
	override void RegisterTransportHit(Transport transport)
	{
	} */
	
	bool Save()
	{
		return false;		
	}	
	
	override bool IsPlayer()
	{
		return false;
	}
}