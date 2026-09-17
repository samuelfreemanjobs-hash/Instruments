export const mockKpis = {
  revenue: 124_580,
  orders: 892,
  customers: 3_421,
  products: 156,
};

export const mockOrders = [
  { id: "ORD-1001", customer: "Jane Doe", total: 129.99, status: "PAID" },
  { id: "ORD-1002", customer: "John Smith", total: 54.5, status: "PENDING" },
];

export const mockProducts = [
  { id: "1", title: "Wireless Headphones", price: 79.99, stock: 42 },
  { id: "2", title: "USB-C Hub", price: 49.0, stock: 18 },
];

export const adminNav = [
  { href: "/admin", label: "Dashboard" },
  { href: "/admin/customers", label: "Customers" },
  { href: "/admin/coupons", label: "Coupons" },
  { href: "/admin/orders", label: "Orders" },
  { href: "/admin/products", label: "All Products" },
  { href: "/admin/products/new", label: "Create Product" },
  { href: "/admin/categories", label: "Categories" },
  { href: "/admin/subcategories", label: "Sub-Categories" },
  { href: "/admin/banners/website", label: "Website Banners" },
  { href: "/admin/banners/app", label: "App Banners" },
  { href: "/admin/topbar", label: "Top Bar" },
  { href: "/admin/offers", label: "Home Offers" },
  { href: "/admin/reviews", label: "Reviews" },
  { href: "/admin/analytics", label: "Analytics" },
] as const;
