---
title: Referencia de la API
nav_order: 90
---

# MCES · Referencia de la API

> Generado automáticamente desde el código (`tools/gen_api_docs.py`). No editar a mano.

## MCES_Data.c

> MCES - Modern Common Economy & Shop: datos de la tienda.
> Una tienda tiene categorias y articulos. Se pueden escribir en un .conf (MCES_ShopConfig) o darlos por script
> desde el proveedor de la mision (MCES_ShopProvider.GetCatalog).

### `MCES_Category`

- `static MCES_Category Make(string id, string name, string icon)`

### `MCES_Item`

- `static MCES_Item Make(string id, string category, string name, ResourceName prefab, int price)`

### `MCES_Extra`

Accion extra en la tienda que no es comprar un articulo (vender, informe, admin...)

- `static MCES_Extra Make(string id, string name, string desc, string icon)`

### `MCES_ShopConfig`


## MCES_Net.c

> MCES - red: el cliente pide comprar y el servidor responde solo a ese jugador.
> Va por un componente que todo jugador tiene en su PlayerController (SCR_PlayerControllerGroupComponent).

### `SCR_PlayerControllerGroupComponent` *(modded)*

- `void MCES_AskBuy(string shopId, string itemId, RplId source)`
- `void MCES_SendResult(bool ok, string msg)`

### `MCES_Net`

- `static void RequestBuy(string shopId, string itemId, IEntity source)`
- `static RplId ToRplId(IEntity e)`
- `static IEntity FromRplId(RplId id)`

## MCES_Res.c

> Generado por tools/build.py: recursos de MCES (no editar a mano)

### `MCES_Res`

| Constante | Tipo | Valor |
|---|---|---|
| `C_DIALOGS` | `ResourceName` | `"{582F49B543FA0B35}Configs/MCES/MCES_Dialogs.conf"` |
| `L_CARD` | `ResourceName` | `"{3AB66D86FC4CD728}UI/layouts/MCES/MCES_Card.layout"` |
| `L_SHOPCONTENT` | `ResourceName` | `"{388300F37F6657E0}UI/layouts/MCES/MCES_ShopContent.layout"` |
| `L_TAB` | `ResourceName` | `"{47F993DDD19269C0}UI/layouts/MCES/MCES_Tab.layout"` |


## MCES_Shop.c

> MCES - punto de entrada y proveedor.
> Como usarlo en tu mision:
> 1) Hereda MCES_ShopProvider y sobrescribe lo que necesites (saldo, catalogo, compra en el servidor).
> 2) Registralo al iniciar (en servidor y clientes): MCES_Shop.SetProvider(new MiProveedor());
> 3) Abre la tienda en el cliente: MCES_Shop.Open("armeria", entidadDelVendedor);
> Sin proveedor propio se usa el basico: lee el catalogo de un MCES_ShopConfig y no cobra (solo para probar).

### `MCES_ShopProvider`

- `void GetCatalog(string shopId, notnull array<ref MCES_Category> cats, notnull array<ref MCES_Item> items)`
  — Catalogo de una tienda (cliente). Por defecto, el .conf registrado con MCES_Shop.RegisterConfig
- `void GetExtras(string shopId, notnull array<ref MCES_Extra> extras)`
  — Otras acciones de la tienda (cliente)
- `void OnExtra(string shopId, string extraId, IEntity source)`
  — Se pulsa una accion extra (cliente)
- `string GetTitle(string shopId)`
  — Titulo de la ventana (cliente)
- `int GetBalance()`
  — Saldo del jugador local para mostrar y comprobar (cliente)
- `string FormatMoney(int amount)`
- `bool Buy(int playerId, IEntity player, string shopId, string itemId, IEntity source, out string message)`
  — Compra en el SERVIDOR: cobra y entrega. Devuelve si salio bien y el mensaje para el jugador.
- `void RequestBuy(string shopId, string itemId, IEntity source)`
  — Pide la compra (cliente). Por defecto va por la red de MCES; una mision puede usar la suya.
- `void OnResult(bool ok, string message)`
  — Respuesta del servidor a una compra (cliente)

### `MCES_Shop`

- `static void SetProvider(MCES_ShopProvider provider)`
- `static MCES_ShopProvider GetProvider()`
- `static bool RegisterConfig(string shopId, ResourceName conf)`
  — Asocia un .conf de tienda a un identificador
- `static MCES_ShopConfig GetConfig(string shopId)`
- `static MCES_ShopDialog Open(string shopId, IEntity source = null)`
  — Abre la ventana de la tienda (cliente)

## MCES_ShopDialog.c

> MCES - ventana de la tienda:
> pestanas por categoria (izquierda) · tarjetas con vista previa 3D (centro) · ficha y boton Comprar (derecha)

### `MCES_ShopDialog` : `SCR_ConfigurableDialogUi`

| Constante | Tipo | Valor |
|---|---|---|
| `COLS` | `int` | `3` |

- `static int LastCardCount()`
- `static int LastTabCount()`
- `static MCES_ShopDialog GetOpen()`
- `static void RefreshOpen()`
  — Vuelve a pintar la ventana abierta (saldo nuevo tras una compra)
- `void Setup(string shopId, IEntity source)`
- `void SelectCategory(string id)`
- `void Select(string id)`
  — Selecciona un articulo por codigo (autotest)
- `void Refresh()`
- `override void OnMenuClose()`

