// MCES - Modern Common Economy & Shop: datos de la tienda.
// Una tienda tiene categorias y articulos. Se pueden escribir en un .conf (MCES_ShopConfig) o darlos por script
// desde el proveedor de la mision (MCES_ShopProvider.GetCatalog).

[BaseContainerProps()]
class MCES_Category
{
	[Attribute("", desc: "Identificador de la categoria (lo usan los articulos)")]
	string m_sId;

	[Attribute("", desc: "Nombre visible")]
	string m_sName;

	[Attribute("star", desc: "Icono del atlas de MCUIK (money, weapon, ammo, medical, vehicle, clothing, tool...)")]
	string m_sIcon;

	static MCES_Category Make(string id, string name, string icon)
	{
		MCES_Category c = new MCES_Category();
		c.m_sId = id;
		c.m_sName = name;
		c.m_sIcon = icon;
		return c;
	}
}

[BaseContainerProps()]
class MCES_Item
{
	[Attribute("", desc: "Identificador unico (es lo que viaja por red al comprar)")]
	string m_sId;

	[Attribute("", desc: "Categoria (m_sId de una MCES_Category)")]
	string m_sCategory;

	[Attribute("", desc: "Nombre visible")]
	string m_sName;

	[Attribute("", UIWidgets.EditBoxMultiline, desc: "Descripcion de la ficha (admite <b> y <color>)")]
	string m_sDesc;

	[Attribute("", UIWidgets.ResourcePickerThumbnail, desc: "Prefab que se entrega y se ve en la vista previa", params: "et")]
	ResourceName m_sPrefab;

	[Attribute("100", desc: "Precio")]
	int m_iPrice;

	[Attribute("", desc: "Icono si no hay prefab (servicios, etc.)")]
	string m_sIcon;

	[Attribute("0", desc: "Vista previa ancha (vehiculos)")]
	bool m_bWide;

	// lo rellena el proveedor en tiempo de juego
	bool m_bLocked;
	string m_sLockReason;

	static MCES_Item Make(string id, string category, string name, ResourceName prefab, int price)
	{
		MCES_Item it = new MCES_Item();
		it.m_sId = id;
		it.m_sCategory = category;
		it.m_sName = name;
		it.m_sPrefab = prefab;
		it.m_iPrice = price;
		return it;
	}
}

//! Accion extra en la tienda que no es comprar un articulo (vender, informe, admin...)
class MCES_Extra
{
	string m_sId;
	string m_sName;
	string m_sDesc;
	string m_sIcon;

	static MCES_Extra Make(string id, string name, string desc, string icon)
	{
		MCES_Extra e = new MCES_Extra();
		e.m_sId = id;
		e.m_sName = name;
		e.m_sDesc = desc;
		e.m_sIcon = icon;
		return e;
	}
}

[BaseContainerProps(configRoot: true)]
class MCES_ShopConfig
{
	[Attribute("Tienda", desc: "Titulo de la ventana")]
	string m_sTitle;

	[Attribute("€", desc: "Simbolo de la moneda")]
	string m_sCurrency;

	[Attribute()]
	ref array<ref MCES_Category> m_aCategories;

	[Attribute()]
	ref array<ref MCES_Item> m_aItems;
}
