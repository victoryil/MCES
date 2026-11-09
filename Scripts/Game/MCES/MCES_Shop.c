// MCES - punto de entrada y proveedor.
//
// Como usarlo en tu mision:
//   1) Hereda MCES_ShopProvider y sobrescribe lo que necesites (saldo, catalogo, compra en el servidor).
//   2) Registralo al iniciar (en servidor y clientes): MCES_Shop.SetProvider(new MiProveedor());
//   3) Abre la tienda en el cliente: MCES_Shop.Open("armeria", entidadDelVendedor);
// Sin proveedor propio se usa el basico: lee el catalogo de un MCES_ShopConfig y no cobra (solo para probar).

class MCES_ShopProvider
{
	//! Catalogo de una tienda (cliente). Por defecto, el .conf registrado con MCES_Shop.RegisterConfig
	void GetCatalog(string shopId, notnull array<ref MCES_Category> cats, notnull array<ref MCES_Item> items)
	{
		MCES_ShopConfig cfg = MCES_Shop.GetConfig(shopId);
		if (!cfg)
			return;
		foreach (MCES_Category c : cfg.m_aCategories)
			cats.Insert(c);
		foreach (MCES_Item it : cfg.m_aItems)
			items.Insert(it);
	}

	//! Otras acciones de la tienda (cliente)
	void GetExtras(string shopId, notnull array<ref MCES_Extra> extras)
	{
	}

	//! Se pulsa una accion extra (cliente)
	void OnExtra(string shopId, string extraId, IEntity source)
	{
	}

	//! Titulo de la ventana (cliente)
	string GetTitle(string shopId)
	{
		MCES_ShopConfig cfg = MCES_Shop.GetConfig(shopId);
		if (cfg)
			return cfg.m_sTitle;
		return "Tienda";
	}

	//! Saldo del jugador local para mostrar y comprobar (cliente)
	int GetBalance()
	{
		return 0;
	}

	string FormatMoney(int amount)
	{
		return MCUIK.Thousands(amount) + " €";
	}

	//! Compra en el SERVIDOR: cobra y entrega. Devuelve si salio bien y el mensaje para el jugador.
	bool Buy(int playerId, IEntity player, string shopId, string itemId, IEntity source, out string message)
	{
		message = "Esta tienda no tiene proveedor: la mision debe registrar uno (MCES_Shop.SetProvider).";
		return false;
	}

	//! Pide la compra (cliente). Por defecto va por la red de MCES; una mision puede usar la suya.
	void RequestBuy(string shopId, string itemId, IEntity source)
	{
		MCES_Net.RequestBuy(shopId, itemId, source);
	}

	//! Respuesta del servidor a una compra (cliente)
	void OnResult(bool ok, string message)
	{
		if (message.IsEmpty())
			return;
		Color c = MCUIK_Theme.Success();
		string icon = "check";
		if (!ok)
		{
			c = MCUIK_Theme.Danger();
			icon = "close";
		}
		MCUIK_Notify.Toast(icon, "Tienda", message, c);
	}
}

class MCES_Shop
{
	protected static ref MCES_ShopProvider s_Provider;
	protected static ref map<string, ref MCES_ShopConfig> s_mConfigs = new map<string, ref MCES_ShopConfig>();

	static void SetProvider(MCES_ShopProvider provider)
	{
		s_Provider = provider;
	}

	static MCES_ShopProvider GetProvider()
	{
		if (!s_Provider)
			s_Provider = new MCES_ShopProvider();
		return s_Provider;
	}

	//! Asocia un .conf de tienda a un identificador
	static bool RegisterConfig(string shopId, ResourceName conf)
	{
		Resource res = BaseContainerTools.LoadContainer(conf);
		if (!res || !res.IsValid())
			return false;
		MCES_ShopConfig cfg = MCES_ShopConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(res.GetResource().ToBaseContainer()));
		if (!cfg)
			return false;
		s_mConfigs.Set(shopId, cfg);
		return true;
	}

	static MCES_ShopConfig GetConfig(string shopId)
	{
		MCES_ShopConfig cfg;
		s_mConfigs.Find(shopId, cfg);
		return cfg;
	}

	//! Abre la ventana de la tienda (cliente)
	static MCES_ShopDialog Open(string shopId, IEntity source = null)
	{
		MCES_ShopDialog dlg = new MCES_ShopDialog();
		SCR_ConfigurableDialogUi.CreateFromPreset(MCES_Res.C_DIALOGS, "mces_shop", dlg);
		dlg.Setup(shopId, source);
		return dlg;
	}
}
