import type { Deal } from "../types";

export interface DealCardProps {
  stealThreshold: number;
  onWatch: (deal: Deal) => void;
  onAddList: (deal: Deal) => void;
  watchIds: Set<string>;
  listIds: Set<string>;
  showStudent?: boolean;
}
