import Link from "next/link";

const COLUMNS = [
  {
    title: "Sounds",
    links: [
      { href: "/sounds", label: "Catalog" },
      { href: "/sounds/memphis-vault-vol-1", label: "Memphis Vault Vol. 1" },
      { href: "/free", label: "Free sounds" },
    ],
  },
  {
    title: "Factory",
    links: [
      { href: "/drum-factory", label: "Drum Factory" },
      { href: "/daw-inbox", label: "DAW inbox" },
      { href: "/plugins", label: "Plugins" },
      { href: "/plugins/memphis-phonk-factory", label: "Memphis Phonk Factory" },
    ],
  },
  {
    title: "Account",
    links: [
      { href: "/login", label: "Sign in" },
      { href: "/account", label: "Account" },
      { href: "/legal", label: "License" },
    ],
  },
];

export function SiteFooter() {
  return (
    <footer className="mt-16 border-t border-gold/30 bg-obsidian">
      <div className="mx-auto grid max-w-6xl gap-8 px-4 py-10 sm:grid-cols-3">
        {COLUMNS.map((column) => (
          <div key={column.title}>
            <h2 className="font-condensed text-sm tracking-[0.18em] text-gold uppercase">{column.title}</h2>
            <ul className="mt-3 space-y-2">
              {column.links.map((link) => (
                <li key={link.href}>
                  <Link href={link.href} className="text-sm text-ivory/80 hover:text-ivory">
                    {link.label}
                  </Link>
                </li>
              ))}
            </ul>
          </div>
        ))}
      </div>
      <p className="px-4 pb-8 text-center font-mono text-xs text-ivory/50">
        DiskLordz Audio · original synthesis · price unset on Memphis Vault
      </p>
    </footer>
  );
}
