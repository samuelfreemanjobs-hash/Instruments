import { BudgetClient } from "./budget-client";
import { loadShoppingBudget } from "./actions";

export default async function BudgetPage() {
  const loaded = await loadShoppingBudget();
  const serverBudget = loaded.ok ? loaded.data : null;

  return (
    <section className="space-y-4">
      <header className="space-y-1">
        <h2 className="text-lg font-semibold">Shopping budget</h2>
        <p className="text-sm text-bfm-muted">Plan spend by category; recommendations respect your max budget.</p>
      </header>
      <BudgetClient serverBudget={serverBudget} />
    </section>
  );
}
