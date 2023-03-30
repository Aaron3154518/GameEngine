import { parse, v4 } from 'uuid';

// Random UUID
export type UUID = Uint8Array;

export function newUUID(): UUID {
  return parse(v4());
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

export function sortListAscending<T>(
  list: T[],
  scoreFunc: (a: T) => number,
  reversed: boolean = false
) {
  list.sort(
    (a: T, b: T) => (scoreFunc(b) - scoreFunc(a)) * (reversed ? -1 : 1)
  );
}
