//  Portions Copyright 2010 Tao Xing (captainxingtao@gmail.com), Xuesong Zhou (xzhou99@gmail.com)
//

//   If you help write or modify the code, please also list your names here.
//   The reason of having Copyright info here is to ensure all the modified version, as a whole, under the GPL 
//   and further prevent a violation of the GPL.

// More about "How to use GNU licenses for your own software"
// http://www.gnu.org/licenses/gpl-howto.html


//    This file is part of NeXTA Version 3 (Open-source).

//    NEXTA is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    NEXTA is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with NEXTA.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "..//Geometry.h"
#include "..//CSVParser.h"
#include "..//TLite.h"
#include "..//Network.h"
#include "..//TLiteDoc.h"
#ifndef _WIN64
#include "..//Data-Interface//include//ogrsf_frmts.h"
#endif 
#include "..//Data-Interface//XLTestDataSource.h"
#include "..//Data-Interface//XLEzAutomation.h"
#include "..//MainFrm.h"

#include "SignalNode.h"
#include "..//Dlg_SignalDataExchange.h"


extern CString g_GetExcelColumnFromNumber(int column);

static string g_DTA_movement_column_name[DTA_SIG_MOVEMENT_MAX_COLUMN] = {"node_id","scenario_no","start_day_no","end_day_no",
"start_time_in_min","end_time_in_min","notes",
"record_name",	"reference_id", 
"NBL2", "NBL","NBT","NBR","NBR2",
"SBL2","SBL","SBT","SBR","SBR2",
"EBL2","EBL","EBT","EBR","EBR2",
"WBL2","WBL","WBT","WBR","WBR2",
"NEL","NET","NER",
"NWL","NWT","NWR",
"SEL","SET","SER",
"SWL","SWT","SWR",
"PED","HOLD",
"geometry"};

static int g_DTA_movement_default_phase[DTA_SIG_MOVEMENT_MAX_COLUMN] = {0,0,0,0,
0,0,0,0,0,
7,7,4,4,4,
3,3,8,8,8,
1,1,6,6,6,
5,5,2,2,2,
7,4,4,
1,6,6,
3,8,8,
5,2,2};

static int g_DTA_phase_default_BRP[17] = {111,112,211,212,121,122,221,222,311,312,411,412,321,322,421,422};


static int g_DTA_default_split_in_seconds[9] = 
{0, 10,21,10,21,10,21,10,21};


static string g_DTA_movement_row_name[DTA_MOVEMENT_ATTRIBUTE_MAX_ROW] = {"Up Node","Dest Node","StreetName","TurnVolume","Lanes","Shared","Width","Storage","StLanes","Grade","Speed","FirstDetect","LastDetect",
"Phase1","PermPhase1","DetectPhase1","EffectiveGreen", "Capacity","V/C","Delay","LOS","IdealFlow","LostTime","SatFlowRatePerLaneGroup","SatFlowPerm","SatFlowRTOR","HeadwayFact","Volume","Peds","Bicycles","PHF","Growth",
"HeavyVehicles","BusStops","Midblock","Distance","TravelTime","Street Name"};


////  phase data 
static string g_DTA_phasing_column_name[DTA_SIG_PHASE_MAX_COLUMN] = {"timing_plan_name", "node_id",
"key","value",
"D1","D2","D3","D4",
"D5","D6","D7","D8", "D9","D10",
"D11","D12","D13","D14",
"D15","D16","D17","D18",
"P1","P2","P3","P4","P5","P6","P7","P8","geometry"};


static string g_DTA_phasing_row_name[DTA_PHASE_ATTRIBUTE_MAX_ROW] = {
	"SplitInSeconds",
	"BRP",
	"MinGreen",
	"MaxGreen",
	"VehExt",
	"TimeBeforeReduce",
	"TimeToReduce",
	"MinGap",
	"Yellow",
	"AllRed",
	"Recall",
	"Walk",
	"DontWalk",
	"PedCalls",
	"MinSplit",
	"DualEntry",
	"InhibitMax",
	"Start",
	"End",
	"Yield",
	"Yield170",
	"LocalStart",
	"LocalYield",
	"LocalYield170",
	"ActGreen",
	"Control_Type",
	"Cycle Length",
	"Lock Timings",
	"Referenced To",
	"Reference Phase",
	"Offset",
	"Master",
	"Node 0",
	"Node 1",
	"RingType",
	"OptimizationMethod",
	"VolumeAdjustmentFactor",
	"MovementCodeVector",
	"MovementVector"

};

void DTA_Movement_Data_Matrix::InitializeKeyValue(int node_id, string geometry_str)
{
	for(int ii = 0; ii < DTA_MOVEMENT_ATTRIBUTE_MAX_ROW; ii++)
	{
		DTA_SIG_MOVEMENT_ROW i = (DTA_SIG_MOVEMENT_ROW)ii;
		SetValue(DTA_SIG_MOVEMENT_NODE_ID,  i, node_id);

		SetValue(DTA_SIG_MOVEMENT_REFERENCE_ID,  i, node_id);

		SetValue(DTA_SIG_MOVEMENT_SCENARIO_NO,  i, 1);
		SetValue(DTA_SIG_MOVEMENT_START_DAY_NO,  i, 1);
		SetValue(DTA_SIG_MOVEMENT_END_DAY_NO,  i, 100);

		SetValue(DTA_SIG_MOVEMENT_START_MIN_IN_SECOND,  i, 0);
		SetValue(DTA_SIG_MOVEMENT_END_MIN_IN_SECOND,  i, 1440);

		SetString(DTA_SIG_MOVEMENT_RECORD_NAME, i, g_DTA_movement_row_name[i]);
		SetString(DTA_SIG_MOVEMENT_GEOMETRY, i, geometry_str);


	}
}
void DTA_Phasing_Data_Matrix::InitializeKeyValue(int node_id, string geometry_str)
{
	for(int ii = 0; ii < DTA_PHASE_ATTRIBUTE_MAX_ROW; ii++)
	{
		DTA_SIG_PHASE_ROW i = (DTA_SIG_PHASE_ROW)ii;
		SetValue(DTA_SIG_PHASE_NODE_ID,  i, node_id);

		SetString(DTA_SIG_Geometry, i, geometry_str);


	}
}

DTA_Direction CTLiteDoc::Find_Closest_Angle_to_Approach(int angle)
{
	if(angle < 23)
	{
		return DTA_East;
	}else if(angle < 45 && m_ApproachMap.find(DTA_East)== m_ApproachMap.end())  // East has not been used previously
	{
		return DTA_East;
	}
	else if(angle < 68 ) 
	{
		if(m_ApproachMap.find(DTA_North)== m_ApproachMap.end())  //North not used
			return DTA_North;
		else
			return DTA_NorthEast;
	}
	else if(angle < 113) 
	{
		return DTA_North;
	}
	else if(angle < 135) 
	{
		if(m_ApproachMap.find(DTA_North)== m_ApproachMap.end())  //North not used
			return DTA_North;
		else
			return DTA_NorthWest;
	}
	else if(angle < 158) 
	{
		if(m_ApproachMap.find(DTA_West)== m_ApproachMap.end())  //West not used
			return DTA_West;
		else
			return DTA_NorthWest;
	}
	else if(angle < 203) 
	{
		return DTA_West;
	}
	else if(angle < 225 && m_ApproachMap.find(DTA_West)== m_ApproachMap.end())  //West not used
	{
		return DTA_West;
	}
	else if(angle < 248) 
	{
		if(m_ApproachMap.find(DTA_South)== m_ApproachMap.end())  //South not used
			return DTA_South;
		else
			return DTA_SouthWest;
	}
	else if(angle < 293) 
	{
		return DTA_South;
	}
	else if(angle < 315) 
	{
		if(m_ApproachMap.find(DTA_South)== m_ApproachMap.end())  //South not used
			return DTA_South;
		else
			return DTA_SouthEast;
	}
	else if(angle < 338) 
	{
		if(m_ApproachMap.find(DTA_East)== m_ApproachMap.end())  //East not used
			return DTA_East;
		else
			return DTA_SouthEast;
	}
	else
		return DTA_East;
}
DTA_Direction CTLiteDoc::Find_Angle_to_Approach_8_direction(int angle)
{
	if(angle < 23)
	{
		return DTA_West;
	}
	else if(angle < 68) 
	{
		return DTA_SouthWest;
	}
	else if(angle < 113) 
	{
		return DTA_South; 
	}
	else if(angle < 158) 
	{
		return DTA_SouthEast;
	}
	else if(angle < 203) 
	{
		return DTA_East; 
	}
	else if(angle < 248) 
	{
		return DTA_NorthEast; 
	}
	else if(angle < 293) 
	{
		return DTA_North; 
	}
	else if(angle < 338) 
	{
		return DTA_NorthWest; 
	}
	else
		return DTA_West; 
}

DTA_Direction CTLiteDoc::Find_Angle_to_Approach_4_directionWithoutGivenDirection(int angle, DTA_Direction not_use_DTA_Direction)
{

	if(not_use_DTA_Direction== DTA_East)
	{
		if(angle > 180) 
		{
			return DTA_South;
		}else
			return DTA_North;

	}

	if(not_use_DTA_Direction== DTA_West)
	{
		if(angle < 180) 
		{
			return DTA_South;
		}else
			return DTA_North;

	}

	if(not_use_DTA_Direction== DTA_North)
	{
		if(angle < 90 || angle > 270) 
		{
			return DTA_East;
		}else
			return DTA_West;

	}

	if(not_use_DTA_Direction== DTA_South)
	{
		if(angle < 90 || angle > 270) 
		{
			return DTA_East;
		}else
			return DTA_West;

	}

	return DTA_NotDefined;
}

DTA_Direction CTLiteDoc::Find_Angle_to_Approach_4_direction(int angle, int &relative_angel_difference_from_main_direction)
{
	relative_angel_difference_from_main_direction = 0;
	if(angle < 45)
	{
		relative_angel_difference_from_main_direction = abs(angle-0);

		return DTA_East;
	}
	else if(angle < 135) 
	{
		relative_angel_difference_from_main_direction = abs(angle-90);
		return DTA_North;
	}
	else if(angle < 225) 
	{
		relative_angel_difference_from_main_direction = abs(angle-180);
		return DTA_West;
	}
	else if(angle < 315) 
	{
		relative_angel_difference_from_main_direction = abs(angle-270);

		return DTA_South;
	}
	else 
	{
		relative_angel_difference_from_main_direction = abs(angle-360);
		return DTA_East;

	}
}




DTA_Turn CTLiteDoc::Find_RelativeAngle_to_Left_OR_Right_Turn(int relative_angle)
{
	int min_diff = 89;

	int ideal_left = 90;
	int ideal_right = -90;
	int ideal_through = 0;

	if(abs(relative_angle - ideal_left) <= min_diff)
		return DTA_LeftTurn;

	if(abs(relative_angle - ideal_right) <= min_diff)
		return DTA_RightTurn;

	min_diff = 179;
	if(abs(relative_angle - ideal_left) <= min_diff)
		return DTA_LeftTurn;

	if(abs(relative_angle - ideal_right) <= min_diff)
		return DTA_RightTurn;


	return DTA_OtherTurn;
}

DTA_Turn CTLiteDoc::Find_RelativeAngle_to_Turn(int relative_angle)
{
	int min_through_diff = 45;

	int ideal_left = 90;
	int ideal_right = -90;
	int ideal_through = 0;


	if(abs(relative_angle - ideal_through) <= min_through_diff)
		return DTA_Through;

	if(abs(relative_angle - ideal_left) <= min_through_diff)
		return DTA_LeftTurn;

	if(abs(relative_angle - ideal_right) <= min_through_diff)
		return DTA_RightTurn;

	min_through_diff = 89;

	if(abs(relative_angle - ideal_left) <= min_through_diff)
		return DTA_LeftTurn;

	if(abs(relative_angle - ideal_right) <= min_through_diff)
		return DTA_RightTurn;


	return DTA_OtherTurn;
}
double CTLiteDoc::Find_P2P_Distance(GDPoint p1, GDPoint p2)
{
	double delta_x  = p2.x - p1.x;
	double delta_y  = p2.y - p1.y;

	return sqrt(delta_x*delta_x + delta_y* delta_y);


}


int CTLiteDoc::Find_P2P_Angle(GDPoint p1, GDPoint p2)
{
	float delta_x  = p2.x - p1.x;
	float delta_y  = p2.y - p1.y;

	if(fabs(delta_x) < 0.00001)
		delta_x =0;

	if(fabs(delta_y) < 0.00001)
		delta_y =0;

	int angle = atan2(delta_y, delta_x) * 180 / PI + 0.5;
	// angle = 90 - angle;

	while(angle < 0) 
		angle += 360;

	while(angle > 360) 
		angle -= 360;

	return angle;

}
DTA_Turn CTLiteDoc::Find_PPP_to_Turn(GDPoint p1, GDPoint p2, GDPoint p3)
{

	int relative_angle = Find_PPP_RelativeAngle(p1,p2,p3);
	return Find_RelativeAngle_to_Turn(relative_angle);

}

DTA_SIG_MOVEMENT GetMovementIndexFromDirectionAndTurn(DTA_Direction direction , DTA_Turn movement_turn)
{

	DTA_SIG_MOVEMENT  movement_approach_turn = DTA_NBL2;  // default value
	switch (direction)
	{
	case DTA_North:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_NBT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_NBL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_NBR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_NBL2; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_NBR2; break;
		}
		break;
	case DTA_East:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_EBT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_EBL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_EBR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_EBL2; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_EBR2; break;
		}
		break;
	case DTA_South:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_SBT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_SBL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_SBR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_SBL2; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_SBR2; break;
		}
		break;
	case DTA_West:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_WBT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_WBL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_WBR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_WBL2; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_WBR2; break;
		}
		break;
	case DTA_NorthEast:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_NET; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_NEL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_NER; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_NEL; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_NER; break;
		}
		break;
	case DTA_NorthWest:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_NWT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_NWL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_NWR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_NWL; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_NWR; break;
		}
		break;
	case DTA_SouthEast:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_SET; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_SEL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_SER; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_SEL; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_SER; break;
		}
		break;
	case DTA_SouthWest:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_SWT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_SWL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_SWR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_SWL; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_SWR; break;
		}
		break;
	}

	return movement_approach_turn;
}


DTA_Turn CTLiteDoc::Find_PPP_to_Turn_with_DTAApproach(GDPoint p1, GDPoint p2, GDPoint p3,DTA_Direction approach1, DTA_Direction approach2)
{
	if(approach2 == m_OpposingDirectionMap[approach1])
		return DTA_Through;
	else
	{
		int relative_angle = Find_PPP_RelativeAngle(p1,p2,p3);
		return Find_RelativeAngle_to_Left_OR_Right_Turn(relative_angle);
	}

}


int CTLiteDoc::Find_PPP_RelativeAngle(GDPoint p1, GDPoint p2, GDPoint p3)
{
	int relative_angle;

	int angle1 = Find_P2P_Angle(p1,p2);
	int angle2 = Find_P2P_Angle(p2,p3);
	relative_angle = angle2 - angle1;

	while(relative_angle > 180) 
		relative_angle -= 360;

	while(relative_angle < -180)
		relative_angle += 360;

	return relative_angle;
}

int CTLiteDoc::FindUniqueLinkID()
{
	int i;
	for( i= 1; i < m_LinkIDRecordVector.size(); i++) 
	{
		if( m_LinkIDRecordVector[i] == 0)
		{
			m_LinkIDRecordVector[i] = 1;
			return i;
		}
	}

	// all link ids have been used;
	m_LinkIDRecordVector.push_back(i);
	m_LinkIDRecordVector[i] = 1;
	return i;
}
void CTLiteDoc::AssignUniqueLinkIDForEachLink()
{
	return;

	std::list<DTALink*>::iterator iLink;

	int max_link_id = 1;
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if((*iLink)->m_LinkID >  max_link_id)
		{
			max_link_id = (*iLink)->m_LinkID ;
		}

	}

	//allocate unique id arrary

	m_LinkIDRecordVector.clear();

	max_link_id = max(max_link_id, m_LinkSet.size());
	for(int i = 0; i <= max_link_id; i++)
	{
		m_LinkIDRecordVector.push_back(0);
	}


	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if((*iLink)->m_LinkID != 0)  // with possible real id
		{
			if(m_LinkIDRecordVector[(*iLink)->m_LinkID] == 0)  // not been used before
			{
				// mark link id
				m_LinkIDRecordVector[(*iLink)->m_LinkID] =1;
			}else
			{  // link id has been used
				(*iLink)->m_LinkID = 0; // need to reset link id;
			}

		}

	}

	// last step: find unique id 
	for (iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		if((*iLink)->m_LinkID == 0)
		{
			// need a new unique link id
			(*iLink)->m_LinkID = FindUniqueLinkID();

		}
	}

}

void CTLiteDoc::ConstructMovementVector() 
// this function has 8 directions
{ 
	CString SettingsFile;
	SettingsFile = this->m_ProjectFile ;
	int prohibit_non_specified_movement_on_freeway_and_ramp = (int)(g_GetPrivateProfileDouble("movement", "prohibit_non_specified_movement_on_freeway_and_ramp", 1, SettingsFile));	
	m_hide_non_specified_movement_on_freeway_and_ramp = (int)(g_GetPrivateProfileDouble("movement", "hide_non_specified_movement_on_freeway_and_ramp", 1, SettingsFile));	


	// input: node, link and geometry
	// output: m_MovementVector for each node

	m_OpposingDirectionMap[DTA_North] = DTA_South;
	m_OpposingDirectionMap[DTA_South] = DTA_North;

	m_OpposingDirectionMap[DTA_East] = DTA_West;
	m_OpposingDirectionMap[DTA_West] =  DTA_East;

	m_OpposingDirectionMap[DTA_NorthEast] =  DTA_SouthWest;
	m_OpposingDirectionMap[DTA_SouthWest] =  DTA_NorthEast;

	m_OpposingDirectionMap[DTA_SouthEast] =  DTA_NorthWest;
	m_OpposingDirectionMap[DTA_NorthWest] =  DTA_SouthEast;

	m_ApproachMap.clear();

	for(int tp = 0; tp< m_TimingPlanVector.size(); tp++)  //  loop for all timing plans
	{
		std::string timing_plan_name = m_TimingPlanVector[tp].timing_plan_name;  // fetch timing_plan (unique) name


	int signal_count = 0;
	// generate all movements
	int i = 0;
	for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++, i++)
	{  // for current node

		(*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .clear ();

		signal_count ++;
		// step 1: mark movement turn first
		m_ApproachMap.clear();
		for(int inbound_i= 0; inbound_i< (*iNode)->m_IncomingLinkVector.size(); inbound_i++)
		{
			// for each incoming link
			int IncomingLinkNo = (*iNode)->m_IncomingLinkVector[inbound_i];

			if(m_LinkNoMap.find(IncomingLinkNo) != m_LinkNoMap.end())
			{
			DTALink* pIncomingLink = m_LinkNoMap[IncomingLinkNo];

			for(int outbound_i= 0; outbound_i< (*iNode)->m_OutgoingLinkVector.size(); outbound_i++)
			{
				//for each outging link
				//for each outging link
				int OutgoingLinkNo = (*iNode)->m_OutgoingLinkVector[outbound_i];

				if(m_LinkNoMap.find(OutgoingLinkNo) == m_LinkNoMap.end())
					continue;

				DTALink* pOutgoingLink = m_LinkNoMap[OutgoingLinkNo];

				if(pOutgoingLink==NULL)
					continue;



				if (m_LinkNoMap.find(OutgoingLinkNo)!= m_LinkNoMap.end() && pIncomingLink->m_FromNodeNumber  != pOutgoingLink->m_ToNodeNumber )
				{
					// do not consider u-turn

					DTANodeMovement element;

					element.in_link_to_node_id = pIncomingLink->m_ToNodeID ;					
					element.IncomingLinkNo = pIncomingLink->m_LinkNo ;
					element.OutgoingLinkNo = pOutgoingLink ->m_LinkNo ;
					element.in_link_from_node_id = pIncomingLink->m_FromNodeID ;		
					element.out_link_to_node_id = pOutgoingLink ->m_ToNodeID ;


					GDPoint p1, p2, p3;
					p1  = m_NodeNoMap[element.in_link_from_node_id]->pt;
					p2  = m_NodeNoMap[element.in_link_to_node_id]->pt;
					p3  = m_NodeNoMap[element.out_link_to_node_id]->pt;

					element.movement_direction = Find_Angle_to_Approach_8_direction(Find_P2P_Angle(p1,p2));

					m_ApproachMap[element.movement_direction] = 1;

				}  // not u turn

			} // outgoing link

			}
		}  //incoming link
		// step 2: scan each inbound link and outbound link


		for(int inbound_i= 0; inbound_i< (*iNode)->m_IncomingLinkVector.size(); inbound_i++)
		{
			// for each incoming link
			int IncomingLinkNo = (*iNode)->m_IncomingLinkVector[inbound_i];

			if(m_LinkNoMap.find(IncomingLinkNo) == m_LinkNoMap.end())
				continue;

			DTALink* pIncomingLink = m_LinkNoMap[IncomingLinkNo];

			// pre processing to determine the number of turning movements

			std::map<DTA_Turn,int> MovementCount;
			std::map<DTA_Turn,int> MaxAbsAngelForMovement;

			for(int outbound_i= 0; outbound_i< (*iNode)->m_OutgoingLinkVector.size(); outbound_i++)
			{
				//for each outging link
				//for each outging link
				int OutgoingLinkNo = (*iNode)->m_OutgoingLinkVector[outbound_i];


				if(m_LinkNoMap.find(OutgoingLinkNo) == m_LinkNoMap.end())
					continue;

				DTALink* pOutgoingLink = m_LinkNoMap[OutgoingLinkNo];

				if(pOutgoingLink==NULL)
					continue;

				if (pIncomingLink->m_FromNodeNumber  != pOutgoingLink->m_ToNodeNumber )
				{
					// do not consider u-turn

					DTANodeMovement element;

					element.in_link_to_node_id = pIncomingLink->m_ToNodeID ;					
					element.IncomingLinkNo = pIncomingLink->m_LinkNo;
					element.OutgoingLinkNo = pOutgoingLink ->m_LinkNo ;
					element.in_link_from_node_id = pIncomingLink->m_FromNodeID ;		
					element.out_link_to_node_id = pOutgoingLink ->m_ToNodeID ;

					int up_node = m_NodeNoMap[element.in_link_from_node_id]->m_NodeNumber;

					GDPoint p1, p2, p3;
					p1  = m_NodeNoMap[element.in_link_from_node_id]->pt;
					p2  = m_NodeNoMap[element.in_link_to_node_id]->pt;
					p3  = m_NodeNoMap[element.out_link_to_node_id]->pt;


					// method 1: identify movement approach from scratch 
					element.movement_direction = Find_Closest_Angle_to_Approach(Find_P2P_Angle(p1,p2));
					// method 2: read pre-generated layout file from synchro

					CString str_key;
					str_key.Format("%d,%d",element.in_link_to_node_id, element.in_link_from_node_id);


					element.movement_turn = Find_PPP_to_Turn(p1,p2,p3);

					if (m_PredefinedApproachMap.find(str_key) != m_PredefinedApproachMap.end())  // approach has been predefined in synchro_layout.csv file
					{
						element.movement_direction = m_PredefinedApproachMap[str_key];

						str_key.Format("%d,%d",element.in_link_to_node_id, element.out_link_to_node_id);

						DTA_Direction approach2= m_PredefinedApproachMap[str_key];

						element.movement_turn = Find_PPP_to_Turn_with_DTAApproach(p1,p2,p3,element.movement_direction,approach2);
						MovementCount[element.movement_turn]+=1;

						if(MaxAbsAngelForMovement.find(element.movement_turn) == MaxAbsAngelForMovement.end()) // initialize
						{

							MaxAbsAngelForMovement[element.movement_turn] = abs(Find_PPP_RelativeAngle(p1,p2,p3));
						}else  // with data before
						{
							int relative_angel = abs(Find_PPP_RelativeAngle(p1,p2,p3));

							if(relative_angel > MaxAbsAngelForMovement[element.movement_turn])
								MaxAbsAngelForMovement[element.movement_turn] = relative_angel;

						}

					}

				}
			}  // outgoing

			////////////////////////////////// end of processing /////////////////////////////////////////////// //

			// for each incoming link
			for(int outbound_i= 0; outbound_i< (*iNode)->m_OutgoingLinkVector.size(); outbound_i++)
			{
				int OutgoingLinkNo = (*iNode)->m_OutgoingLinkVector[outbound_i];

				if(m_LinkNoMap.find(OutgoingLinkNo) == m_LinkNoMap.end())
					continue;
				DTALink* pOutgoingLink = m_LinkNoMap[OutgoingLinkNo];

				if (pIncomingLink->m_FromNodeNumber  != pOutgoingLink->m_ToNodeNumber )
				{
					// do not consider u-turn

					DTANodeMovement element;


					element.in_link_to_node_id = pIncomingLink->m_ToNodeID ;					
					element.IncomingLinkNo = pIncomingLink->m_LinkNo ;
					element.OutgoingLinkNo = pOutgoingLink ->m_LinkNo ;
					element.in_link_from_node_id = pIncomingLink->m_FromNodeID ;		
					element.out_link_to_node_id = pOutgoingLink ->m_ToNodeID ;

					int up_node = m_NodeNoMap[element.in_link_from_node_id]->m_NodeNumber;

					if(element.in_link_from_node_id == 49 &&  element.in_link_to_node_id == 14 &&  element.out_link_to_node_id == 20)
					{
						TRACE("Up Node: %d\n",up_node);
					}

					GDPoint p1, p2, p3;
					p1  = m_NodeNoMap[element.in_link_from_node_id]->pt;
					p2  = m_NodeNoMap[element.in_link_to_node_id]->pt;
					p3  = m_NodeNoMap[element.out_link_to_node_id]->pt;


					// method 1: identify movement approach from scratch 
					element.movement_direction = Find_Closest_Angle_to_Approach(Find_P2P_Angle(p1,p2));
					// method 2: read pre-generated layout file from synchro

					CString str_key;
					str_key.Format("%d,%d",element.in_link_to_node_id, element.in_link_from_node_id);


					element.movement_turn = Find_PPP_to_Turn(p1,p2,p3);


					int node_number =  m_NodeNoMap[element.out_link_to_node_id]->m_NodeNumber ;

					switch (element.movement_turn)
					{
					case DTA_Through: 
						pIncomingLink->m_DSPThroughNodeNumber = node_number ; 
						element.QEM_Lanes = pIncomingLink->m_NumberOfLanes;
						break;
					case DTA_LeftTurn: 
						pIncomingLink->m_DSPLeftNodeNumber = node_number;
						element.QEM_Lanes = pIncomingLink->m_NumberOfLanes;
						break;
					case DTA_LeftTurn2:
						pIncomingLink->m_DSPOther1NodeNumber = node_number; 
						element.QEM_Lanes = pIncomingLink->m_NumberOfLeftTurnLanes ;
						break;

					case DTA_RightTurn:
						pIncomingLink->m_DSPRightNodeNumber = node_number; 
						element.QEM_Lanes =  pIncomingLink->m_NumberOfLeftTurnLanes ;
						break;

					case DTA_RightTurn2: 
						pIncomingLink->m_DSPOther2NodeNumber =node_number; 
						element.QEM_Lanes = pIncomingLink->m_NumberOfRightTurnLanes ; 
						break;
					}

					element.QEM_Speed  = pIncomingLink->m_SpeedLimit ;



					if (m_PredefinedApproachMap.find(str_key) != m_PredefinedApproachMap.end())  // approach has been predefined in synchro_layout.csv file
					{
						element.movement_direction = m_PredefinedApproachMap[str_key];

						str_key.Format("%d,%d",element.in_link_to_node_id, element.out_link_to_node_id);

						DTA_Direction approach2= m_PredefinedApproachMap[str_key];

						element.movement_turn = Find_PPP_to_Turn_with_DTAApproach(p1,p2,p3,element.movement_direction,approach2);

						if(MovementCount[element.movement_turn] >=2)
						{
							// we have more than 1 movement, use angle to determine movement again

							int relative_angel = abs(Find_PPP_RelativeAngle(p1,p2,p3));

							if(relative_angel == MaxAbsAngelForMovement[element.movement_turn])
							{  // adjust movement

								if(element.movement_turn == DTA_LeftTurn)
									element.movement_turn = DTA_LeftTurn2;

								if(element.movement_turn == DTA_RightTurn)
									element.movement_turn = DTA_RightTurn2;

							}


						}

					}


					// determine  movement type /direction here
					element.movement_approach_turn = DTA_LANES_COLUME_init;
					switch (element.movement_direction)
					{
					case DTA_North:

						
						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_NBT; element.QEM_Phase1 = 2; break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_NBL; element.QEM_PermPhase1 = 2; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_NBR; element.QEM_PermPhase1 = 2; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_NBL2; element.QEM_PermPhase1 = 2; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_NBR2; element.QEM_PermPhase1 = 2; break;
						}
						break;
					case DTA_East:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_EBT; element.QEM_Phase1 = 4;  break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_EBL; element.QEM_PermPhase1 = 4; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_EBR; element.QEM_PermPhase1 = 4; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_EBL2; element.QEM_PermPhase1 = 4; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_EBR2; element.QEM_PermPhase1 = 4;  break;
						}
						break;
					case DTA_South:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_SBT; element.QEM_Phase1 = 8;  break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_SBL; element.QEM_PermPhase1 = 8;  break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_SBR; element.QEM_PermPhase1 = 8; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_SBL2; element.QEM_PermPhase1 = 8; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_SBR2; element.QEM_PermPhase1 = 8; break;
						}
						break;
					case DTA_West:
						
						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_WBT; element.QEM_Phase1 = 6; break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_WBL; element.QEM_PermPhase1 = 6; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_WBR; element.QEM_PermPhase1 = 6; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_WBL2; element.QEM_PermPhase1 = 6; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_WBR2; element.QEM_PermPhase1 = 6;  break;
						}
						break;
					case DTA_NorthEast:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_NET; element.QEM_Phase1 = 2; break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_NEL; element.QEM_PermPhase1 = 2; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_NER; element.QEM_PermPhase1 = 2; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_NEL; element.QEM_PermPhase1 = 2; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_NER; element.QEM_PermPhase1 = 2;  break;
						}
						break;
					case DTA_NorthWest:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_NWT; 						element.QEM_Phase1 = 4; break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_NWL; 						element.QEM_PermPhase1 = 4; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_NWR; 						element.QEM_PermPhase1 = 4; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_NWL; 						element.QEM_PermPhase1 = 4; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_NWR; 						element.QEM_PermPhase1 = 4; break;
						}
						break;
					case DTA_SouthEast:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_SET; 						element.QEM_Phase1 = 8;  break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_SEL; 						element.QEM_PermPhase1 = 8; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_SER; 						element.QEM_PermPhase1 = 8; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_SEL; 						element.QEM_PermPhase1 = 8; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_SER; 						element.QEM_PermPhase1 = 8; break;
						}
						break;
					case DTA_SouthWest:
						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_SWT; 						element.QEM_Phase1 = 6;break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_SWL; 						element.QEM_PermPhase1 = 6; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_SWR; 						element.QEM_PermPhase1 = 6; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_SWL;						element.QEM_PermPhase1 = 6; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_SWR; 						element.QEM_PermPhase1 = 6; break;
						}
						break;
					}

					// rule 1: 

					if(element.movement_turn == DTA_OtherTurn || element.movement_turn == DTA_LeftTurn || element.movement_turn == DTA_RightTurn2 )

					{
						if( (m_LinkTypeMap[pIncomingLink->m_link_type].IsFreeway () &&  m_LinkTypeMap[pOutgoingLink->m_link_type].IsRamp  () ) 
							|| (m_LinkTypeMap[pIncomingLink->m_link_type].IsRamp () &&  m_LinkTypeMap[pOutgoingLink->m_link_type].IsFreeway  ()))
						{

							element.bNonspecifiedTurnDirectionOnFreewayAndRamps = true;



							// do not load the non-specified movement for ramp to freeway or freeway to ramp connection
							element.turning_prohibition_flag = true;

						}
					}

					//rule 2:
					if(m_LinkTypeMap[pIncomingLink->m_link_type].IsRamp () &&  m_LinkTypeMap[pOutgoingLink->m_link_type].IsRamp  ())
					{

						// test if the incoming ramp is connected with a freeway link
						bool bWithConnectedFreewayLink = false;
						DTANode* pNode = m_NodeNoMap[ pIncomingLink->m_ToNodeID ]; 

						for(int outbound_ii= 0; outbound_ii<pNode->m_OutgoingLinkVector.size(); outbound_ii++)
						{
							int OutgoingLinkNo2 = pNode->m_OutgoingLinkVector[outbound_ii];
							DTALink* pOutgoingLink2 = m_LinkNoMap[OutgoingLinkNo2];

							if(m_LinkTypeMap[pOutgoingLink2->m_link_type].IsFreeway ())
							{
								bWithConnectedFreewayLink = true;
								break;
							}

						}
						// 

						if(bWithConnectedFreewayLink == true)
						{
							element.bNonspecifiedTurnDirectionOnFreewayAndRamps = true;

							// do not load the non-specified movement for ramp to freeway or freeway to ramp connection
							element.turning_prohibition_flag = true;
						}
					}

					//rule 3:

					if( element.movement_turn == DTA_OtherTurn  && m_LinkTypeMap[pIncomingLink->m_link_type].IsFreeway () &&  m_LinkTypeMap[pOutgoingLink->m_link_type].IsFreeway  () ) 
					{

						// test if the outgoing freeway is connected with an incoming ramp
						bool bWithConnectedRampLink = false;
						DTANode* pNode = m_NodeNoMap[ pIncomingLink->m_ToNodeID ]; 

						for(int inbound_ii= 0; inbound_ii<pNode->m_IncomingLinkVector.size(); inbound_ii++)
						{
							int IncomingLinkNo2 = pNode->m_IncomingLinkVector[inbound_ii];
							DTALink* pIncomingLink2 = m_LinkNoMap[IncomingLinkNo2];

							if(m_LinkTypeMap[pIncomingLink2->m_link_type].IsRamp ())
							{
								bWithConnectedRampLink = true;
								break;
							}

						}
						// 

						if(bWithConnectedRampLink == true)
						{
							element.bNonspecifiedTurnDirectionOnFreewayAndRamps = true;

							// do not load the non-specified movement for ramp to freeway or freeway to ramp connection
							element.turning_prohibition_flag = true;
						}					
					}


					if(element.QEM_EffectiveGreen == 0 && ((*iNode)->m_ControlType == m_ControlType_PretimedSignal ||  (*iNode)->m_ControlType == m_ControlType_ActuatedSignal) && element.turning_prohibition_flag==0) 
						element.QEM_EffectiveGreen = 6;

				
						// create movement vector for all timing plans

					(*iNode)->SetupNodeMovementVector( timing_plan_name, element);


				}  // for each feasible movement (without U-turn)

			} // for each outbound link

		} // for each inbound link

	}// for each node

	m_ApproachMap.clear();

	}  // for each timing plan
	m_bMovementAvailableFlag = 1;
}


void CTLiteDoc::OnFileConstructandexportsignaldata()
{

	//	if(m_import_shape_files_flag == 1)
	//		SaveProject(m_ProjectFile);

	CDlg_SignalDataExchange dlg;
	dlg.m_pDoc = this;
	dlg.DoModal();

}
void CTLiteDoc::Constructandexportsignaldata()
{


	m_Synchro_ProjectDirectory  = m_ProjectDirectory + "Exporting_Synchro_UTDF\\";
	if ( GetFileAttributes(m_Synchro_ProjectDirectory) == INVALID_FILE_ATTRIBUTES) 
	{
		CreateDirectory(m_Synchro_ProjectDirectory,NULL);
	}



	if (m_bMovementAvailableFlag == 0)  // has not been initialized. 
	{
		ConstructMovementVector();
	}
	ExportSynchroVersion6Files();

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	ShellExecute( NULL,  "explore", m_Synchro_ProjectDirectory, NULL,  NULL, SW_SHOWNORMAL );




}


void CTLiteDoc::ExportSynchroVersion6Files(std::string TimingPlanName)
{
	// try to set world coordinate so that all coordinates are non-negative

	m_Origin.x  = 1000000;
	m_Origin.y  = 1000000;


	//find minimum value:
	std::list<DTANode*>::iterator  iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		m_Origin.x = min(m_Origin.x,(*iNode)->pt .x);
		m_Origin.y= min(m_Origin.y,(*iNode)->pt .y);
	}

	bool bUseOriginalNodeNumber = false;
	//if (AfxMessageBox("Do you want to use the original and (unsorted) node numbers?\nClick No to regenerate sequential node numbers for Synchro.",
	//	MB_YESNO | MB_ICONINFORMATION) == IDYES)
	//	bUseOriginalNodeNumber = true;

	int i_n = 0;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++, i_n++)
	{  // for current node	

		if (bUseOriginalNodeNumber == true)
			(*iNode)->UTDF_node_id = (*iNode)->m_NodeNumber;
		else
			(*iNode)->UTDF_node_id = (*iNode)->m_NodeNo +1; // internal node starts from 0


		(*iNode)->m_QEM_ReferenceNodeNumber =
			(*iNode)->UTDF_node_id;

		double resolution = 1/max(0.000001,m_UnitFeet);
		float pt_x = NPtoSP_X((*iNode)->pt,resolution);
		float pt_y = NPtoSP_Y((*iNode)->pt,resolution);

		(*iNode)->UTDF_pt.x = pt_x;
		(*iNode)->UTDF_pt.y = pt_y;

		int control_type = 0;
		if((*iNode)->m_ControlType == m_ControlType_PretimedSignal)
			control_type = 0;   //signalized 
		else
		{
			if( ((*iNode)->m_IncomingLinkVector.size() > 0 && (*iNode)->m_OutgoingLinkVector.size() ==0) /* only has incoming links*/ 
				||  ((*iNode)->m_IncomingLinkVector.size() == 0 && (*iNode)->m_OutgoingLinkVector.size() > 0 
				) /* only has outgoing links*/ )
			{
				control_type = 1;   // external nodes
			}else
			{
				control_type = 3;   // unsignalized

			}

			if((*iNode)->m_IncomingLinkVector.size() == 1 && (*iNode)->m_OutgoingLinkVector.size()==1 )
			{
				int IncomingLinkNo = (*iNode)->m_IncomingLinkVector[0];
				int OutgoingLinkNo = (*iNode)->m_OutgoingLinkVector[0];

				DTALink* pIncomingLink = m_LinkNoMap[IncomingLinkNo];
				DTALink* pOutgoingLink = m_LinkNoMap[OutgoingLinkNo];	

				if(pIncomingLink->m_FromNodeNumber == pOutgoingLink->m_ToNodeNumber )
				{
					control_type = 1;   // external nodes
				}

			}
		}


		(*iNode)->UTDF_control_type = control_type;

	}

	FILE* st = NULL;
	fopen_s(&st,m_Synchro_ProjectDirectory+"UTDF.csv","w");
	if(st==NULL)
	{
		AfxMessageBox("File UTDF.csv file cannot be opened.");
		return;
	}

	fprintf(st,"[Network]\n"	
		"Network Settings,\n"	
		"RECORDNAME,DATA\n"
		"UTDFVERSION,8\n"
		"Metric,0\n"
		"yellowTime,3.5\n"
		"allRedTime,0.5\n"
		"Walk,5\n"
		"DontWalk,11\n"
		"HV,0.02\n"
		"PHF,1\n"
		"DefWidth,12\n"
		"DefFlow,1900\n"
		"vehLength,25\n"
		"heavyvehlength,45\n"
		"criticalgap,4.5\n"
		"followuptime,2.5\n"
		"stopthresholdspeed,5\n"
		"criticalmergegap,3.7\n"
		"growth,1\n"
		"PedSpeed,4\n"
		"LostTimeAdjust,0\n"
		"ScenarioTime,%s\n", GetTimeStampString(m_DemandLoadingEndTimeInMin) );





	fprintf(st,"\n[Nodes]\n"											
		"Node Data\n"											
		"INTID,TYPE,X,Y,Z,DESCRIPTION,CBD,Inside,Radius,Outside,Radius,Roundabout,Lanes,Circle,Speed\n");	

	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		fprintf(st, "%i,%i,%f,%f,0\n", (*iNode)->UTDF_node_id , (*iNode)->UTDF_control_type ,
			(*iNode)->UTDF_pt .x , (*iNode)->UTDF_pt .y);

	}


	FILE* st_layout = NULL;
	fopen_s(&st_layout, m_Synchro_ProjectDirectory + "LAYOUT.csv", "w");
	if (st_layout == NULL)
	{
		AfxMessageBox("File LAYOUT.csv file cannot be opened.");

	}
	else
	{
	
		fprintf(st_layout, "Layout Data \n");
		fprintf(st_layout, "INTID,INTNAME,TYPE,X,Y,NID,SID,EID,WID,NEID,NWID,SEID,SWID,NNAME,SNAME,ENAME,WNAME,NENAME,NWNAME,SENAME,SWNAME");
		fprintf(st_layout, "\n");

	
		int i_n = 0;
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++, i_n++)
		{  // for current node	


			int control_type = 0;
			if ((*iNode)->m_ControlType == m_ControlType_PretimedSignal)
				control_type = 0;
			else
			{
				if (((*iNode)->m_IncomingLinkVector.size() > 0 && (*iNode)->m_OutgoingLinkVector.size() == 0) /* only has incoming links*/
					|| ((*iNode)->m_IncomingLinkVector.size() == 0 && (*iNode)->m_OutgoingLinkVector.size() > 0) /* only has outgoing links*/)
				{
					control_type = 1;   // external nodes
				}
				else
				{
					control_type = 3;   // unsignalized

				}

			}
			fprintf(st_layout, "%i,%i,%i,%f,%f,", (*iNode)->UTDF_node_id, (*iNode)->m_NodeNumber, control_type,
				(*iNode)->UTDF_pt.x, (*iNode)->UTDF_pt.y);
			// find connecting nodes and links at different directions

			int Up_ID[10] = { 0 };
			for (int inbound_i = 0; inbound_i < (*iNode)->m_IncomingLinkVector.size(); inbound_i++)
			{
				if (m_LinkNoMap.find((*iNode)->m_IncomingLinkVector[inbound_i]) == m_LinkNoMap.end())
					continue;

				DTALink* pInLink = m_LinkNoMap[(*iNode)->m_IncomingLinkVector[inbound_i]];

				int up_node_id = m_NodeNoMap[pInLink->m_FromNodeID]->UTDF_node_id;
				GDPoint p1 = m_NodeNoMap[pInLink->m_FromNodeID]->pt;
				GDPoint p2 = (*iNode)->pt;

				DTA_Direction incoming_approach = Find_Angle_to_Approach_8_direction(Find_P2P_Angle(p1, p2));
				switch (incoming_approach)
				{
				case DTA_North:
					Up_ID[1] = up_node_id;
					break;
				case DTA_South:
					Up_ID[0] = up_node_id;
					break;
				case DTA_East:
					Up_ID[3] = up_node_id;
					break;
				case DTA_West:
					Up_ID[2] = up_node_id;
					break;
				case DTA_NorthEast:
					Up_ID[7] = up_node_id;
					break;
				case DTA_NorthWest:
					Up_ID[6] = up_node_id;
					break;
				case DTA_SouthEast:
					Up_ID[5] = up_node_id;
					break;
				case DTA_SouthWest:
					Up_ID[4] = up_node_id;
					break;
				}
			}

			// write into file
			for (int i = 0; i< 8; i++)
			{
				if (Up_ID[i]!=0)
					fprintf(st_layout, "%i,", Up_ID[i]);
				else
					fprintf(st_layout, ",");
			}
			for (int i = 0; i<8; i++)
			{
					fprintf(st_layout, ",");
			}
			fprintf(st_layout, "\n");
		}
		fprintf(st_layout, "\n");

		fclose(st_layout);
	
	}




	fprintf(st,"\n[Links]\n"								
		"Link Data\n"								
		"RECORDNAME,INTID,NB,SB,EB,WB,NE,NW,SE,SW");



	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++, i_n++)
	{
		// find connecting nodes and links at different directions
		DTALink* pLinkVector[8] = {NULL};

		int NumberOfLanes[8] = {0};
		// try outbound link first, will be overwriten by the link
		//for(int outbound_i= 0; outbound_i< (*iNode)->m_OutgoingLinkVector  .size(); outbound_i++)
		//{		
		//	int LinkID = (*iNode)->m_OutgoingLinkVector [outbound_i];

		//	DTALink* pLink = m_LinkNoMap[LinkID];

		//	if(pLink->m_NumberOfLanes > 0 )
		//	{
		//		int down_node_id = pLink ->m_ToNodeID; 
		//		GDPoint p1  = m_NodeNoMap[down_node_id]->pt;
		//		GDPoint p2  = (*iNode)->pt;
		//		int outgoing_approach = Find_Angle_to_Approach_8_direction(Find_P2P_Angle(p1,p2));
		//		int index = (int)(outgoing_approach);

		//		pLinkVector [index] = pLink;
		//		NumberOfLanes [index]  = 0;  //dummy link


		//	}
		//} // for each outbound link


		//
		for(int inbound_i= 0; inbound_i< (*iNode)->m_IncomingLinkVector .size(); inbound_i++)
		{		
			int LinkID = (*iNode)->m_IncomingLinkVector [inbound_i];

			DTALink* pIncomingLink = m_LinkNoMap[LinkID];

			if(pIncomingLink->m_NumberOfLanes > 0 && pIncomingLink->m_FromNodeID != (*iNode)->m_NodeNo )
			{
				int up_node_id = pIncomingLink ->m_FromNodeID; 
				GDPoint p1  = m_NodeNoMap[up_node_id]->pt;
				GDPoint p2  = (*iNode)->pt;
				int incoming_approach = Find_Angle_to_Approach_8_direction(Find_P2P_Angle(p2,p1));
				int index = (int)(incoming_approach);

				pLinkVector [index] = pIncomingLink;
				NumberOfLanes [index] = pIncomingLink->m_NumberOfLanes;


			}
		} // for each incoming link

		//attribute 1:

		int i;
		fprintf(st,"\nUp ID,%d,", (*iNode)->UTDF_node_id );
		for(i=0; i<8; i++)  // 8 directions
		{
			if (pLinkVector[i]!=NULL) 
				fprintf(st, "%i,", pLinkVector[i]->m_FromNodeID+1);
			else
				fprintf(st, ",");
		}

		//attribute 2:
		fprintf(st,"\nLanes,%d,", (*iNode)->UTDF_node_id );
		for(i=0; i<8; i++)  // 8 directions
		{
			if (pLinkVector[i]!=NULL) 
				fprintf(st, "%i,",NumberOfLanes[i] );
			else
				fprintf(st, ",");
		}

		//attribute 3:
		fprintf(st,"\nName,%d,", (*iNode)->UTDF_node_id );
		for(i=0; i<8; i++)  // 8 directions
		{
			if (pLinkVector[i]!=NULL && pLinkVector[i]->m_Name.size() > 0) 
				fprintf(st, "%s,", pLinkVector[i]->m_Name  );
			else
			fprintf(st, ",");
		}

		//attribute 4:
		fprintf(st,"\nDistance,%d,", (*iNode)->UTDF_node_id );
		for(i=0; i<8; i++)  // 8 directions
		{
			if (pLinkVector[i]!=NULL) 
				fprintf(st, "%f,", pLinkVector[i]->m_Length * 5480  );  // miles to feet
			else
				fprintf(st, ",");
		}

		//attribute 4:
		fprintf(st,"\nSpeed,%d,", (*iNode)->UTDF_node_id );
		for(i=0; i<8; i++)  // 8 directions
		{
			if (pLinkVector[i]!=NULL) 
				fprintf(st, "%f,", pLinkVector[i]->m_SpeedLimit  );  
			else
				fprintf(st, ",");
		}
		//attribute 4:
		fprintf(st,"\nTime,%d,", (*iNode)->UTDF_node_id );
		for(i=0; i<8; i++)  // 8 directions
		{
			if (pLinkVector[i]!=NULL) 
				fprintf(st, "7," );  
			else
				fprintf(st, ",");
		}
		//attribute 5:
		fprintf(st,"\nGrade,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "0," ); else 	fprintf(st, ",");}
		fprintf(st,"\nMedian,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "0," ); else 	fprintf(st, ",");}
		fprintf(st,"\nOffset,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "0," ); else 	fprintf(st, ",");}
		fprintf(st,"\nTWLTL,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "0," ); else 	fprintf(st, ",");}
		fprintf(st,"\nCrosswalk Width,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "16," ); else 	fprintf(st, ",");}
		fprintf(st,"\nMandatory Distance,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "0," ); else 	fprintf(st, ",");}
		fprintf(st,"\nMandatory Distance2,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "0," ); else 	fprintf(st, ",");}

		fprintf(st,"\nPositioning Distance,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "0," ); else 	fprintf(st, ",");}
		fprintf(st,"\nPositioning Distance2,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "0," ); else 	fprintf(st, ",");}

		fprintf(st,"\nCurve Pt X,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "," ); else 	fprintf(st, ",");}
		fprintf(st,"\nCurve Pt Y,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "," ); else 	fprintf(st, ",");}
		fprintf(st,"\nCurve Pt Z,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "," ); else 	fprintf(st, ",");}

		fprintf(st,"\nLink Is Hidden,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "FALSE," ); else 	fprintf(st, ",");}
		fprintf(st,"\nStreet Name Is Hidden,%d,", (*iNode)->UTDF_node_id );  	for(i=0; i<8; i++) 	{if (pLinkVector[i]!=NULL) fprintf(st, "FALSE," ); else 	fprintf(st, ",");}

	}
	fprintf(st,"\n\n[Lanes]\n"																											
		"Lane Group Data\n"																										
		"RECORDNAME,INTID,");

	for(int j= DTA_NBL2; j<= DTA_HOLD;j++)
		fprintf(st, "%s,", g_DTA_movement_column_name[j].c_str());

	fprintf(st,"\n");																											

	std::string timing_plan_name = "ALLDAY";
	// prepare lanes data 
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

//		if ((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)  
		{

			CString key;
			key.Format ("%d:FREE", (*iNode)->UTDF_node_id);

			for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .size(); m++)
			{

				// assign values per movement

				DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];


				if( (*iNode)->IsEmptyPhaseNumber(TimingPlanName) == true)
				{
					(*iNode)->ResetToDefaultPhaseNumbers(TimingPlanName); 

				}

				if( ((*iNode)->m_ControlType == m_ControlType_PretimedSignal ||  (*iNode)->m_ControlType == m_ControlType_ActuatedSignal) && movement.turning_prohibition_flag==0)
				{
					if(movement.QEM_Lanes >=1 && movement.QEM_EffectiveGreen < 6)
					{
						DTALink* pIncomingLink = m_LinkNoMap[movement.IncomingLinkNo ];

						(*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m].QEM_EffectiveGreen =  (int)((*iNode)->m_CycleLengthInSecond* pIncomingLink->m_LaneCapacity / pIncomingLink->m_Saturation_flow_rate_in_vhc_per_hour_per_lane);
					}


				}

				movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m]; // refetch data

				DTA_SIG_MOVEMENT movement_index = movement.movement_approach_turn;

				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_UpNode,movement.in_link_from_node_id+1);
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_DestNode,movement.out_link_to_node_id +1);

				DTATimingPlan timing_plan =  GetTimingPlanInfo("ALLDAY");
				float number_of_hours = max(0.01, (m_DemandLoadingEndTimeInMin - m_DemandLoadingStartTimeInMin) / 60.0);
				float sim_turn_hourly_count = movement.sim_turn_count;
				
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_TurnVolume,sim_turn_hourly_count);
				
				
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_Lanes,movement.QEM_Lanes);
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_Shared,movement.QEM_Shared );
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_Width,movement.QEM_Width );
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_Storage,movement.QEM_Storage  );
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_StLanes,movement.QEM_StLanes   );
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_Grade,movement.QEM_Grade    );
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_Speed,movement.QEM_Speed     );

				// DTA_MOVEMENT_ATTRIBUTE_FirstDetect: no variable in movement
				// DTA_MOVEMENT_ATTRIBUTE_LastDetect: no variable in movement
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_Phase1,movement.QEM_Phase1     );
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_PermPhase1,movement.QEM_PermPhase1      );
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_IdealFlow,movement.QEM_IdealFlow      );

				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_LostTime,movement.QEM_LostTime       );
				m_DTAMovementMap[key].SetValue (movement_index, DTA_MOVEMENT_ATTRIBUTE_SatFlow,movement.QEM_SatFlow        );\

					// setup phase data

					// this phase number is used now. 

					int phase_no  = movement.QEM_Phase1 ;

				DTA_SIG_PHASE phase_index = (DTA_SIG_PHASE)(phase_no+ 8) ;
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_MinGreen, 4);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_MaxGreen, 19);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_Yellow, 3.5);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_AllRed, 0.5);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_VehExt, 3);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_MinGap, 3);

				m_DTAPhasingMap[key].SetValue(DTA_SIG_PHASE_VALUE, TIMING_CycleLength, 50);

				// 

			}  // for each movement



			// for each record

			// write data per record name and then per movement 
			for(int i = DTA_MOVEMENT_ATTRIBUTE_UpNode; i < DTA_MOVEMENT_ATTRIBUTE_MAX_ROW; i++)
			{
				fprintf(st,"%s,%d,",g_DTA_movement_row_name[i].c_str (), (*iNode)->UTDF_node_id );

				for(int j= DTA_NBL2; j<= DTA_HOLD;j++)
				{
					fprintf(st,"%s,",m_DTAMovementMap[key].m_AMSMovementData[i][j].c_str ());
				}

				fprintf(st,"\n");  // go to the next line

			}

		}	// condition
	}	// for each node

	/// lane file

	FILE* st_lanes = NULL;
	fopen_s(&st_lanes, m_Synchro_ProjectDirectory + "LANES.csv", "w");
	if (st_lanes == NULL)
	{
		AfxMessageBox("File LANES.csv file cannot be opened.");

	}
	else
	{
		fprintf(st_lanes, "Lane Group Data \n");
		fprintf(st_lanes, "RECORDNAME,INTID,");  // write titles of fields

		for (int j = DTA_NBL2; j <= DTA_HOLD; j++)
			fprintf(st_lanes, "%s,", g_DTA_movement_column_name[j].c_str());

		fprintf(st_lanes, "\n");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)  // for each node block
		{

	//		if ((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
			{

				CString key;
				key.Format("%d:FREE", (*iNode)->UTDF_node_id);


				// write data per record name and then per movement 
				for (int i = DTA_MOVEMENT_ATTRIBUTE_UpNode; i < DTA_MOVEMENT_ATTRIBUTE_MAX_ROW; i++)
				{
					fprintf(st_lanes, "%s,%d,", g_DTA_movement_row_name[i].c_str(), (*iNode)->UTDF_node_id);

					for (int j = DTA_NBL2; j <= DTA_HOLD; j++)
					{
						fprintf(st_lanes, "%s,", m_DTAMovementMap[key].m_AMSMovementData[i][j].c_str());
					}

					fprintf(st_lanes, "\n");  // go to the next line

				}
			}
		}
	
		fclose(st_lanes);
	}

	float hourly_volume_conversion_factor = 60.0f/max(1,m_DemandLoadingEndTimeInMin - m_DemandLoadingStartTimeInMin);
	FILE* st_volume = NULL;
	fopen_s(&st_volume, m_Synchro_ProjectDirectory + "VOLUME.csv", "w");
	if (st_volume == NULL)
	{
		AfxMessageBox("File VOLUME.csv file cannot be opened.");

	}
	else
	{
		fprintf(st_volume, "Turning Movement Count \n");
		fprintf(st_volume, "60 Minute Counts \n");
		fprintf(st_volume, "DATE,TIME,INTID,");  // write titles of fields

		for (int j = DTA_NBL2; j <= DTA_HOLD; j++)
			fprintf(st_volume, "%s,", g_DTA_movement_column_name[j].c_str());
		
		
		fprintf(st_volume, "\n");  // go to the next line


		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)  // for each node block
		{


				CString key;
				key.Format("%d:FREE", (*iNode)->UTDF_node_id);


				// write data per record name and then per movement 
				fprintf(st_volume, "07/20/2011,1700,%d, ", (*iNode)->UTDF_node_id);

					for (int j = DTA_NBL2; j <= DTA_HOLD; j++)
					{
						fprintf(st_volume, "%s,", m_DTAMovementMap[key].m_AMSMovementData[DTA_MOVEMENT_ATTRIBUTE_TurnVolume][j].c_str());
					}

					fprintf(st_volume, "\n");  // go to the next line

			}

	
		fclose(st_volume);
	}

	
	// write timing data
	fprintf(st,"\n[Timeplans]\n"		
		"Timing Plan Settings\n"		
		"RECORDNAME,INTID,DATA");
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		if ((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)  
		{

			CString key;
			key.Format ("%d;0;0,100,0,1440", (*iNode)->UTDF_node_id);

			// phasing data

			// for each record

			for(int i = TIMING_ControlType; i < TIMING_Node_1; i++)
			{
				// g_DTA_phasing_row_name
				fprintf(st,"\n%s,%d,", g_DTA_phasing_row_name[i].c_str (), (*iNode)->UTDF_node_id);

				if(m_DTAPhasingMap[key].m_AMSPhasingData[i][DTA_SIG_PHASE_VALUE].size() > 0)
					fprintf(st, "%s,", m_DTAPhasingMap[key].m_AMSPhasingData[i][DTA_SIG_PHASE_VALUE].c_str());
				else
					fprintf(st, "0,");

			}



		}  // signalized node

	} // for each node



	// write phasing data

	fprintf(st,"\n\n[Phases]\n"	
		"Phasing Data\n"	
		"RECORDNAME,INTID,");

	for(int j=DTA_SIG_PHASE_D1; j<= DTA_SIG_PHASE_D16;j++)
		fprintf(st, "%s,", g_DTA_phasing_column_name[j].c_str());

	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		if ((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)  
		{

			CString key;
			key.Format ("%d;0;0,100,0,1440", (*iNode)->UTDF_node_id);

			// phasing data

			// for the first line of BRP
			fprintf(st,"\nBRP,%d,", (*iNode)->UTDF_node_id);

			for(int j=DTA_SIG_PHASE_D1; j<= DTA_SIG_PHASE_D16;j++)
			{
				fprintf(st, "%d,", g_DTA_phase_default_BRP[j-DTA_SIG_PHASE_D1]);

			}


			for(int i = PHASE_MinGreen; i <= PHASE_ActGreen; i++)
			{
				// g_DTA_phasing_row_name
				fprintf(st,"\n%s,%d,", g_DTA_phasing_row_name[i].c_str (), (*iNode)->UTDF_node_id);

				//for each phase
				for(int j=DTA_SIG_PHASE_D1; j<= DTA_SIG_PHASE_D16;j++)
				{
					fprintf(st, "%s,", m_DTAPhasingMap[key].m_AMSPhasingData[i][j].c_str());

				}

			}


		}  // signalized node

	} // for each node




	//FILE* st = NULL;

	//// write lanes/movements file
	//const int LaneColumnSize = 32;
	//const int LaneRowSize = 30;
	//string lane_Column_name_str[LaneColumnSize] = { "NBL2","NBL","NBT","NBR","NBR2","SBL2","SBL","SBT","SBR","SBR2","EBL2","EBL","EBT","EBR","EBR2","WBL2","WBL","WBT","WBR","WBR2","NEL","NET","NER","NWL","NWT","NWR","SEL","SET","SER","SWL","SWT","SWR"};
	//string lane_row_name_str[LaneRowSize] = {"Up Node","Dest Node","Lanes","Shared","Width","Storage","StLanes","Grade","Speed","FirstDetect","LastDetect","Phase1","PermPhase1","DetectPhase1","IdealFlow","LostTime","SatFlowRatePerLaneGroup","SatFlowPerm","SatFlowRTOR","HeadwayFact","Volume","Peds","Bicycles","PHF","Growth","HeavyVehicles","BusStops","Midblock","Distance","TravelTime"};

	//int i,j, m;
	//int movement_size = m_MovementVector.size();

	//fopen_s(&st,m_Synchro_ProjectDirectory+"Lanes.csv","w");
	//if(st!=NULL)
	//{		
	//	fprintf(st, "Lane Group Data \n");
	//	fprintf(st, "RECORDNAME,INTID,");  // write titles of fields
	//	for(j=0; j<LaneColumnSize;j++)
	//		fprintf(st, "%s,", lane_Column_name_str[j].c_str());
	//	fprintf(st,"\n");

	//	for (m=0; m<movement_size;m++)
	//	{
	//		// write UpNodeID and DestNodeID using original m_NodeNumber
	//		for(i=0; i<2; i++)
	//		{
	//			fprintf(st, "%s,", lane_row_name_str[i].c_str());
	//			fprintf(st, "%i,", m_NodeNoMap[m_MovementVector[m].CurrentNodeID]->m_NodeNumber);  // current node id

	//			for(j=0; j<LaneColumnSize;j++)
	//			{
	//				int NodeID = (int)(m_MovementVector[m].DataMatrix[i][j].m_text);
	//				TRACE("Node Label: %d\n",NodeID);

	//				if(NodeID>=0)  //this movement has been initialized
	//				{
	//					if(m_NodeNoMap.find(NodeID) == m_NodeNoMap.end())
	//					{
	//						AfxMessageBox("Error in node id!");
	//						return;
	//					}
	//					fprintf(st, "%i,",m_NodeNoMap[NodeID]->m_NodeNumber);  
	//				}else  // this movement has not been initialized, so the default value is -1
	//				{
	//					fprintf(st, ",");  

	//				}
	//			}
	//			fprintf(st,"\n");
	//		}
	//		for(i=2; i<LaneRowSize; i++)
	//		{
	//			fprintf(st, "%s,", lane_row_name_str[i].c_str());
	//			fprintf(st, "%i,", m_NodeNoMap[m_MovementVector[m].CurrentNodeID]->m_NodeNumber);

	//			for(j=0; j<LaneColumnSize;j++)
	//			{

	//				float text = m_MovementVector[m].DataMatrix[i][j].m_text; // default value
	//				//find movement, overwrite data
	//				if( lane_row_name_str[i].find("Phase1")!= string::npos || lane_row_name_str[i].find("PermPhase1")!= string::npos ||  lane_row_name_str[i].find("DetectPhase1")!= string::npos )
	//				{
	//					int FromNodeID = (int)(m_MovementVector[m].DataMatrix[0][j].m_text);
	//					int DestNodeID = (int)(m_MovementVector[m].DataMatrix[1][j].m_text);

	//					int FromNodeNumber = 0;
	//					int DestNodeNumber = 0;
	//					int CurrentNodeNumber = m_NodeNoMap[m_MovementVector[m].CurrentNodeID]->m_NodeOriginalNumber;

	//					if(FromNodeID > 0)
	//						FromNodeNumber = m_NodeNoMap[FromNodeID]->m_NodeOriginalNumber ;

	//					if(DestNodeID > 0)
	//						DestNodeNumber = m_NodeNoMap[DestNodeID]->m_NodeOriginalNumber;

	//					CString movement_label;
	//					movement_label.Format ("%d;%d;%d",FromNodeNumber,CurrentNodeNumber,DestNodeNumber);

	//					if(FromNodeNumber >0 && m_Movement3NodeMap.find(movement_label) != m_Movement3NodeMap.end())
	//					{
	//						if (lane_row_name_str[i].find("Phase1")!= string::npos && m_Movement3NodeMap[movement_label].Phase1 != -1)  // has been set/ initialized
	//						{
	//							text = m_Movement3NodeMap[movement_label].Phase1;
	//						}
	//						if (lane_row_name_str[i].find("PermPhase1")!= string::npos && m_Movement3NodeMap[movement_label].PermPhase1!= -1)
	//						{
	//							text = m_Movement3NodeMap[movement_label].PermPhase1;
	//						}

	//						if (lane_row_name_str[i].find("DetectPhase1")!= string::npos && m_Movement3NodeMap[movement_label].DetectPhase1!=-1)
	//						{
	//							text = m_Movement3NodeMap[movement_label].DetectPhase1;
	//						}
	//					}
	//				}

	//				if (text >= 0)
	//					fprintf(st, "%f,",text);
	//				else
	//					fprintf(st, ",");
	//			}
	//			fprintf(st,"\n");
	//		}
	//	}
	//	fprintf(st,"\n");

	//	fclose(st);
	//}else
	//{
	//	AfxMessageBox("File Lanes.csv cannot be opened.");
	//}

	//// write phase file
	//const int PhaseColumnSize = 8;
	//const int PhaseRowSize = 23;
	//string phase_Column_name_str[PhaseColumnSize] = { "D1","D2","D3","D4","D5","D6","D7","D8"};
	//string phase_row_name_str[PhaseRowSize] = {"BRP","MinGreen","MaxGreen","VehExt","TimeBeforeReduce","TimeToReduce","MinGap","Yellow","AllRed","Recall","Walk","DontWalk","PedCalls","MinSplit","DualEntry","InhibitMax","Start","End","Yield","Yield170","LocalStart","LocalYield","LocalYield170"};

	//int p, phase_size = m_PhaseVector.size();

	//fopen_s(&st,m_Synchro_ProjectDirectory+"Phasing.csv","w");
	//if(st!=NULL)
	//{
	//	fprintf(st, "Phasing Data \n");
	//	fprintf(st, "RECORDNAME,INTID,");
	//	for(j=0; j<PhaseColumnSize;j++)
	//		fprintf(st, "%s,", phase_Column_name_str[j].c_str());
	//	fprintf(st,"\n");

	//	for (p=0; p<phase_size;p++)
	//	{
	//		for(i=0; i<PhaseRowSize; i++)
	//		{
	//			fprintf(st, "%s,", phase_row_name_str[i].c_str());
	//			fprintf(st, "%i,", m_NodeNoMap[m_PhaseVector[p].CurrentNodeID]->m_NodeNumber);  // intersection id

	//			DTANode* pNode  = m_NodeNoMap[m_PhaseVector[p].CurrentNodeID];
	//			for(j=0; j<PhaseColumnSize;j++)
	//			{
	//				if(i==1)
	//					fprintf(st, "%d,",pNode->m_SignalData .phase_vector [j+1].min_green); //
	//				else if(i==2)
	//					fprintf(st, "%d,",pNode->m_SignalData .phase_vector [j+1].max_green); //
	//				else if(i==3)
	//					fprintf(st, "%.1f,",pNode->m_SignalData .phase_vector [j+1].VehExt1 ); //
	//				else
	//					fprintf(st, "%f,",m_PhaseVector[p].DataMatrix[i][j]); //
	//			}
	//			fprintf(st,"\n");
	//		}
	//	}
	//	fprintf(st,"\n");

	//	fclose(st);
	//}else
	//{
	//	AfxMessageBox("File Phasing.csv cannot be opened.");
	//}

	//// write layout file
	//DTA_Direction incoming_approach, out_approach;
	//GDPoint p1, p2;
	//int current_node_id, up_node_id, down_node_id;
	//long LinkID;
	//const int Dir_size = 8;

	//fopen_s(&st,m_Synchro_ProjectDirectory+"Layout.csv","w");
	//if(st!=NULL)
	//{
	//	fprintf(st, "Layout Data \n");
	//	fprintf(st, "INTID,INTNAME,TYPE,X,Y,NID,SID,EID,WID,NEID,NWID,SEID,SWID,NNAME,SNAME,ENAME,WNAME,NENAME,NWNAME,SENAME,SWNAME");
	//	fprintf(st,"\n");


	//	// write timing file
	//	fopen_s(&st,m_Synchro_ProjectDirectory+"Timing.csv","w");
	//	if(st!=NULL)
	//	{
	//		fprintf(st, "Timing Plans \n");
	//		fprintf(st, "PLANID,INTID,S1,S2,S3,S4,S5,S6,S7,S8,CL,OFF,LD,REF,CLR \n");
	//		DTA_TimePlan plan;
	//		plan.initial();

	//		for (p=0; p<phase_size;p++)
	//		{
	//			fprintf(st, "%i,%i,", plan.PlanID, m_NodeNoMap[m_PhaseVector[p].CurrentNodeID]->m_NodeNumber);
	//			DTA_Timing timing;
	//			timing.initial(m_PhaseVector[p], 120);

	//			for(i=0; i<8; i++)
	//			{
	//				fprintf(st, "%i,", timing.DataAry[i]);
	//			}
	//			fprintf(st, "%i,%i,%i,%s,\n", timing.Cycle, timing.OFF, timing.LD, timing.REF.c_str());
	//		
	//		}
	//		fprintf(st,"\n");

	//		fclose(st);
	//	}else
	//	{
	//		AfxMessageBox("File Timing.csv file cannot be opened.");
	//	}

	//CWaitCursor  wait;

	//float hourly_volume_conversion_factor = 60.0f/max(1,m_DemandLoadingEndTimeInMin - m_DemandLoadingStartTimeInMin);


	//// write volume file
	//fopen_s(&st,m_Synchro_ProjectDirectory+"Volume.csv","w");
	//if(st!=NULL)
	//{
	//	fprintf(st, "Turning Movement Count \n");
	//	fprintf(st, "60 Minute Counts \n");
	//	fprintf(st, "DATE,TIME,INTID,NBL2,NBL,NBT,NBR,NBR2,SBL2,SBL,SBT,SBR,SBR2,EBL2,EBL,EBT,EBR,EBR2,WBL2,WBL,WBT,WBR,WBR2,NEL,NET,NER,NWL,NWT,NWR,SEL,SET,SER,SWL,SWT,SWR\n");
	//	for (m=0; m<movement_size;m++)
	//	{				
	//		// DATE,TIME,INTID
	//		int CurNodeNumber  =  m_NodeNoMap[m_MovementVector[m].CurrentNodeID]->m_NodeNo +1;
	//		fprintf(st, "07/20/2011,1700,%i,",CurNodeNumber);
	//		// 
	//		for(j=0; j<LaneColumnSize;j++)
	//		{

	//			int FromNodeID = (int)(m_MovementVector[m].DataMatrix[0][j].m_text);
	//			int DestNodeID = (int)(m_MovementVector[m].DataMatrix[1][j].m_text);

	//			int FromNodeNumber = 0;
	//			int DestNodeNumber = 0;
	//			int CurrentNodeNumber = m_NodeNoMap[m_MovementVector[m].CurrentNodeID]->m_NodeOriginalNumber;

	//			if(FromNodeID > 0)
	//				FromNodeNumber = m_NodeNoMap[FromNodeID]->m_NodeOriginalNumber;

	//			if(DestNodeID > 0)
	//				DestNodeNumber = m_NodeNoMap[DestNodeID]->m_NodeOriginalNumber;

	//			CString movement_label;
	//			movement_label.Format ("%d;%d;%d",FromNodeNumber,CurrentNodeNumber,DestNodeNumber);

	//			int count = 0;

	//			if(m_Movement3NodeMap.find(movement_label) != m_Movement3NodeMap.end())
	//			{
	//				count = m_Movement3NodeMap[movement_label].TotalVehicleSize* hourly_volume_conversion_factor;

	//			}

	//			fprintf(st, "%i,",count);
	//		}
	//		fprintf(st, "\n");


	//	}

	//	fprintf(st,"\n");

	//	fclose(st);

	//}else
	//{
	//	AfxMessageBox("File Volume.csv file cannot be opened.");
	//}


	//m_Origin = m_Origin_Current;  // restore value


	////restore node number 
	//for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	//{

	//	(*iNode)->m_NodeNumber  = (*iNode)->m_NodeOriginalNumber;
	//}

	fclose(st);
}

void CTLiteDoc::ExportSingleSynchroFile(CString SynchroProjectFile)
{ 

}


bool CTLiteDoc::ReadSynchroPreGeneratedLayoutFile(LPCTSTR lpszFileName)
{
	const int approach_size = 8;

	string approach_column_name_str[approach_size] = { "NID","SID",	"EID","WID","NEID","NWID","SEID","SWID"};

	// we need to use the opposite direction
	DTA_Direction approach_vector[approach_size] = { 
		DTA_South,
		DTA_North,
		DTA_West,
		DTA_East,
		DTA_SouthWest,
		DTA_SouthEast,
		DTA_NorthWest,
		DTA_NorthEast};

		int approach_node_id[approach_size];

		m_PredefinedApproachMap.clear();

		CCSVParser parser;
		parser.m_bSkipFirstLine  = true;  // skip the first line  
		if (parser.OpenCSVFile(lpszFileName))
		{
			int i=0;
			while(parser.ReadRecord())
			{
				int node_id;
				string name;
				DTANode* pNode = 0;


				if(parser.GetValueByFieldName("INTID",node_id) == false)
					break;

				for(int a = 0; a< approach_size; a++)
				{	
					approach_node_id[a]=0;  // initialization
					bool bField_Exist = parser.GetValueByFieldName(approach_column_name_str[a],approach_node_id[a]);

					if(bField_Exist && approach_node_id[a]>0)
					{
						TRACE("node = %d,%d,%s\n", node_id, approach_node_id[a], approach_column_name_str[a].c_str ());

						CString str_key;
						str_key.Format("%d,%d",node_id, approach_node_id[a]);

						m_PredefinedApproachMap[str_key] = approach_vector[a];

					}

				}


			}

			AfxMessageBox("Synchro_layout.csv file is used to specify the movement vector in QEM and Synchro exporting functions.", MB_ICONINFORMATION);
			return true;
		}
		return false;
}


void CTLiteDoc::ExportQEMData(int ThisNodeNumber)
{
	RunQEMTool(ThisNodeNumber,true );
}


typedef std::pair<CString, int> LinkProp_Pair;

struct IntCmp 
{

	bool operator()(const LinkProp_Pair &lhs, const LinkProp_Pair &rhs) {
		return lhs.second > rhs.second;
	}
};


void CTLiteDoc::OnDetectorExportlinkflowproportionmatrixtocsvfile()
{

	CWaitCursor wait;
	
	std::string timing_plan_name = "ALLDAY";
	
	m_LinkFlowProportionMap.clear ();
	m_LinkFlowProportionODMap.clear ();

	if(m_VehicleSet.size()>=50000)
	{
		AfxMessageBox("Generating link flow proportion matrix from vehicle/agent files might take a while...", MB_ICONINFORMATION);

	}

	int sensor_count = 0;
	for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		DTALink* pLink = (*iLink);
		if(pLink->m_bSensorData || pLink->m_observed_peak_hourly_volume >1)
		{
			sensor_count ++;
		}
	}

	//if(sensor_count == 0)
	//{
	//	AfxMessageBox("Please input sensor data or specify observed link peak hourly count first, before generating link flow proportion matrix. Currently, there is 0 link with observation data.", MB_ICONINFORMATION);
	//	return;
	//}


	int departure_time_interval = 1440;

	std::list<DTAVehicle*>::iterator iVehicle;

	for (iVehicle = m_VehicleSet.begin(); iVehicle != m_VehicleSet.end(); iVehicle++)
	{
		DTAVehicle* pVehicle = (*iVehicle);

		if(pVehicle->m_VehicleID == 34)
		{
			TRACE("");
		}

		if(pVehicle->m_bComplete )
		{

			for(int i= 2; i<  pVehicle->m_NodeSize; i++)
			{
				DTALink* pLink0 = m_LinkNoMap[pVehicle->m_NodeAry[i-1].LinkNo]; // i=0, LinkNo = -1;
				DTALink* pLink1 = m_LinkNoMap[pVehicle->m_NodeAry[i].LinkNo];

				CString movement_label;

				if(pLink0->m_bSensorData || pLink0->m_observed_peak_hourly_volume >=1)  // with sensor data
				{

					if(pLink0->m_FromNodeNumber  == 1 && 
						pLink0->m_ToNodeNumber == 14)
					{
						TRACE("");
					}


					//movement
					movement_label.Format ("%03d,%03d,%02d,%d;%d;%d", pVehicle->m_OriginZoneID ,pVehicle->m_DestinationZoneID , (int) (pVehicle->m_DepartureTime / departure_time_interval), 
						pLink0->m_FromNodeNumber  ,  pLink0->m_ToNodeNumber , pLink1->m_ToNodeNumber);
					m_LinkFlowProportionMap[movement_label]++;

					// link
					movement_label.Format ("%03d,%03d,%02d,%d->%d", pVehicle->m_OriginZoneID ,pVehicle->m_DestinationZoneID , (int)(pVehicle->m_DepartureTime / departure_time_interval), 
						pLink0->m_FromNodeNumber  ,  pLink0->m_ToNodeNumber);
					m_LinkFlowProportionMap[movement_label]++;



				}

				if(i==pVehicle->m_NodeSize -1 && pLink1->m_bSensorData || pLink1->m_observed_peak_hourly_volume >=1)  //last link
				{
					movement_label.Format ("%03d,%03d,%02d,%d->%d", pVehicle->m_OriginZoneID ,pVehicle->m_DestinationZoneID , (int)(pVehicle->m_DepartureTime / departure_time_interval), 
						pLink1->m_FromNodeNumber  ,  pLink1->m_ToNodeNumber);
					m_LinkFlowProportionMap[movement_label]++;

				}

			}

			CString OD_label;

			//movement
			OD_label.Format ("%03d,%03d,%02d,", pVehicle->m_OriginZoneID ,pVehicle->m_DestinationZoneID , int(pVehicle->m_DepartureTime / departure_time_interval)); 

			m_LinkFlowProportionODMap[OD_label]++;

		}
	}

	int NumberOfSensors = 0;
	{


		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData || pLink->m_observed_peak_hourly_volume >=1)
			{

				NumberOfSensors ++;

			}

		}

		std::list<DTANode*>::iterator iNode;
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					NumberOfSensors++;
				}

			}
		}

	}

	int NumberOfODPairs = m_LinkFlowProportionODMap.size();


	// export the output 

	// for each element in m_LinkFlowProportionODMap

	FILE* st;

	CString directory;
	directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);

	CString AMS_File = directory+"AMS_link_flow_proportion_matrix.csv";
	fopen_s(&st,AMS_File,"w");
	if(st!=NULL)
	{
		// first line 
		CString ColumnSizeName = g_GetExcelColumnFromNumber(NumberOfSensors+8);

		fprintf(st,"Target Cell/Objective Function,=SUM(D6:D%d)+SUM(G3:%s3),,,,Sensor Link=>,", NumberOfODPairs+5,ColumnSizeName);

		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData || pLink->m_observed_peak_hourly_volume >=1)
			{

				fprintf(st,"%d->%d,",(*iLink)->m_FromNodeNumber , (*iLink)->m_ToNodeNumber );

			}

		}
		fprintf(st,",Movement=>,");

		std::list<DTANode*>::iterator iNode;
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					fprintf(st,"%d->%d->%d (%s),", 
						m_NodeNoMap[movement.in_link_from_node_id ]->m_NodeNumber  ,
						m_NodeNoMap[movement.in_link_to_node_id]->m_NodeNumber  ,
						m_NodeNoMap[movement.out_link_to_node_id]->m_NodeNumber  ,
						GetTurnDirectionString(movement.movement_approach_turn));
				}

			}
		}
		fprintf(st,"\n");

		// second line: name
		fprintf(st,",,,,,Name=>,");
		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData || pLink->m_observed_peak_hourly_volume >=1)
			{

				fprintf(st,"%s (%c),",(*iLink)->m_Name .c_str (), GetApproachChar((*iLink)->m_FromApproach));

			}

		}
		fprintf(st,",Movement=>,");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					int FromNode = m_NodeNoMap[movement.in_link_from_node_id ]->m_NodeNumber;
					int CurrentNode = m_NodeNoMap[movement.in_link_to_node_id]->m_NodeNumber;
					int DestinationNode = m_NodeNoMap[movement.out_link_to_node_id]->m_NodeNumber;

					DTALink* pLink1 = FindLinkWithNodeNumbers(FromNode,CurrentNode);
					DTALink* pLink2 = FindLinkWithNodeNumbers(CurrentNode, DestinationNode);

					fprintf(st,"%s->%s (%s),", 

						pLink1->m_Name.c_str (), 
						pLink2->m_Name.c_str (), 
						GetTurnDirectionString(movement.movement_approach_turn));
				}

			}
		}


		fprintf(st,"\n,,,,,Deviation of Observed and Estimated Count=>,");
		int ColumnIndex = 7;

		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData || pLink->m_observed_peak_hourly_volume >=1)
			{
				// with sensor data
				CString ColumnName = g_GetExcelColumnFromNumber(ColumnIndex);
				fprintf(st,"=(%s4-%s5)*(%s4-%s5),",ColumnName, ColumnName,ColumnName, ColumnName);

				ColumnIndex++;

			}
		}

		ColumnIndex++;
		ColumnIndex++;

		fprintf(st,",,");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					CString ColumnName = g_GetExcelColumnFromNumber(ColumnIndex);
					fprintf(st,"=(%s4-%s5)*(%s4-%s5),",ColumnName, ColumnName,ColumnName, ColumnName);
					ColumnIndex++;

				}

			}
		}



		fprintf(st,"\n,,,,,Observed Count=>,");
		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);

			if(pLink->m_FromNodeNumber  == 1 && 
				pLink->m_ToNodeNumber == 14)
			{
				TRACE("");
			}

			if(pLink->m_bSensorData || pLink->m_observed_peak_hourly_volume >=1)
			{
				// with sensor data
				if(pLink->m_bSensorData)
					fprintf(st,"%.1f,",(*iLink)->GetSensorLinkHourlyVolume( 0, 1440) );
				else  // no sensor data input 
				{
					if( pLink->m_observed_peak_hourly_volume >=1) //we must have peak hourly count data
					{
						fprintf(st,"%d,",pLink->m_observed_peak_hourly_volume );
					}

				}
			}

		}

		fprintf(st,",,");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					fprintf(st,"%d,", movement.obs_turn_hourly_count);
				}

			}
		}


		fprintf(st,"\norigin zone id,destination zone id,departure time (hour),");

		fprintf(st,"OD volume deviation,Target OD Volume,||Estimated OD volume: Estimated Flow count=>,");

		ColumnIndex = 7;

		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData || pLink->m_observed_peak_hourly_volume >=1)
			{
				CString ColumnName = g_GetExcelColumnFromNumber(ColumnIndex);
				fprintf(st,"=SUMPRODUCT(($F$6:$F$%d)*(%s6:%s%d)),",NumberOfODPairs+5,ColumnName, ColumnName,NumberOfODPairs+5);

				ColumnIndex++;

				// with sensor data

			}

		}

		fprintf(st,",,");

		ColumnIndex++;
		ColumnIndex++;

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{

					CString ColumnName = g_GetExcelColumnFromNumber(ColumnIndex);
					fprintf(st,"=SUMPRODUCT(($F$6:$F$%d)*(%s6:%s%d)),",NumberOfODPairs+5,ColumnName, ColumnName,NumberOfODPairs+5);
					ColumnIndex++;


				}

			}
		}



		fprintf(st,"\n");

		// sort link flow porportion matrix
		std::vector<LinkProp_Pair> myvec(m_LinkFlowProportionODMap.begin(), m_LinkFlowProportionODMap.end());
		std::sort(myvec.begin(), myvec.end(), IntCmp());

		// second block


		int row_index = 6;
		int movement_index = 1;
		//		for (itr = m_LinkFlowProportionODMap.begin(); itr != m_LinkFlowProportionODMap.end(); itr++)
		for(unsigned ii= 0; ii< myvec.size(); ii++)
		{

			fprintf(st,"%s=(E%d-F%d)*(E%d-F%d),%d,%d,", myvec[ii].first, row_index,row_index,row_index,row_index,
				myvec[ii].second ,myvec[ii].second );

			row_index++;
			// OD pair

			for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
			{
				DTALink* pLink = (*iLink);
				if(pLink->m_bSensorData || pLink->m_observed_peak_hourly_volume >=1)
				{
					CString LinkString;

					LinkString.Format ("%s%d->%d",myvec[ii].first,(*iLink)->m_FromNodeNumber , (*iLink)->m_ToNodeNumber );

					if(m_LinkFlowProportionMap.find(LinkString) != m_LinkFlowProportionMap.end())
					{

						float ratio = m_LinkFlowProportionMap[LinkString]*1.0f/max(1,myvec[ii].second );

						fprintf(st,"%.4f,",ratio);

					}else
					{

						fprintf(st,"0,");

					}

				}

			}

			fprintf(st,",,");
			std::list<DTANode*>::iterator iNode;
			for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{

				for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector .size(); m++)
				{
					DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];
					if(movement.obs_turn_hourly_count >=1)
					{

						CString MovementString;

						MovementString.Format ("%s%d;%d;%d",myvec[ii].first ,
							m_NodeNoMap[movement.in_link_from_node_id ]->m_NodeNumber  ,
							m_NodeNoMap[movement.in_link_to_node_id]->m_NodeNumber  ,
							m_NodeNoMap[movement.out_link_to_node_id]->m_NodeNumber  );


						if(m_LinkFlowProportionMap.find(MovementString) != m_LinkFlowProportionMap.end())
						{

							float ratio = m_LinkFlowProportionMap[MovementString]*1.0f/max(1,myvec[ii].second );

							fprintf(st,"%.4f,",ratio);

						}else
						{

							fprintf(st,"0,");

						}



					}

				}  // all movments

			} // all nodes
			fprintf(st,"\n");

		} // for all OD pairs



		fclose(st);

		OpenCSVFileInExcel(AMS_File);

	}else
	{
		AfxMessageBox("File AMS_link_flow_proportion_matrix.csv cannot be opened.");

	}

}



void CTLiteDoc::SaveAMSPhasingData(LPCTSTR lpszPathName)
{

	FILE* st = NULL;

	fopen_s(&st,lpszPathName,"w");
	if(st!=NULL)
	{		
		for(int j=0; j< DTA_SIG_PHASE_MAX_COLUMN;j++)
			fprintf(st, "%s,", g_DTA_phasing_column_name[j].c_str());

		fprintf(st,"\n");

		// print out matrix 


		for(int tp = 0; tp< m_TimingPlanVector.size(); tp++)  // first loop for each timing plan
		{

			std::string timing_plan_name = m_TimingPlanVector[tp].timing_plan_name;  // fetch timing_plan (unique) name

			for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{
				if(((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal) && 
					(*iNode)->IfTrafficExistInThisTimingPlan(timing_plan_name) == true )  // volume data exists
				{


					CString node_key;
					node_key.Format ("%d", (*iNode)->m_NodeNumber);

					//initialize the row data for the given key in the vector
					for(int i=0; i< DTA_PHASE_ATTRIBUTE_MAX_ROW;i++)
					{
						CString phasing_map_key  =  GetPhasingMapKey((*iNode)->m_NodeNumber,timing_plan_name);

						DTA_SIG_PHASE_ROW row_no =  (DTA_SIG_PHASE_ROW)i;

						m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_TIMING_PLAN_NAME, row_no, timing_plan_name );
						m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, row_no, CString2StdString(node_key) );
						m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, row_no, g_DTA_phasing_row_name[row_no]);
					}


					DTA_Phasing_Data_Matrix element_current = GetPhaseData((*iNode)->m_NodeNumber ,  timing_plan_name);

					//check ring type
					CString ring_type_str  = element_current.GetString (DTA_SIG_PHASE_VALUE, TIMING_RingType); 
	
					if(ring_type_str.GetLength ()	 == 0 )  // no default value
						SetupSignalValue((*iNode)->m_NodeNumber,timing_plan_name,TIMING_RingType, "dual_ring");

					//check optimizationmethod
					CString optimization_method_str  = element_current.GetString (DTA_SIG_PHASE_VALUE, TIMING_OptimizationMethod); 
					if(optimization_method_str.GetLength ()	 == 0 )  // no default value
						SetupSignalValue((*iNode)->m_NodeNumber,timing_plan_name,TIMING_OptimizationMethod, "yes");

					//check control type

					CString control_str  = element_current.GetString (DTA_SIG_PHASE_VALUE, TIMING_ControlType); 
	
					if(control_str.Compare ("actuated_signal") !=0  )  // set default value as pretimed_signal
						SetupSignalValue((*iNode)->m_NodeNumber,timing_plan_name,TIMING_ControlType, "pretimed_signal");


					if(ring_type_str.GetLength ()	 == 0 )  // no default value
						SetupSignalValue((*iNode)->m_NodeNumber,timing_plan_name,TIMING_RingType, "dual_ring");

					//refetch data 
					DTA_Phasing_Data_Matrix element = GetPhaseData((*iNode)->m_NodeNumber ,  timing_plan_name);

					for(int i=0; i< DTA_PHASE_ATTRIBUTE_MAX_ROW;i++)
					{
						DTA_SIG_PHASE_ROW row_no =  (DTA_SIG_PHASE_ROW)i;

						// node id 
						fprintf(st, "%s,", timing_plan_name.c_str () );
						// node id 
						fprintf(st, "%d,", (*iNode)->m_NodeNumber );

						for(int j=2; j< DTA_SIG_PHASE_MAX_COLUMN;j++)
						{
							fprintf(st, "%s,", element.m_AMSPhasingData[row_no][j].c_str() );

						}
						fprintf(st,"\n");
					}


				}
			}
		}
		fclose(st);
	}else
	{
		AfxMessageBox("File AMS_phasing.csv cannot be opened.");
	}

}


void CTLiteDoc::ReadAMSPhasingFile(LPCTSTR lpszFileName)
{

	CString PhasingMapKey;


	m_DTAPhasingMap.clear();

	CCSVParser parser;
	if (parser.OpenCSVFile(lpszFileName))
	{

		while(parser.ReadRecord())
		{

			int node_id;

			std::string timing_plan_name, key,scenario_no,start_day_no,end_day_no,start_time_in_min,end_time_in_min;

			parser.GetValueByFieldName("timing_plan_name",timing_plan_name);

			if(timing_plan_name.size() == 0)
			{
				timing_plan_name = "ALLDAY";
			}
			parser.GetValueByFieldName("node_id",node_id);
			parser.GetValueByFieldName("key",key);

			CString phasing_map_key  =  GetPhasingMapKey(node_id,timing_plan_name);

			int row_no = 0;

			for(row_no = 0; row_no < DTA_PHASE_ATTRIBUTE_MAX_ROW; row_no++)
			{
				if( key.compare (g_DTA_phasing_row_name[row_no]) == 0)
				{

					// row_no is the selecte no. 
					break;

				}

			}



			if(row_no < DTA_PHASE_ATTRIBUTE_MAX_ROW)
			{
				// fetch string for each column 
				for(int j = 0; j< DTA_SIG_PHASE_MAX_COLUMN; j++)
				{

					parser.GetValueByFieldName(g_DTA_phasing_column_name[j],m_DTAPhasingMap[phasing_map_key ].m_AMSPhasingData [row_no] [j]);

				}

			}



		}
	}

			//start validility checking 
			for(int tp = 0; tp< m_TimingPlanVector.size(); tp++)  // first loop for each timing plan
		{

			std::string timing_plan_name = m_TimingPlanVector[tp].timing_plan_name ;  // fetch timing_plan (unique) name

			for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
			{

					DTA_Phasing_Data_Matrix element_current = GetPhaseData((*iNode)->m_NodeNumber ,  timing_plan_name);


						DTA_Phasing_Data_Matrix element = GetPhaseData((*iNode)->m_NodeNumber ,  timing_plan_name);

						//fetch cycle length value
						int CycleLength = atoi(element.GetString (DTA_SIG_PHASE_VALUE, TIMING_CycleLength));

						(*iNode) ->m_CycleLengthInSecond  = CycleLength;
				


			}

	}


}


void CTLiteDoc::SetupPhaseData(int node_id, std::string timing_plan_name,  int phase_numbr, DTA_SIG_PHASE_ROW attribute, float value)
{
	CString phasing_map_key = GetPhasingMapKey(node_id, timing_plan_name);

	std::string value_str;

	if(value - (int)value >=0.1)
		value_str = string_format( "%.0f", value);
	else
		value_str = string_format( "%.1f", value);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);
	m_DTAPhasingMap[phasing_map_key].SetString ( (DTA_SIG_PHASE) (phase_numbr+ DTA_SIG_PHASE_VALUE), attribute, value_str);

}

void CTLiteDoc::SetupPhaseData(int node_id, std::string timing_plan_name,  int phase_numbr, DTA_SIG_PHASE_ROW attribute, int value)
{
	CString phasing_map_key = GetPhasingMapKey(node_id, timing_plan_name);


	std::string value_str;
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);

	value_str = string_format( "%d", value);

	m_DTAPhasingMap[phasing_map_key].SetString ( (DTA_SIG_PHASE) (phase_numbr+ DTA_SIG_PHASE_VALUE), attribute, value_str);

}

void CTLiteDoc::SetupPhaseData(int node_id, std::string timing_plan_name, int phase_numbr, DTA_SIG_PHASE_ROW attribute, std::string value_str)
{
	CString phasing_map_key = GetPhasingMapKey(node_id, timing_plan_name);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);
	m_DTAPhasingMap[phasing_map_key].SetString ( (DTA_SIG_PHASE) (phase_numbr+ DTA_SIG_PHASE_VALUE), attribute, value_str);

}

void CTLiteDoc::SetupPhaseData(int node_id, std::string timing_plan_name, int phase_numbr, DTA_SIG_PHASE_ROW attribute, CString value_str)
{
	CString phasing_map_key = GetPhasingMapKey(node_id, timing_plan_name);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);
	m_DTAPhasingMap[phasing_map_key].SetString ( (DTA_SIG_PHASE) (phase_numbr+ DTA_SIG_PHASE_VALUE), attribute, CString2StdString(value_str));

}

void CTLiteDoc::SetupSignalValue(int node_id, std::string timing_plan_name,  DTA_SIG_PHASE_ROW attribute, int value)
{
	CString phasing_map_key = GetPhasingMapKey(node_id, timing_plan_name);
	std::string value_str;
	value_str = string_format( "%d", value);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_VALUE, attribute, value_str);

}
void CTLiteDoc::SetupSignalValue(int node_id, std::string timing_plan_name, DTA_SIG_PHASE_ROW attribute, float value)
{
	CString phasing_map_key = GetPhasingMapKey(node_id, timing_plan_name);
	std::string value_str;


	if(value - (int)value >=0.1)
		value_str = string_format( "%.0f", value);
	else
		value_str = string_format( "%.1f", value);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_VALUE, attribute, value_str);

}

void CTLiteDoc::SetupSignalValue(int node_id, std::string timing_plan_name,  DTA_SIG_PHASE_ROW attribute, CString value_str)
{
	CString phasing_map_key  = GetPhasingMapKey(node_id, timing_plan_name);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_VALUE, attribute, CString2StdString(value_str));

}

DTA_Phasing_Data_Matrix CTLiteDoc::GetPhaseData(int node_id, std::string timing_plan_name)
{

	DTA_Phasing_Data_Matrix element;  // empty element

	CString key = GetPhasingMapKey(node_id,timing_plan_name);


	if(m_DTAPhasingMap.find(key) != m_DTAPhasingMap.end())
		return m_DTAPhasingMap[key];
	else 
	{  
		//CString str;
		//str.Format("Phasing Map Key (%d:%s) not defined", node_id,timing_plan_name.c_str () );
		//AfxMessageBox(str);

	}
	return element;

}

BOOL  CTLiteDoc::IfMovementIncludedInPhase(int node_id, std::string timing_plan_name, int phase_no, int from_node_id, int destination_node_id)
{
	DTA_Phasing_Data_Matrix element  = GetPhaseData(node_id, timing_plan_name);

	CString MovementVector = element.GetString( (DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE+phase_no), PHASE_MOVEMENT_VECTOR );

	CString value;
	value.Format("%d:%d", m_NodeNoMap[from_node_id]->m_NodeNumber ,m_NodeNoMap[destination_node_id]->m_NodeNumber );

	if( MovementVector.Find(value) != -1) // find the value
		return true;
	else
		return false;


}

void CTLiteDoc::UpdateAllMovementGreenStartAndEndTime(std::string timing_plan_name)
{

	std::list<DTANode*>::iterator iNode;
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

		UpdateMovementGreenStartAndEndTimeFromPhasingData((*iNode)->m_NodeNumber,timing_plan_name );
	}
}

void CTLiteDoc::UpdateMovementGreenStartAndEndTimeFromPhasingData(int NodeNumber, std::string timing_plan_name)
{

	DTANode* pNode  = this->m_NodeNumberMap  [NodeNumber];

	if(pNode == NULL)
		return;
	DTA_Phasing_Data_Matrix element = GetPhaseData(NodeNumber ,timing_plan_name);

	// save phasing data
	if(pNode->m_ControlType == m_ControlType_PretimedSignal
		|| pNode->m_ControlType == m_ControlType_ActuatedSignal)
	{

		// reset QEM_start time and end time for all signalized intersections
		for (unsigned int i=0;i< pNode->m_MovementDataMap["ALLDAY"].m_MovementVector .size();i++)
		{
			pNode->m_MovementDataMap["ALLDAY"].m_MovementVector[i].QEM_StartTime = 9999;
			pNode->m_MovementDataMap["ALLDAY"].m_MovementVector[i].QEM_EndTime = 0;

		}

		for(int p=1; p<=8; p++)
		{

			float start_time = element.GetValue  ( (DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE+p), PHASE_Start);
			float end_time = element.GetValue  ( (DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE+p), PHASE_End);


			for (unsigned int i=0;i< pNode->m_MovementDataMap["ALLDAY"].m_MovementVector .size();i++)
			{

				BOOL bMovementIncluded = IfMovementIncludedInPhase(NodeNumber ,
					timing_plan_name,p, 
					pNode->m_MovementDataMap["ALLDAY"].m_MovementVector[i].in_link_from_node_id,
					pNode->m_MovementDataMap["ALLDAY"].m_MovementVector[i].out_link_to_node_id );

				if(bMovementIncluded == true)
				{
					pNode->m_MovementDataMap["ALLDAY"].m_MovementVector[i].QEM_StartTime = min(pNode->m_MovementDataMap["ALLDAY"].m_MovementVector[i].QEM_StartTime, start_time);
					pNode->m_MovementDataMap["ALLDAY"].m_MovementVector[i].QEM_EndTime =  max(pNode->m_MovementDataMap["ALLDAY"].m_MovementVector[i].QEM_EndTime, end_time);
				}

			}


		}	



	}

}


bool CTLiteDoc::ReadSynchroCombinedCSVFile(LPCTSTR lpszFileName)
{
	CWaitCursor wait;

	CString file_name;
	file_name.Format ("%s", lpszFileName);
	CString Synchro_directory = file_name.Left(file_name.ReverseFind('\\') + 1);
	// read users' prespecified control type

	CopyDefaultFile(m_DefaultDataFolder,Synchro_directory,Synchro_directory,"input_node_control_type.csv");
	CopyDefaultFile(m_DefaultDataFolder,Synchro_directory,Synchro_directory,"input_link_type.csv");

	if(ReadNodeControlTypeCSVFile(Synchro_directory+"input_node_control_type.csv") == false)
	{
		CString msg;
		msg.Format("Please first make sure file %s\\input_node_control_type.csv exists before importing synchro single csv file.", Synchro_directory);
		AfxMessageBox(msg, MB_ICONINFORMATION);

	}
	if(ReadLinkTypeCSVFile(Synchro_directory+"input_link_type.csv") == false)
	{
		CString msg;
		msg.Format("Please first make sure file %s\\input_link_type.csv exists before importing synchro single csv file.", Synchro_directory);
		AfxMessageBox(msg, MB_ICONINFORMATION);

	}

	string direction_vector[12] = {"NBL","NBT","NBR","SBL","SBT","SBR","EBL","EBT","EBR","WBL",	"WBT",	"WBR"};

	string lane_att_name_str[28] = {"Lanes","Shared","Width","Storage","StLanes","Grade","Speed",
					"FirstDetect","LastDetect","Phase1","PermPhase1","DetectPhase1","IdealFlow","LostTime",
					"SatFlowRatePerLaneGroup","Volume","SatFlowRTOR","HeadwayFact","Volume","Peds","Bicycles","PHF","Growth","HeavyVehicles","BusStops","Midblock","Distance","TravelTime"};

string phasing_column_name[DTA_SIG_PHASE_MAX_COLUMN] = {"node_id", "D1","D2","D3","D4",
"D5","D6","D7","D8", "D9","D10",
"D11","D12","D13","D14",
"D15","D16","D17","D18"};



	std::map<int,int> UpNodeNumberMap;
	std::map<int,int> DestNodeNumberMap;


	std::map<int, int> m_INTIDMap;
	std::map<int, int> m_NodeOrgNumber2INTIDMap;
	std::map<int, int> m_NodeOrgNumber2NodeNoMap;


	std::string timing_plan_name = "ALLDAY"; 
	
	CCSVParser parser;
	parser.m_bSynchroSingleCSVFile   = true;  // single file
	if (parser.OpenCSVFile(lpszFileName, false /*do not read first line as the header*/))
	{
		int i=0;
		int j= 1;  // actual node id for synchro

		std::map<int,int> from_node_id_map;

		while(parser.ReadRecord())
		{

			if(parser.m_SynchroSectionName.find ("Nodes")!=  string::npos) 
			{

				int intid;
				string name;
				DTANode* pNode = 0;

				int node_type;
				double X;
				double Y;
				if(parser.GetValueByFieldName("INTID",intid) == false)
					continue;

				if(!parser.GetValueByFieldName("INTNAME",name))
					name = "";

				if(!parser.GetValueByFieldName("TYPE",node_type))
					node_type = 0;

				// use the X and Y as default values first
				bool bFieldX_Exist = parser.GetValueByFieldName("X",X);
				parser.GetValueByFieldName("Y",Y);

				pNode = new DTANode;

				pNode->m_Name = name;

				pNode->m_ControlType = m_ControlType_NoControl;  // default value


				pNode->pt.x = X/5280.0f;  // feet to mile
				pNode->pt.y = Y/5280.0f;  // feet to mile


				int node_number_estimator  = j;
				if(intid < 10000 && intid > j)
				{
					node_number_estimator = intid;  // encounter skipped nodes, move 
					j = node_number_estimator;

				}
				pNode->m_NodeOriginalNumber = pNode->m_NodeNumber;
				pNode->m_NodeNumber = node_number_estimator; // intid is the name used , start from 1
				m_INTIDMap [intid] = i;
				m_NodeOrgNumber2INTIDMap[intid] = node_number_estimator;
				pNode->m_NodeNo = i;
				pNode->m_ZoneID = 0;
				pNode->m_CycleLengthInSecond = 0;
				m_NodeSet.push_back(pNode);
				m_NodeNoMap[i] = pNode;
				m_NodeNotoNumberMap[i] = node_number_estimator; // start from 1
				m_NodeNumbertoNodeNoMap[node_number_estimator] = i;
				m_NodeOrgNumber2NodeNoMap[intid] = i;
				i++;
				j++;

			} // node block
			////////////////////////////////////////////

			m_DefaultLinkType = m_LinkTypeArterial;

			if(parser.m_SynchroSectionName.find ("Links")!=  string::npos) 
			{


				string direction_vector[8] = {"NB","SB","EB","WB","NE","NW","SE","SW"};

				int intid;
				string name;
				DTANode* pNode = 0;

				int node_type;
				double X;
				double Y;
				if(parser.GetValueByFieldName("INTID",intid) == false)
					continue;

				parser.GetValueByFieldName("RECORDNAME",name);
				int INTID;
				int m;
				//			switch (name)
				if (name == "Up ID" || name == "UpNodeID")
				{


					for(int direction = 0; direction < 8; direction++)
					{
						int  incoming_node_number;
						if(parser.GetValueByFieldName(direction_vector[direction],incoming_node_number))// value exits
						{
							// add a new link
							int to_node_id = m_INTIDMap[intid];


							if(m_NodeNumbertoNodeNoMap.find(incoming_node_number) != m_NodeNumbertoNodeNoMap.end() )
							{
								int from_node_id = m_NodeNumbertoNodeNoMap[incoming_node_number];
								from_node_id_map[direction] = from_node_id;
								//if(m_NodeNoMap[to_node_id]->m_ControlType == m_ControlType_ExternalNode) XUESONG
								// add new link if the outbound node is an external node
								AddNewLink(from_node_id, to_node_id,true, false);
								AddNewLink(to_node_id,from_node_id,true, false);

							} else
							{

								if(m_NodeOrgNumber2NodeNoMap.find(incoming_node_number) != m_NodeOrgNumber2NodeNoMap.end())
								{

									int from_node_id = m_NodeOrgNumber2NodeNoMap[incoming_node_number];
									from_node_id_map[direction] = from_node_id;
									//if(m_NodeNoMap[to_node_id]->m_ControlType == m_ControlType_ExternalNode) XUESONG
									// add new link if the outbound node is an external node
									AddNewLink(from_node_id, to_node_id,true);
									AddNewLink(to_node_id,from_node_id,true);
								}

							}


						}


					}

				}

				///

				
				if (name == "Lanes")
				{

					for(int direction = 0; direction < 8; direction++)
					{
						int  value;
						if(parser.GetValueByFieldName(direction_vector[direction],value))// value exits
						{
							// add a new link
							int to_node_id = m_INTIDMap[intid];
							int from_node_id = from_node_id_map[direction];
							DTALink* pLink = FindLinkWithNodeIDs(from_node_id, to_node_id);

							if(pLink!=NULL)
						 {
							 pLink->m_NumberOfLanes = value;
						 }

						}


					}	

				} // end of Lanes attributes;

				if (name == "Distance")
				{

					for(int direction = 0; direction < 8; direction++)
					{
						float  value;
						if(parser.GetValueByFieldName(direction_vector[direction],value))// value exits
						{
							// add a new link
							int to_node_id = m_INTIDMap[intid];
							int from_node_id = from_node_id_map[direction];
							DTALink* pLink = FindLinkWithNodeIDs(from_node_id, to_node_id);

							if(pLink!=NULL)
						 {
							 pLink->m_Length  = value/5280;  // feet to mile
						 }

						}


					}	

				} // end of Lanes attributes;

				if (name == "Speed")
				{

					for(int direction = 0; direction < 8; direction++)
					{
						float  value;
						if(parser.GetValueByFieldName(direction_vector[direction],value))// value exits
						{
							// add a new link
							int to_node_id = m_INTIDMap[intid];
							int from_node_id = from_node_id_map[direction];
							DTALink* pLink = FindLinkWithNodeIDs(from_node_id, to_node_id);

							if(pLink!=NULL)
						 {
							 pLink->m_SpeedLimit   = value;  // speed limit
						 }

						}


					}	

				} // end of Lanes attributes;

				if (name == "Name")
				{

					for(int direction = 0; direction < 8; direction++)
					{
						string  name;
						if(parser.GetValueByFieldName(direction_vector[direction],name))// value exits
						{
							// add a new link
							int to_node_id = m_INTIDMap[intid];
							int from_node_id = from_node_id_map[direction];
							DTALink* pLink = FindLinkWithNodeIDs(from_node_id, to_node_id);

							if(pLink!=NULL)
						 {
							 pLink->m_Name = name;
						 }

						}


					}	

				} // end of name attributes;

			} // link block

			if(parser.m_SynchroSectionName.find ("Lanes")!=  string::npos) 
			{

				// finish reading node and link blocks, now we construct movement data

				if(m_bMovementAvailableFlag==0)  // has not been initialized. 
				{
					ConstructMovementVector();


					m_MovementPointerMap.clear();

					std::list<DTANode*>::iterator iNode;
					for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
					{
						if((*iNode)->m_NodeNo  == 7)
						{
							TRACE("");

						}

						for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap["ALLDAY"].m_MovementVector .size(); m++)
						{

							DTANodeMovement movement = (*iNode)->m_MovementDataMap["ALLDAY"]. m_MovementVector[m];

							CString label;
							int up_node_id = m_NodeNoMap[movement.in_link_from_node_id]->m_NodeNo     ;
							int dest_node_id = m_NodeNoMap[movement.out_link_to_node_id ]->m_NodeNo ;
							label.Format("%d;%d;%d", up_node_id,(*iNode)->m_NodeNo ,dest_node_id);

							m_MovementPointerMap[label] = &((*iNode)->m_MovementDataMap["ALLDAY"]. m_MovementVector[m]); // store pointer


						}

					}

				}

				//


				int intid;
				string name;
				DTANode* pNode = 0;

				int node_type;
				double X;
				double Y;
				if(parser.GetValueByFieldName("INTID",intid) == false)
					continue;

				parser.GetValueByFieldName("RECORDNAME",name);

				int m;
				//			switch (name)
				if (name == "Up Node" || name == "UpNodeID")
				{
					UpNodeNumberMap.clear();
					DestNodeNumberMap.clear();

					for(int direction = 0; direction < 12; direction++)
					{
						int  incoming_node_number;
						if(parser.GetValueByFieldName(direction_vector[direction],incoming_node_number))// value exits
						{
							UpNodeNumberMap[direction] = incoming_node_number;


						}


					} //direction

				}  // Up Node

				if (name == "Dest Node" || name == "DestNodeID")
				{

					for(int direction = 0; direction < 12; direction++)
					{
						int  outgoing_node_number;
						if(parser.GetValueByFieldName(direction_vector[direction],outgoing_node_number))// value exits
						{

							DestNodeNumberMap[direction] = outgoing_node_number;

						}

					} //direction

				}  // Dest Node

				// Lanes

				int attribute_index = -1;

				if(name == "Volume")
				{
					TRACE("");
				}

				for(int n = 0;  n < 28; n ++)
				{

					if (name == lane_att_name_str[n])
					{
						attribute_index = n;
						break;
					}


				}


				if (attribute_index>=0)  // attrite has been defined
				{

					int to_node_id = m_INTIDMap[intid];
					for(int direction = 0; direction < 12; direction++)
					{



						if(UpNodeNumberMap.find(direction) != UpNodeNumberMap.end() )
						{

							int UpNodeNo = -1;

							if(m_NodeNumbertoNodeNoMap.find(UpNodeNumberMap[direction]) != m_NodeNumbertoNodeNoMap.end())
							{
								UpNodeNo = m_NodeNumbertoNodeNoMap[UpNodeNumberMap[direction]];

							}else
							{
								if(m_NodeOrgNumber2NodeNoMap.find(UpNodeNumberMap[direction]) != m_NodeOrgNumber2NodeNoMap.end())
								{
									UpNodeNo = m_NodeOrgNumber2NodeNoMap[UpNodeNumberMap[direction]];
								}			

							}



							int DestNodeNo = -1;

							if(m_NodeNumbertoNodeNoMap.find(DestNodeNumberMap[direction]) != m_NodeNumbertoNodeNoMap.end())
							{
								DestNodeNo = m_NodeNumbertoNodeNoMap[DestNodeNumberMap[direction]];

							}else
							{
								if(m_NodeOrgNumber2NodeNoMap.find(DestNodeNumberMap[direction]) != m_NodeOrgNumber2NodeNoMap.end())
								{
									DestNodeNo = m_NodeOrgNumber2NodeNoMap[DestNodeNumberMap[direction]];
								}	


							}


							int CurrentNodeNo = -1;
							if(m_NodeNumbertoNodeNoMap.find(intid) != m_NodeNumbertoNodeNoMap.end())
							{
								CurrentNodeNo = m_NodeNumbertoNodeNoMap[intid];

							}else
							{
								if(m_NodeOrgNumber2NodeNoMap.find(intid) != m_NodeOrgNumber2NodeNoMap.end())
								{
									CurrentNodeNo = m_NodeOrgNumber2NodeNoMap[intid];
								}	


							}


							if(UpNodeNo<0)
								TRACE("\nNode ID %d cannot be found.",UpNodeNumberMap[direction]);

							if(DestNodeNo<0)
								TRACE("\nNode ID %d cannot be found.",DestNodeNumberMap[direction]);

							if(CurrentNodeNo<0)
								TRACE("\nNode ID %d cannot be found.",intid);


							CString label;
							label.Format("%d;%d;%d", UpNodeNo,CurrentNodeNo,DestNodeNo);
							if(m_MovementPointerMap.find(label) != m_MovementPointerMap.end())
							{
								DTANodeMovement* pMovement = m_MovementPointerMap[label];

								int  value = 0;
								if(parser.GetValueByFieldName(direction_vector[direction],value))// value exits
								{

									pMovement->QEM_dir_string = direction_vector[direction];


									/* {"Lanes","Shared","Width","Storage","StLanes","Grade","Speed",
									"FirstDetect","LastDetect","Phase1","PermPhase1","DetectPhase1","IdealFlow","LostTime","SatFlowRatePerLaneGroup",
									14 "SatFlowPerm","SatFlowRTOR","HeadwayFact","Volume","Peds","Bicycles","PHF","Growth","HeavyVehicles","BusStops","Midblock","Distance","TravelTime"};
									*/
									switch(attribute_index)
									{
									case  0:// "Lanes"
										pMovement->QEM_Lanes = value; break;
									case  1:// "Shared"
										pMovement->QEM_Shared = value; break;
									case  2:// "Width"
										pMovement->QEM_Width = value; break;
									case  3:// "Storage"
										pMovement->QEM_Storage = value; break;
									case  4:// "StLanes"
										pMovement->QEM_StLanes = value; break;
									case  5:// "Grade"
										pMovement->QEM_Grade = value; break;
									case  6:// "Speed"
										pMovement->QEM_Speed = value; break;
									case  9:// "Phase1"
										pMovement->QEM_Phase1 = value; break;
									case  10:// "PermPhase1"
										pMovement->QEM_PermPhase1 = value; break;
									case  11:// "DetectPhase1"
										pMovement->QEM_DetectPhase1 = value; break;
									case  12:// "IdealFlow"
										pMovement->QEM_IdealFlow = value; break;
									case  13:// "LostTime"
										pMovement->QEM_LostTime = value; break;
									case  14:// "SatFlowRatePerLaneGroup"
										pMovement->QEM_SatFlow = value; break;
									case  15:// "Volume"
										pMovement->QEM_TurnVolume  = value; break;

									}
								}

							}else
							{

								TRACE("Cannot find movement %s\n", label);

							}

						}

					}  //direction
				} // lanes

				/////////////


			}  // Lane attribute



			if(parser.m_SynchroSectionName.find ("Timeplans")!=  string::npos) 
			{


				string name;
				parser.GetValueByFieldName("RECORDNAME",name);
				int intid;
				if(parser.GetValueByFieldName("INTID",intid) == false)
					continue;

				if(name == "Cycle Length")
				{
					int intid;
					if(parser.GetValueByFieldName("INTID",intid) == false)
						continue;

					int node_id = m_INTIDMap[intid];

					int data = 0;
					if(parser.GetValueByFieldName("DATA",data) == false)
						continue;

					m_NodeNoMap[node_id]->m_CycleLengthInSecond = data;

					if(data > 10)  // set 
						m_NodeNoMap[node_id]->m_ControlType = m_ControlType_PretimedSignal;

	
					SetupSignalValue(intid , timing_plan_name,TIMING_CycleLength,data);



				}

				if(name == "Offset")
				{
					int intid;
					if(parser.GetValueByFieldName("INTID",intid) == false)
						continue;

					int node_id = m_INTIDMap[intid];

					int data = 0;
					if(parser.GetValueByFieldName("DATA",data) == false)
						continue;

					m_NodeNoMap[node_id]->m_SignalOffsetInSecond  = data;

					SetupSignalValue(intid , timing_plan_name,TIMING_Offset,data);
				}


			}

			if(parser.m_SynchroSectionName.find ("Phases")!=  string::npos) 
			{

				string name;
				parser.GetValueByFieldName("RECORDNAME",name);


				int intid;
				if(parser.GetValueByFieldName("INTID",intid) == false)
					continue;


				int node_id = m_INTIDMap[intid];

				CString phasing_map_key  =  GetPhasingMapKey(intid,"ALLDAY");


				if(name == "Start")
				{

					for(int d = 1; d <=16; d++)
					{
						parser.GetValueByFieldName(phasing_column_name[d],m_NodeNoMap[node_id]->m_SignalPhaseStartTime[d]);
					}


				}

				if(name == "MaxGreen")
				{

					for(int d = 0; d <=16; d++)
					{
						parser.GetValueByFieldName(phasing_column_name[d],m_NodeNoMap[node_id]->m_SignalPhaseGreenTime[d]);
					}
				}

			int row_no = 0;

			for(row_no = 0; row_no < DTA_PHASE_ATTRIBUTE_MAX_ROW; row_no++)
			{
				if( name.compare (g_DTA_phasing_row_name[row_no]) == 0)
				{

					// row_no is the selecte no. 
					break;

				}

			}


			if(row_no < DTA_PHASE_ATTRIBUTE_MAX_ROW)
			{
				for(int j = DTA_SIG_PHASE_D1; j< DTA_SIG_PHASE_D16; j++)
				{

					string value;
					parser.GetValueByFieldName(g_DTA_phasing_column_name[j],value);
					m_DTAPhasingMap[phasing_map_key ].m_AMSPhasingData [row_no] [j] = value;
				}

			}
			
			}
		}//while

		parser.CloseCSVFile ();

		// update QEM turn percentage

		// turning percentage
		std::list<DTANode*>::iterator iNode;

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{
			for(unsigned int m = 0; m< (*iNode)->m_MovementDataMap["ALLDAY"].m_MovementVector .size(); m++)
			{
				DTANodeMovement* pMovement = &((*iNode)->m_MovementDataMap["ALLDAY"].m_MovementVector[m]);


				if(pMovement->QEM_Phase1 >0 && pMovement->QEM_Phase1 <=16)
				{

				pMovement->QEM_StartTime = (*iNode)->m_SignalPhaseStartTime [pMovement->QEM_Phase1];
				pMovement->QEM_EndTime = pMovement->QEM_StartTime + (*iNode)->m_SignalPhaseGreenTime [pMovement->QEM_Phase1];

						std::string movement_vector; 

						DTA_Phasing_Data_Matrix element  = GetPhaseData((*iNode)->m_NodeNumber , timing_plan_name);


						DTA_SIG_PHASE column = (DTA_SIG_PHASE)( DTA_SIG_PHASE_VALUE + pMovement->QEM_Phase1) ;
						//get existing movement data
						movement_vector = element.GetSTDString( column, PHASE_MOVEMENT_VECTOR );

						// add movement data
						movement_vector += string_format("%d",m_NodeNoMap[pMovement->in_link_from_node_id]->m_NodeNumber);
						movement_vector += ":";
						movement_vector += string_format("%d",m_NodeNoMap[pMovement->out_link_to_node_id ]->m_NodeNumber); 
						movement_vector += ";";


						int QEM_end_time = (int)(pMovement->QEM_EndTime+0.5);

				SetupPhaseData((*iNode)->m_NodeNumber ,timing_plan_name, pMovement->QEM_Phase1, PHASE_End, QEM_end_time);



						SetupPhaseData((*iNode)->m_NodeNumber ,timing_plan_name, pMovement->QEM_Phase1, PHASE_MOVEMENT_VECTOR, movement_vector);

				}
				DTALink* pLink0 = m_LinkNoMap[pMovement->IncomingLinkNo  ];

				int total_link_count = 0;
				for(unsigned int j = 0; j< (*iNode)->m_MovementDataMap["ALLDAY"].m_MovementVector .size(); j++)
				{

					if((*iNode)->m_MovementDataMap["ALLDAY"].m_MovementVector[j].IncomingLinkNo == pMovement->IncomingLinkNo )
					{
						total_link_count+= (*iNode)->m_MovementDataMap["ALLDAY"].m_MovementVector[j].QEM_TurnVolume ;
					}

				}

				if (pMovement->QEM_TurnVolume >=1)
				{
					pMovement->QEM_TurnPercentage = 
						pMovement->QEM_TurnVolume * 100.0f / max(1,total_link_count);
				}
			}

		}


		UpdateAllMovementGreenStartAndEndTime("ALLDAY");

		m_NodeDataLoadingStatus.Format ("%d nodes are loaded from file %s.",m_NodeSet.size(),lpszFileName);
		return true;
	}else
	{
		AfxMessageBox("Error: File LAYOUT.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
		return false;
		//		g_ProgramStop();
	}


}




void CTLiteDoc::ExportDataForQEMFile(CString filename, bool bUseSequentialNodeID)
{
	FILE* st = NULL;
	fopen_s(&st, filename, "w");
	if (st == NULL)
	{
		CString msg;
		msg.Format("File %s file cannot be opened.", filename);
		AfxMessageBox("msg");
		return;
	}

	std::list<DTANode*>::iterator iNode;

	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{
		// find connecting nodes and links at different directions
		DTALink* pLinkVector[8] = { NULL };

		int NumberOfLanes[8] = { 0 };
		
		for (int inbound_i = 0; inbound_i< (*iNode)->m_IncomingLinkVector.size(); inbound_i++)
		{
			int LinkID = (*iNode)->m_IncomingLinkVector[inbound_i];

			if (m_LinkNoMap.find(LinkID) != m_LinkNoMap.end())
			{
			
			DTALink* pIncomingLink = m_LinkNoMap[LinkID];

			if (pIncomingLink!=NULL && pIncomingLink->m_NumberOfLanes > 0 && pIncomingLink->m_FromNodeID != (*iNode)->m_NodeNo)
			{
				int up_node_id = pIncomingLink->m_FromNodeID;
				GDPoint p1 = m_NodeNoMap[up_node_id]->pt;
				GDPoint p2 = (*iNode)->pt;
				int incoming_approach = Find_Angle_to_Approach_8_direction(Find_P2P_Angle(p2, p1));
				int index = (int)(incoming_approach);

				pLinkVector[index] = pIncomingLink;
				NumberOfLanes[index] = pIncomingLink->m_NumberOfLanes;


			}
			}
		} // for each incoming link


	}
	fprintf(st, "RECORDNAME,INTID,");

	for (int j = DTA_NBL2; j <= DTA_HOLD; j++)
		fprintf(st, "%s,", g_DTA_movement_column_name[j].c_str());

	fprintf(st, "\n");

	std::string timing_plan_name = "ALLDAY";
	CString TimingPlanName = "ALLDAY";

	int signal_count = 0;
	// prepare lanes data 
	for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
	{

	//	if ((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal)
		{
			signal_count++;

			CString key;
			key.Format("%d", (*iNode)->m_NodeNumber);

			for (unsigned int m = 0; m< (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector.size(); m++)
			{

				// assign values per movement

				DTANodeMovement movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m];


				if (((*iNode)->m_ControlType == m_ControlType_PretimedSignal || (*iNode)->m_ControlType == m_ControlType_ActuatedSignal) && movement.turning_prohibition_flag == 0)
				{
					if (movement.QEM_Lanes >= 1 && movement.QEM_EffectiveGreen < 6)
					{
						DTALink* pIncomingLink = m_LinkNoMap[movement.IncomingLinkNo];
						if (pIncomingLink!=NULL)
						(*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m].QEM_EffectiveGreen = (int)((*iNode)->m_CycleLengthInSecond* pIncomingLink->m_LaneCapacity / pIncomingLink->m_Saturation_flow_rate_in_vhc_per_hour_per_lane);
					}


				}

				movement = (*iNode)->m_MovementDataMap[timing_plan_name].m_MovementVector[m]; // refetch data

				DTA_SIG_MOVEMENT movement_index = movement.movement_approach_turn;

				if (bUseSequentialNodeID)
				{
					m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_UpNode, movement.in_link_from_node_id + 1);
					m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_DestNode, movement.out_link_to_node_id + 1);
				}
				else
				{
					int UpNodeNumber = m_NodeNoMap[movement.in_link_from_node_id]->m_NodeNumber; 
					int DestNodeNumber = m_NodeNoMap[movement.out_link_to_node_id]->m_NodeNumber;
					m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_UpNode, UpNodeNumber);
					m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_DestNode, DestNodeNumber);
				}

				if (movement.QEM_Lanes >= 1)
				{
					DTALink* pIncomingLink = m_LinkNoMap[movement.IncomingLinkNo];
					if (pIncomingLink != NULL)
					m_DTAMovementMap[key].SetString(movement_index, DTA_MOVEMENT_ATTRIBUTE_StreetName, pIncomingLink->m_Name);
				}
				

				DTATimingPlan timing_plan = GetTimingPlanInfo("ALLDAY");
				float number_of_hours = max(0.01, (timing_plan.end_time_in_min - timing_plan.start_time_in_min) / 60.0);
				float sim_turn_hourly_count = movement.sim_turn_count / number_of_hours;


				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_TurnVolume, sim_turn_hourly_count);


				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_Lanes, movement.QEM_Lanes);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_Shared, movement.QEM_Shared);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_Width, movement.QEM_Width);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_Storage, movement.QEM_Storage);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_StLanes, movement.QEM_StLanes);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_Grade, movement.QEM_Grade);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_Speed, movement.QEM_Speed);

				// DTA_MOVEMENT_ATTRIBUTE_FirstDetect: no variable in movement
				// DTA_MOVEMENT_ATTRIBUTE_LastDetect: no variable in movement
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_Phase1, movement.QEM_Phase1);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_PermPhase1, movement.QEM_PermPhase1);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_IdealFlow, movement.QEM_IdealFlow);

				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_LostTime, movement.QEM_LostTime);
				m_DTAMovementMap[key].SetValue(movement_index, DTA_MOVEMENT_ATTRIBUTE_SatFlow, movement.QEM_SatFlow); \



					// setup phase data

					// this phase number is used now. 

					int phase_no = movement.QEM_Phase1;

				DTA_SIG_PHASE phase_index = (DTA_SIG_PHASE)(phase_no + 8);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_MinGreen, 4);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_MaxGreen, 19);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_Yellow, 3.5);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_AllRed, 0.5);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_VehExt, 3);
				m_DTAPhasingMap[key].SetValue(phase_index, PHASE_MinGap, 3);

				m_DTAPhasingMap[key].SetValue(DTA_SIG_PHASE_VALUE, TIMING_CycleLength, 50);

				// 

			}  // for each movement



			// for each record

			// write data per record name and then per movement 
			for (int i = DTA_MOVEMENT_ATTRIBUTE_UpNode; i < DTA_MOVEMENT_ATTRIBUTE_FirstDetect; i++)
			{

				if (bUseSequentialNodeID)
					fprintf(st, "%s,%d,", g_DTA_movement_row_name[i].c_str(), (*iNode)->m_NodeNo+1);
				else
					fprintf(st, "%s,%d,", g_DTA_movement_row_name[i].c_str(), (*iNode)->m_NodeNumber );

				for (int j = DTA_NBL2; j <= DTA_HOLD; j++)
				{
					fprintf(st, "%s,", m_DTAMovementMap[key].m_AMSMovementData[i][j].c_str());
				}

				fprintf(st, "\n");  // go to the next line

			}

		}	// condition
	}	// for each node

	if (signal_count == 0)
	{
		fprintf(st, "\nNo node has been specified as an signalized intersection, so no data is output to the QEM program.");

	}
		fclose(st);
}

