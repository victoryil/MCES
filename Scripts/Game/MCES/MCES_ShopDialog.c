// MCES - ventana de la tienda:
//   pestanas por categoria (izquierda) · tarjetas con vista previa 3D (centro) · ficha y boton Comprar (derecha)

class MCES_ShopDialog : SCR_ConfigurableDialogUi
{
	static const int COLS = 3;
	protected static MCES_ShopDialog s_Open;
	protected static int s_iLastCards;
	protected static int s_iLastTabs;

	protected string m_sShop;
	protected IEntity m_Source;
	protected ref array<ref MCES_Category> m_aCats = {};
	protected ref array<ref MCES_Item> m_aItems = {};
	protected ref array<ref MCES_Extra> m_aExtras = {};
	protected string m_sCat;
	protected string m_sSel;          // articulo o "x:" + accion extra
	protected string m_sFirst;
	protected ref map<SCR_ModularButtonComponent, string> m_mTabIds = new map<SCR_ModularButtonComponent, string>();
	protected ref map<SCR_ModularButtonComponent, string> m_mCardIds = new map<SCR_ModularButtonComponent, string>();
	protected ref map<string, Widget> m_mTabs = new map<string, Widget>();
	protected ref map<string, Widget> m_mCards = new map<string, Widget>();

	static int LastCardCount() { return s_iLastCards; }
	static int LastTabCount() { return s_iLastTabs; }
	static MCES_ShopDialog GetOpen() { return s_Open; }

	//! Vuelve a pintar la ventana abierta (saldo nuevo tras una compra)
	static void RefreshOpen()
	{
		if (s_Open)
			s_Open.Refresh();
	}

	void Setup(string shopId, IEntity source)
	{
		s_Open = this;
		m_sShop = shopId;
		m_Source = source;
		MCES_ShopProvider p = MCES_Shop.GetProvider();
		p.GetCatalog(shopId, m_aCats, m_aItems);
		p.GetExtras(shopId, m_aExtras);
		BuildTabs();
		if (!m_aCats.IsEmpty())
			SelectCategory(m_aCats[0].m_sId);
		else if (!m_aExtras.IsEmpty())
			SelectCategory("_extras");
		Refresh();
	}

	protected Widget W(string name)
	{
		Widget root = GetContentLayoutRoot();
		if (!root)
			return null;
		return root.FindAnyWidget(name);
	}

	protected int CountIn(string cat)
	{
		int n = 0;
		foreach (MCES_Item it : m_aItems)
		{
			if (it.m_sCategory == cat)
				n++;
		}
		return n;
	}

	protected MCES_Item FindItem(string id)
	{
		foreach (MCES_Item it : m_aItems)
		{
			if (it.m_sId == id)
				return it;
		}
		return null;
	}

	protected MCES_Extra FindExtra(string id)
	{
		foreach (MCES_Extra e : m_aExtras)
		{
			if (e.m_sId == id)
				return e;
		}
		return null;
	}

	// ------------------------------------------------------------------ pestanas

	protected void BuildTabs()
	{
		Widget list = W("Tabs");
		if (!list)
			return;
		s_iLastTabs = 0;
		foreach (MCES_Category c : m_aCats)
		{
			int n = CountIn(c.m_sId);
			if (n == 0)
				continue;
			AddTab(list, c.m_sId, c.m_sName, c.m_sIcon, n.ToString());
		}
		if (!m_aExtras.IsEmpty())
			AddTab(list, "_extras", "Otros tratos", "star", m_aExtras.Count().ToString());
	}

	protected void AddTab(Widget list, string id, string name, string icon, string count)
	{
		Widget tab = MCUIK.Create(MCES_Res.L_TAB, list);
		if (!tab)
			return;
		MCUIK.SetIcon(tab, "Icon", icon, MCUIK_Theme.Muted());
		MCUIK.SetText(tab, "Label", name);
		MCUIK.SetText(tab, "Count", count);
		m_mTabs.Set(id, tab);
		SCR_ModularButtonComponent btn = SCR_ModularButtonComponent.FindComponent(tab.FindAnyWidget("TabBtn"));
		if (btn)
		{
			m_mTabIds.Set(btn, id);
			btn.m_OnClicked.Insert(OnTabClicked);
		}
		s_iLastTabs++;
	}

	protected void OnTabClicked(SCR_ModularButtonComponent comp)
	{
		string id;
		if (m_mTabIds.Find(comp, id))
			SelectCategory(id);
	}

	void SelectCategory(string id)
	{
		m_sCat = id;
		foreach (string tid, Widget tab : m_mTabs)
		{
			float bgA = 0;
			float stripA = 0;
			Color ic = MCUIK_Theme.Muted();
			if (tid == id)
			{
				bgA = 0.12;
				stripA = 1;
				ic = MCUIK_Theme.Accent();
			}
			MCUIK.SetColor(tab, "Bg", MCUIK_Theme.Accent(bgA));
			MCUIK.SetColor(tab, "Strip", MCUIK_Theme.Accent(stripA));
			MCUIK.SetColor(tab, "Icon", ic);
		}
		BuildGrid();
	}

	// ------------------------------------------------------------------ tarjetas

	protected void BuildGrid()
	{
		Widget grid = W("Grid");
		if (!grid)
			return;
		MCUIK.Clear(grid);
		m_mCardIds.Clear();
		m_mCards.Clear();
		m_sFirst = "";
		s_iLastCards = 0;
		Widget row;
		int col = 0;
		if (m_sCat == "_extras")
		{
			foreach (MCES_Extra e : m_aExtras)
			{
				if (col == 0)
					row = MCUIK.HBox(grid);
				AddCard(row, "x:" + e.m_sId, e.m_sName, "", "", e.m_sIcon, false, false);
				col = (col + 1) % COLS;
			}
		}
		else
		{
			MCES_ShopProvider p = MCES_Shop.GetProvider();
			foreach (MCES_Item it : m_aItems)
			{
				if (it.m_sCategory != m_sCat)
					continue;
				if (col == 0)
					row = MCUIK.HBox(grid);
				AddCard(row, it.m_sId, it.m_sName, p.FormatMoney(it.m_iPrice), it.m_sPrefab, it.m_sIcon, it.m_bLocked, it.m_bWide);
				col = (col + 1) % COLS;
			}
		}
		// la primera tarjeta queda seleccionada para que la ficha no salga vacia
		m_sSel = m_sFirst;
		Refresh();
	}

	protected void AddCard(Widget row, string id, string name, string price, ResourceName prefab, string icon, bool locked, bool wide)
	{
		Widget card = MCUIK.Create(MCES_Res.L_CARD, row);
		if (!card)
			return;
		MCUIK.Pad(card, 0, 0, 8, 8);
		MCUIK.SetText(card, "Name", name);
		MCUIK.SetText(card, "Price", price);
		MCUIK.SetVisible(card, "Lock", locked);
		bool shown = ShowPreview(card.FindAnyWidget("Preview"), prefab);
		MCUIK.SetVisible(card, "Icon", !shown);
		if (!shown)
		{
			if (icon.IsEmpty())
				icon = "shop";
			MCUIK.SetIcon(card, "Icon", icon, MCUIK_Theme.Accent());
		}
		if (locked)
		{
			// bloqueado: apagado (texto gris y vista previa tenue)
			MCUIK.SetColor(card, "Name", MCUIK_Theme.Muted());
			Widget pv = card.FindAnyWidget("PreviewOv");
			if (pv)
				pv.SetOpacity(0.4);
		}
		m_mCards.Set(id, card);
		if (m_sFirst.IsEmpty())
			m_sFirst = id;
		SCR_ModularButtonComponent btn = SCR_ModularButtonComponent.FindComponent(card.FindAnyWidget("CardBtn"));
		if (btn)
		{
			m_mCardIds.Set(btn, id);
			btn.m_OnClicked.Insert(OnCardClicked);
		}
		s_iLastCards++;
	}

	protected bool ShowPreview(Widget w, ResourceName prefab)
	{
		ItemPreviewWidget preview = ItemPreviewWidget.Cast(w);
		if (!preview)
			return false;
		bool shown = false;
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (world && !prefab.IsEmpty())
		{
			ItemPreviewManagerEntity mgr = world.GetItemPreviewManager();
			if (mgr)
			{
				mgr.SetPreviewItemFromPrefab(preview, prefab);
				shown = true;
			}
		}
		preview.SetVisible(shown);
		return shown;
	}

	protected void OnCardClicked(SCR_ModularButtonComponent comp)
	{
		string id;
		if (!m_mCardIds.Find(comp, id))
			return;
		m_sSel = id;
		Refresh();
	}

	//! Selecciona un articulo por codigo (autotest)
	void Select(string id)
	{
		MCES_Item it = FindItem(id);
		if (it && it.m_sCategory != m_sCat)
			SelectCategory(it.m_sCategory);
		m_sSel = id;
		Refresh();
	}

	// ------------------------------------------------------------------ ficha

	void Refresh()
	{
		MCES_ShopProvider p = MCES_Shop.GetProvider();
		int money = p.GetBalance();
		SetTitle(string.Format("%1 · tienes %2", p.GetTitle(m_sShop), p.FormatMoney(money)));

		// tarjetas: seleccionada resaltada; precio en rojo si no llega
		foreach (string cid, Widget card : m_mCards)
		{
			bool on = cid == m_sSel;
			float sa = 0;
			if (on)
				sa = 1;
			MCUIK.SetColor(card, "Strip", MCUIK_Theme.Accent(sa));
			if (on)
				MCUIK.SetColor(card, "Bg", MCUIK_Theme.Rgb(38, 46, 40, 0.95));
			else
				MCUIK.SetColor(card, "Bg", MCUIK_Theme.Rgb(18, 26, 32, 0.92));
			MCES_Item ci = FindItem(cid);
			if (ci)
			{
				Color pc = MCUIK_Theme.Accent();
				if (ci.m_bLocked)
					pc = MCUIK_Theme.Muted();
				else if (money < ci.m_iPrice)
					pc = MCUIK_Theme.Danger();
				MCUIK.SetColor(card, "Price", pc);
			}
		}

		SCR_InputButtonComponent buy = FindButton("confirm");
		Widget big = W("BigPreview");
		if (m_sSel.StartsWith("x:"))
		{
			MCES_Extra e = FindExtra(m_sSel.Substring(2, m_sSel.Length() - 2));
			if (!e)
				return;
			ShowPreview(big, "");
			MCUIK.SetVisible(GetContentLayoutRoot(), "BigIcon", true);
			MCUIK.SetIcon(GetContentLayoutRoot(), "BigIcon", e.m_sIcon, MCUIK_Theme.Accent());
			MCUIK.SetText(GetContentLayoutRoot(), "DCat", "OTROS TRATOS");
			MCUIK.SetText(GetContentLayoutRoot(), "DName", e.m_sName);
			MCUIK.SetText(GetContentLayoutRoot(), "DPrice", "");
			MCUIK.SetVisible(GetContentLayoutRoot(), "DPriceRow", false);
			MCUIK.SetText(GetContentLayoutRoot(), "DDesc", e.m_sDesc);
			MCUIK.SetText(GetContentLayoutRoot(), "DState", "");
			if (buy)
			{
				buy.SetEnabled(true);
				buy.SetLabel(e.m_sName);
			}
			return;
		}
		MCES_Item it = FindItem(m_sSel);
		if (!it)
		{
			if (buy)
				buy.SetEnabled(false);
			return;
		}
		bool shown = ShowPreview(big, it.m_sPrefab);
		MCUIK.SetVisible(GetContentLayoutRoot(), "BigIcon", !shown);
		if (!shown)
		{
			string icon = it.m_sIcon;
			if (icon.IsEmpty())
				icon = "shop";
			MCUIK.SetIcon(GetContentLayoutRoot(), "BigIcon", icon, MCUIK_Theme.Accent());
		}
		string catName;
		foreach (MCES_Category c : m_aCats)
		{
			if (c.m_sId == it.m_sCategory)
				catName = c.m_sName;
		}
		MCUIK.SetText(GetContentLayoutRoot(), "DCat", MCUIK.Upper(catName));
		MCUIK.SetText(GetContentLayoutRoot(), "DName", it.m_sName);
		MCUIK.SetVisible(GetContentLayoutRoot(), "DPriceRow", true);
		MCUIK.SetText(GetContentLayoutRoot(), "DPrice", p.FormatMoney(it.m_iPrice));
		MCUIK.SetText(GetContentLayoutRoot(), "DDesc", it.m_sDesc);
		bool canBuy = false;
		string state;
		if (it.m_bLocked)
		{
			state = MCUIK_Theme.Paint("Bloqueado", MCUIK_Theme.Muted());
			if (!it.m_sLockReason.IsEmpty())
				state += ": " + it.m_sLockReason;
		}
		else if (money < it.m_iPrice)
			state = MCUIK_Theme.Paint(string.Format("No te llega: faltan %1.", p.FormatMoney(it.m_iPrice - money)), MCUIK_Theme.Danger());
		else
		{
			state = MCUIK_Theme.Paint(string.Format("Disponible. Te quedarán %1.", p.FormatMoney(money - it.m_iPrice)), MCUIK_Theme.Success());
			canBuy = true;
		}
		MCUIK.SetText(GetContentLayoutRoot(), "DState", state);
		if (buy)
		{
			buy.SetEnabled(canBuy);
			buy.SetLabel("Comprar · " + p.FormatMoney(it.m_iPrice));
		}
	}

	//! Boton principal: compra lo seleccionado (o hace la accion extra) y deja la ventana abierta
	protected override void OnConfirm()
	{
		if (m_sSel.IsEmpty())
			return;
		MCES_ShopProvider p = MCES_Shop.GetProvider();
		if (m_sSel.StartsWith("x:"))
		{
			p.OnExtra(m_sShop, m_sSel.Substring(2, m_sSel.Length() - 2), m_Source);
			GetGame().GetCallqueue().CallLater(Refresh, 900, false);
			return;
		}
		p.RequestBuy(m_sShop, m_sSel, m_Source);
		SCR_InputButtonComponent buy = FindButton("confirm");
		if (buy)
			buy.SetEnabled(false);
		GetGame().GetCallqueue().CallLater(Refresh, 900, false);
	}

	override void OnMenuClose()
	{
		GetGame().GetCallqueue().Remove(Refresh);
		if (s_Open == this)
			s_Open = null;
		super.OnMenuClose();
	}
}
