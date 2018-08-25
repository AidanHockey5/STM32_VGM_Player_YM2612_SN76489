I've tried to keep as many components on Digikey as possible, but obviously, not everything will be on there.
That means that if you'd like a complete order of components, you must have all of the components from the DIGIKEY-BOM.csv file AND the EXTERNAL_BOM_ITEMS.txt list.

For items like resistors: Bog-standard 1/4 watt (the most common sizes you'd normally find in kits) will work, but I tried to track some down on Digikey any ways. 

For items listed on Aliexpress: Shop around and do your research. The links I provided should give you a great head start. Also check sources like eBay for similar goods.
LCSC can also provide parts at a significantly lower price than Digikey, but their selection is not as good. If you order a PCB from JLCPCB, you can ship components from LCSC combined for free.

You can use the Digikey BOM import tool to make this entire process much faster: https://www.digikey.com/BOM
Or you can sign-in to Digikey and view the parts cart here: https://www.digikey.com/short/jf18pj

Also, the 1000uF capacitors (C37, C36, and C34) are actually have 2.5mm pin spacing, but Digikey only had 3.5mm spaced 8mm diameter caps. The fit isn't exactly perfect, but it's close enough to work.
If you can find 1000uF (16V or higher) capacitors that are 8mm in diameter and have 2.5mm pin spacing, I reccomend that you use those for a better fit. 
