// MCES - red: el cliente pide comprar y el servidor responde solo a ese jugador.
// Va por un componente que todo jugador tiene en su PlayerController (SCR_PlayerControllerGroupComponent).

modded class SCR_PlayerControllerGroupComponent
{
	void MCES_AskBuy(string shopId, string itemId, RplId source)
	{
		if (Replication.IsServer())
		{
			MCES_RpcAskBuy(shopId, itemId, source);
			return;
		}
		Rpc(MCES_RpcAskBuy, shopId, itemId, source);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void MCES_RpcAskBuy(string shopId, string itemId, RplId source)
	{
		PlayerController pc = PlayerController.Cast(GetOwner());
		if (!pc)
			return;
		int pid = pc.GetPlayerId();
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(pid);
		string msg;
		bool ok = MCES_Shop.GetProvider().Buy(pid, player, shopId, itemId, MCES_Net.FromRplId(source), msg);
		MCES_SendResult(ok, msg);
	}

	void MCES_SendResult(bool ok, string msg)
	{
		PlayerController pc = PlayerController.Cast(GetOwner());
		if (pc && pc == GetGame().GetPlayerController())
		{
			MCES_RpcDoResult(ok, msg);
			return;
		}
		Rpc(MCES_RpcDoResult, ok, msg);
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void MCES_RpcDoResult(bool ok, string msg)
	{
		MCES_Shop.GetProvider().OnResult(ok, msg);
		MCES_ShopDialog.RefreshOpen();
	}
}

class MCES_Net
{
	static void RequestBuy(string shopId, string itemId, IEntity source)
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return;
		SCR_PlayerControllerGroupComponent comp = SCR_PlayerControllerGroupComponent.Cast(pc.FindComponent(SCR_PlayerControllerGroupComponent));
		if (!comp)
			return;
		comp.MCES_AskBuy(shopId, itemId, ToRplId(source));
	}

	static RplId ToRplId(IEntity e)
	{
		if (!e)
			return RplId.Invalid();
		RplComponent rpl = RplComponent.Cast(e.FindComponent(RplComponent));
		if (!rpl)
			return RplId.Invalid();
		return rpl.Id();
	}

	static IEntity FromRplId(RplId id)
	{
		if (!id.IsValid())
			return null;
		RplComponent rpl = RplComponent.Cast(Replication.FindItem(id));
		if (!rpl)
			return null;
		return rpl.GetEntity();
	}
}
