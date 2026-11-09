# Rehace los recursos de MCES: layouts de la tienda, presets de dialogo, metas y constantes (MCES_Res.c).
# Usa el generador de layouts y el atlas de iconos de MCUIK.
# Uso: python tools/build.py
import io, os, sys

T = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(T)
KIT = os.path.join(os.path.dirname(ROOT), "MCUIK", "tools")
sys.path.insert(0, KIT)
import gen_layouts as L
import metas

OUT = os.path.join(ROOT, "UI", "layouts", "MCES")
MODBTN = '{7A737AACCAA99F51}Configs/WLib/WLib_ModularButton_ListMenuLine.conf'
FILL = {"HorizontalAlign": 3, "VerticalAlign": 3}

W, img, text, sized, overlay, hbox, vbox = L.W, L.img, L.text, L.sized, L.overlay, L.hbox, L.vbox


def button(name, children, slot=None):
	comp = "components {\n SCR_ModularButtonComponent " + L.gid(name + "btn") + ' : "' + MODBTN + '" {\n }\n}'
	return W("ButtonWidget", name, slot or dict(FILL), ["Color 1 1 1 0", comp], children)


def scroll(name, child, slot=None):
	return W("ScrollLayoutWidget", name, slot or dict(FILL), [], [child])


def icon(name, ic, size, color=L.ACCENT, slot=None):
	return W("ImageWidget", name, slot or {"HorizontalAlign": 1, "VerticalAlign": 1}, [f'Texture "{L.ICONS}"', f'Image "{ic}"', f"Color {color}", f"Size {size} {size}"])


def preview(name):
	return W("ItemPreviewWidget", name, dict(FILL), [])


def build():
	os.makedirs(OUT, exist_ok=True)
	CW, CH = 1090, 640

	# ---------------------------------------------------------------- contenido de la ventana
	tabs = sized("TabsSize", 200, None, {"VerticalAlign": 3, "Padding": "0 0 14 0"}, [
		overlay("TabsOv", [
			img("TabsBg", "0.03 0.045 0.058 0.75"),
			scroll("TabsScroll", vbox("Tabs", [], {"HorizontalAlign": 3, "VerticalAlign": 0, "Padding": "0 6 0 6"})),
		]),
	])
	grid = scroll("GridScroll", vbox("Grid", [], {"HorizontalAlign": 3, "VerticalAlign": 0}), {"SizeMode": "Fill", "Padding": "0 0 14 0"})
	detail = sized("DetailSize", 310, None, {"VerticalAlign": 3}, [
		overlay("DetailOv", [
			img("DetailBg", L.PANEL),
			sized("DetailStripSize", None, 3, {"HorizontalAlign": 3, "VerticalAlign": 0}, [img("DetailStrip", L.ACCENT)]),
			vbox("Detail", [
				sized("BigPreviewSize", 278, 160, {"Padding": "0 0 0 12"}, [
					overlay("BigPreviewOv", [
						img("BigPreviewBg", "1 1 1 0.05"),
						preview("BigPreview"),
						icon("BigIcon", "shop", 64),
					]),
				]),
				text("DCat", "CATEGORÍA", 13, L.BOLD, L.ACCENT, {"Padding": "0 0 0 2"}),
				text("DName", "Elige algo", 24, L.BOLD, L.TEXT, rich=True, wrap=True),
				hbox("DPriceRow", [
					icon("DPriceIcon", "money", 22, L.ACCENT, {"VerticalAlign": 1, "Padding": "0 0 8 0"}),
					text("DPrice", "", 28, L.BOLD, L.ACCENT, {"VerticalAlign": 1}),
				], {"Padding": "0 6 0 8"}),
				text("DDesc", "Haz clic en una tarjeta para ver qué es y cuánto cuesta.", 16, L.REG, L.MUTED, rich=True, wrap=True),
				text("DState", "", 16, L.BOLD, L.TEXT, {"Padding": "0 12 0 0"}, rich=True, wrap=True),
			], {"HorizontalAlign": 3, "VerticalAlign": 0, "Padding": "16 16 16 16"}),
		]),
	])
	content = W("FrameWidget", "ContentRoot", {}, [], [
		sized("Size", None, None, {"Anchor": "0.5 0.5 0.5 0.5", "OffsetLeft": 0, "OffsetTop": 0, "SizeX": CW, "OffsetRight": -CW, "SizeY": CH, "OffsetBottom": -CH, "Alignment": "0.5 0.5"}, [
			hbox("Panes", [tabs, grid, detail], dict(FILL)),
		]),
	])
	content.children[0].props += ["AllowMinDesiredWidth 1", f"MinDesiredWidth {CW}", "AllowMinDesiredHeight 1", f"MinDesiredHeight {CH}"]
	L.save("MCES_ShopContent", content, "LayoutSlot", OUT)

	# ---------------------------------------------------------------- tarjeta de articulo
	card = sized("CardRoot", 176, 196, {}, [
		button("CardBtn", [
			overlay("CardOv", [
				img("Bg", "0.07 0.1 0.125 0.92"),
				sized("StripSize", None, 3, {"HorizontalAlign": 3, "VerticalAlign": 2}, [img("Strip", "1 0.78 0.17 0")]),
				vbox("Col", [
					sized("PreviewSize", 156, 92, {"Padding": "0 0 0 8"}, [
						overlay("PreviewOv", [
							img("PreviewBg", "1 1 1 0.05"),
							preview("Preview"),
							icon("Icon", "shop", 40),
						]),
					]),
					text("Name", "Artículo", 16, L.BOLD, L.TEXT, rich=True, wrap=True),
					hbox("PriceRow", [
						text("Price", "0 €", 18, L.BOLD, L.ACCENT, {"SizeMode": "Fill", "VerticalAlign": 1}),
						icon("Lock", "lock", 16, L.MUTED, {"VerticalAlign": 1}),
					], {"HorizontalAlign": 3, "VerticalAlign": 2, "Padding": "0 4 0 0"}),
				], {"HorizontalAlign": 3, "VerticalAlign": 3, "Padding": "10 10 10 10"}),
			]),
		]),
	])
	L.save("MCES_Card", card, None, OUT)

	# ---------------------------------------------------------------- pestana de categoria
	tab = sized("TabRoot", None, 46, {"HorizontalAlign": 3}, [
		button("TabBtn", [
			overlay("TabOv", [
				img("Bg", "1 0.78 0.17 0"),
				sized("StripSize", 3, None, {"HorizontalAlign": 0, "VerticalAlign": 3}, [img("Strip", "1 0.78 0.17 0")]),
				hbox("Row", [
					icon("Icon", "star", 20, L.MUTED, {"VerticalAlign": 1, "Padding": "0 0 10 0"}),
					text("Label", "Categoría", 16, L.BOLD, L.TEXT, {"SizeMode": "Fill", "VerticalAlign": 1}),
					text("Count", "", 14, L.BOLD, L.MUTED, {"VerticalAlign": 1, "Padding": "8 0 0 0"}),
				], {"HorizontalAlign": 3, "VerticalAlign": 3, "Padding": "14 0 12 0"}),
			]),
		]),
	])
	L.save("MCES_Tab", tab, None, OUT)

	# ---------------------------------------------------------------- presets del dialogo
	conf = """SCR_ConfigurableDialogUiPresets {
 m_aPresets {
  SCR_ConfigurableDialogUiPreset "{4D3E5A1B2C6F7081}" {
   m_sLayout "{604721ED8E6ED459}UI/layouts/Menus/Dialogs/ConfigurableDialog_Big.layout"
   m_sContentLayout "%CONTENT%"
   m_sTag "mces_shop"
   m_sTitle "Tienda"
   m_aButtons {
    SCR_ConfigurableDialogUiButtonPreset "{4D3E5A1B2C6F7082}" {
     m_sTag "cancel"
     m_sActionName "MenuBack"
     m_sLabel "Cerrar"
     m_eAlign RIGHT
    }
    SCR_ConfigurableDialogUiButtonPreset "{4D3E5A1B2C6F7083}" {
     m_sTag "confirm"
     m_sActionName "MenuSelect"
     m_sLabel "Comprar"
     m_eAlign RIGHT
    }
   }
  }
 }
}
"""
	# primero los metas de los layouts para saber el GUID del contenido
	buf = io.StringIO()
	old = sys.stdout
	sys.stdout = buf
	metas.main(ROOT)
	sys.stdout = old
	guids = {}
	for l in buf.getvalue().splitlines():
		g, rel = l.split(" ")[0], l.split(" ")[1]
		guids[rel] = g
	content_ref = "{" + guids["UI/layouts/MCES/MCES_ShopContent.layout"] + "}UI/layouts/MCES/MCES_ShopContent.layout"
	io.open(os.path.join(ROOT, "Configs", "MCES", "MCES_Dialogs.conf"), "w", encoding="utf-8", newline="\n").write(conf.replace("%CONTENT%", content_ref))
	buf = io.StringIO()
	sys.stdout = buf
	metas.main(ROOT)
	sys.stdout = old
	lines = ["// Generado por tools/build.py: recursos de MCES (no editar a mano)", "class MCES_Res", "{"]
	for l in buf.getvalue().splitlines():
		g, rel = l.split(" ")[0], l.split(" ")[1]
		name = os.path.splitext(os.path.basename(rel))[0].replace("MCES_", "").upper()
		kind = "L_" if rel.endswith(".layout") else "C_"
		lines.append(f'\tstatic const ResourceName {kind}{name} = "{{{g}}}{rel}";')
	lines.append("}")
	io.open(os.path.join(ROOT, "Scripts", "Game", "MCES", "MCES_Res.c"), "w", encoding="utf-8", newline="\n").write("\n".join(lines) + "\n")
	print("\n".join(lines))


if __name__ == "__main__":
	build()
