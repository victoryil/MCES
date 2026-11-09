---
title: Proveedor
nav_order: 4
---

# El proveedor: `MCES_ShopProvider`

Hereda de él y registra **una** instancia con `MCES_Shop.SetProvider(new MiProveedor())`, en servidor **y** clientes, al iniciar la partida. Un solo proveedor puede atender varias tiendas: distínguelas con `shopId`.

| Método | Dónde corre | Por defecto | Qué haces tú |
|---|---|---|---|
| `GetTitle(shop)` | cliente | `m_sTitle` del `.conf` | Título de la ventana; MCES le añade «· tienes X». |
| `GetBalance()` | cliente | `0` | Saldo **del jugador local**. Tiene que estar replicado a los clientes en tu misión. |
| `FormatMoney(n)` | cliente | `12.500 €` | Otra moneda: `return n.ToString() + " créditos";`. |
| `GetCatalog(shop, cats, items)` | cliente | el `.conf` registrado | Ver [Catálogo](catalogo.md). |
| `GetExtras(shop, extras)` / `OnExtra(shop, id, source)` | cliente | nada | Acciones extra. |
| `RequestBuy(shop, item, source)` | cliente | red de MCES | Normalmente no lo toques. |
| **`Buy(pid, player, shop, item, source, out msg)`** | **servidor** | rechaza | **Valida, cobra y entrega.** Devuelve `true` si ha comprado y en `msg` lo que verá el jugador. |
| `OnResult(ok, msg)` | cliente | aviso de MCUIK (verde o rojo) | Déjalo vacío si tu misión ya avisa por su cuenta. |

## Seguridad

- **Valida todo en `Buy`.** Comprueba que el artículo existe, que no está bloqueado, el precio (el del servidor, no uno que mande el cliente) y que el saldo alcanza.
- `source` es la entidad del vendedor si se pasó al abrir. Úsala para comprobar que el jugador está cerca: `vector.Distance(player.GetOrigin(), source.GetOrigin()) < 10`.
- `pid` es el id del jugador que pide, ya resuelto por MCES desde su `PlayerController`. No viene del cliente.

## Entregar

MCES no entrega nada: lo hace tu `Buy`. Formas habituales:

```c
// al inventario del jugador (si no cabe, al suelo)
SCR_InventoryStorageManagerComponent inv = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
if (!inv || !inv.TrySpawnPrefabToStorage(it.m_sPrefab))
{
    EntitySpawnParams p = new EntitySpawnParams();
    p.TransformMode = ETransformMode.WORLD;
    p.Transform[3] = player.GetOrigin() + "0 0.2 0";
    GetGame().SpawnEntityPrefab(Resource.Load(it.m_sPrefab), GetGame().GetWorld(), p);
}

// un vehículo: en un punto de entrega libre cerca del vendedor
```

## Ejemplo real

Reconquista Canaria (`RC_ShopProvider.c`):
- **Catálogo:** sale de su `RC_Catalog`, con descripciones, dónde se entrega y si es ilegal llevarlo.
- **Bloqueos:** según el paso de la historia.
- **Saldo:** los euros de la compañía (replicados).
- **Compra:** llama a su `RC_Requests.HandleFor(…BUY…)`, que cobra, entrega y avisa; devuelve `true` si el dinero ha bajado.
- **`OnResult`:** vacío, porque la misión ya manda sus propios avisos.
