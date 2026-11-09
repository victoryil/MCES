---
title: Guía para IA
nav_order: 6
---

# Guía para un asistente de IA

Receta para montar una tienda con MCES en una misión de Arma Reforger (Enforce Script).


## Dónde está todo

- Workshop: https://reforger.armaplatform.com/workshop/AE9A7FAB88A56E07 (GUID `AE9A7FAB88A56E07`, versión publicada 1.0.0)
- Código y documentación en Markdown: https://github.com/victoryil/MCES (rama `main`, carpeta `docs/`)
- Resumen para modelos: https://github.com/victoryil/MCES/blob/main/llms.txt
- Web: https://victoryil.github.io/MCES/

## Pasos

1. **Dependencias.** Añade `"B8DFCB8639E9DA37"` (MCUIK) y `"AE9A7FAB88A56E07"` (MCES) al `addon.gproj`.
2. **Catálogo.** Elige cómo darlo:
   - **`.conf`:** crea un `MCES_ShopConfig` (formato en [Empezar](empezar.md)) y regístralo con `MCES_Shop.RegisterConfig(shopId, ruta)`.
   - **Script:** sobrescribe `GetCatalog`.

   Copia los prefabs de prefabs reales: busca rutas `{GUID}Prefabs/...et` en el proyecto o en los mods instalados. **Nunca inventes un GUID.**
3. **Proveedor.** Hereda `MCES_ShopProvider`:
   - **`GetBalance()`:** el saldo del jugador local. Tiene que ser un valor **replicado**, por ejemplo un `[RplProp()]` de tu componente del modo de juego.
   - **`Buy(...)`:** se ejecuta en el **servidor**. Valida el artículo, el bloqueo, el precio del servidor, el saldo y la distancia al vendedor; cobra, entrega y devuelve `true` con un mensaje.
4. **Registro.** Llama a `MCES_Shop.SetProvider(new MiProveedor())` en **servidor y clientes**. Un buen sitio es `OnPostInit` del componente del modo de juego, que corre en ambos.
5. **Abrir.** En el cliente, desde una `ScriptedUserAction` (`PerformAction`, solo si `pUserEntity` es el jugador local) o desde un menú: `MCES_Shop.Open(shopId, vendedor)`.
6. **Probar.**
   - Compila en servidor dedicado.
   - En un cliente: abre la tienda, `RequestBuy` por código y comprueba que el saldo baja.
   - Busca «Virtual Machine Exception» en el log.

## Reglas

| Regla | Por qué |
|---|---|
| `Buy` es la única fuente de verdad: nunca confíes en el precio ni en el saldo que ve el cliente. | Un cliente modificado podría pedir lo que quisiera. |
| El proveedor se registra en ambos lados. | Sin él en el servidor, la compra responde «Esta tienda no tiene proveedor». |
| `m_sId` de cada artículo, único y estable. | Es lo que viaja por red. |
| Los bloqueos (`m_bLocked`) son visuales: vuelve a comprobarlos en `Buy`. | Ídem. |
| Si tu misión ya avisa de las compras, deja `OnResult` vacío. | Si no, el jugador ve el aviso dos veces. |
| Las reglas de Enforce Script de la [guía de MCUIK](https://github.com/victoryil/MCUIK/blob/main/docs/guia-ia.md) también valen aquí: `Color` en variable local, palabras reservadas, `%` en `Format`. | Errores reales que ya han pasado. |

## Archivos

| Archivo | Qué es |
|---|---|
| `Scripts/Game/MCES/MCES_Data.c` | `MCES_Category`, `MCES_Item`, `MCES_Extra` y `MCES_ShopConfig`. |
| `Scripts/Game/MCES/MCES_Shop.c` | `MCES_ShopProvider` (lo que heredas) y `MCES_Shop` (registro y apertura). |
| `Scripts/Game/MCES/MCES_Net.c` | RPC de compra (`modded SCR_PlayerControllerGroupComponent`). |
| `Scripts/Game/MCES/MCES_ShopDialog.c` | La ventana. |
| `Scripts/Game/MCES/MCES_Res.c` | GUIDs de layouts y presets (generado). |
| `tools/build.py` | Genera layouts, presets, metas y `MCES_Res.c`. |
