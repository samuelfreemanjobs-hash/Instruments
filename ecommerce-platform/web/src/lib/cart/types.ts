export type CartLine = {
  productId: string;
  quantity: number;
};

export type CartProductLine = {
  productId: string;
  quantity: number;
  title: string;
  slug: string;
  price: number;
  stock: number;
  lineTotal: number;
};

export type CartSnapshot = {
  lines: CartProductLine[];
  subtotal: number;
  itemCount: number;
};
