import Link from "next/link";
import { DataTable } from "@/components/admin/DataTable";
import { PageHeader } from "@/components/admin/PageHeader";

type Row = Record<string, unknown>;

export function AdminListStub({
  title,
  description,
  columns,
  rows,
  createHref,
  createLabel = "Create",
}: {
  title: string;
  description: string;
  columns: { key: string; header: string }[];
  rows: Row[];
  createHref?: string;
  createLabel?: string;
}) {
  return (
    <>
      <PageHeader
        title={title}
        description={description}
        action={
          createHref ? (
            <Link
              href={createHref}
              className="rounded-md bg-slate-900 px-4 py-2 text-sm font-medium text-white hover:bg-slate-800"
            >
              {createLabel}
            </Link>
          ) : undefined
        }
      />
      <DataTable columns={columns} rows={rows} />
    </>
  );
}
