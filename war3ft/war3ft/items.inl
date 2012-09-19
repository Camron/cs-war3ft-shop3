/*
*	Item Functions
*/

// Item Setup Functions
ITEM_Init()
{
	ITEM_COST[ITEM_ANKH]	    = 1500;			// Ankh of Reincarnation
	ITEM_COST[ITEM_BOOTS]       = 2500;			// Boots of Speed
	ITEM_COST[ITEM_CLAWS]	    = 1000;			// Claws of Attack
	ITEM_COST[ITEM_CLOAK]	    = 800;			// Cloak of Shadows
	ITEM_COST[ITEM_MASK]	    = 2000;			// Mask of Death
	ITEM_COST[ITEM_NECKLACE]	= 800;			// Necklace of Immunity
	ITEM_COST[ITEM_FROST]	    = 2000;			// Orb of Frost
	ITEM_COST[ITEM_HEALTH]	    = 1000;			// Periapt of Health
	ITEM_COST[ITEM_TOME]	    = 4000;			// Tome of Experience
	ITEM_COST[ITEM_SCROLL]	    = 6000;			// Scroll of Respawning
	ITEM_COST[ITEM_PROTECTANT]	= 1500;			// Mole Protectant
	ITEM_COST[ITEM_HELM]	    = 3000;			// Helm of Excellence
	ITEM_COST[ITEM_AMULET]	    = 1500;			// Amulet of the Cat
	ITEM_COST[ITEM_SOCK]	    = 1500;			// Sock of the Feather
	ITEM_COST[ITEM_GLOVES]	    = 1750;			// Flaming Gloves of Warmth
	ITEM_COST[ITEM_RING]	    = 1000;			// Ring of Regeneration + 1
	ITEM_COST[ITEM_CHAMELEON]	= 4500;			// Chameleon
	ITEM_COST[ITEM_MOLE]	    = 16000;		// Mole
	
	ITEM_COST[ITEM_ORCPROT]	    = 1500;
	ITEM_COST[ITEM_TRUEPROT]    = 1500;
	ITEM_COST[ITEM_STEALCASH]   = 100;
	ITEM_COST[ITEM_STEALARMOR]	    = 200;
	ITEM_COST[ITEM_SHARPBULLETS]	 = 2000;
	ITEM_COST[ITEM_SLAP]	    = 1500;
	ITEM_COST[ITEM_LONGJUMP]	    = 2000;
	ITEM_COST[ITEM_MIRROR]	    = 1500;
	ITEM_COST[ITEM_ONEHIT]	    = 3000;
	
	// Item costs are a little different for DOD
	if ( g_MOD == GAME_DOD )
	{
		ITEM_COST[ITEM_SCROLL]	= 800;
	}

	// Items are chargeable
	g_iFlag[ITEM_NECKLACE]	|= ITEM_CHARGEABLE;
	g_iFlag[ITEM_HELM]		|= ITEM_CHARGEABLE;
	g_iFlag[ITEM_RING]		|= ITEM_CHARGEABLE;
	
	g_iFlag[ITEM_ONEHIT]		|= ITEM_CHARGEABLE;
	g_iFlag[ITEM_SLAP]		|= ITEM_CHARGEABLE;

	// Items should be used when bought
	g_iFlag[ITEM_TOME]		|= ITEM_USEONBUY;

	// Items CAN be bought when dead
	g_iFlag[ITEM_ANKH]		|= ITEM_BUYWHENDEAD;
	g_iFlag[ITEM_SCROLL]	|= ITEM_BUYWHENDEAD;
	g_iFlag[ITEM_MOLE]		|= ITEM_BUYWHENDEAD;
	g_iFlag[ITEM_TOME]		|= ITEM_BUYWHENDEAD;

	// Items are used when the next round starts...
	g_iFlag[ITEM_ANKH]		|= ITEM_NEXTROUNDUSE;
	g_iFlag[ITEM_MOLE]		|= ITEM_NEXTROUNDUSE;
}

// We created this to allow for different prices of items at different levels
ITEM_Cost( id, iItem )
{
	// How much should ratio increase per level?
	new Float:fInc = ( 1.0 - ITEM_COST_RATIO ) / float( MAX_LEVELS );

	// Cost * ratio (ITEM_COST_RATIO to 1.0 dependent on level)
	new Float:fRatio = (float( p_data[id][P_LEVEL] ) * fInc) + ITEM_COST_RATIO;

	return ( p_data[id][P_RACE] != RACE_NONE ) ? floatround( float( ITEM_COST[iItem] ) * fRatio ) : ITEM_COST[iItem];
}


public ITEM_CanBuy( id, iItem )
{
	// User doesn't have the money
	if ( SHARED_GetUserMoney( id ) < ITEM_Cost( id, iItem ) )
	{
		client_print( id, print_center, "%L", id, "INSUFFICIENT_FUNDS" );

		return false;
	}
	
	// User already owns the item and it's not a chargeable item!
	//#BUG. some 1 time use items are marked chargable, so they can be bought more than once.
	//else if ( ITEM_Has( id, iItem ) > ITEM_NONE && !ITEM_CheckFlag( iItem, ITEM_CHARGEABLE ) )
	
	else if ( ITEM_Has( id, iItem ) > ITEM_NONE)
	{
		
		//#HACK. should really make a new flag/
		//Lets us check these 2 chargables, even tho we shouldnt.
		//if (iItem == ITEM_SLAP || iItem == ITEM_ONEHIT)
		 if (iItem == ITEM_ONEHIT)
		{
			client_print( id, print_center, "%L", id, "ALREADY_OWN_THAT_ITEM" );
			return false;
		}
		
		if (ITEM_CheckFlag( iItem, ITEM_CHARGEABLE ))
		{
			//dont do anything. allowed to have more than 1 charge (stack)
		}
		else
		{
			client_print( id, print_center, "%L", id, "ALREADY_OWN_THAT_ITEM" );
			return false;
		}
	}
	
	
	
	// Make sure these items can be bought if the user is dead
	else if ( !is_user_alive( id ) && !ITEM_CheckFlag( iItem, ITEM_BUYWHENDEAD ) ) 
	{
		client_print( id, print_center, "%L", id, "NOT_PURCHASE_WHEN_DEAD" );

		return false;
	}
	
	// User has necklace + blink, they don't need a necklace
	else if ( iItem == ITEM_NECKLACE && p_data_b[id][PB_WARDENBLINK] )
	{
		client_print( id, print_center, "You are already immune to ultimates through one of your skills!" );

		return false;
	}

	// User doesn't need an ankh if they're going to reincarnate
	else if ( iItem == ITEM_ANKH && SM_GetSkillLevel( id, SKILL_REINCARNATION ) == 3 && !p_data[id][P_CHANGERACE] && p_data[id][P_RACE] != RACE_CHAMELEON )
	{
		client_print( id, print_center, "You will already reincarnate your weapons through one of your skills!" );

		return false;
	}
	
	// User has purchased the maximum allowed rings
	if ( g_iTotalRings[id] >= 5 && iItem == ITEM_RING )
	{
		client_print( id, print_center, "%L", id, "NOT_PURCHASE_MORE_THAN_FIVE_RINGS" );

		return false;
	}

	// User has purchased gloves when they're disabled on this map
	else if ( iItem == ITEM_GLOVES && g_bGlovesDisabled )
	{
		client_print( id, print_chat, "Gloves are disabled on this map!" );

		return false;
	}

	// User is already going to reincarnate weapons because they bought an ankh earlier (probably lost it when died)
	else if ( ( iItem == ITEM_ANKH && g_bPlayerBoughtAnkh[id] ) || ( iItem == ITEM_MOLE && g_bPlayerBoughtMole[id] ) )
	{
		client_print( id, print_center, "%L", id, "ALREADY_OWN_THAT_ITEM" );

		return false;
	}
	
	//Slap
	else if ( iItem == ITEM_SLAP && g_bPlayerAlreadyBoughtSlap[id] )
	{
		client_print( id, print_center, "%L", id, "BUY_ONLY_ONCE_PER_ROUND" );

		return false;
	}
	
	//One hit
	else if ( iItem == ITEM_ONEHIT && g_bPlayerAlreadyBoughtOneHit[id] )
	{
		client_print( id, print_center, "%L", id, "BUY_ONLY_ONCE_PER_ROUND" );

		return false;
	}
	return true;
}

public ITEM_Buy( id, iItem )
{

	
	// If the user can buy this item...
	if ( ITEM_CanBuy( id, iItem ) )
	{

		// User's items are full
		if ( ITEM_GetSlot( id ) == ITEM_SLOT_FULL && !ITEM_CheckFlag( iItem, ITEM_USEONBUY ) )
		{

			// We only care about items being full if this item isn't a use on buy
			if ( !ITEM_CheckFlag( iItem, ITEM_USEONBUY ) )
			{

				new bool:bShowReplaceMenu = false;

				// One time use...
				if ( !ITEM_CheckFlag( iItem, ITEM_CHARGEABLE ) )
				{
					//client_print( id, print_chat, "[DEBUG] Item is not chargeable" );

					bShowReplaceMenu = true;
				}

				// We also need to replace it if the item is chargeable but they don't own that item
				if ( ITEM_Has( id, iItem ) == ITEM_NONE && ITEM_CheckFlag( iItem, ITEM_CHARGEABLE ) )
				{
					//client_print( id, print_chat, "[DEBUG] Doesn't have item and new item is chargeable" );

					bShowReplaceMenu = true;
				}

				if ( bShowReplaceMenu )
				{
					g_iFutureItem[id] = iItem;

					MENU_ReplaceItem( id );

					return;
				}
			}
		}
	
		// We're clear!

		// Remove user's money
		new iNewMoney = SHARED_GetUserMoney( id ) - ITEM_Cost( id, iItem );
		SHARED_SetUserMoney( id, iNewMoney );

		ITEM_GiveItem( id, iItem );
	}

	return;
}

// Item Buy Functions
bool:ITEM_MenuCanBuyCheck( id )
{
	// Duh how can they buy if they're dead!
	if ( !p_data_b[id][PB_ISCONNECTED] )
	{
		return false;
	}

	new isPlayerAlive		= is_user_alive( id );

	if ( !get_pcvar_num( CVAR_wc3_buy_dead ) && !isPlayerAlive )
	{
		client_print( id, print_center, "%L", id, "NOT_BUY_ITEMS_WHEN_DEAD" );
		
		return false;
	}
	
	else if ( g_MOD == GAME_CSTRIKE || g_MOD == GAME_CZERO )
	{
		new isPlayerInBuyZone	= cs_get_user_buyzone( id )
		
		if ( get_pcvar_num( CVAR_wc3_buy_time ) && !g_buyTime )
		{
			client_print( id, print_center, "%L", id, "SECONDS_HAVE_PASSED_CANT_BUY", ( get_cvar_float( "mp_buytime" ) * 60.0 ) );

			return false;
		}
		
		else if ( get_pcvar_num( CVAR_wc3_buy_zone ) && !isPlayerInBuyZone && isPlayerAlive )
		{
			client_print( id, print_center, "%L", id, "MUST_BE_IN_BUYZONE" );
			
			return false;
		}
	}
	
	return true;
}

// Item Preset Function
ITEM_GiveItem( id, iItem )
{

	// This item we should use instantly
	if ( ITEM_CheckFlag( iItem, ITEM_USEONBUY ) )
	{
		if ( iItem == ITEM_TOME )
		{
			ITEM_Tome( id );

			return;
		}
	}

	// They are just adding some charges
	else
	{
		// Actually set our item variable
		if ( is_user_alive( id ) )
		{
			ITEM_Equip( id, iItem );
		}

		// Display message to the user regarding the item they just purchased
		ITEM_DisplayMessage( id, iItem );

		// Give bonuses
		ITEM_GiveBonuses( id, iItem );

		// Play purchase sound
		emit_sound( id, CHAN_STATIC, g_szSounds[SOUND_PICKUPITEM], 1.0, ATTN_NORM, 0, PITCH_NORM );
	}

	WC3_ShowBar( id );

	return;
}

ITEM_DisplayMessage( id, iItem )
{
	// Display a message regarding the item they just purchased
	switch ( iItem )
	{
		case ITEM_ANKH:
		{
			(g_MOD == GAME_DOD)								? client_print( id, print_chat,"%s %L", g_MODclient, id, "DOD_INFO_SHOPMENU_1" ) : 0;
			(g_MOD == GAME_CSTRIKE || g_MOD == GAME_CZERO)	? client_print( id, print_chat,"%s %L", g_MODclient, id, "INFO_SHOPMENU_1" ) : 0;
		}

		case ITEM_BOOTS:
		{
			if ( g_MOD == GAME_CSTRIKE || g_MOD == GAME_CZERO )
			{
				client_print( id, print_chat,"%s %L", g_MODclient, id, "INFO_SHOPMENU_2", ( ( get_pcvar_float( CVAR_wc3_boots ) ) * 100.00 ) );
			}

			else if ( g_MOD == GAME_DOD )
			{
				client_print( id, print_chat,"%s %L", g_MODclient, id, "DOD_INFO_SHOPMENU_2" );
			}
		}

		case ITEM_CLAWS:
		{
			client_print( id, print_chat,"%s %L", g_MODclient, id, "INFO_SHOPMENU_3", get_pcvar_num( CVAR_wc3_claw ) );
		}

		case ITEM_CLOAK:
		{
			new Float:fInvis = 100.0 * ( float( get_pcvar_num( CVAR_wc3_cloak ) ) / 255.0 );
			client_print(id, print_chat,"%s %L", g_MODclient, id, "INFO_SHOPMENU_4", fInvis );
		}

		case ITEM_MASK:
		{
			new Float:fMask = ( 100.0 * get_pcvar_float( CVAR_wc3_mask ) );
			client_print(id, print_chat,"%s %L", g_MODclient, id, "INFO_SHOPMENU_5", fMask );
		}

		case ITEM_NECKLACE:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU_6", NECKLACE_CHARGES );
		}

		case ITEM_FROST:
		{
			new Float:fFrost = 100.0 * ( get_pcvar_float( CVAR_wc3_frost ) / 260.0 );
			client_print(id, print_chat,"%s %L", g_MODclient, id, "INFO_SHOPMENU_7", fFrost );
		}

		case ITEM_HEALTH:
		{
			client_print(id, print_chat,"%s %L", g_MODclient, id, "INFO_SHOPMENU_8", get_pcvar_num( CVAR_wc3_health ) );
		}

		case ITEM_SCROLL:
		{
			if ( is_user_alive( id ) )
			{
				client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_1" );
			}

			else
			{
				client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_1_DEAD" );
			}
		}

		case ITEM_PROTECTANT:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_2" );
		}

		case ITEM_HELM:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_3", HELM_CHARGES );
		}

		case ITEM_AMULET:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_4" );
		}

		case ITEM_SOCK:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_5" );
		}

		case ITEM_GLOVES:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_6", get_pcvar_num( CVAR_wc3_glove_timer ) );
		}

		case ITEM_RING:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_7" );
		}

		case ITEM_CHAMELEON:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_8" );
		}

		case ITEM_MOLE:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU2_9" );
		}
		
		//Shop 3
		
		case ITEM_ORCPROT:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_1" );
		}
		
		case ITEM_TRUEPROT:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_2" );
		}
		
		case ITEM_STEALCASH:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_3", get_pcvar_num( CVAR_wc3_cashsteal ) );
		}
		
		case ITEM_STEALARMOR:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_4" , get_pcvar_num( CVAR_wc3_armorsteal ) );
		}
		
		case ITEM_SHARPBULLETS:
		{
			//client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_5" );
			client_print( id, print_chat,"%s %L", g_MODclient, id, "INFO_SHOPMENU3_5", get_pcvar_num( CVAR_wc3_sharpbullets ) );
		}
		
		case ITEM_SLAP:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_6" );
		}
		
		case ITEM_LONGJUMP:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_7" );
		}
		
		case ITEM_MIRROR:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_8" );
		}
		
		case ITEM_ONEHIT:
		{
			client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU3_9" );
		}
	}
}

// Give the user bonuses for their items (except charges)
ITEM_GiveAllBonuses( id )
{

	// Loop through all item slots
	for ( new i = ITEM_SLOT_ONE; i <= ITEM_SLOT_FULL - 1; i++ )
	{
		
		// Do we have a valid item here?
		if ( g_iShopMenuItems[id][i] != ITEM_NONE )
		{

			// Don't want to give the user more charges for free do we?
			//  And we don't want to give the bonuses if this is a next round use item (i.e. if we do then mole for infinity - that doesn't seem nice)
			if ( !ITEM_CheckFlag( g_iShopMenuItems[id][i], ITEM_CHARGEABLE ) && !ITEM_CheckFlag( g_iShopMenuItems[id][i], ITEM_NEXTROUNDUSE ) )
			{
				ITEM_GiveBonuses( id, g_iShopMenuItems[id][i] );
			}
		}
	}
}

// Give our players their bonus!
ITEM_GiveBonuses( id, iItem )
{
	
	// Display a message regarding the item they just purchased
	switch ( iItem )
	{
		case ITEM_ANKH:
		{
			g_bPlayerBoughtAnkh[id] = true;
		}

		case ITEM_BOOTS:
		{
			SHARED_SetSpeed( id );
		}

		case ITEM_CLOAK:
		{
			SHARED_INVIS_Set( id );
		}

		case ITEM_NECKLACE:
		{
			g_iNecklaceCharges[id] += NECKLACE_CHARGES;
		}

		case ITEM_HEALTH:
		{
			new iHealth = get_pcvar_num( CVAR_wc3_health );
			iHealth += get_user_health( id );
			set_pev( id, pev_dmg_inflictor, 0 );
			set_user_health( id, iHealth );
		}

		case ITEM_SCROLL:
		{
			if ( !is_user_alive( id ) )
			{
				ITEM_Scroll( id );
			}
		}

		case ITEM_HELM:
		{
			g_iHelmCharges[id] += HELM_CHARGES;
		}

		case ITEM_SOCK:
		{
			SHARED_SetGravity( id );
		}

		case ITEM_GLOVES:
		{
			ITEM_Gloves( id );
		}

		case ITEM_RING:
		{
			g_iTotalRings[id] += RING_INCREMENT;

			if ( !task_exists( TASK_ITEM_RING + id ) )
			{
				_ITEM_Ring( id );
			}
		}

		case ITEM_CHAMELEON:
		{
			SHARED_ChangeSkin( id, SKIN_SWITCH );
		}

		case ITEM_MOLE:
		{
			g_bPlayerBoughtMole[id] = true;
		}
		
		//Shop 3
		/*
		case ITEM_NEW5:
		{
			
			
			//Old Steel Skin 
			new iExtraArmor = get_pcvar_num( CVAR_wc3_extraarmor );
			new CsArmorType:ArmorType;
			new iCurArmor = cs_get_user_armor( id, ArmorType );
			new iMaxArmor = get_user_maxarmor( id );
			new iNewArmor = iCurArmor + iExtraArmor;
			
			//TODO: clamp function in amxx?
			if ( iNewArmor > iMaxArmor )
			{
				iNewArmor = iMaxArmor;
			}
			cs_set_user_armor( id, iNewArmor, ArmorType );
		}
		*/
		
		
		case ITEM_ONEHIT:
		{
			//g_iOneHitCharges[id] += ONEHIT_CHARGES;
			g_iOneHitCharges[id] = ONEHIT_CHARGES;
			g_bPlayerAlreadyBoughtOneHit[id] = true;
		}
		
		case ITEM_SLAP:
		{
			//g_iSlapCharges[id] += SLAP_CHARGES;
			g_iSlapCharges[id] = SLAP_CHARGES;
			g_bPlayerAlreadyBoughtSlap[id]=true;
		}
		
		case ITEM_LONGJUMP:
		{
			//give_item( id, "item_longjump" );
			g_bPlayerHasLongJump[id]=true;
		}
	}
}

// Item Equip Function
ITEM_Equip( id, iItem )
{
	new iItemSlot = ITEM_GetSlot( id );

	// Items are not full
	if ( iItemSlot != ITEM_SLOT_FULL )
	{

		new iOldItem = g_iShopMenuItems[id][iItemSlot];

		if ( iItem == iOldItem || ITEM_Has( id, iItem ) > ITEM_NONE )
		{
			// Might hit this if we added charges - we want to update the user's HUD
			WC3_ShowBar( id );

			return;
		}

		// Remove the user's old item if necessary
		else if ( g_iShopMenuItems[id][iItemSlot] > ITEM_NONE )
		{
			ITEM_Remove( id, iItemSlot );
		}
			
		// Set their new item
		g_iShopMenuItems[id][iItemSlot] = iItem;
	}

	WC3_ShowBar( id );

	return;
}

// Item Remove Functions
ITEM_RemoveID( id, iItem )
{
	new iItemSlot = ITEM_Has( id, iItem );

	if ( iItemSlot > ITEM_NONE )
	{
		ITEM_Remove( id, iItemSlot );
	}

	return;
}

ITEM_Remove( id, iItemSlot, bResetAnkhMole = true )
{
	new iItem = g_iShopMenuItems[id][iItemSlot];

	g_iShopMenuItems[id][iItemSlot] = ITEM_NONE;

	switch( iItem )
	{
		case ITEM_ANKH:
		{
			if ( bResetAnkhMole )
			{
				g_bPlayerBoughtAnkh[id] = false;
			}
		}

		case ITEM_BOOTS:
		{
			SHARED_SetSpeed( id );
		}

		case ITEM_CLOAK:
		{
			SHARED_INVIS_Set( id );
		}

		case ITEM_NECKLACE:
		{
			g_iNecklaceCharges[id] = 0;
		}

		case ITEM_HEALTH:
		{
			new iNewHealth = get_user_health( id ) - get_pcvar_num( CVAR_wc3_health );
			
			// Lets not kill the user, give them 1 health
			iNewHealth = ( ( iNewHealth <= 0 ) ? 1 : iNewHealth );
			
			set_pev( id, pev_dmg_inflictor, 0 );
			set_user_health( id, iNewHealth );
		}

		case ITEM_HELM:
		{
			g_iHelmCharges[id] = 0;
		}
		
		case ITEM_SOCK:
		{
			SHARED_SetGravity( id );
		}

		case ITEM_GLOVES:
		{
			if ( task_exists( TASK_ITEM_GLOVES + id ) )
			{
				remove_task( TASK_ITEM_GLOVES + id );
			}
		}

		case ITEM_RING:
		{
			if ( task_exists( TASK_ITEM_RING + id ) )
			{
				remove_task( TASK_ITEM_RING + id );
			}
			
			// Set the number of rings to 0
			g_iTotalRings[id] = 0;
		}

		case ITEM_CHAMELEON:
		{
			SHARED_ChangeSkin( id, SKIN_RESET );
		}

		case ITEM_MOLE:
		{
			if ( bResetAnkhMole )
			{
				g_bPlayerBoughtMole[id] = false;
			}
		}
		
		/*
		case ITEM_NEW5:
		{
			
			new iExtraArmor = get_pcvar_num( CVAR_wc3_extraarmor );
			new CsArmorType:ArmorType;
			new iCurArmor = cs_get_user_armor( id, ArmorType );
			new iNewArmor = iCurArmor - iExtraArmor;
			
			if (iNewArmor <0)
			{
				iNewArmor=0;
			}
				cs_set_user_armor( id, iNewArmor, ArmorType );
		}
		*/
		
		case ITEM_ONEHIT:
		{
			g_iOneHitCharges[id] = 0;
		}
		
		case ITEM_SLAP:
		{
			g_iSlapCharges[id] = 0;
		}
		
		case ITEM_LONGJUMP:
		{
			
			/*
			if (is_user_connected(id))
			{
					engfunc(EngFunc_SetPhysicsKeyValue, id, "slj", "0");
					//m_fLongJump offset = 356
					set_pdata_int(id,356,0);
			}
			*/
			g_bPlayerHasLongJump[id]=false;
		}
	}

	WC3_ShowBar( id );

	return;
}

ITEM_RemoveCharge( id, iItem )
{
	if ( ITEM_Has( id, iItem ) > ITEM_NONE )
	{
		switch ( iItem )
		{
			case ITEM_NECKLACE:
			{
				g_iNecklaceCharges[id] -= CHARGE_DISPOSE;
				
				if ( g_iNecklaceCharges[id] <= 0 )
				{
					ITEM_RemoveID( id, iItem );
				}
			}

			case ITEM_HELM:
			{
				g_iHelmCharges[id] -= CHARGE_DISPOSE;
				
				if ( g_iHelmCharges[id] <= 0 )
				{
					ITEM_RemoveID( id, iItem );
				}
			}

			case ITEM_RING:
			{
				g_iTotalRings[id] -= CHARGE_DISPOSE;
				
				if ( g_iTotalRings[id] <= 0 )
				{
					ITEM_RemoveID( id, iItem );
				}
			}
			//
			case ITEM_ONEHIT:
			{
				g_iOneHitCharges[id] -= CHARGE_DISPOSE;
				
				if ( g_iOneHitCharges[id] <= 0 )
				{
					ITEM_RemoveID( id, iItem );
				}
			}
			
			case ITEM_SLAP:
			{
				g_iSlapCharges[id] -= CHARGE_DISPOSE;
				
				if ( g_iSlapCharges[id] <= 0 )
				{
					ITEM_RemoveID( id, iItem );
				}
			}
		}
	}

	WC3_ShowBar( id );
	return;
}


// Item Get Functions
ITEM_GetSlot( id )
{
	if ( g_iShopMenuItems[id][ITEM_SLOT_ONE] > ITEM_NONE && g_iShopMenuItems[id][ITEM_SLOT_TWO] > ITEM_NONE && g_iShopMenuItems[id][ITEM_SLOT_THREE] > ITEM_NONE )
		return ITEM_SLOT_FULL;

		new bool:firstFull=false;
		new bool:secondFull=false;
		new bool:thirdFull=false;
		
	if ( g_iShopMenuItems[id][ITEM_SLOT_ONE] > ITEM_NONE )
		firstFull=true;
	
	if ( g_iShopMenuItems[id][ITEM_SLOT_TWO] > ITEM_NONE )
		secondFull=true;
	if ( g_iShopMenuItems[id][ITEM_SLOT_THREE] > ITEM_NONE )
		thirdFull=true;
		
		if (!firstFull)
			return ITEM_SLOT_ONE;
		if (!secondFull)
			return ITEM_SLOT_TWO;
		if (!thirdFull)
			return ITEM_SLOT_THREE;

	//all failed?
	return ITEM_SLOT_FULL;
}

ITEM_isSlotFilled( id, slot )
{
	if (g_iShopMenuItems[id][slot] > ITEM_NONE )
	{
		return true;
	}
	return false;
}


ITEM_Has( id, iItem )
{
	if ( g_iShopMenuItems[id][ITEM_SLOT_ONE] == iItem )
		return ITEM_SLOT_ONE;

	else if ( g_iShopMenuItems[id][ITEM_SLOT_TWO] == iItem )
		return ITEM_SLOT_TWO;
		
	else if ( g_iShopMenuItems[id][ITEM_SLOT_THREE] == iItem )
		return ITEM_SLOT_THREE;

	return ITEM_NONE;
}

//Mine. only returns true or false. if it has. makes it less chance of a mistake
ITEM_doesHave( id, iItem )
{
	if ( g_iShopMenuItems[id][ITEM_SLOT_ONE] == iItem )
		return true;
		

	else if ( g_iShopMenuItems[id][ITEM_SLOT_TWO] == iItem )
		return true;
		
	else if ( g_iShopMenuItems[id][ITEM_SLOT_THREE] == iItem )
		return true;
	
	//Didnt find
	return false;
}

// Item Death Function
ITEM_UserDied( id )
{
	// The user just died, remove all items
	if ( g_iShopMenuItems[id][ITEM_SLOT_ONE] > ITEM_NONE )
	{
		ITEM_Remove( id, ITEM_SLOT_ONE, false );
	}

	if ( g_iShopMenuItems[id][ITEM_SLOT_TWO] > ITEM_NONE )
	{
		ITEM_Remove( id, ITEM_SLOT_TWO, false );
	}
}

// Item Specific Functions
ITEM_Offensive( iAttacker, iVictim, iWeapon, iDamage, iHitPlace )
{

	// Claws of Attack
	if ( ITEM_Has( iAttacker, ITEM_CLAWS ) > ITEM_NONE )
	{	
		WC3_Damage( iVictim, iAttacker, get_pcvar_num( CVAR_wc3_claw ), iWeapon, iHitPlace );
		
		SHARED_Glow( iAttacker, (2 * get_pcvar_num( CVAR_wc3_claw ) ), 0, 0, 0 );

		Create_ScreenFade( iVictim, (1<<10), (1<<10), (1<<12), 255, 0, 0, g_GlowLevel[iVictim][0] );
	}

	// Mask of Death
	if ( ITEM_Has( iAttacker, ITEM_MASK ) > ITEM_NONE && p_data_b[iAttacker][PB_ISCONNECTED])
	{
		new iHealth = get_user_health( iAttacker );
		new iBonusHealth = floatround( float( iDamage ) * get_pcvar_float( CVAR_wc3_mask ) );
		
		new iVampiricBonus = p_data_b[iAttacker][PB_HEXED] ? 0 : SM_GetSkillLevel( iAttacker, SKILL_VAMPIRICAURA );

		// Then the user already gets a bonus, lets lower the total amount the user is going to get
		if ( iVampiricBonus > 0 )
		{
			iBonusHealth /= iVampiricBonus;
		}
		
		// User needs to be set to max health
		if ( iHealth + iBonusHealth > get_user_maxhealth( iAttacker ) )
		{
			set_pev( iAttacker, pev_dmg_inflictor, 0 );
			set_user_health( iAttacker, get_user_maxhealth( iAttacker ) );
		}
		
		// Give them bonus
		else
		{
			set_pev( iAttacker, pev_dmg_inflictor, 0 );
			set_user_health( iAttacker, iHealth + iBonusHealth );
		}

		SHARED_Glow( iAttacker, 0, iBonusHealth, 0, 0 );

		Create_ScreenFade( iAttacker, (1<<10), (1<<10), (1<<12), 0, 255, 0, g_GlowLevel[iAttacker][1] );
	}

	// Orb of Frost
	if ( ITEM_Has( iAttacker, ITEM_FROST ) > ITEM_NONE )
	{
		// Only slow them if they aren't slowed/stunned already
		if ( !SHARED_IsPlayerSlowed( iVictim ) )
		{

			p_data_b[iVictim][PB_SLOWED]	= true;

			SHARED_SetSpeed( iVictim );

			set_task( 1.0, "SHARED_ResetMaxSpeed", TASK_RESETSPEED + iVictim );

			SHARED_Glow( iAttacker, 0, 0, 0, 100 );

			Create_ScreenFade( iVictim, (1<<10), (1<<10), (1<<12), 255, 255, 255, g_GlowLevel[iVictim][3] );
		}
	}
	
	//Steal money
	if ( ITEM_doesHave( iAttacker, ITEM_STEALCASH ) && p_data_b[iAttacker][PB_ISCONNECTED])
	{
		new iMoneyToTake = get_pcvar_num( CVAR_wc3_cashsteal );
		// Remove the money from the victim
		SHARED_SetUserMoney( iVictim, SHARED_GetUserMoney( iVictim ) - iMoneyToTake, 1 );
		
		// Give the money to the attacker
		SHARED_SetUserMoney( iAttacker, SHARED_GetUserMoney( iAttacker ) + iMoneyToTake, 1 );

		// Make the user glow!
		SHARED_Glow( iVictim, 0, iDamage, 0, 0 );
	
		// Create a screen fade (purplish)
		Create_ScreenFade( iAttacker, (1<<10), (1<<10), (1<<12), 144, 58, 255, g_GlowLevel[iAttacker][1] );
	}
	
	//Steal armor
	if ( ITEM_doesHave( iAttacker, ITEM_STEALARMOR) && p_data_b[iAttacker][PB_ISCONNECTED])
	{
		new iArmorToTake = get_pcvar_num( CVAR_wc3_armorsteal );
		new CsArmorType:AttackerArmorType;
		new CsArmorType:VictimArmorType;
		new iAttackerCurArmor = cs_get_user_armor( iAttacker, AttackerArmorType );
		new iVictimCurArmor = cs_get_user_armor( iVictim, VictimArmorType );
		//new iMaxArmor = SHARED_GetMaxArmor( iAttacker );
		
		if (iVictimCurArmor < iArmorToTake)
		{
			iArmorToTake = iVictimCurArmor;
		}
		
		if (iArmorToTake >0)
		{
			iAttackerCurArmor += iArmorToTake;
			if ( iAttackerCurArmor >= 200 )
			{
				iAttackerCurArmor = 200;
			}
			cs_set_user_armor( iAttacker, iAttackerCurArmor, AttackerArmorType );
			
			cs_set_user_armor( iVictim, iVictimCurArmor - iArmorToTake , VictimArmorType );
		}
		
	}
	
	//One Hit
	if ( ITEM_doesHave( iAttacker, ITEM_ONEHIT ) && p_data_b[iAttacker][PB_ISCONNECTED])
	{
		
		if (ITEM_doesHave(iVictim, ITEM_NECKLACE))
		{
			//do block stuff
			ITEM_RemoveCharge( iVictim, ITEM_NECKLACE );
		}
		else
		{
			set_user_health(iVictim,1);
			WC3_Damage( iVictim, iAttacker, 300, iWeapon, iHitPlace );
			ITEM_RemoveCharge( iAttacker, ITEM_ONEHIT );
		}
	}
	
	//Slap
	if (ITEM_doesHave( iAttacker, ITEM_SLAP) && p_data_b[iAttacker][PB_ISCONNECTED])
	{
		
			 user_slap ( iVictim, 0);
			 //WC3_Damage( iVictim, iAttacker, 10, iWeapon, iHitPlace );

		ITEM_RemoveCharge( iAttacker, ITEM_SLAP );
	}
	
	// Sharp Bullets
	if ( ITEM_Has( iAttacker, ITEM_SHARPBULLETS ) > ITEM_NONE )
	{	
		WC3_Damage( iVictim, iAttacker, get_pcvar_num( CVAR_wc3_sharpbullets ), iWeapon, iHitPlace );
		
		SHARED_Glow( iAttacker, (2 * get_pcvar_num( CVAR_wc3_sharpbullets ) ), 0, 0, 0 );

		Create_ScreenFade( iVictim, (1<<10), (1<<10), (1<<12), 255, 0, 0, g_GlowLevel[iVictim][0] );
	}
	
}

ITEM_Defensive( iAttacker, iVictim, iWeapon, iDamage, iHitPlace )
{
	//mirror
	if ( ITEM_doesHave( iVictim, ITEM_MIRROR) && p_data_b[iVictim][PB_ISCONNECTED] && is_user_alive( iAttacker ))
	{
		static iAdditionalDamage;
		iAdditionalDamage = floatround( float( iDamage ) * 0.10 );
		
		// Damage the user
		WC3_Damage( iAttacker, iVictim, iAdditionalDamage, CSW_THORNS, iHitPlace );

		// Make the user glow!
		SHARED_Glow( iAttacker, ( 3 * iAdditionalDamage ), 0, 0, 0 );
		
		// Create a screen fade
		Create_ScreenFade( iAttacker, (1<<10), (1<<10), (1<<12), 0, 0, 255, iAdditionalDamage );
		
		//to clear warning
		iWeapon = iWeapon + 0
	}
}

ITEM_Tome( id )
{
	new iXp = get_pcvar_num( CVAR_wc3_tome ) + XP_GivenByLevel( p_data[id][P_LEVEL] );
		
	if ( g_MOD == GAME_DOD )
	{
		iXp *= 2;
	}

	new iBonusXP = XP_Give( id, iXp );

	if ( iBonusXP != 0 )
	{
		client_print( id, print_chat, "%s %L", g_MODclient, id, "INFO_SHOPMENU_9", iBonusXP );
	}

	emit_sound( id, CHAN_STATIC, "warcraft3/Tomes.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );

	return;
}

ITEM_Gloves( id )
{
	if ( !WC3_Check() )
	{
		return;
	}

	if ( !SHARED_HasGrenade( id ) )
	{
		g_iGloveTimer[id] = 0;

		_ITEM_Glove_Give( id );
	}

	return;
}

ITEM_Glove_Begin( id )
{
	// Then lets start a timer to give them a grenade!
	g_iGloveTimer[id] = get_pcvar_num( CVAR_wc3_glove_timer );

	WC3_StatusText( id, TXT_TIMER, "%d second(s) until your next grenade", g_iGloveTimer[id] );

	g_iGloveTimer[id]--;

	set_task( 1.0, "_ITEM_Glove_Give", TASK_ITEM_GLOVES + id );
}

public _ITEM_Glove_Give( id )
{
	if ( !WC3_Check() )
	{
		return;
	}

	if ( id >= TASK_ITEM_GLOVES )
	{
		id -= TASK_ITEM_GLOVES;
	}

	if ( !p_data_b[id][PB_ISCONNECTED] || !is_user_alive( id ) )
	{
		return;
	}

	// Only need to save once! - this is b/c I'm not sure when the loss of a grenade is updated - and I wanted SHARED_HasGrenade to work @ all times!
	if ( g_iGloveTimer[id] == get_pcvar_num( CVAR_wc3_glove_timer ) - 1 )
	{
		SHARED_SaveWeapons( id );
	}

	// If somehow they already got a grenade - stop this!
	/*new bool:bHasGrenade = false;
	if ( g_MOD == GAME_CSTRIKE || g_MOD == GAME_CZERO )
	{
		if ( cs_get_user_bpammo( id, CSW_HEGRENADE ) > 0 )
		{
			bHasGrenade = true;
		}
	}

	// Lets not check in DOD b/c I *believe* you can have 2 grenades with a certain class
	else if ( g_MOD == GAME_DOD )
	{
		if ( dod_get_user_ammo( id, DODW_HANDGRENADE ) > 0 || dod_get_user_ammo( id, DODW_STICKGRENADE ) > 0 )
		{
			bHasGrenade = true;
		}
	}*/

	// Lets do CS/CZ only
	if ( g_MOD == GAME_CSTRIKE || g_MOD == GAME_CZERO )
	{
		// Already have a grenade!!
		if ( SHARED_HasGrenade( id ) )
		{
			g_iGloveTimer[id] = 0;

			return;
		}
	}


	if ( g_iGloveTimer[id] > 0 )
	{
		WC3_StatusText( id, TXT_TIMER, "%d second(s) until your next grenade", g_iGloveTimer[id] );

		g_iGloveTimer[id]--;

		set_task( 1.0, "_ITEM_Glove_Give", TASK_ITEM_GLOVES + id );

		return;
	}

	// Counter-Strike or Condition Zero grenade
	if ( g_MOD == GAME_CSTRIKE || g_MOD == GAME_CZERO )
	{
		give_item( id, "weapon_hegrenade" );
	}

	// Day of Defeat
	else if ( g_MOD == GAME_DOD )
	{
		if ( get_user_team( id ) == ALLIES )
		{
			give_item( id, "weapon_handgrenade" );
		}
		else
		{
			give_item( id, "weapon_stickgrenade" );
		}
	}

	// Display a message to the user
	WC3_StatusText( id, TXT_TIMER, "%L", id, "ENJOY_A_GRENADE" )

	return;
}		

ITEM_BuyRings( id )
{

	new iItemSlot = ITEM_GetSlot( id );

	// Items are full
	if ( iItemSlot == ITEM_SLOT_FULL && ITEM_Has( id, ITEM_RING ) == ITEM_NONE )
	{
		g_iFutureItem[id] = -3;

		MENU_ReplaceItem( id );

		return;
	}

	new iMoney;
	new iAdditionalRings = 0;
	
	while ( g_iTotalRings[id] + iAdditionalRings < 5 )
	{
		iMoney = SHARED_GetUserMoney( id );

		if ( iMoney < ITEM_Cost( id, ITEM_RING ) )
		{
			break;
		}

		iAdditionalRings++;
		
		new iNewMoney = iMoney - ITEM_Cost( id, ITEM_RING );
		SHARED_SetUserMoney( id, iNewMoney, 1 );
	}

	// Then we need to give them some rings!
	if ( iAdditionalRings > 0 )
	{

		// Subtract 1 b/c ITEM_GiveItem will add one
		g_iTotalRings[id] += ( ( iAdditionalRings * RING_INCREMENT ) - ( RING_INCREMENT ) );

		ITEM_GiveItem( id, ITEM_RING );
	}

	return;
}

public _ITEM_Ring( id )
{
	if ( !WC3_Check() )
	{
		return;
	}

	if ( id >= TASK_ITEM_RING )
	{
		id -= TASK_ITEM_RING;
	}

	if ( !p_data_b[id][PB_ISCONNECTED] || ITEM_Has( id, ITEM_RING ) == ITEM_NONE )
	{
		return;
	}

	new iBonusHealth = g_iTotalRings[id];

	while ( iBonusHealth > 0 )
	{
		new iHealth =  get_user_health( id ) + 1;

		if ( iHealth <= get_user_maxhealth( id ) )
		{
			set_pev( id, pev_dmg_inflictor, 0 );
			set_user_health( id, iHealth );
		}

		iBonusHealth--;
	}

	set_task( 2.0, "_ITEM_Ring", TASK_ITEM_RING + id );

	return;
}

ITEM_Scroll( id )
{
	// Make sure the user isn't about to respawn when we do these checks
	if ( !p_data[id][P_RESPAWNBY] )
	{
		p_data[id][P_RESPAWNBY] = RESPAWN_ITEM;

		set_task( SPAWN_DELAY, "_SHARED_Spawn", TASK_SPAWN + id );
	}
}

ITEM_CheckFlag( iItemID, iFlag )
{
	if ( g_iFlag[iItemID] & iFlag )
	{
		return true;
	}

	return false;	
}

// Format the item for WC3_ShowBar
ITEM_Format( id, iItem, szItemString[], iLen )
{
	new szItemName[32];
	LANG_GetItemName( iItem, id, szItemName, 31, true );

	// Special options
	if ( iItem == ITEM_NECKLACE )
	{
		formatex( szItemString, iLen, "%s[%d]", szItemName, g_iNecklaceCharges[id] );
	}

	else if ( iItem == ITEM_HELM )
	{
		formatex( szItemString, iLen, "%s[%d]", szItemName, g_iHelmCharges[id] );
	}

	else if ( iItem == ITEM_RING )
	{
		formatex( szItemString, iLen, "%s[%d]", szItemName, g_iTotalRings[id] );
	}
	
	//
	
	else if ( iItem == ITEM_SLAP )
	{
		formatex( szItemString, iLen, "%s[%d]", szItemName, g_iSlapCharges[id] );
	}
	
	// All other cases
	else
	{
		copy( szItemString, iLen, szItemName );
	}
}

//New Long Jump
/*
public ITEM_PlayerJump(id)
{
	if( !is_user_alive(id) )
	{
		return HAM_IGNORED
	}
	
	new flags = entity_get_int(id, EV_INT_flags)
	
	//are they on the ground, don't want someone jumping in mid air!
	if (flags&FL_ONGROUND) 
	{

		new Float:fOldVelocity[3], Float:fNewVelocity[3]
		new toadXtraUp = 0

		entity_get_vector(id, EV_VEC_velocity, fOldVelocity)


		fNewVelocity[0] = fOldVelocity[0] / 2 * 4
		fNewVelocity[1] = fOldVelocity[1] / 2 * 4
		fNewVelocity[2] = (4 * (100.0 + toadXtraUp))

		//make them jump now.
		entity_set_vector(id, EV_VEC_velocity, fNewVelocity)

		//make some sound when he jumps
		emit_sound(id,4,"player/pl_jump1.wav",1.0,0.8,0,100)
		return HAM_SUPERCEDE;
	}
	return HAM_IGNORED
}
public ITEM_PlayerDuck(id)
{
	
}
*/

public ITEM_Forward_CmdStart(id, uc_handle, seed) 
{ 
    if (g_bPlayerHasLongJump[id])
	{
		static Button, oldButton;
		Button = get_uc(uc_handle, UC_Buttons);
		oldButton = pev(id, pev_oldbuttons);
		
		//if(Button & IN_JUMP && !(oldButton & IN_JUMP) && pev(id,pev_flags) & FL_ONGROUND)
		if(Button & IN_JUMP && (Button & IN_DUCK) && !(oldButton & IN_JUMP) && pev(id,pev_flags) & FL_ONGROUND)
		{
			new Float: Angle[3];
			new Float: Out[3];
			
			entity_get_vector(id,EV_VEC_angles,Angle)
			angle_vector(Angle,ANGLEVECTOR_FORWARD,Out);
			
			//new Float: velocity[3];
			//Out[0]=Out[0]*4000;
			Out[0]=Out[0]*get_pcvar_num( CVAR_wc3_longjumpvel1 );
			Out[1]=Out[1]*get_pcvar_num( CVAR_wc3_longjumpvel2 );
			Out[2]=Out[2] + get_pcvar_num( CVAR_wc3_longjumpheight );
			
			//get_user_velocity(id,velocity);
			//velocity[2]=400.0;
			//elocity[0]=700.0;
			//velocity_by_aim(id, 800, velocity);
			set_pev(id,pev_velocity,Out);
			//set_user_velocity(id,velocity);
		}
	}
}  
