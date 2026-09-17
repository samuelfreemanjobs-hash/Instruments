import { PageHeader } from "@/components/admin/PageHeader";
import { CategoryForm } from "@/components/admin/CategoryForm";

export default function NewCategoryPage() {
  return (
    <>
      <PageHeader title="New category" description="Adds a category to MongoDB via server action." />
      <CategoryForm />
    </>
  );
}
