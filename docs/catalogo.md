---
title: Catálogo
nav_order: 3
---

# Catálogo

## Categorías: `MCES_Category`

| Campo | Qué es |
|---|---|
| `m_sId` | Identificador (lo usan los artículos en `m_sCategory`). |
| `m_sName` | Nombre de la pestaña. |
| `m_sIcon` | Icono de MCUIK: `weapon`, `ammo`, `medical`, `tool`, `vehicle`, `materials`, `clothing`, `shop`… (lista completa en [MCUIK · Iconos](https://github.com/victoryil/MCUIK/blob/main/docs/iconos.md)). |

Las pestañas salen en el orden de la lista. Una categoría sin artículos no se muestra.

## Artículos: `MCES_Item`

| Campo | Qué es |
|---|---|
| `m_sId` | Identificador único. **Es lo que viaja por red al comprar.** |
| `m_sCategory` | `m_sId` de su categoría. |
| `m_sName` | Nombre en la tarjeta y la ficha. |
| `m_sDesc` | Descripción de la ficha. Admite `<b>`, `<i>`, `<br/>` y `<color rgba="r,g,b,a">`. |
| `m_sPrefab` | Prefab: se usa para la vista 3D y para entregarlo, si tu `Buy` lo usa. |
| `m_iPrice` | Precio. |
| `m_sIcon` | Icono si no hay prefab (servicios, cosas abstractas). |
| `m_bWide` | Vista previa ancha (vehículos). |
| `m_bLocked` / `m_sLockReason` | **Solo por script**: bloqueado (gris con candado) y por qué. Por ejemplo: «avanzad en la historia». |

## Catálogo por script (sin `.conf`)

Sobrescribe `GetCatalog` en tu proveedor. Así el catálogo puede cambiar según la partida:

```c
override void GetCatalog(string shop, notnull array<ref MCES_Category> cats, notnull array<ref MCES_Item> items)
{
    cats.Insert(MCES_Category.Make("armas", "Armas", "weapon"));
    MCES_Item m = MCES_Item.Make("m4", "armas", "Fusil M4", "{GUID}Prefabs/.../Rifle_M4.et", 400);
    m.m_sDesc = "Fusil de asalto.";
    if (MiHistoria.Paso() < 3)
    {
        m.m_bLocked = true;
        m.m_sLockReason = "aún no está disponible.";
    }
    items.Insert(m);
}
```

`GetCatalog` se llama cada vez que se abre la ventana, así que siempre está al día.

## Acciones extra (no son compras)

Salen en una pestaña «Otros tratos» como tarjetas. Al pulsar el botón principal se llama a `OnExtra` en el cliente; desde ahí mandas tu propia petición al servidor.

```c
override void GetExtras(string shop, notnull array<ref MCES_Extra> extras)
{
    extras.Insert(MCES_Extra.Make("vender", "Vender vehículo", "Te paga la mitad de lo que vale.", "vehicle"));
}

override void OnExtra(string shop, string extraId, IEntity source)
{
    if (extraId == "vender")
        MiRed.PedirVenta(source);
}
```
