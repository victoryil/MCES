---
title: Empezar
nav_order: 2
---

# Empezar

## 1. Dependencias

En el `addon.gproj` de tu misión:

```
Dependencies {
 "58D0FB3206B6F859"
 "B8DFCB8639E9DA37"
 "AE9A7FAB88A56E07"
}
```

## 2. El catálogo en un `.conf`

En Workbench: Resource Browser → clic derecho → *Create → Config*, clase `MCES_ShopConfig`. Si lo escribes a mano, el formato es este:

```
MCES_ShopConfig {
 m_sTitle "Armería del cuartel"
 m_sCurrency "€"
 m_aCategories {
  MCES_Category "{1A2B3C4D5E6F7001}" {
   m_sId "armas"
   m_sName "Armas"
   m_sIcon "weapon"
  }
  MCES_Category "{1A2B3C4D5E6F7002}" {
   m_sId "medico"
   m_sName "Médico"
   m_sIcon "medical"
  }
 }
 m_aItems {
  MCES_Item "{1A2B3C4D5E6F7101}" {
   m_sId "m4"
   m_sCategory "armas"
   m_sName "Fusil M4"
   m_sDesc "Fusil de asalto 5,56. <b>Va a tu inventario.</b>"
   m_sPrefab "{GUID}Prefabs/Weapons/Rifles/M4/Rifle_M4.et"
   m_iPrice 400
  }
 }
}
```

Las claves y GUIDs entre llaves de cada entrada solo tienen que ser únicas dentro del archivo. El `m_sPrefab` hay que copiarlo de un prefab que exista: no lo inventes.

## 3. Un proveedor mínimo

```c
class MiProveedor : MCES_ShopProvider
{
    override int GetBalance()
    {
        return MiEconomia.DineroLocal();         // lo que se ve en la ventana (cliente)
    }

    override bool Buy(int pid, IEntity player, string shop, string itemId, IEntity source, out string msg)
    {
        // SERVIDOR: valida, cobra y entrega
        MCES_ShopConfig cfg = MCES_Shop.GetConfig(shop);
        MCES_Item it;
        foreach (MCES_Item i : cfg.m_aItems)
        {
            if (i.m_sId == itemId)
                it = i;
        }
        if (!it)
        {
            msg = "Ese artículo no existe.";
            return false;
        }
        if (!MiEconomia.Cobrar(pid, it.m_iPrice))
        {
            msg = "No te llega.";
            return false;
        }
        MiEntrega.AlInventario(player, it.m_sPrefab);
        msg = it.m_sName + " comprado.";
        return true;
    }
}
```

## 4. Registrar y abrir

```c
// al iniciar el modo de juego, EN SERVIDOR Y EN CLIENTES:
MCES_Shop.RegisterConfig("armeria", "{GUID}Configs/MiArmeria.conf");
MCES_Shop.SetProvider(new MiProveedor());

// en el cliente, por ejemplo desde una ScriptedUserAction del vendedor:
MCES_Shop.Open("armeria", GetOwner());
```

El provider tiene que estar registrado **también en el servidor**, porque allí se ejecuta `Buy`. El `.conf` también, si `Buy` lo lee.

## 5. Probar

- Abre la tienda en un cliente y comprueba que salen las pestañas y las tarjetas.
- En un script de prueba: `MCES_ShopDialog.LastTabCount()`, `LastCardCount()` y `GetOpen().Select("m4")`.
- Para comprar por código: `MCES_Shop.GetProvider().RequestBuy("armeria", "m4", null)`.
