# MCES – Modern Common Economy & Shop

> 🧩 **Workshop:** https://reforger.armaplatform.com/workshop/AE9A7FAB88A56E07 · **Docs:** https://victoryil.github.io/MCES/ · **Código:** https://github.com/victoryil/MCES

> 📖 **Documentación completa** (para personas y para IA): carpeta [`docs/`](docs/index.md), publicable como GitHub Pages. Resumen para modelos de lenguaje: [`llms.txt`](llms.txt). Asistentes de IA: empezad por [`docs/guia-ia.md`](docs/guia-ia.md).


Tienda configurable para misiones de Arma Reforger. La ventana tiene tres partes:
- pestañas por categoría con icono;
- tarjetas con vista 3D del objeto;
- ficha con precio, descripción y estado.

La compra se valida en el servidor y la red ya está hecha.

Depende de MCUIK. GUID: `AE9A7FAB88A56E07`.

## Uso rápido: catálogo en un `.conf`

1. Crea un `MCES_ShopConfig` en Workbench (clic derecho → crear config) con:
   - título;
   - moneda;
   - categorías (`m_sId`, `m_sName`, `m_sIcon`);
   - artículos (`m_sId`, `m_sCategory`, `m_sName`, `m_sDesc`, `m_sPrefab`, `m_iPrice`).
2. Al iniciar la partida (en servidor y clientes):

   ```c
   MCES_Shop.RegisterConfig("armeria", "{GUID}Configs/MiTienda.conf");
   MCES_Shop.SetProvider(new MiProveedor());
   ```

3. Para abrirla (en el cliente, desde una acción de usuario o un menú): `MCES_Shop.Open("armeria", vendedor);`

## Tu economía: el proveedor

MCES no decide cuánto dinero hay ni cómo se entrega: eso lo pone tu misión heredando `MCES_ShopProvider`.

| Método | Dónde corre | Qué hace |
|---|---|---|
| `GetBalance()` | cliente | Saldo del jugador local (para mostrarlo y bloquear lo que no llega). |
| `FormatMoney(n)` | cliente | Cómo se escribe el dinero (por defecto `12.500 €`). |
| `GetCatalog(shop, cats, items)` | cliente | Catálogo. Por defecto, el `.conf` registrado. Puedes generarlo por script y marcar `m_bLocked` con `m_sLockReason`. |
| `GetExtras / OnExtra` | cliente | Acciones que no son comprar (vender, informes, admin…). |
| `Buy(pid, player, shop, item, source, out msg)` | **servidor** | Cobra y entrega. Devuelve si salió bien y el mensaje para el jugador. |
| `RequestBuy(shop, item, source)` | cliente | Por defecto va por la red de MCES. Puedes usar la tuya. |
| `OnResult(ok, msg)` | cliente | Respuesta del servidor (por defecto, un aviso de MCUIK). |

La red usa un `modded class SCR_PlayerControllerGroupComponent`, que todo jugador ya tiene. No hace falta tocar prefabs.

## Ejemplo de proveedor

```c
class MiProveedor : MCES_ShopProvider
{
    override int GetBalance() { return MiEconomia.Dinero(); }

    override bool Buy(int pid, IEntity player, string shop, string item, IEntity source, out string msg)
    {
        MCES_ShopConfig cfg = MCES_Shop.GetConfig(shop);
        // busca el artículo, comprueba el saldo, cobra y entrega el prefab...
        msg = "Comprado.";
        return true;
    }
}
```

Reconquista Canaria lo usa así en `RC_ShopProvider.c`: el catálogo viene de su `RC_Catalog`, lo que se desbloquea depende de la historia y la compra va por su `RC_Requests`.
