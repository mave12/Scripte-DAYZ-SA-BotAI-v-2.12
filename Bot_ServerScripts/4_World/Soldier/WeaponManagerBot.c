﻿modded class WeaponManager {
	
	override void StartPendingAction()
	{		
		m_WeaponInHand = Weapon_Base.Cast(m_player.GetItemInHands());
		if(!m_WeaponInHand)
		{
			OnWeaponActionEnd();
			return;
		}
		switch (m_PendingWeaponAction)
		{
			case AT_WPN_ATTACH_MAGAZINE:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventAttachMagazine(m_player, m_PendingTargetMagazine) );
				break;
			}
			case AT_WPN_SWAP_MAGAZINE:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventSwapMagazine(m_player, m_PendingTargetMagazine) );
				break;
			}
			case AT_WPN_DETACH_MAGAZINE:
			{
				Magazine mag = Magazine.Cast(m_PendingInventoryLocation.GetItem());
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventDetachMagazine(m_player, mag, m_PendingInventoryLocation) );
				break;
			}
			case AT_WPN_LOAD_BULLET:
			{
				m_WantContinue = false;
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventLoad1Bullet(m_player, m_PendingTargetMagazine) );
				break;
			}
			case AT_WPN_LOAD_MULTI_BULLETS_START:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventLoad1Bullet(m_player, m_PendingTargetMagazine) );
				break;
			}
			case AT_WPN_LOAD_MULTI_BULLETS_END:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventContinuousLoadBulletEnd(m_player) );
				break;
			}
			case AT_WPN_UNJAM:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventUnjam(m_player, NULL) );
				break;
			}
			case AT_WPN_EJECT_BULLET:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventMechanism(m_player, NULL) );
				break;
			}
			case AT_WPN_SET_NEXT_MUZZLE_MODE:
			{
				m_player.GetDayZPlayerInventory().PostWeaponEvent( new WeaponEventSetNextMuzzleMode(m_player, NULL) );
				break;
			}
			default:
				m_InProgress = false;
				Error("unknown actionID=" + m_PendingWeaponAction);
		}	
		m_IsEventSended = true;
		m_canEnd = false;
	}
	
	override bool StartAction(int action, Magazine mag, InventoryLocation il, ActionBase control_action = NULL)
	{
		if (control_action) 
		{
			m_ControlAction = ActionBase.Cast(control_action);
			m_PendingWeaponAction = action;
			m_InProgress = true;
			m_IsEventSended = false;
			m_PendingTargetMagazine = mag;
			m_PendingInventoryLocation = il;
			
			StartPendingAction();
			
			return true;
		}
		if (GetGame().IsMultiplayer() && GetGame().IsServer() && !m_player.IsBot())
			return false;
		
		
		if ( !ScriptInputUserData.CanStoreInputUserData() )
			return false;
		
		
		if ( !InventoryReservation(mag, il) )
			return false;
		

		m_PendingWeaponAction = action;
		m_InProgress = true;
		m_IsEventSended = false;
		
		if ( !GetGame().IsMultiplayer() ) 
		{
			m_readyToStart = true;
		} 
		else if (m_player.IsBot()) 
		{
			m_PendingInventoryLocation = il;			
			m_PendingTargetMagazine = mag;
			
			SynchronizeServer(mag, il);
		} 
		else 
		{
			Synchronize();
		}
		
		return true;
	}
	

	private bool SynchronizeServer(Magazine mag, InventoryLocation il)
	{
		if(GetGame().IsServer() && m_player.IsBot()) 
		{			
			m_PendingWeaponActionAcknowledgmentID = ++m_LastAcknowledgmentID;
			
			InventoryLocation il2;
			
			Weapon_Base wpn;
			int mi;
			
			int slotID;
			bool accepted = false;			

			m_InProgress = true;
			m_IsEventSended = false;
			
			Weapon_Base.CastTo( wpn, m_player.GetItemInHands() );
			if ( wpn )
				mi = wpn.GetCurrentMuzzle();
			
			switch (m_PendingWeaponAction)
			{
				case AT_WPN_ATTACH_MAGAZINE: 
				{
					if ( !mag || !wpn )
						break;

					slotID = wpn.GetSlotFromMuzzleIndex(mi);
					il2 = new InventoryLocation;
					il2.SetAttachment(wpn,mag,slotID);
					if( GetGame().AddInventoryJuncture(m_player, mag, il2, false, 10000) )
						accepted = true;
					
					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_SWAP_MAGAZINE: 
				{
					if ( !mag || !wpn )
						break;

					if ( !wpn.GetMagazine(mi) )
						break;
					
					if ( GetGame().AddActionJuncture(m_player,mag,10000) )
						accepted = true;
					
					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_DETACH_MAGAZINE:
				{
					if ( !il.IsValid() )
						break;
					
					if ( !wpn )
						break;
					
					Magazine det_mag = wpn.GetMagazine(mi);
					mag = Magazine.Cast(il.GetItem());
					if ( !det_mag || ( mag != det_mag) )
						break;
					
					if( GetGame().AddInventoryJuncture(m_player, il.GetItem(), il, true, 10000))
						accepted = true;
					
					m_PendingInventoryLocation = il;
					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_LOAD_BULLET:
				{
					if ( !mag )
						break;
					
					if( GetGame().AddActionJuncture(m_player,mag,10000) )
						accepted = true;
					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_LOAD_MULTI_BULLETS_START:
				{
					if ( !mag )
						break;
					
					if( GetGame().AddActionJuncture(m_player,mag,10000) )
						accepted = true;
					m_PendingTargetMagazine = mag;
					break;
				}
				case AT_WPN_UNJAM:
				{
					accepted = true;
					//Unjam();
					break;
				}
				case AT_WPN_EJECT_BULLET:
				{
					accepted = true;
					break;
				}
				case AT_WPN_SET_NEXT_MUZZLE_MODE:
				{
					accepted = true;
					break;
				}
				default:
					Error("unknown actionID=" + m_PendingWeaponAction);
					break;
			}
			DayZPlayerSyncJunctures.SendWeaponActionAcknowledgment(m_player, m_PendingWeaponActionAcknowledgmentID, accepted);
			
			return accepted;
		}
		
		
		return false;
	}
}
