"use client";

import { useState, useTransition } from "react";
import {
  addWishlistItem,
  createWishlist,
  deleteWishlist,
  removeWishlistItem,
} from "./actions";
import { Button } from "@/components/ui/button";
import { Input } from "@/components/ui/input";
import { EmptyState } from "@/components/ui/empty-state";

export type WishlistRow = {
  id: string;
  name: string;
  created_at: string;
};

export type WishlistItemRow = {
  id: string;
  wishlist_id: string;
  external_deal_id: string | null;
  target_price: number | null;
  note: string | null;
  added_at: string;
};

export function WishlistClient({
  lists,
  items,
}: {
  lists: WishlistRow[];
  items: WishlistItemRow[];
}) {
  const [name, setName] = useState("Holiday picks");
  const [dealId, setDealId] = useState("");
  const [selectedList, setSelectedList] = useState(lists[0]?.id ?? "");
  const [pending, start] = useTransition();

  if (lists.length === 0) {
    return (
      <EmptyState
        title="No wishlists"
        description="Create a list to track deals across devices."
        action={
          <Button
            type="button"
            onClick={() =>
              start(() => {
                void createWishlist("My wishlist");
              })
            }
          >
            Create default list
          </Button>
        }
      />
    );
  }

  const listItems = items.filter((i) => i.wishlist_id === (selectedList || lists[0].id));

  return (
    <div className="space-y-4">
      <div className="flex flex-wrap gap-2">
        {lists.map((w) => (
          <Button
            key={w.id}
            type="button"
            variant={selectedList === w.id ? "default" : "outline"}
            onClick={() => setSelectedList(w.id)}
          >
            {w.name}
          </Button>
        ))}
      </div>

      <form
        className="flex flex-wrap gap-2"
        onSubmit={(e) => {
          e.preventDefault();
          start(() => {
            void createWishlist(name).then(() => setName(""));
          });
        }}
      >
        <Input placeholder="New list name" value={name} onChange={(e) => setName(e.target.value)} />
        <Button type="submit" disabled={pending}>
          Add list
        </Button>
      </form>

      <form
        className="flex flex-wrap gap-2"
        onSubmit={(e) => {
          e.preventDefault();
          const lid = selectedList || lists[0].id;
          start(() => {
            void addWishlistItem(lid, dealId).then(() => setDealId(""));
          });
        }}
      >
        <Input placeholder="Catalog deal id" value={dealId} onChange={(e) => setDealId(e.target.value)} />
        <Button type="submit" variant="outline" disabled={pending}>
          Add deal id
        </Button>
      </form>

      <ul className="space-y-2 text-sm">
        {listItems.map((i) => (
          <li key={i.id} className="flex items-center justify-between rounded-lg border border-bfm-border px-3 py-2">
            <span>
              {i.note || i.external_deal_id}
              {i.target_price != null && ` · target $${Number(i.target_price).toFixed(2)}`}
            </span>
            <Button
              type="button"
              variant="ghost"
              className="text-bfm-danger"
              onClick={() => start(() => void removeWishlistItem(i.id))}
            >
              Remove
            </Button>
          </li>
        ))}
        {listItems.length === 0 && <li className="text-bfm-muted">Empty list — add from Search or Assistant.</li>}
      </ul>

      <Button
        type="button"
        variant="ghost"
        className="text-bfm-danger"
        onClick={() =>
          start(() => {
            if (selectedList) void deleteWishlist(selectedList);
          })
        }
      >
        Delete selected list
      </Button>
    </div>
  );
}
