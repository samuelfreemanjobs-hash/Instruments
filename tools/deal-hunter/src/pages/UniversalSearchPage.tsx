import { useCallback, useEffect, useState } from "react";
import { DealCard } from "../components/DealCard";
import { searchByImageRgb, universalSearch } from "../lib/api";
import type { ConditionFilter, Deal } from "../types";
import { DEAL_TYPE_LABELS, type DealType } from "../types";

const DEAL_TYPES: DealType[] = [
  "black_friday",
  "cyber_monday",
  "clearance",
  "open_box",
  "flash",
];

interface Props {
  stealThreshold: number;
  conditionFilter: ConditionFilter;
  couponsOnly: boolean;
  onWatch: (d: Deal) => void;
  onAddList: (d: Deal) => void;
  watchIds: Set<string>;
  listIds: Set<string>;
}

export function UniversalSearchPage({ conditionFilter, couponsOnly, ...props }: Props) {
  const [q, setQ] = useState("");
  const [brand, setBrand] = useState("");
  const [retailer, setRetailer] = useState("");
  const [category, setCategory] = useState("");
  const [model, setModel] = useState("");
  const [upc, setUpc] = useState("");
  const [minPrice, setMinPrice] = useState("");
  const [maxPrice, setMaxPrice] = useState("");
  const [minDiscount, setMinDiscount] = useState("");
  const [specKey, setSpecKey] = useState("");
  const [specValue, setSpecValue] = useState("");
  const [freeShip, setFreeShip] = useState(false);
  const [hits, setHits] = useState<Deal[]>([]);
  const [fetchedAt, setFetchedAt] = useState("");
  const [loading, setLoading] = useState(false);
  const [retailers, setRetailers] = useState<string[]>([]);

  useEffect(() => {
    void import("../lib/api").then((m) =>
      m.fetchCategories().then((c) => setRetailers(c.retailers)),
    );
  }, []);

  const run = useCallback(async () => {
    setLoading(true);
    try {
      const res = await universalSearch({
        q: q || undefined,
        brand: brand || undefined,
        retailer: retailer || undefined,
        category: category || undefined,
        model: model || undefined,
        upc: upc || undefined,
        minPrice: minPrice ? Number(minPrice) : undefined,
        maxPrice: maxPrice ? Number(maxPrice) : undefined,
        minDiscount: minDiscount ? Number(minDiscount) : undefined,
        freeShippingOnly: freeShip,
        conditionFilter,
        couponsOnly,
        specKey: specKey || undefined,
        specValue: specValue || undefined,
        includeIntelligence: true,
      });
      setHits(res.hits);
      setFetchedAt(res.fetchedAt);
    } finally {
      setLoading(false);
    }
  }, [q, brand, retailer, category, model, upc, minPrice, maxPrice, minDiscount, freeShip, conditionFilter, couponsOnly, specKey, specValue]);

  useEffect(() => {
    void run();
  }, [run]);

  const onImage = (file: File | null) => {
    if (!file) return;
    const img = new Image();
    const url = URL.createObjectURL(file);
    img.onload = () => {
      const canvas = document.createElement("canvas");
      canvas.width = canvas.height = 32;
      const ctx = canvas.getContext("2d");
      if (!ctx) return;
      ctx.drawImage(img, 0, 0, 32, 32);
      const data = ctx.getImageData(0, 0, 32, 32).data;
      let r = 0,
        g = 0,
        b = 0;
      const n = data.length / 4;
      for (let i = 0; i < data.length; i += 4) {
        r += data[i];
        g += data[i + 1];
        b += data[i + 2];
      }
      r = Math.round(r / n);
      g = Math.round(g / n);
      b = Math.round(b / n);
      void searchByImageRgb(r, g, b).then((res) => {
        setHits(res.hits);
        setFetchedAt(res.fetchedAt);
      });
      URL.revokeObjectURL(url);
    };
    img.src = url;
  };

  return (
    <div className="universal-search">
      <h2>Universal product search</h2>
      <p className="panel-hint">
        Replace Google Shopping tabs: search by name, brand, model, UPC, specs, retailer, price, discount,
        or product photo.
      </p>
      <form
        className="search-grid"
        onSubmit={(e) => {
          e.preventDefault();
          void run();
        }}
      >
        <input placeholder="Product name" value={q} onChange={(e) => setQ(e.target.value)} />
        <input placeholder="Brand (Adidas, Sephora…)" value={brand} onChange={(e) => setBrand(e.target.value)} />
        <input placeholder="Model" value={model} onChange={(e) => setModel(e.target.value)} />
        <input placeholder="UPC" value={upc} onChange={(e) => setUpc(e.target.value)} />
        <select value={retailer} onChange={(e) => setRetailer(e.target.value)}>
          <option value="">Retailer</option>
          {retailers.map((r) => (
            <option key={r} value={r}>
              {r}
            </option>
          ))}
        </select>
        <input placeholder="Category" value={category} onChange={(e) => setCategory(e.target.value)} />
        <input type="number" placeholder="Min $" value={minPrice} onChange={(e) => setMinPrice(e.target.value)} />
        <input type="number" placeholder="Max $" value={maxPrice} onChange={(e) => setMaxPrice(e.target.value)} />
        <input type="number" placeholder="Min % off" value={minDiscount} onChange={(e) => setMinDiscount(e.target.value)} />
        <input placeholder="Spec key" value={specKey} onChange={(e) => setSpecKey(e.target.value)} />
        <input placeholder="Spec value" value={specValue} onChange={(e) => setSpecValue(e.target.value)} />
        <label className="row-check">
          <input type="checkbox" checked={freeShip} onChange={(e) => setFreeShip(e.target.checked)} />
          Free shipping
        </label>
        <label className="image-upload">
          Search by image
          <input type="file" accept="image/*" onChange={(e) => onImage(e.target.files?.[0] ?? null)} />
        </label>
        <button type="submit" className="btn-primary">
          Search live
        </button>
      </form>
      <p className="fresh-line">Results ranked by deal quality · updated {fetchedAt ? new Date(fetchedAt).toLocaleTimeString() : "—"}</p>
      {loading ? (
        <p className="empty">Searching…</p>
      ) : hits.length === 0 ? (
        <p className="empty">No matches — try brand “Adidas” or retailer “Ulta”.</p>
      ) : (
        <div className="deal-grid">
          {hits.map((deal) => (
            <DealCard key={deal.id} deal={deal} stealThreshold={props.stealThreshold} onWatch={props.onWatch} onAddList={props.onAddList} isWatched={props.watchIds.has(deal.id)} inList={props.listIds.has(deal.id)} />
          ))}
        </div>
      )}
      <div className="chip-row">
        {DEAL_TYPES.map((t) => (
          <span key={t} className="chip">
            {DEAL_TYPE_LABELS[t]}
          </span>
        ))}
      </div>
    </div>
  );
}
