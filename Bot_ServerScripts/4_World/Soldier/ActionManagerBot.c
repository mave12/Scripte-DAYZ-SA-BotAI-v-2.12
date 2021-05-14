class ActionManagerBot: ActionManagerBase
{		
	protected ActionBase 				m_PendingAction;
	ref ActionReciveData				m_PendingActionReciveData;
	
	// Lifted from ActionManagerClient
	protected int 								m_LastAcknowledgmentID;
	protected ref ActionData 					m_PendingActionData;
	protected ref InventoryActionHandlerBot		m_InventoryActionHandler;
	
	void ActionManagerBot(PlayerBase player)
	{
		m_PendingAction = NULL;
		m_PendingActionReciveData = NULL;
		m_LastAcknowledgmentID = 1;		
		m_InventoryActionHandler = new InventoryActionHandlerBot(player);
	}
	
	override void OnJumpStart()
	{
		if(m_CurrentActionData)
		{
			if( m_CurrentActionData.m_State == UA_AM_PENDING || m_CurrentActionData.m_State == UA_AM_REJECTED || m_CurrentActionData.m_State == UA_AM_ACCEPTED)
			{
				OnActionEnd();
				m_PendingActionAcknowledgmentID = -1;
			}
			else
			{
				m_CurrentActionData.m_Action.Interrupt(m_CurrentActionData);
			}
		}
	}
	
	// Single----------------------------------------------------
	override void OnSingleUse()
	{

	}
	
	// Interact----------------------------------------------------
	override void OnInteractAction() //Interact
	{
	
	}
	
	override void StartDeliveredAction()
	{
		if( !m_CurrentActionData )
		{;
			return;
		}
		
		m_Interrupted = false;
		
		ActionBase picked_action;
		bool accepted = false;
		bool is_target_free = true;
		ref ActionTarget target;
		ItemBase item;
		
		picked_action = m_CurrentActionData.m_Action;
		target = m_CurrentActionData.m_Target;
		item = m_CurrentActionData.m_MainItem;

		accepted = true;
		if( picked_action.HasTarget())
		{
			EntityAI targetEntity;
			if ( EntityAI.CastTo(targetEntity,target.GetObject()) )
			{

				if( !AdvancedCommunication.Cast(targetEntity) && !Building.Cast(targetEntity) && !Fireplace.Cast(targetEntity) )
				{
					if( !GetGame().AddActionJuncture(m_Player,targetEntity,10000) )
					{
						accepted = false;
					}
				}
			}
		}
		
		if( accepted )
		{
			if(picked_action.UseAcknowledgment())
			{
				m_CurrentActionData.m_State = UA_AM_PENDING;
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, true);
			}
			else
			{
				m_CurrentActionData.m_State = UA_AM_ACCEPTED;
			}
		}
		else
		{
			if (picked_action.UseAcknowledgment())
			{
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false);
			}
			else
			{
				DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
			}
		}
	}
	
	override void OnActionEnd()
	{
		if (m_CurrentActionData)
		{
			if ( m_CurrentActionData.m_Target )
			{
				EntityAI targetEntity;
				if ( targetEntity.CastTo(targetEntity, m_CurrentActionData.m_Target.GetObject()) && !Building.Cast(targetEntity) )
				{
					GetGame().ClearJuncture(m_CurrentActionData.m_Player, targetEntity);
				}
			}
			super.OnActionEnd();
		}
	}
	
	
	override void Update(int pCurrentCommandID)
	{
		m_InventoryActionHandler.OnUpdate();
		super.Update(pCurrentCommandID);
		
		if (m_PendingAction)
		{
			if ( m_CurrentActionData )
			{
				DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false); // Note - why is this false?
			}
			else
			{
				ref ActionTarget target = new ActionTarget(NULL, NULL, -1, vector.Zero, 0); 
				bool success = true;

				m_ActionWantEndRequest = false;
				m_ActionInputWantEnd = false;
						
				Debug.Log("[Action DEBUG] Start time stamp ++: " + m_Player.GetSimulationTimeStamp());
				if (!m_PendingAction.SetupAction(m_Player,target,m_Player.GetItemInHands(),m_CurrentActionData))
				{
					success = false;
				}
				//Debug.Log("[AM] Action data synced (" + m_Player + ") success: " + success);
			
				if (success)
				{
					StartDeliveredAction();
				}
				else
				{
					if (m_PendingAction.UseAcknowledgment())
					{
						DayZPlayerSyncJunctures.SendActionAcknowledgment(m_Player, m_PendingActionAcknowledgmentID, false);
					}
					else
					{
						DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
					}
				}
			}
			m_PendingAction = NULL;
			m_PendingActionReciveData = NULL;
		}
	
		if (m_CurrentActionData)
		{			
			if (m_CurrentActionData.m_State != UA_AM_PENDING && m_CurrentActionData.m_State != UA_AM_REJECTED && m_CurrentActionData.m_State != UA_AM_ACCEPTED)
			{
				m_CurrentActionData.m_Action.OnUpdateServer(m_CurrentActionData);
				
				if (m_CurrentActionData.m_RefreshJunctureTimer > 0)
				{
					m_CurrentActionData.m_RefreshJunctureTimer--;
				}
				else
				{
					m_CurrentActionData.m_RefreshJunctureTimer = m_CurrentActionData.m_Action.GetRefreshReservationTimerValue();
					if ( m_CurrentActionData.m_Target )
					{
						EntityAI targetEntity;
						if ( targetEntity.CastTo(targetEntity, m_CurrentActionData.m_Target.GetObject()) && !Building.Cast(targetEntity) )
						{
							GetGame().ExtendActionJuncture(m_CurrentActionData.m_Player, targetEntity, 10000);
						}
					}
				}
			}
			
			switch (m_CurrentActionData.m_State)
			{
				case UA_AM_PENDING:
					break;
			
				case UA_AM_ACCEPTED:
					if ( ActionPossibilityCheck(pCurrentCommandID) )
					{
						m_CurrentActionData.m_State = UA_START;
						m_CurrentActionData.m_Action.Start(m_CurrentActionData);
					
						if ( m_CurrentActionData.m_Action && m_CurrentActionData.m_Action.IsInstant() )
							OnActionEnd();
					}
					else
					{
						OnActionEnd();
					}
					m_PendingActionAcknowledgmentID = -1;
					break;
				
				case UA_AM_REJECTED:
					OnActionEnd();
					m_PendingActionAcknowledgmentID = -1;
					break;
			
				default:
					if (m_ActionInputWantEnd)
					{
						m_ActionInputWantEnd = false;
						m_CurrentActionData.m_Action.EndInput(m_CurrentActionData);
					}
				
					if (m_ActionWantEndRequest)
					{
						m_ActionWantEndRequest = false;
						m_CurrentActionData.m_Action.EndRequest(m_CurrentActionData);
					}
					break;
			}
		}
	}
	
	protected void ActionStart(ActionBase action, ActionTarget target, ItemBase item, Param extra_data = NULL )
	{
		if ( !m_CurrentActionData && action ) 
		{
			m_ActionWantEndRequest = false;
			m_ActionInputWantEnd = false;

			Debug.Log("Action=" + action.Type() + " started, STS=" + m_Player.GetSimulationTimeStamp());
			m_Interrupted = false;
			if ( GetGame().IsMultiplayer() && !action.IsLocal() )
			{
				if (!ScriptInputUserData.CanStoreInputUserData())
				{
					DPrint("ScriptInputUserData already posted - ActionManagerClient");
					return;
				}
			}
			
			
			if ( GetGame().IsMultiplayer() && !action.IsLocal() )
			{			
				m_PendingAction = action;
			//	Print("0, PendingAction: " + m_PendingAction.ToString() + " instance: " + this.ToString());
				Update(DayZPlayerConstants.COMMANDID_ACTION);
			}
			else
			{
				action.Start(m_CurrentActionData);
				if( action.IsInstant() )
					OnActionEnd();
			}
		}
	}
	
	void PerformActionStart(ActionBase action, ActionTarget target, ItemBase item, Param extra_data = NULL )
	{
		if (!GetGame().IsMultiplayer())
		{
			m_PendingActionData = new ActionData;
	
			if (!action.SetupAction(m_Player,target,item,m_PendingActionData,extra_data))
				m_PendingActionData = null;
		}
		else
			ActionStart(action, target, item, extra_data);
	}
	
	override void Interrupt()
	{
		if ( m_CurrentActionData )
			DayZPlayerSyncJunctures.SendActionInterrupt(m_Player);
	}
	
	override ActionReciveData GetReciveData()
	{
		return m_PendingActionReciveData;
	}
	
	// These three functions were lifted from the client action manager; they are unused for now, might need them later
	void SetInventoryAction(ActionBase action_name, ItemBase target_item, ItemBase main_item)
	{
		m_InventoryActionHandler.SetAction(action_name, target_item, main_item);
	}

	void SetInventoryAction(ActionBase action_name, ActionTarget target, ItemBase main_item)
	{
		m_InventoryActionHandler.SetAction(action_name, target, main_item);
	}
	
	void UnsetInventoryAction()
	{
		m_InventoryActionHandler.DeactiveAction();
	}
};
