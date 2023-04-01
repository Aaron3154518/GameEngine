import { parse, v4 } from 'uuid';

// Random UUID
export type UUID = Uint8Array;

export function newUUID(): string {
  return v4();
}

// Search filtering
export function searchScore<T>(
  search: string,
  getSearchStrings: (t: T) => string[]
): (item: T) => number {
  return (item: T) =>
    getSearchStrings(item).reduce((s: number, str: string) => {
      let idx: number = str.indexOf(search);
      return s + (idx === -1 ? 0 : idx === 0 ? 1 : 0.5);
    }, 0);
}

export function sortList<T>(
  list: T[],
  scoreFunc: (a: T) => number,
  ascending: boolean = true
) {
  list.sort(
    (a: T, b: T) => (scoreFunc(b) - scoreFunc(a)) * (ascending ? 1 : -1)
  );
}

// Variable/type regexes
export const alphanum_: string = '0-9a-zA-Z_';

export function validateVar(s: string): boolean {
  return RegExp(`[a-zA-Z_][${alphanum_}]*`).test(s);
}

export function sanitizeVar(s: string): string {
  return `1${s}`
    .replace(RegExp(`[^${alphanum_}]`, 'g'), '')
    .replace(RegExp('[0-9]+'), '');
}

export function validateType(s: string): boolean {
  return RegExp(`[a-zA-Z_][${alphanum_}]*(::[${alphanum_}]+)*`).test(s);
}

export function sanitizeType(s: string): string {
  return `1${s}`
    .replace(RegExp(`[^${alphanum_}:]`, 'g'), '')
    .replace(RegExp('[0-9]+'), '');
}
