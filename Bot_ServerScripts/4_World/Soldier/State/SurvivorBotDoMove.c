class SurvivorBotDoMove
{
    protected int m_Speed = 0;
    protected bool m_OverrideSpeed = false;
    protected bool m_OverrideMove;
    protected bool m_OverrideFinal = false;
	protected bool m_OverrideWalk = false;
	protected bool s_Idle;
	

	AIWorld world = GetGame().GetWorld().GetAIWorld();
	ref PGFilter m_pgFilter = new PGFilter();
	ref PGFilter m_pgFilterRay = new PGFilter();
	
	ref TIntArray randAngle = {-90, 90};

    protected float m_TargetDistance;
    protected int m_CurrentPosition;

	protected vector m_TargetPosition;
	protected vector m_TargetDirection;

    protected BotrBase m_Player;
    protected SurvivorBotBase m_Soldier;
	protected bool IsNavmesh;
	protected  float m_distToTargetPos;
	
	ref TVectorArray waypoints = new TVectorArray;
	protected bool IsWaypoints = false;
	protected bool IsUseWaypoints = false;
	protected bool m_onMovement = false;
	protected bool IsResetWaipoint = false;
	protected bool isManToFront = false;
	protected bool isTreeToFront = false;
	
	
	protected bool m_IsCollision;
	protected bool m_DisableTargetMovement;
    protected EntityAI m_NavmehObjectTarget;
    protected int m_CheckpointCount = 0;
	protected int m_LastCheckpointNum = 0;
	
	protected bool OnWaypoint;
	
	protected int m_waypointCount = 0;
	protected int m_WaypointNum = 0;
	
	vector targ_pos;
	
    void SurvivorBotDoMove(BotrBase m_Owner, SurvivorBotBase m_SoldierOwner) { m_Player = m_Owner; m_Soldier = m_SoldierOwner; OnEntry(); }
    void ~SurvivorBotDoMove() { OnExit(); }

    void OnEntry()
    {
     //   Print("[esbsdm] OnEntry " + this);

        m_TargetDistance = 1;
        m_CurrentPosition = 0;
        m_TargetPosition = "0 0 0";
		targ_pos = "0 0 0";
		m_distToTargetPos = 0;
       
        m_Speed = 0;
        m_OverrideSpeed = false;
        m_OverrideMove = false;
        m_OverrideFinal = false;
		m_OverrideWalk = false;
		IsNavmesh = false;
		OnWaypoint = true;
		
		m_IsCollision = false;
		m_DisableTargetMovement = false;
		
		m_NavmehObjectTarget = EntityAI.Cast(GetGame().CreateObject("test_emtyObj", m_Soldier.GetPosition()));
		
		if (m_Soldier.IsUseCheckpoint())
		{
			m_CheckpointCount = m_Soldier.m_ArrayCheckpoint.Count();
		//	SendLog("Бот использует чекпоинты! Колличество: " + m_CheckpointCount.ToString());
		}
    }
	
    void EnableTargetMovement()
	{
	   //SendLog("---IsCollision EnableTargetMovement--- m_DisableTargetMovement = false" );
//	   SetDirNavmeh(m_TargetPosition);
	   if (m_DisableTargetMovement)
		   m_DisableTargetMovement = false;
	 //  m_pgFilter.SetFlags(PGPolyFlags.WALK, PGPolyFlags.JUMP, 0);
	 //  m_pgFilter.SetCost(PGAreaType.TREE, 1);
	}


   
	void IgnoreObjects()
	{	
	
		bool b_isObjInFront = IsObjectInFront(m_Soldier, 0);
		if (b_isObjInFront)
		{		
			if (!m_DisableTargetMovement)
			{
				m_DisableTargetMovement = true;
				if (m_IsCollision)
				{
					m_Soldier.GetDoJumpFSM().bot_JumpOrClimb();	
					ResetWaypoints();
				}
				
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(EnableTargetMovement, 3000, false); 
			}
		}
	}
	
	void SetDirNavmeh(vector m_VectorNav)
	{
		vector direction_vector = vector.Direction(m_VectorNav, m_Soldier.GetPosition()).Normalized() * -1;
		vector direction_no_z = Vector(direction_vector[0],0,direction_vector[2]).Normalized();
		m_Soldier.SetDirection( direction_no_z.Normalized() );	
	}

	void OnExit () 
    {
        Print("[esbsdm] OnExit " + this);
		if (m_Soldier)
		{
			m_Soldier.GetInputController().OverrideMovementSpeed(false, 0); 
			m_Soldier.GetInputController().OverrideMovementAngle(false, 0);
		}

        m_TargetDistance = 0;
        m_CurrentPosition = 0;
        m_TargetPosition = "0 0 0";
		targ_pos = "0 0 0";

        m_Speed = 0;
        m_OverrideSpeed = false;
        m_OverrideMove = false;
        m_OverrideFinal = false;
		IsNavmesh = false;
		/*
		if(m_NavmehObjectTarget)
		{
			GetGame().ObjectDelete( m_NavmehObjectTarget );
		} */
		
    }
	
	void OnUpdate (float dt) 
    {
			float gunHeight = 1.5 + m_Soldier.GetPosition()[1]; 
			float targetHeight;
			float aimAngle;
		//	Print("aimAngle: " + aimAngle);
			
		
		if (m_Soldier.GetTarget())
		{
			targetHeight = 1.0 + m_Soldier.GetTarget().GetPosition()[1];
			aimAngle = Math.Atan2(targetHeight - gunHeight, vector.Distance(m_Soldier.GetPosition(), m_Soldier.GetTarget().GetPosition()));
			m_distToTargetPos = vector.Distance(m_Soldier.GetPosition(), m_Soldier.GetTarget().GetPosition());
		}
		else
		{
			targetHeight = 1.0;
			m_distToTargetPos = vector.Distance(m_Soldier.GetPosition(), m_Soldier.GetBeginPosition());
			aimAngle = Math.Atan2(targetHeight - gunHeight, vector.Distance(m_Soldier.GetPosition(), m_TargetPosition));
		}
		
		m_Soldier.targetAngle = aimAngle * Math.RAD2DEG;
		
		if (!m_Soldier.GetStandUp())
				IgnoreObjects();	
			
		if(m_distToTargetPos > m_TargetDistance && m_OverrideMove && !isManToFront)
		{

			
			if (m_Soldier.GetCommand_Move() && m_Soldier.IsBot()) 
        	{
				m_Soldier.GetCommand_Move().ForceStanceUp(DayZPlayerConstants.STANCEIDX_ERECT);
            	m_Soldier.GetCommand_Move().ForceStance(DayZPlayerConstants.STANCEIDX_ERECT);
        	}
				
			if (m_Soldier.GetTarget())
			{
				m_TargetPosition = GetNewPoint(m_Soldier.GetTarget().GetPosition());											
			}
			else
			{
				m_TargetPosition = GetNewPoint(m_Soldier.GetBeginPosition());
			}
			
			bool m_CalcDir = CalculateNewDirection();
			
			m_Soldier.SetDirection(m_TargetDirection);
			
			if (m_CalcDir && !m_Soldier.GetStandUp())
			{				
				m_onMovement = true;
				m_Soldier.GetInputController().OverrideMovementSpeed( true, CalculateSpeedMode() ); 
				m_Soldier.GetInputController().OverrideMovementAngle( true, 0 ); 	
			}

		}
		else if (!m_Soldier.GetStandUp() && CalculateNewDirection() && m_Soldier.GetTarget() && m_distToTargetPos < 5)
		{
			HumanMovementState b_State = new HumanMovementState;
			m_Soldier.GetMovementState(b_State);
			if (b_State.IsRaised())
			{
			//	IgnoreObjects();
				m_Soldier.GetInputController().OverrideMovementSpeed( true, 2 ); 
				m_Soldier.GetInputController().OverrideMovementAngle( true, 180 ); 	
			}
			else
			{
				m_Soldier.GetInputController().OverrideMovementSpeed( false, 0 ); 
				m_Soldier.GetInputController().OverrideMovementAngle( false, 0 );			
			}
		}
		else
		{
			m_Soldier.SetDirection(m_TargetDirection);
			m_onMovement = false;
			if (!isTreeToFront)
			{	
				m_Soldier.GetInputController().OverrideMovementSpeed( false, 0 ); 
				m_Soldier.GetInputController().OverrideMovementAngle( false, 0 );
				CalculateNewDirection();
			}
			
		//	if (m_Soldier.GetTarget() && m_onMovement)
		//		SetDirNavmeh(m_Soldier.GetTarget().GetPosition());	
		}
		
		if (!m_Soldier.GetTarget() && !m_Soldier.IsIdle()) 
        {	
			if (!IsResetWaipoint)
			{
				ResetWaypointsFix();
				SetTarget(m_Soldier.GetBeginPosition(), true, true, m_Soldier.GetSpeedPatrol(), 0);
				IsResetWaipoint = true;			
			}
		}
		else if (!m_Soldier.GetTarget()) 
		{
			if (!IsResetWaipoint)
			{
				ResetWaypointsFix();
				SetTarget(m_Soldier.GetBeginPosition(), true, true, Math.RandomInt(1, 3), 0.3);
				IsResetWaipoint = true;
			}
		}
		
		if (m_Soldier.IsUseCheckpoint() && m_Soldier.m_ArrayCheckpoint.Count() != 1 && m_Soldier.m_ArrayCheckpoint.Count() != 0)
			UseCheckpoint();
    }
	
	void onMovement()
	{
		m_Soldier.GetInputController().OverrideMovementSpeed( true, CalculateSpeedMode() ); 		
		if (!m_IsCollision)
		{
			m_Soldier.GetInputController().OverrideMovementAngle( true, 0 ); 
			m_Soldier.SetDirection(m_TargetDirection);
		}
		else
		{
			m_Soldier.GetInputController().OverrideMovementAngle( true, randAngle.GetRandomElement() ); 
		}
		
	}
	
    void UseCheckpoint()
	{
		vector m_Checkpoint;
		float IsCheckpointDist = 3.0;
		m_distToTargetPos = vector.Distance(m_Soldier.GetPosition(), m_TargetPosition);
		if (m_LastCheckpointNum == 0)
		{
			m_LastCheckpointNum ++;
		//	m_Checkpoint = m_Soldier.m_ArrayCheckpoint.Get(m_LastCheckpointNum);
			m_Soldier.SetBeginPosition(m_Soldier.m_ArrayCheckpoint.Get(m_LastCheckpointNum));
		}
        else if (vector.Distance(Vector(m_Soldier.m_ArrayCheckpoint.Get(m_LastCheckpointNum)[0], m_Soldier.GetPosition()[1], m_Soldier.m_ArrayCheckpoint.Get(m_LastCheckpointNum)[2]), m_Soldier.GetPosition()) < IsCheckpointDist && m_LastCheckpointNum != m_CheckpointCount)
		{
			
			m_LastCheckpointNum ++;
		//	m_Checkpoint = m_Soldier.m_ArrayCheckpoint.Get(m_LastCheckpointNum);
			m_Soldier.SetBeginPosition(m_Soldier.m_ArrayCheckpoint.Get(m_LastCheckpointNum));
		}
		if (m_LastCheckpointNum == m_CheckpointCount)
		{
			if (m_Soldier.GetCicleCheckPoint())
			{
				m_Soldier.m_ArrayCheckpoint.Invert();
				m_LastCheckpointNum = 0;
			}
			else
			{
				m_Soldier.SetIdle(true);
			}
		}
		
	}
	
	void SetTarget(vector Position, bool OverrideMove = true, bool OverrideSpeed = false, int Speed = 1, float Distance = 1)
	{	
	//	m_TargetPosition = GetNewPoint(Position);
		m_TargetPosition = Position;
		m_TargetDistance = Distance;

		m_OverrideSpeed = OverrideSpeed;
		m_OverrideMove = OverrideMove;
		m_Speed = Speed;
		
	} 	
	
	

    void OnSelectPositionLatter()
	{
		if (!m_Soldier.GetTarget()) 
        {
			SetTarget(m_Soldier.GetBeginPosition(), true, false, 1);
		}
	}
/*
	void OnMovement()
	{
		if (m_OverrideWalk)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater( OnSelectPositionLatter, 10, false );

			m_OverrideWalk = false;
		}	
	}
*/
	
    void SetStop()
    {
        m_OverrideMove = false;
		m_OverrideWalk = false;
	//	ResetWaypoints();
    }

    void SetMove()
    {
		m_OverrideMove = true;
		m_OverrideWalk = true;
    }
	
	void generate_WayPoint(vector pos_Target)
	{
		m_pgFilter.SetCost(PGAreaType.TERRAIN, 1.0);
		m_pgFilter.SetCost(PGAreaType.JUMP, 100.0);
		m_pgFilter.SetCost(PGAreaType.WATER, 15.0);
		m_pgFilter.SetCost(PGAreaType.WATER_DEEP, 15.0);
		m_pgFilter.SetCost(PGAreaType.WATER_SEA, 15.0);
		m_pgFilter.SetCost(PGAreaType.OBJECTS_NOFFCON, 1000000.0); 
		m_pgFilter.SetCost(PGAreaType.OBJECTS, 20.0);
		m_pgFilter.SetCost(PGAreaType.BUILDING, 20.0);
		m_pgFilter.SetCost(PGAreaType.ROADWAY, 0.2);
		m_pgFilter.SetCost(PGAreaType.TREE, 15.0);
		m_pgFilter.SetCost(PGAreaType.ROADWAY_BUILDING, 20.0);
		m_pgFilter.SetCost(PGAreaType.DOOR_OPENED, 1.0);
		m_pgFilter.SetCost(PGAreaType.DOOR_CLOSED, 10.0); 
		m_pgFilter.SetCost(PGAreaType.LADDER, 1000000.0);
		m_pgFilter.SetCost(PGAreaType.CRAWL, 1000000.0);
		m_pgFilter.SetCost(PGAreaType.CROUCH, 1000000.0);
		m_pgFilter.SetCost(PGAreaType.FENCE_WALL, 1000000.0);
		if (m_Soldier.GetTarget())
		{
			if (m_Soldier.CanOpenDoor())
				m_pgFilter.SetFlags(PGPolyFlags.WALK|PGPolyFlags.JUMP, PGPolyFlags.LADDER|PGPolyFlags.CRAWL|PGPolyFlags.CROUCH, 0);
			else
				m_pgFilter.SetFlags(PGPolyFlags.WALK|PGPolyFlags.JUMP, PGPolyFlags.LADDER|PGPolyFlags.CRAWL|PGPolyFlags.CROUCH|PGPolyFlags.DOOR, 0);
		}
		else
		{
			m_pgFilter.SetFlags(PGPolyFlags.WALK, PGPolyFlags.LADDER|PGPolyFlags.DOOR|PGPolyFlags.SPECIAL|PGPolyFlags.INSIDE, 0);
		}
		
		
		if (OnWaypoint)
		{
			IsUseWaypoints = true;
	
			IsWaypoints = world.FindPath(m_Soldier.GetPosition(), pos_Target, m_pgFilter, waypoints);

		}
		OnWaypoint = false;
	} 
	

	
	vector GetNewPoint(vector pos)
	{		
		m_waypointCount = waypoints.Count();
		
		if (IsUseWaypoints && IsWaypoints)
		{
				
			if (m_WaypointNum == 0)
			{	
				m_WaypointNum = 0;
			}			
			else if (vector.Distance( Vector(waypoints[m_WaypointNum][0], m_Soldier.GetPosition()[1], waypoints[m_WaypointNum][2] ), m_Soldier.GetPosition()) < 0.1 && m_WaypointNum != m_waypointCount)
			{
				m_WaypointNum ++;
			}
			
			if (m_WaypointNum == m_waypointCount)
			{
				ResetWaypoints();
			}
			targ_pos = waypoints[m_WaypointNum];
			if (m_WaypointNum == 0)
			{	
				m_WaypointNum ++;
			}
		}
		else
		{
			generate_WayPoint( pos );		
		}
		return targ_pos;
	}
	
	void ResetWaypoints()
	{
		if (m_waypointCount > 0)
			waypoints.Clear();
		
		m_WaypointNum = 0;
		m_waypointCount = 0;
		IsUseWaypoints = false;	
		OnWaypoint = true;
		IsResetWaipoint = false;
	}
	
	void ResetWaypointsFix()
	{
		if (m_waypointCount > 0)
			waypoints.Clear();
		
		m_WaypointNum = 0;
		m_waypointCount = 0;
		IsUseWaypoints = false;	
		OnWaypoint = true;
		SetStop();
	}
	
    bool CalculateNewDirection()
	{	

		if (m_TargetPosition)
		{			
			vector direction_vector = vector.Direction(m_TargetPosition, m_Soldier.GetPosition()).Normalized() * -1;
			vector direction_no_z = Vector(direction_vector[0],0,direction_vector[2]).Normalized();
			vector bot_dir = m_Soldier.GetDirection().Normalized();
			vector bot_dir_no_z = Vector(bot_dir[0],0,bot_dir[2]).Normalized();

			float direction_angle = Math.Atan2(direction_no_z[0],direction_no_z[2]) * Math.RAD2DEG;
			float bot_angle = Math.Atan2(bot_dir_no_z[0],bot_dir_no_z[2]) * Math.RAD2DEG;

			if (direction_angle < 0)
			{
				direction_angle += 360;
			}
			if (bot_angle < 0)
			{
				bot_angle += 360;
			}

			float deltaDir = direction_angle - bot_angle;

			if (deltaDir > 180)
			{
				deltaDir -= 360;
			}
			if (deltaDir < -180)
			{
				deltaDir += 360;
			}	

			vector new_bot_dir = direction_no_z;
			if (Math.AbsFloat(deltaDir) > 5 )
			{	
				float multi = 5;
				if(deltaDir < 0)
				{
					multi = -1;
				}

				bot_angle += (2.5 * multi);

				float dX = Math.Sin(bot_angle * Math.DEG2RAD);
				float dY = Math.Cos(bot_angle * Math.DEG2RAD);
				new_bot_dir = Vector( dX, 0, dY).Normalized();	
			}
			else
			{
				return true;
			}
			if (m_onMovement) 
				m_TargetDirection = direction_vector;
			else
				m_TargetDirection = new_bot_dir;
		}
		return false;
	}

	int CalculateSpeedMode()
	{
		if(m_OverrideSpeed && m_Speed > 0)
			return m_Speed;
		int speed;
		float d;
		if (m_Soldier.GetTarget())
		{
			d = vector.Distance(m_Soldier.GetTarget().GetPosition(), m_Soldier.GetPosition());
			if(d > 5)
			{
				speed = 3;
			}
			else if(d > 1)
			{
				speed = 2;
			}	
			else
			{
				speed = 2;
			}			
		}
		else
		{
			d = vector.Distance(m_TargetPosition, m_Soldier.GetPosition());

			if(d > 10)
			{
				speed = 3;
			}
			else if(d > 1)
			{
				speed = 2;
			}
			else
			{
				speed = 1;
			}
		}
		return speed;
	}
	
	
	bool IsObjectInFront(PlayerBase Target, int azimunt)
	{
		if (Target) 
        {
			int headIndex = Target.GetBoneIndexByName("Head");
			
			vector rayStart = Target.GetBonePositionWS(headIndex);
			
			float centerX = Target.GetPosition()[0];
			float centerZ = Target.GetPosition()[2];

			float angle = Target.GetOrientation()[0] + azimunt;
			float rads = angle * Math.DEG2RAD;		

			float dX = Math.Sin(rads) * 1;
			float dZ = Math.Cos(rads) * 1;

			float x = centerX + dX;
			float z = centerZ + dZ;
			float y = GetGame().SurfaceY(x,z);
		//	float y = Target.GetPosition()[1] + 0.4;

			vector rayEnd = Vector(x,y,z);
   			auto objs = GetObjectsAt( rayStart, rayEnd, Target );
			
			if (m_Soldier.GetStandUp())
				return false;
			
			if (objs)
			{
				if( objs.Count() ) 
				{	
					if (objs[0].IsTree() || objs[0].IsBush()) 
					{	
						if (objs[0].IsTree())
						{	
							m_OverrideMove = false;
							isTreeToFront = true;
							m_Soldier.GetInputController().OverrideMovementSpeed( true, 2 ); 
							m_Soldier.GetInputController().OverrideMovementAngle( true, 90 ); 
							ResetWaypoints();
						}
					//	Print("Object Tree " + objs[0].GetType());
						m_IsCollision = false;						
						return true;
					}
					
					if (objs[0].IsMan() && !objs[0].IsInherited(SurvivorBotBase) && PlayerBase.Cast(objs[0]).IsAlive()) 
					{
						if (!m_Soldier.IsVoice() && m_Soldier.GetUseVoice())
						{
							m_Soldier.SetIsVoice(true);
							GetGame().CreateSoundOnObject(m_Soldier, "Bot_moveout_" + Math.RandomInt(1, 4).ToString(), 40, false);;
							GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(m_Soldier.VoiceEnd, Math.RandomInt(6500, 9000), false); 
						}

						if (!m_Soldier.GetTarget())
						{
							isManToFront = true;
						}
						
						m_IsCollision = false;
					//	ResetWaypoints();						
						return true;
					} 
					if (objs[0].IsBuilding()) 
					{	
						GetGame().UpdatePathgraphRegionByObject( objs[0] );
						m_IsCollision = true;
						
						if (m_Soldier.CanOpenDoor())
							bOpenDoors(objs[0]);
						
					
					//	Print("Object Building " + objs[0].GetType());
					//	ResetWaypoints();				
						return true;
					} 
					if (objs[0].IsTransport()) 
					{	
						GetGame().UpdatePathgraphRegionByObject( objs[0] );
						m_IsCollision = true;
					//	Print("Object Transport " + objs[0].GetType());
					//	ResetWaypoints();				
						return true;
					} 
					if (objs[0].IsPlainObject() && !objs[0].IsBuilding() && !objs[0].IsWell() && m_Soldier.GetTarget()) 
					{	
					//	GetGame().UpdatePathgraphRegionByObject( objs[0] );
					//	Print("Object Any " + objs[0].GetType());
						m_IsCollision = true;
					//	ResetWaypoints();
						return true;
					} 					
				}
			}
		}
		
		if (objs)
			objs.Clear();
		
		isTreeToFront = false;
		isManToFront = false;
		m_IsCollision = false;
		return false;
	}
	
	bool IsDoor(Building building, int idx)
	{
		if (!building.IsDoorOpen(idx) && !building.IsDoorLocked(idx) || building.IsDoorOpen(idx) && building.IsDoorLocked(idx))
			return true;
		else
			return false;
	}
	
	void bOpenDoors( Object target )
	{
		TStringArray doorsArray = new TStringArray;
		Building building;
	
		if( Class.CastTo(building, target)) 
		{
			
			int countDoors = GetGame().ConfigGetChildrenCount("CfgVehicles " + target.GetType() +  " Doors");
			
			for (int i = 0; i < countDoors; ++i)
			{
				if ( !building.IsDoorOpen(i) && !building.IsDoorLocked(i) )
				{
					float doorDist = vector.Distance(building.GetDoorSoundPos(i), m_Soldier.GetPosition());
					if (doorDist < 2)
						building.OpenDoor(i);
				}
			}
		}			
	}
	
    bool IsFacingTarget( Object Target )
	{
		vector pdir = m_Soldier.GetDirection();
		vector ptv = Target.GetPosition() - m_Soldier.GetPosition();

		pdir.Normalize();
		ptv.Normalize();

		if (Math.AbsFloat(pdir[0] - ptv[0]) < 1 && Math.AbsFloat(pdir[2] - ptv[2]) < 1 )
		{
			return true;			
		}

		return false;
	}
	
	static set< Object > GetObjectsAt( vector from, vector to, Object ignore = NULL, float radius = 0.5, Object with = NULL )
	{
		vector contact_pos;
		vector contact_dir;
		int contact_component;

		set< Object > geom = new set< Object >;
		set< Object > view = new set< Object >;

		DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contact_component, geom, with, ignore, false, false, ObjIntersectGeom, radius );
		DayZPhysics.RaycastRV( from, to, contact_pos, contact_dir, contact_component, view, with, ignore, false, false, ObjIntersectView, radius );
		
		
		if ( geom.Count() ) 
		{
			return geom;		
		}
		if ( geom.Count() ) 
		{
			return view;			
		}
		return NULL;
	}
	
	bool GetMove() { return m_OverrideMove; }
	bool GetWalk() { return m_OverrideWalk; }
	
 	void SendLog(string message) 
	{ 
		Print("AI BOT LOG: " + message);
	} 
}